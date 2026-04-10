# ⚙️ Настройки движка OpenSDL

Все настройки находятся в файле **StandartOpenGlRender.hpp** в публичных переменных класса `StandartOpenGlRender`.

## 🎨 Графика

- `ENABLE_SHADOWS = true` — Включить карты теней (на слабых ПК выключить)
- `ENABLE_VSYNC = false` — Вертикальная синхронизация
- `ENABLE_DEBUG_COLLIDERS = false` — Рисовать зелёные рамки коллайдеров
- `ENABLE_GPU_MATRICES = true` — Матрицы на GPU (требует OpenGL 4.3+)
- `MAX_GPU_OBJECTS = 5000` — Максимум объектов для GPU-матриц
- `WORLD_CLEAR_COLOR = (0.5, 0.7, 1.0)` — Цвет неба (RGB)

## ⚙️ Физика

- `ENABLE_GPU_PHYSICS = false` — Физика на GPU (экспериментально, требует OpenGL 4.3+)
- `MAX_PHYSICS_OBJECTS = 5000` — Максимум объектов для GPU-физики
- `GRAVITY = (0, -9.8, 0)` — Вектор гравитации (м/с²)

> При `ENABLE_GPU_PHYSICS = false` используется CPU-физика (стабильнее на школьных ПК).

## 🎮 Управление

- `PLAYER_MOVE_SPEED = 8.0` — Скорость движения (м/с)
- `PLAYER_MOUSE_SENS = 0.005` — Чувствительность мыши
- `PLAYER_JUMP_POWER = 5.5` — Сила прыжка (м/с)
- `PLAYER_GRAVITY = 12.0` — Гравитация игрока (м/с²)
- `INVERT_X = true` — Инвертировать мышь по горизонтали
- `INVERT_Y = true` — Инвертировать мышь по вертикали

## 📷 Камера

- `CAMERA_FOV = 80.0` — Поле зрения (градусы)
- `CAMERA_NEAR = 0.1` — Ближняя плоскость отсечения
- `CAMERA_FAR = 1000.0` — Дальняя плоскость отсечения
- `CAMERA_START_POS = (0, 3, 15)` — Стартовая позиция камеры

## 💡 Как изменить

В `Main.cpp` перед `render.init()`:

```cpp
StandartOpenGlRender render;
render.ENABLE_SHADOWS = false;    // отключить тени
render.PLAYER_MOVE_SPEED = 15.0f; // быстрее бегать
render.init(800, 600, "My Game");