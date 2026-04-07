#include "StandartOpenGlRender.hpp"
#include "Sun.hpp"
#include "Plane.hpp"
#include "CameraController.hpp"
#include "CameraObject.hpp"
#include "WhiteSphere.hpp"  // <-- ДОБАВИТЬ
#include <iostream>
#include <GL/glew.h>

extern CameraController* g_cameraController;

bool debugMode = true;

// Инициализация статического флага ObjMesh для теней
bool ObjMesh::s_shadowPass = false;

StandartOpenGlRender::StandartOpenGlRender() {}

StandartOpenGlRender::~StandartOpenGlRender() {
    if (glContext) SDL_GL_DestroyContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
bool StandartOpenGlRender::init(int width, int height, const char* title) {
    windowWidth = width;
    windowHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(
        title,
        windowWidth, windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cout << "Window Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

    // ===== ИНИЦИАЛИЗАЦИЯ ТЕНЕЙ =====
    if (!shadowMap.init(1024, 1024)) {
        std::cout << "Warning: ShadowMap initialization failed!" << std::endl;
        shadowsEnabled = false;
    }

    // Загрузка шейдера для карты глубины
    if (!shadowShader.loadFromString(
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "uniform mat4 uModel;\n"
        "uniform mat4 uLightSpaceMatrix;\n"
        "void main() {\n"
        "    gl_Position = uLightSpaceMatrix * uModel * vec4(aPos, 1.0);\n"
        "}\n",
        "#version 330 core\n"
        "void main() {}\n"
    )) {
        std::cout << "Warning: Shadow shader failed to load!" << std::endl;
        shadowsEnabled = false;
    }

    // Загрузка основного шейдера
    if (!mainShader.loadFromString(
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "layout(location = 1) in vec3 aNormal;\n"
        "layout(location = 2) in vec4 aColor;\n"
        "out vec3 vNormal;\n"
        "out vec4 vColor;\n"
        "out vec4 vFragPosLightSpace;\n"
        "out vec3 vFragPos;\n"
        "uniform mat4 uModel;\n"
        "uniform mat4 uView;\n"
        "uniform mat4 uProjection;\n"
        "uniform mat4 uLightSpaceMatrix;\n"
        "void main() {\n"
        "    vec4 worldPos = uModel * vec4(aPos, 1.0);\n"
        "    gl_Position = uProjection * uView * worldPos;\n"
        "    vNormal = mat3(transpose(inverse(uModel))) * aNormal;\n"
        "    vColor = aColor;\n"
        "    vFragPosLightSpace = uLightSpaceMatrix * worldPos;\n"
        "    vFragPos = worldPos.xyz;\n"
        "}\n",
        "#version 330 core\n"
        "in vec3 vNormal;\n"
        "in vec4 vColor;\n"
        "in vec4 vFragPosLightSpace;\n"
        "in vec3 vFragPos;\n"
        "out vec4 FragColor;\n"
        "uniform vec3 uLightPos;\n"
        "uniform vec3 uLightColor;\n"
        "uniform sampler2D uShadowMap;\n"
        "float ShadowCalculation(vec4 fragPosLightSpace) {\n"
        "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
        "    projCoords = projCoords * 0.5 + 0.5;\n"
        "    if (projCoords.z > 1.0) return 0.0;\n"
        "    float shadow = 0.0;\n"
        "    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);\n"
        "    float bias = 0.005;\n"
        "    for (int x = -1; x <= 1; x++) {\n"
        "        for (int y = -1; y <= 1; y++) {\n"
        "            float closestDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;\n"
        "            float currentDepth = projCoords.z;\n"
        "            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;\n"
        "        }\n"
        "    }\n"
        "    return shadow / 9.0;\n"
        "}\n"
        "void main() {\n"
        "    vec3 normal = normalize(vNormal);\n"
        "    vec3 lightDir = normalize(uLightPos - vFragPos);\n"
        "    vec3 ambient = 0.4 * vColor.rgb;\n"
        "    float diff = max(dot(normal, lightDir), 0.2);\n"
        "    vec3 diffuse = diff * uLightColor * vColor.rgb;\n"
        "    float shadow = ShadowCalculation(vFragPosLightSpace);\n"
        "    float shadowIntensity = 0.4;\n"
        "    vec3 result = ambient + (1.0 - shadow * shadowIntensity) * diffuse;\n"
        "    FragColor = vec4(result, vColor.a);\n"
        "}\n"
    )) {
        std::cout << "Warning: Main shader failed to load!" << std::endl;
        shadowsEnabled = false;
    }

    // Камера
    camera.setPosition(vec3(0, 3, 15));
    camera.lookAt(vec3(0, 0, 0));

    lastTime = SDL_GetTicks() / 1000.0f;

    return true;
}

void StandartOpenGlRender::run() {
    while (running) {
        float currentTime = SDL_GetTicks() / 1000.0f;
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (deltaTime > 0.033f) deltaTime = 0.033f;

        frameCount++;
        if (deltaTime > 0) {
            fps = (int)(1.0f / deltaTime);
        }

        // ===== ЗАГОЛОВОК ОКНА =====
        std::string title = "3D Engine | FPS: " + std::to_string(fps);
        setWindowTitle(title);

        // ===== ОБРАБОТКА СОБЫТИЙ =====
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
                glViewport(0, 0, windowWidth, windowHeight);
            }

            // ===== АВТОМАТИЧЕСКИЙ ЗАХВАТ/ОТПУСКАНИЕ МЫШИ =====
            if (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED) {
                for (auto& obj : sceneObjects) {
                    CameraObject* cam = dynamic_cast<CameraObject*>(obj.get());
                    if (cam) {
                        cam->setMouseLocked(true);
                        break;
                    }
                }
            }

            if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
                for (auto& obj : sceneObjects) {
                    CameraObject* cam = dynamic_cast<CameraObject*>(obj.get());
                    if (cam) {
                        cam->setMouseLocked(false);
                        break;
                    }
                }
            }

            // Захват мыши по правой кнопке
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT) {
                for (auto& obj : sceneObjects) {
                    CameraObject* cam = dynamic_cast<CameraObject*>(obj.get());
                    if (cam) {
                        cam->setMouseLocked(true);
                        break;
                    }
                }
            }

            // Отпускание мыши по ESC
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                for (auto& obj : sceneObjects) {
                    CameraObject* cam = dynamic_cast<CameraObject*>(obj.get());
                    if (cam) {
                        cam->setMouseLocked(false);
                        break;
                    }
                }
            }

            // Движение мыши
            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                for (auto& obj : sceneObjects) {
                    CameraObject* cam = dynamic_cast<CameraObject*>(obj.get());
                    if (cam) {
                        cam->handleMouseMotion(event.motion.xrel, event.motion.yrel);
                        break;
                    }
                }
            }
        }

        // ===== ОБНОВЛЕНИЕ ФИЗИКИ =====
        m_physicsWorld.update(deltaTime);

        // ===== FIXED UPDATE =====
        if (m_fixedUpdateCallback) {
            m_accumulator += deltaTime;
            while (m_accumulator >= m_fixedTimestep) {
                m_fixedUpdateCallback(m_fixedTimestep);
                m_accumulator -= m_fixedTimestep;
            }
        }

        // ===== UPDATE ОБЪЕКТОВ =====
        for (auto& obj : sceneObjects) {
            obj->update(deltaTime);
        }

        // ===== ГЛОБАЛЬНЫЙ UPDATE ИЗ MAIN =====
        if (m_updateCallback) {
            m_updateCallback(deltaTime);
        }

        // ===== НАХОДИМ КАМЕРУ-ОБЪЕКТ ДЛЯ РЕНДЕРА =====
        CameraObject* mainCamera = nullptr;
        for (auto& obj : sceneObjects) {
            mainCamera = dynamic_cast<CameraObject*>(obj.get());
            if (mainCamera) break;
        }

        if (mainCamera) {
            camera.setPosition(mainCamera->getEyePosition());
            camera.setRotation(mainCamera->transform.rotation);
        }

        // ===== ПОИСК СОЛНЦА =====
        Sun* sun = nullptr;
        for (auto& obj : sceneObjects) {
            if (dynamic_cast<Sun*>(obj.get())) {
                sun = static_cast<Sun*>(obj.get());
                break;
            }
        }

        // ===== ПРОХОД 1: КАРТА ГЛУБИНЫ =====
        if (shadowsEnabled && sun && shadowMap.isValid()) {
            shadowMap.bindForWriting();
            glClear(GL_DEPTH_BUFFER_BIT);
            glDisable(GL_LIGHTING);

            shadowShader.use();
            shadowShader.setUniform("uLightSpaceMatrix", sun->getLightSpaceMatrix());

            for (auto& obj : sceneObjects) {
                if (dynamic_cast<Sun*>(obj.get())) continue;
                if (dynamic_cast<CameraObject*>(obj.get())) continue;
                shadowShader.setUniform("uModel", obj->transform.getMatrix());
                obj->render();
            }

            shadowMap.unbind();
            glEnable(GL_LIGHTING);
        }

        // ===== ПРОХОД 2: ОСНОВНОЙ РЕНДЕР =====
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ===== НЕБО (ТЕКСТУРА) =====
        m_skyTexture.render();
        // ===== ТЕСТ 2D РЕНДЕРА =====
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 1, 0, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(0.1f, 0.1f);
        glVertex2f(0.3f, 0.1f);
        glVertex2f(0.3f, 0.3f);
        glVertex2f(0.1f, 0.3f);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        // ===== НАСТРОЙКА 3D ПРОЕКЦИИ =====
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = (float)windowWidth / windowHeight;
        mat4 projection = camera.getProjectionMatrix(aspect);
        glMultMatrixf(projection.ptr());

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(camera.getViewMatrix().ptr());

        // ===== ОСНОВНОЙ РЕНДЕР ОБЪЕКТОВ =====
        if (shadowsEnabled && sun) {
            mainShader.use();
            mainShader.setUniform("uLightSpaceMatrix", sun->getLightSpaceMatrix());
            mainShader.setUniform("uLightPos", sun->getPosition());
            mainShader.setUniform("uLightColor", sun->getLightColor());
            mainShader.setUniform("uView", camera.getViewMatrix());
            mainShader.setUniform("uProjection", projection);
            shadowMap.bindForReading(1);
            mainShader.setUniform("uShadowMap", 1);

            for (auto& obj : sceneObjects) {
                if (dynamic_cast<Sun*>(obj.get())) continue;
                if (dynamic_cast<CameraObject*>(obj.get())) continue;
                mainShader.setUniform("uModel", obj->transform.getMatrix());
                obj->render();
            }
        }
        else {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);

            if (sun) {
                float lightPos[] = { sun->getPosition().x, sun->getPosition().y, sun->getPosition().z, 1.0f };
                glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
                float lightColor[] = { sun->getLightColor().x * sun->getIntensity(),
                                       sun->getLightColor().y * sun->getIntensity(),
                                       sun->getLightColor().z * sun->getIntensity(), 1.0f };
                glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
                glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor);
            }

            for (auto& obj : sceneObjects) {
                if (dynamic_cast<Sun*>(obj.get())) continue;
                if (dynamic_cast<CameraObject*>(obj.get())) continue;
                glPushMatrix();
                glMultMatrixf(obj->transform.getMatrix().ptr());
                obj->render();
                glPopMatrix();
            }
        }

        // ===== ОТРИСОВКА КОЛЛАЙДЕРОВ (ДЛЯ ОТЛАДКИ) =====
        if (debugMode) {
            glUseProgram(0);
            glDisable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            for (auto& obj : sceneObjects) {
                if (dynamic_cast<Sun*>(obj.get())) continue;
                if (dynamic_cast<CameraObject*>(obj.get())) continue;

                vec3 min, max;
                obj->collider.getWorldBounds(obj->transform, min, max);

                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                glBegin(GL_LINE_LOOP);
                glVertex3f(min.x, min.y, min.z);
                glVertex3f(max.x, min.y, min.z);
                glVertex3f(max.x, max.y, min.z);
                glVertex3f(min.x, max.y, min.z);
                glEnd();
                glBegin(GL_LINE_LOOP);
                glVertex3f(min.x, min.y, max.z);
                glVertex3f(max.x, min.y, max.z);
                glVertex3f(max.x, max.y, max.z);
                glVertex3f(min.x, max.y, max.z);
                glEnd();
                glBegin(GL_LINES);
                glVertex3f(min.x, min.y, min.z); glVertex3f(min.x, min.y, max.z);
                glVertex3f(max.x, min.y, min.z); glVertex3f(max.x, min.y, max.z);
                glVertex3f(max.x, max.y, min.z); glVertex3f(max.x, max.y, max.z);
                glVertex3f(min.x, max.y, min.z); glVertex3f(min.x, max.y, max.z);
                glEnd();
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_LIGHTING);
        }

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }
}

void StandartOpenGlRender::stop() {
    running = false;
}

void StandartOpenGlRender::setWindowTitle(const std::string& title) {
    if (window) {
        SDL_SetWindowTitle(window, title.c_str());
    }
}