#include "StandartOpenGlRender.hpp"
#include "Sun.hpp"
#include "Plane.hpp"
#include "CameraController.hpp"
#include "CameraObject.hpp"
#include "WhiteSphere.hpp"
#include "StandartPlayerController.hpp"
#include <iostream>
#include <GL/glew.h>
#include "GPUPhysics.hpp" 

extern StandartPlayerController* g_playerController;
bool ObjMesh::s_shadowPass = false;

StandartOpenGlRender::StandartOpenGlRender() {}

StandartOpenGlRender::~StandartOpenGlRender() {
    if (glContext) SDL_GL_DestroyContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void StandartOpenGlRender::printSettings() {
    std::cout << "\n========== НАСТРОЙКИ ДВИЖКА ==========\n";
    std::cout << "----- ГРАФИКА -----\n";
    std::cout << "Тени: " << (ENABLE_SHADOWS ? "ВКЛ" : "ВЫКЛ") << "\n";
    std::cout << "Vsync: " << (ENABLE_VSYNC ? "ВКЛ" : "ВЫКЛ") << "\n";
    std::cout << "GPU матрицы: " << (ENABLE_GPU_MATRICES ? "ВКЛ" : "ВЫКЛ") << "\n";
    std::cout << "Макс объектов GPU: " << MAX_GPU_OBJECTS << "\n";
    std::cout << "Коллайдеры (отладка): " << (ENABLE_DEBUG_COLLIDERS ? "ВКЛ" : "ВЫКЛ") << "\n";
    std::cout << "----- ФИЗИКА -----\n";
    std::cout << "GPU физика: " << (ENABLE_GPU_PHYSICS ? "ВКЛ" : "ВЫКЛ") << "\n";
    std::cout << "Макс объектов физики: " << MAX_PHYSICS_OBJECTS << "\n";
    std::cout << "Гравитация: (" << GRAVITY.x << ", " << GRAVITY.y << ", " << GRAVITY.z << ")\n";
    std::cout << "----- УПРАВЛЕНИЕ -----\n";
    std::cout << "Скорость игрока: " << PLAYER_MOVE_SPEED << "\n";
    std::cout << "Чувствительность мыши: " << PLAYER_MOUSE_SENS << "\n";
    std::cout << "Сила прыжка: " << PLAYER_JUMP_POWER << "\n";
    std::cout << "Гравитация игрока: " << PLAYER_GRAVITY << "\n";
    std::cout << "Инверт X: " << (INVERT_X ? "ДА" : "НЕТ") << "\n";
    std::cout << "Инверт Y: " << (INVERT_Y ? "ДА" : "НЕТ") << "\n";
    std::cout << "----- КАМЕРА -----\n";
    std::cout << "FOV: " << CAMERA_FOV << "\n";
    std::cout << "Плоскости: near=" << CAMERA_NEAR << " far=" << CAMERA_FAR << "\n";
    std::cout << "Старт позиция: (" << CAMERA_START_POS.x << ", " << CAMERA_START_POS.y << ", " << CAMERA_START_POS.z << ")\n";
    std::cout << "=====================================\n" << std::endl;
}

void StandartOpenGlRender::uploadMatricesToGPU() {
    if (!ENABLE_GPU_MATRICES) return;
    if (!m_matricesDirty) return;

    m_gpuModels.clear();
    m_gpuNormalMatrices.clear();

    for (auto& obj : sceneObjects) {
        if (dynamic_cast<Sun*>(obj.get())) continue;
        if (dynamic_cast<CameraObject*>(obj.get())) continue;
        if ((int)m_gpuModels.size() >= MAX_GPU_OBJECTS) break;

        m_gpuModels.push_back(obj->transform.getMatrix());
        m_gpuNormalMatrices.push_back(obj->transform.getMatrix().inverse().transpose());
    }

    if (!m_gpuModels.empty()) {
        m_modelBuffer.upload(m_gpuModels.data(), m_gpuModels.size() * sizeof(mat4));
        m_normalMatrixBuffer.upload(m_gpuNormalMatrices.data(), m_gpuNormalMatrices.size() * sizeof(mat4));
    }

    m_matricesDirty = false;
}

bool StandartOpenGlRender::init(int width, int height, const char* title) {
    windowWidth = width;
    windowHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Выбор версии OpenGL
    if (ENABLE_GPU_MATRICES || ENABLE_GPU_PHYSICS) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        std::cout << "OpenGL 4.3 (GPU features enabled)" << std::endl;
    }
    else {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        std::cout << "OpenGL 3.3 (GPU features disabled)" << std::endl;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(title, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cout << "Window Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();
    SDL_GL_SetSwapInterval(ENABLE_VSYNC ? 1 : 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(WORLD_CLEAR_COLOR.x, WORLD_CLEAR_COLOR.y, WORLD_CLEAR_COLOR.z, 1.0f);

    // Инициализация теней
    if (ENABLE_SHADOWS) {
        if (!shadowMap.init(1024, 1024)) {
            std::cout << "ShadowMap failed!" << std::endl;
            ENABLE_SHADOWS = false;
        }
    }

    // Шейдер теней
    if (!shadowShader.loadFromString(
        // шейдер 4
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "uniform mat4 uModel;\n"
        "uniform mat4 uLightSpaceMatrix;\n"
        "void main() { gl_Position = uLightSpaceMatrix * uModel * vec4(aPos, 1.0); }\n",
        "#version 330 core\nvoid main() {}\n"
    )) {
        std::cout << "Shadow shader failed!" << std::endl;
        ENABLE_SHADOWS = false;
    }

    // Основной шейдер
    std::string vertexShaderSource;
        if (ENABLE_GPU_MATRICES) {
            vertexShaderSource =
                "#version 430 core\n"
                "layout(location = 0) in vec3 aPos;\n"
                "layout(location = 1) in vec3 aNormal;\n"
                "layout(location = 2) in vec4 aColor;\n"
                "layout(std430, binding = 0) buffer ModelBuffer { mat4 models[]; };\n"
                "layout(std430, binding = 1) buffer NormalMatrixBuffer { mat4 normalMatrices[]; };\n"
                "uniform mat4 uView;\n"
                "uniform mat4 uProjection;\n"
                "uniform mat4 uLightSpaceMatrix;\n"
                "uniform vec3 uLightPos;\n"
                "uniform int uObjectIndex;\n"
                "out vec4 vColor;\n"
                "out vec3 vNormal;\n"
                "out vec3 vFragPos;\n"
                "out vec4 vFragPosLightSpace;\n"
                "\n"
                "void main() {\n"
                "    mat4 uModel = models[uObjectIndex];\n"
                "    mat4 uNormalMatrix = normalMatrices[uObjectIndex];\n"
                "    vec4 worldPos = uModel * vec4(aPos, 1.0);\n"
                "    gl_Position = uProjection * uView * worldPos;\n"
                "    vNormal = mat3(uNormalMatrix) * aNormal;\n"
                "    vFragPos = worldPos.xyz;\n"
                "    vColor = aColor;\n"
                "    vFragPosLightSpace = uLightSpaceMatrix * worldPos;\n"
                "}\n"
        ;}
    
    else {
        vertexShaderSource =
            "#version 430 core\n"
            "layout(location = 0) in vec3 aPos;\n"
            "layout(location = 1) in vec3 aNormal;\n"
            "layout(location = 2) in vec4 aColor;\n"
            "layout(std430, binding = 0) buffer ModelBuffer { mat4 models[]; };\n"
            "layout(std430, binding = 1) buffer NormalMatrixBuffer { mat4 normalMatrices[]; };\n"
            "uniform mat4 uView;\n"
            "uniform mat4 uProjection;\n"
            "uniform vec3 uLightPos;\n"
            "uniform int uObjectIndex;\n"
            "out vec4 vColor;\n"
            "\n"
            "void main() {\n"
            "    mat4 uModel = models[uObjectIndex];\n"
            "    mat4 uNormalMatrix = normalMatrices[uObjectIndex];\n"
            "    vec4 worldPos = uModel * vec4(aPos, 1.0);\n"
            "    gl_Position = uProjection * uView * worldPos;\n"
            "    vec3 normal = mat3(uNormalMatrix) * aNormal;\n"
            "    vec3 lightDir = normalize(uLightPos - worldPos.xyz);\n"
            "    float diff = dot(normal, lightDir);\n"
            "    float isLit = diff > 0.0 ? 1.0 : 0.0;\n"
            "    vColor = vec4(aColor.rgb * (0.3 + isLit * 0.7), aColor.a);\n"
            "}\n";
    }
  
    // шейдер 1
    if (!mainShader.loadFromString(vertexShaderSource,
        "#version 330 core\n"
        "in vec4 vColor;\n"
        "in vec3 vNormal;\n"
        "in vec3 vFragPos;\n"
        "out vec4 FragColor;\n"
        "uniform vec3 uLightPos;\n"
        "uniform vec3 uLightColor;\n"
        "uniform vec3 uCameraPos;\n"
        "\n"
        "void main() {\n"
        "    vec3 normal = normalize(vNormal);\n"
        "    vec3 lightDir = normalize(uLightPos - vFragPos);\n"
        "    vec3 viewDir = normalize(uCameraPos - vFragPos);\n"
        "    \n"
        "    float ambient = 0.4;\n"
        "    float diff = max(dot(normal, lightDir), 0.2);\n"
        "    \n"
        "    vec3 reflectDir = reflect(-lightDir, normal);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"
        "    float specular = spec * 0.3;\n"
        "    \n"
        "    float brightness = ambient + diff + specular;\n"
        "    vec3 result = vColor.rgb * brightness;\n"
        "    FragColor = vec4(result, vColor.a);\n"
        "}\n"
    )) {
        std::cout << "Main shader failed!" << std::endl;
    }

    // GPU-буферы для матриц
    if (ENABLE_GPU_MATRICES) {
        m_modelBuffer.create(MAX_GPU_OBJECTS * sizeof(mat4), 0);
        m_normalMatrixBuffer.create(MAX_GPU_OBJECTS * sizeof(mat4), 1);
        std::cout << "GPU buffers created (" << MAX_GPU_OBJECTS << " objects max)" << std::endl;
    }

    // GPU-физика
    if (ENABLE_GPU_PHYSICS) {
        if (!m_gpuPhysics.init(MAX_PHYSICS_OBJECTS)) {
            std::cout << "GPU Physics initialization failed!" << std::endl;
            ENABLE_GPU_PHYSICS = false;
        }
    }

    // Камера
    camera.setFov(CAMERA_FOV);
    camera.setPosition(CAMERA_START_POS);
    camera.lookAt(vec3(0, 0, 0));
    getCamera().setPosition(CAMERA_START_POS);
    getCamera().lookAt(vec3(0, 0, 0));

    lastTime = SDL_GetTicks() / 1000.0f;

    printSettings();
    return true;
}

void StandartOpenGlRender::run() {
    while (running) {
        float currentTime = SDL_GetTicks() / 1000.0f;
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        if (deltaTime > 0.033f) deltaTime = 0.033f;

        frameCount++;
        if (deltaTime > 0) fps = (int)(1.0f / deltaTime);
        setWindowTitle("3D Engine | FPS: " + std::to_string(fps));

        // Обработка событий
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
                glViewport(0, 0, windowWidth, windowHeight);
            }
            if (event.type == SDL_EVENT_MOUSE_MOTION && g_playerController)
                g_playerController->handleMouseMotion(event.motion.xrel, event.motion.yrel);
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT && g_playerController)
                g_playerController->setMouseLocked(true);
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE && g_playerController)
                g_playerController->setMouseLocked(false);
        }

        // ===== ФИЗИКА =====
        if (ENABLE_GPU_PHYSICS && m_gpuPhysics.isValid()) {
            // 1. Подготавливаем данные для GPU
            std::vector<GPURigidbody> gpuBodies;
            gpuBodies.reserve(sceneObjects.size());

            for (auto& obj : sceneObjects) {
                if (dynamic_cast<Sun*>(obj.get())) continue;
                if (dynamic_cast<CameraObject*>(obj.get())) continue;

                GPURigidbody body;
                body.position = obj->transform.position;
                body.velocity = obj->rigidbody.getVelocity();
                body.acceleration = vec3(0, 0, 0);
                body.mass = obj->rigidbody.getMass();
                body.isStatic = obj->rigidbody.isStatic() ? 1 : 0;
                body.restitution = obj->rigidbody.getRestitution();
                body.friction = obj->rigidbody.getFriction();
                body.inverseMass = (body.mass > 0) ? 1.0f / body.mass : 0.0f;
                body.padding[0] = body.padding[1] = body.padding[2] = 0;

                gpuBodies.push_back(body);
            }

            // 2. Загружаем на GPU
            if (!gpuBodies.empty()) {
                m_gpuPhysics.uploadData(gpuBodies);

                // 3. Запускаем вычисления
                m_gpuPhysics.update(deltaTime, GRAVITY, (int)gpuBodies.size());

                // 4. Скачиваем результаты обратно
                m_gpuPhysics.downloadResults(gpuBodies);

                // 5. Обновляем SceneObject
                int idx = 0;
                for (auto& obj : sceneObjects) {
                    if (dynamic_cast<Sun*>(obj.get())) continue;
                    if (dynamic_cast<CameraObject*>(obj.get())) continue;
                    if (idx < (int)gpuBodies.size()) {
                        obj->transform.position = gpuBodies[idx].position;
                        obj->rigidbody.setVelocity(gpuBodies[idx].velocity);
                    }
                    idx++;
                }

                // Матрицы нужно обновить после изменения позиций
                m_matricesDirty = true;
            }
        }
        else {
            m_physicsWorld.update(deltaTime);
        }

        // Fixed update
        if (m_fixedUpdateCallback) {
            m_accumulator += deltaTime;
            while (m_accumulator >= m_fixedTimestep) {
                m_fixedUpdateCallback(m_fixedTimestep);
                m_accumulator -= m_fixedTimestep;
            }
        }

        // Update объектов
        for (auto& obj : sceneObjects) obj->update(deltaTime);
        if (m_updateCallback) m_updateCallback(deltaTime);

        // Поиск солнца
        Sun* sun = nullptr;
        for (auto& obj : sceneObjects) {
            if (dynamic_cast<Sun*>(obj.get())) {
                sun = static_cast<Sun*>(obj.get());
                break;
            }
        }

        // Загрузка матриц на GPU
        if (ENABLE_GPU_MATRICES) uploadMatricesToGPU();

        // ===== ТЕНЕВОЙ ПРОХОД =====
        if (ENABLE_SHADOWS && sun && shadowMap.isValid()) {
            shadowMap.bindForWriting();
            glClear(GL_DEPTH_BUFFER_BIT);
            glDisable(GL_LIGHTING);
            shadowShader.use();
            shadowShader.setUniform("uLightSpaceMatrix", sun->getLightSpaceMatrix());

            if (ENABLE_GPU_MATRICES) {
                int objIdx = 0;
                for (auto& obj : sceneObjects) {
                    if (dynamic_cast<Sun*>(obj.get())) continue;
                    if (dynamic_cast<CameraObject*>(obj.get())) continue;
                    if (objIdx < (int)m_gpuModels.size()) {
                        shadowShader.setUniform("uModel", m_gpuModels[objIdx]);
                        obj->render();
                    }
                    objIdx++;
                }
            }
            else {
                for (auto& obj : sceneObjects) {
                    if (dynamic_cast<Sun*>(obj.get()) || dynamic_cast<CameraObject*>(obj.get())) continue;
                    shadowShader.setUniform("uModel", obj->transform.getMatrix());
                    obj->render();
                }
            }
            shadowMap.unbind();
            glEnable(GL_LIGHTING);
        }

        // Основной рендер
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_skyTexture.render();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = (float)windowWidth / windowHeight;
        mat4 projection = getCamera().getProjectionMatrix(aspect);
        glMultMatrixf(projection.ptr());

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(getCamera().getViewMatrix().ptr());

        if (ENABLE_SHADOWS && sun) {
            mainShader.use();
            mainShader.setUniform("uLightPos", sun->getPosition());
            mainShader.setUniform("uView", getCamera().getViewMatrix());
            mainShader.setUniform("uProjection", projection);
            mainShader.setUniform("uCameraPos", getCamera().getPosition());

            if (ENABLE_GPU_MATRICES) {
                int idx = 0;
                for (auto& obj : sceneObjects) {
                    if (dynamic_cast<Sun*>(obj.get()) || dynamic_cast<CameraObject*>(obj.get())) continue;
                    mainShader.setUniform("uObjectIndex", idx);
                    obj->render();
                    idx++;
                }
            }
            else {
                for (auto& obj : sceneObjects) {
                    if (dynamic_cast<Sun*>(obj.get()) || dynamic_cast<CameraObject*>(obj.get())) continue;
                    mainShader.setUniform("uModel", obj->transform.getMatrix());
                    obj->render();
                }
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
                if (dynamic_cast<Sun*>(obj.get()) || dynamic_cast<CameraObject*>(obj.get())) continue;
                glPushMatrix();
                glMultMatrixf(obj->transform.getMatrix().ptr());
                obj->render();
                glPopMatrix();
            }
        }

        // Отладка коллайдеров
        if (ENABLE_DEBUG_COLLIDERS) {
            glUseProgram(0);
            glDisable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            for (auto& obj : sceneObjects) {
                if (dynamic_cast<Sun*>(obj.get()) || dynamic_cast<CameraObject*>(obj.get())) continue;
                vec3 min, max;
                obj->collider.getWorldBounds(obj->transform, min, max);
                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                glBegin(GL_LINE_LOOP);
                glVertex3f(min.x, min.y, min.z); glVertex3f(max.x, min.y, min.z);
                glVertex3f(max.x, max.y, min.z); glVertex3f(min.x, max.y, min.z);
                glEnd();
                glBegin(GL_LINE_LOOP);
                glVertex3f(min.x, min.y, max.z); glVertex3f(max.x, min.y, max.z);
                glVertex3f(max.x, max.y, max.z); glVertex3f(min.x, max.y, max.z);
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
    }
}

void StandartOpenGlRender::stop() { running = false; }

void StandartOpenGlRender::setWindowTitle(const std::string& title) {
    if (window) SDL_SetWindowTitle(window, title.c_str());
}