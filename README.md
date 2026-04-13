# 🎮 OpenSDL Game Engine

**Лёгкий 3D-движок на C++ и OpenGL**  
Создан для авиасимулятора и оптимизирован для работы на школьных ПК

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17%2F20-blue)](https://isocpp.org)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3%2F4.3-red)](https://opengl.org)
[![SDL3](https://img.shields.io/badge/SDL3-3.4.2-orange)](https://github.com/libsdl-org/SDL)
[![GLEW](https://img.shields.io/badge/GLEW-2.2.0-green)](https://glew.sourceforge.net)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078d7)](https://microsoft.com)
[![Visual Studio](https://img.shields.io/badge/VS-2026-purple)](https://visualstudio.microsoft.com)

---

## ✨ Возможности

### 🎨 Графика
- OpenGL 3.3 / 4.3 (автоматический выбор)
- GPU-матрицы (SSBO) — до 10 000 объектов
- Phong освещение + specular блики
- Мягкие тени (PCF 2x2 / 3x3)
- Видимое солнце (сфера)
- Текстурное небо (SkyTexture)

### ⚙️ Физика
- **CPU-физика** — стабильная, для школьных ПК
- **GPU-физика** — compute shader, для мощных ПК
- Гравитация, прыжки, коллизии AABB
- Инерция и покачивание камеры

### 🔊 Звук
- XAudio2 (WAV-файлы)
- Поддержка петель и громкости

---

## 🚀 Быстрый старт

### Как это работает

1. Движок инициализирует OpenGL контекст (3.3 или 4.3)
2. Загружает демо-сцену: плоскость + сетка из 81 куба
3. Запускает физику (CPU по умолчанию) и рендер
4. Управление камерой через WASD + мышь

### Управление

**Меню управления:**
- `WASD` — движение
- `Мышь` — поворот камеры
- `Пробел` — прыжок
- `ПКМ` — захватить мышь
- `ESC` — отпустить мышь

### Настройка производительности

Если на слабом ПК низкий FPS:
1. Открой `StandartOpenGlRender.hpp`
2. Выключи `ENABLE_SHADOWS`
3. Выключи `ENABLE_VSYNC`
4. Используй `CPU-физику` (GPU отключена по умолчанию)

---

## 🔧 Системные требования и сборка

### Зависимости
Для сборки проекта необходимы следующие компоненты:

- **Visual Studio 2022** (с установленной нагрузкой для разработки на C++ Desktop)
- **SDL3 3.4.2** (библиотека для окна, ввода и аудио) — [Скачать](https://github.com/libsdl-org/SDL/releases/tag/release-3.4.2)
- **GLEW 2.2.0** (библиотека для загрузки OpenGL расширений) — [Скачать](https://glew.sourceforge.net)
- **OpenGL** (версия 3.3 или выше, драйвер поддерживает)

### Настройка окружения
1. Распакуйте SDL3 и GLEW в папку `Libs/` в корне проекта.
2. Откройте решение `OpenSDL.sln` в Visual Studio 2022.
3. Выберите конфигурацию `Release | x64` и нажмите `Ctrl+Shift+B` для сборки.

### Шаги сборки

1. Открой `OpenSDL.sln`
2. Выбери `Release | x64`
3. Нажми `Ctrl+Shift+B`
4. Запусти `Ctrl+F5`

### Windows 7 / слабые ПК

Для старых ПК используйте конфигурацию `Release | x86` и отключите тени в настройках.

---

## ⚙️ Аргументы конфигурации

Все настройки в файле `StandartOpenGlRender.hpp`.

| Переменная | Описание | По умолчанию |
|------------|-----------|---------------|
| `ENABLE_SHADOWS` | Мягкие тени (PCF) | `true` |
| `ENABLE_VSYNC` | Вертикальная синхронизация | `false` |
| `ENABLE_GPU_MATRICES` | GPU-матрицы (требуется OpenGL 4.3+) | `true` |
| `ENABLE_GPU_PHYSICS` | GPU-физика (экспериментально) | `false` |
| `MAX_GPU_OBJECTS` | Максимум объектов | `5000` |
| `GRAVITY` | Вектор гравитации | `(0, -9.8, 0)` |
| `PLAYER_MOVE_SPEED` | Скорость игрока | `5.0` |
| `PLAYER_JUMP_POWER` | Сила прыжка | `5.0` |
| `CAMERA_FOV` | Поле зрения | `75.0` |

Подробнее — в файле `Settings.md`.

### Пример настройки под школьный ПК

```cpp
#define ENABLE_SHADOWS false
#define ENABLE_VSYNC false
#define ENABLE_GPU_MATRICES false
#define MAX_GPU_OBJECTS 1000
```
## Производительность

Тестировалось на системе: NVIDIA RTX 4060, AMD Ryzen 5 5600X, 16 ГБ ОЗУ

| Сцена | FPS |
|-------|-----|
| Простые сцены | 1000+ |
| 2 500 кубов + физика | 60–80 |
| 10 000 кубов (без физики) | ~30 |

**Не хватает FPS?**

Отключи тени и включи VSYNC. Если не помогло, используй CPU-физику и уменьши MAX_GPU_OBJECTS до 1000.

---

## Планы на будущее

- Аэродинамика самолёта
- Камера от 3-го лица
- Ландшафт (генерация высот)
- Фрустум каллинг
- LOD (Level of Detail)
- PBR (Physically Based Rendering)

---

## Лицензия

MIT License

---

## Автор

glooobus1 — энтузиаст

---

Движок создан для авиасимулятора и оптимизирован для школьных ПК (уровня CS 1.6)
