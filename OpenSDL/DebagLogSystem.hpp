#include <cstdio>
#include <cmath>
#include "vec3.hpp"  
#include "vec2.hpp"
#include "mat4.hpp"
#include "quat.hpp"
#include "vec4.hpp"
#include "color.hpp"

bool DebagLog = true;

void INC_Spdl() {
    if (DebagLog) {
        // Тест vec3
        vec3 dub(1, 1, 1);
        printf("> DebagLog: vec3 test: %s\n",
            (dub == vec3(1, 1, 1)) ? "true" : "false");

        // Переменная для отслеживания ошибок
        bool all_mat4_tests_passed = true;

        // Тест 1: Единичная матрица
        mat4 identity;
        bool identity_ok = true;
        // Проверяем, что единичная матрица создалась правильно
        for (int i = 0; i < 16; i++) {
            float expected = (i % 5 == 0) ? 1.0f : 0.0f; // Диагональные элементы = 1
            if (i == 0 || i == 5 || i == 10 || i == 15) expected = 1.0f;
            else expected = 0.0f;

            if (fabs(identity[i] - expected) > 0.001f) {
                identity_ok = false;
                break;
            }
        }

        if (!identity_ok) {
            printf("> DebagLog: Mat4 incealize False\n");
            printf("> DebagLog: Mat4 Error: identity matrix\n");
            all_mat4_tests_passed = false;
        }

        // Тест 2: Матрица перемещения
        mat4 translate = mat4::translation(vec3(5, 2, -3));
        bool translate_ok = (fabs(translate.m[12] - 5.0f) < 0.001f &&
            fabs(translate.m[13] - 2.0f) < 0.001f &&
            fabs(translate.m[14] + 3.0f) < 0.001f); // -3

        if (!translate_ok) {
            printf("> DebagLog: Mat4 incealize False\n");
            printf("> DebagLog: Mat4 Error: translation matrix\n");
            all_mat4_tests_passed = false;
        }

        // Тест 3: Матрица поворота Y
        mat4 rotate = mat4::rotationY(3.14159f / 2);
        bool rotate_ok = (fabs(rotate.m[0] - 0.0f) < 0.001f &&
            fabs(rotate.m[2] + 1.0f) < 0.001f && // -1? Проверяем знак
            fabs(rotate.m[8] - 1.0f) < 0.001f &&
            fabs(rotate.m[10] - 0.0f) < 0.001f);

        // Проверяем на тестовой точке
        vec3 test_point(1, 0, 0);
        vec3 rotated_point = rotate * test_point;
        bool rotate_transform_ok = (fabs(rotated_point.x - 0.0f) < 0.001f &&
            fabs(rotated_point.y - 0.0f) < 0.001f &&
            fabs(rotated_point.z + 1.0f) < 0.001f); // Должно быть (0,0,-1)

        if (!rotate_ok || !rotate_transform_ok) {
            printf("> DebagLog: Mat4 incealize False\n");
            printf("> DebagLog: Mat4 Error: rotationY matrix\n");
            if (!rotate_ok) printf("  - matrix values incorrect\n");
            if (!rotate_transform_ok) printf("  - transform incorrect: got (%.2f,%.2f,%.2f) expected (0,0,-1)\n",
                rotated_point.x, rotated_point.y, rotated_point.z);
            all_mat4_tests_passed = false;
        }

        // Тест 4: Умножение матриц (translate * rotate)
        mat4 combined = translate * rotate;

        // Проверяем комбинированное преобразование
        vec3 transformed = combined * test_point;
        vec3 expected(5, 2, -4); // Поворот потом перемещение

        bool combined_ok = (fabs(transformed.x - expected.x) < 0.01f &&
            fabs(transformed.y - expected.y) < 0.01f &&
            fabs(transformed.z - expected.z) < 0.01f);

        if (!combined_ok) {
            printf("> DebagLog: Mat4 incealize False\n");
            printf("> DebagLog: Mat4 Error: matrix multiplication\n");
            printf("  - got (%.2f,%.2f,%.2f) expected (%.2f,%.2f,%.2f)\n",
                transformed.x, transformed.y, transformed.z,
                expected.x, expected.y, expected.z);
            all_mat4_tests_passed = false;
        }

        // Тест 5: Другой порядок умножения (rotate * translate)
        mat4 combined2 = rotate * translate;
        vec3 transformed2 = combined2 * test_point;
        vec3 expected2(-3, 2, -6); // Перемещение потом поворот

        bool combined2_ok = (fabs(transformed2.x - expected2.x) < 0.01f &&
            fabs(transformed2.y - expected2.y) < 0.01f &&
            fabs(transformed2.z - expected2.z) < 0.01f);

        if (!combined2_ok) {
            printf("> DebagLog: Mat4 incealize False\n");
            printf("> DebagLog: Mat4 Error: matrix multiplication (reverse order)\n");
            printf("  - got (%.2f,%.2f,%.2f) expected (%.2f,%.2f,%.2f)\n",
                transformed2.x, transformed2.y, transformed2.z,
                expected2.x, expected2.y, expected2.z);
            all_mat4_tests_passed = false;
        }

        // Финальный результат
        if (all_mat4_tests_passed) {
            printf("> DebagLog: Mat4 incealize True\n");
        }
        else {
            // Уже вывели ошибки выше
        }

        // Для отладки можно вывести матрицы если нужно
#ifdef DEBUG_PRINT_MATRICES
        if (all_mat4_tests_passed) {
            identity.print("  identity");
            translate.print("  translate");
            rotate.print("  rotate");
            combined.print("  combined (translate*rotate)");
            combined2.print("  combined2 (rotate*translate)");

            printf("> DebagLog: point (1,0,0) tests:\n");
            printf("  rotated: (%.2f, %.2f, %.2f)\n",
                rotated_point.x, rotated_point.y, rotated_point.z);
            printf("  translate*rotate: (%.2f, %.2f, %.2f)\n",
                transformed.x, transformed.y, transformed.z);
            printf("  rotate*translate: (%.2f, %.2f, %.2f)\n",
                transformed2.x, transformed2.y, transformed2.z);
        }
#endif
       // КВАДРИОНЫ
       // ===== ТЕСТ QUAT =====
        bool all_quat_tests_passed = true;

        // Тест 1: Создание единичного кватерниона
        quat q_identity;
        bool q_identity_ok = (fabs(q_identity.x) < 0.001f &&
            fabs(q_identity.y) < 0.001f &&
            fabs(q_identity.z) < 0.001f &&
            fabs(q_identity.w - 1.0f) < 0.001f);

        if (!q_identity_ok) {
            printf("> DebagLog: Quat identity test: false\n");
            all_quat_tests_passed = false;
        }

        // Тест 2: Создание из оси и угла (90° вокруг Y)
        vec3 axis_y(0, 1, 0);
        quat q_rotate = quat::fromAxisAngle(axis_y, 3.14159f / 2);
        vec3 test_point_1(1, 0, 0);  // Переименовал в test_point_1
        vec3 rotated_point_1 = q_rotate * test_point_1;  // Переименовал в rotated_point_1

        bool q_rotate_ok = (fabs(rotated_point_1.x) < 0.001f &&
            fabs(rotated_point_1.y) < 0.001f &&
            fabs(rotated_point_1.z + 1.0f) < 0.001f);

        if (!q_rotate_ok) {
            printf("> DebagLog: Quat rotation test: false\n");
            printf("  - got (%.2f,%.2f,%.2f) expected (0,0,-1)\n",
                rotated_point_1.x, rotated_point_1.y, rotated_point_1.z);
            all_quat_tests_passed = false;
        }

        // Тест 3: Нормализация
        quat q_not_normalized(2, 0, 0, 0);
        quat q_normalized = q_not_normalized.normalize();
        bool q_normalize_ok = (fabs(q_normalized.length() - 1.0f) < 0.001f);

        if (!q_normalize_ok) {
            printf("> DebagLog: Quat normalize test: false\n");
            all_quat_tests_passed = false;
        }

        // Тест 4: Умножение кватернионов
        quat q_x = quat::fromAxisAngle(vec3(1, 0, 0), 3.14159f / 2);
        quat q_y = quat::fromAxisAngle(vec3(0, 1, 0), 3.14159f / 2);
        quat q_combined = q_y * q_x;

        vec3 test_point_2(0, 0, 1);  // Новое имя
        vec3 combined_rotated = q_combined * test_point_2;

        bool q_multiply_ok = (fabs(combined_rotated.x) < 0.001f &&
            fabs(combined_rotated.y + 1.0f) < 0.001f &&
            fabs(combined_rotated.z) < 0.001f);

        if (!q_multiply_ok) {
            printf("> DebagLog: Quat multiplication test: false\n");
            all_quat_tests_passed = false;
        }

        // Тест 5: SLERP интерполяция
        quat q_start = quat::fromAxisAngle(vec3(0, 1, 0), 0);           // 0°
        quat q_end = quat::fromAxisAngle(vec3(0, 1, 0), 3.14159f);      // 180°
        quat q_mid = q_start.slerp(q_end, 0.5f);                        // Должно быть 90°

        vec3 test_point_3(0, 0, 1);
        vec3 mid_rotated = q_mid * test_point_3;

        // Для 90° вокруг Y: (0,0,1) должно стать (1,0,0) или (-1,0,0)?
        // Проверим оба варианта:
        bool q_slerp_ok = (fabs(mid_rotated.x - 1.0f) < 0.001f &&    // (1,0,0)
            fabs(mid_rotated.y) < 0.001f &&
            fabs(mid_rotated.z) < 0.001f) ||
            (fabs(mid_rotated.x + 1.0f) < 0.001f &&    // (-1,0,0)
                fabs(mid_rotated.y) < 0.001f &&
                fabs(mid_rotated.z) < 0.001f);

        if (!q_slerp_ok) {
            printf("> DebagLog: Quat slerp test: false\n");
            printf("  - got (%.2f,%.2f,%.2f) expected (1,0,0) or (-1,0,0)\n",
                mid_rotated.x, mid_rotated.y, mid_rotated.z);
            all_quat_tests_passed = false;
        }

        // Тест 6: Конвертация в матрицу
        mat4 q_matrix = q_rotate.toMatrix();
        vec3 matrix_rotated = q_matrix * test_point_1;  // Использую test_point_1
        bool q_to_matrix_ok = (fabs(matrix_rotated.x - rotated_point_1.x) < 0.001f &&
            fabs(matrix_rotated.y - rotated_point_1.y) < 0.001f &&
            fabs(matrix_rotated.z - rotated_point_1.z) < 0.001f);

        if (!q_to_matrix_ok) {
            printf("> DebagLog: Quat to matrix test: false\n");
            all_quat_tests_passed = false;
        }

        // Финальный результат
        printf("> DebagLog: quat test: %s\n", all_quat_tests_passed ? "true" : "false");

        // ===== ТЕСТ VEC2 =====
        bool all_vec2_tests_passed = true;

        // Тест 1: Конструкторы
        vec2 v1;
        vec2 v2(3, 4);
        vec2 v3 = vec2::zero();
        vec2 v4 = vec2::one();

        bool vec2_constructor_ok = (v1.x == 0 && v1.y == 0 &&
            v2.x == 3 && v2.y == 4 &&
            v3.x == 0 && v3.y == 0 &&
            v4.x == 1 && v4.y == 1);

        if (!vec2_constructor_ok) {
            printf("> DebagLog: vec2 constructor test: false\n");
            all_vec2_tests_passed = false;
        }

        // Тест 2: Операторы
        vec2 a(2, 3);
        vec2 b(4, 5);

        vec2 sum = a + b;
        vec2 diff = a - b;
        vec2 mul = a * 2;
        vec2 div = a / 2;

        bool vec2_operators_ok = (sum == vec2(6, 8) &&
            diff == vec2(-2, -2) &&
            mul == vec2(4, 6) &&
            div == vec2(1, 1.5f));

        if (!vec2_operators_ok) {
            printf("> DebagLog: vec2 operators test: false\n");
            all_vec2_tests_passed = false;
        }

        // Тест 3: Длина
        vec2 v(3, 4);
        float len = v.length();
        float lenSq = v.lengthSq();

        bool vec2_length_ok = (fabs(len - 5.0f) < 0.001f &&
            fabs(lenSq - 25.0f) < 0.001f);

        if (!vec2_length_ok) {
            printf("> DebagLog: vec2 length test: false\n");
            all_vec2_tests_passed = false;
        }

        // Тест 4: Нормализация
        vec2 norm = v.normalize();
        bool vec2_normalize_ok = (fabs(norm.x - 0.6f) < 0.001f &&
            fabs(norm.y - 0.8f) < 0.001f &&
            fabs(norm.length() - 1.0f) < 0.001f);

        if (!vec2_normalize_ok) {
            printf("> DebagLog: vec2 normalize test: false\n");
            all_vec2_tests_passed = false;
        }

        // Тест 5: Скалярное произведение
        float dot = a.dot(b);
        bool vec2_dot_ok = (fabs(dot - 23.0f) < 0.001f);

        if (!vec2_dot_ok) {
            printf("> DebagLog: vec2 dot test: false\n");
            all_vec2_tests_passed = false;
        }

        // Тест 6: Поворот
        vec2 original(1, 0);
        vec2 rotated = original.rotate(3.14159f / 2); // 90°

        bool vec2_rotate_ok = (fabs(rotated.x) < 0.001f &&
            fabs(rotated.y - 1.0f) < 0.001f);

        if (!vec2_rotate_ok) {
            printf("> DebagLog: vec2 rotate test: false\n");
            printf("  - got (%.2f,%.2f) expected (0,1)\n",
                rotated.x, rotated.y);
            all_vec2_tests_passed = false;
        }

        // Тест 7: Угол
        float angle = original.angle();
        bool vec2_angle_ok = (fabs(angle) < 0.001f);

        if (!vec2_angle_ok) {
            printf("> DebagLog: vec2 angle test: false\n");
            all_vec2_tests_passed = false;
        }

        // Финальный результат
        printf("> DebagLog: vec2 test: %s\n", all_vec2_tests_passed ? "true" : "false");

        // ===== ТЕСТ VEC4 =====
        {
            bool all_vec4_tests_passed = true;

            // Конструкторы
            vec4 vec4_v1;
            vec4 vec4_v2(1, 2, 3, 4);
            vec3 vec4_v3d(5, 6, 7);
            vec4 vec4_v3(vec4_v3d, 8);
            vec4 vec4_v4(vec4_v3d); // w = 1

            bool vec4_constructor_ok = (vec4_v1.x == 0 && vec4_v1.y == 0 && vec4_v1.z == 0 && vec4_v1.w == 0 &&
                vec4_v2.x == 1 && vec4_v2.y == 2 && vec4_v2.z == 3 && vec4_v2.w == 4 &&
                vec4_v3.x == 5 && vec4_v3.y == 6 && vec4_v3.z == 7 && vec4_v3.w == 8 &&
                vec4_v4.x == 5 && vec4_v4.y == 6 && vec4_v4.z == 7 && vec4_v4.w == 1);

            if (!vec4_constructor_ok) {
                printf("> DebagLog: vec4 constructor test: false\n");
                all_vec4_tests_passed = false;
            }

            // Операторы
            vec4 vec4_a(1, 2, 3, 4);
            vec4 vec4_b(5, 6, 7, 8);

            bool vec4_operators_ok = ((vec4_a + vec4_b) == vec4(6, 8, 10, 12) &&
                (vec4_a - vec4_b) == vec4(-4, -4, -4, -4) &&
                (vec4_a * 2) == vec4(2, 4, 6, 8) &&
                (vec4_a / 2) == vec4(0.5f, 1, 1.5f, 2));

            if (!vec4_operators_ok) {
                printf("> DebagLog: vec4 operators test: false\n");
                all_vec4_tests_passed = false;
            }

            // Длина
            float vec4_len = vec4_v2.length();
            float vec4_lenSq = vec4_v2.lengthSq();
            bool vec4_length_ok = (fabs(vec4_len - 5.477f) < 0.001f &&
                fabs(vec4_lenSq - 30.0f) < 0.001f);

            if (!vec4_length_ok) {
                printf("> DebagLog: vec4 length test: false\n");
                all_vec4_tests_passed = false;
            }

            // Проекция
            vec4 clip(10, 20, 30, 5);
            vec3 projected = clip.project();
            bool vec4_project_ok = (fabs(projected.x - 2.0f) < 0.001f &&
                fabs(projected.y - 4.0f) < 0.001f &&
                fabs(projected.z - 6.0f) < 0.001f);

            if (!vec4_project_ok) {
                printf("> DebagLog: vec4 project test: false\n");
                all_vec4_tests_passed = false;
            }

            // toVec3
            vec3 fromVec4 = vec4_v2.toVec3();
            bool vec4_toVec3_ok = (fromVec4.x == 1 && fromVec4.y == 2 && fromVec4.z == 3);

            if (!vec4_toVec3_ok) {
                printf("> DebagLog: vec4 toVec3 test: false\n");
                all_vec4_tests_passed = false;
            }

            printf("> DebagLog: vec4 test: %s\n", all_vec4_tests_passed ? "true" : "false");
        }

        // ===== ТЕСТ COLOR =====
        {
            bool all_color_tests_passed = true;

            // Из RGB 0-255
            Color c1 = Color::fromRGB(255, 0, 0);
            bool color_rgb_ok = (fabs(c1.r - 1.0f) < 0.001f &&
                fabs(c1.g - 0.0f) < 0.001f &&
                fabs(c1.b - 0.0f) < 0.001f &&
                fabs(c1.a - 1.0f) < 0.001f);

            if (!color_rgb_ok) {
                printf("> DebagLog: Color fromRGB test: false\n");
                all_color_tests_passed = false;
            }

            // Из HEX
            Color c2 = Color::fromHEX(0x00FF00);
            bool color_hex_ok = (fabs(c2.r - 0.0f) < 0.001f &&
                fabs(c2.g - 1.0f) < 0.001f &&
                fabs(c2.b - 0.0f) < 0.001f &&
                fabs(c2.a - 1.0f) < 0.001f);

            if (!color_hex_ok) {
                printf("> DebagLog: Color fromHEX test: false\n");
                all_color_tests_passed = false;
            }

            // Предопределённые цвета
            Color c3 = Color::red();
            Color c4 = Color::blue();
            bool color_const_ok = (c3.r == 1 && c3.g == 0 && c3.b == 0 &&
                c4.r == 0 && c4.g == 0 && c4.b == 1);

            if (!color_const_ok) {
                printf("> DebagLog: Color constants test: false\n");
                all_color_tests_passed = false;
            }

            // Смешивание
            Color c5 = Color::red().mix(Color::blue(), 0.5f);
            bool color_mix_ok = (fabs(c5.r - 0.5f) < 0.001f &&
                fabs(c5.g - 0.0f) < 0.001f &&
                fabs(c5.b - 0.5f) < 0.001f &&
                fabs(c5.a - 1.0f) < 0.001f);

            if (!color_mix_ok) {
                printf("> DebagLog: Color mix test: false\n");
                all_color_tests_passed = false;
            }

            // Яркость
            float lum = Color::red().luminance();
            bool color_lum_ok = (fabs(lum - 0.2126f) < 0.001f);

            if (!color_lum_ok) {
                printf("> DebagLog: Color luminance test: false\n");
                all_color_tests_passed = false;
            }

            printf("> DebagLog: Color test: %s\n", all_color_tests_passed ? "true" : "false");
        }
    }
}