#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <cmath>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <random>
#include <algorithm>

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "Point3D.h"
#include "Curve3D.h"
#include "Helix.h"
#include "Circle.h"
#include "Ellipse.h"

#include "drawing.h"
#include "gui.h"
#include "tasks.h"

int main()
{
    const int screenW = 1280, screenH = 720;
    InitWindow(screenW, screenH, "3D Curves Editor (Raylib + RayGUI)");
    SetTargetFPS(60);

    // Инициализация камеры
    Camera3D camera = { 0 };
    camera.position = Vector3{ 10.0f, 10.0f, 10.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Контейнер кривых и состояние приложения
    AppState state;
    InitializeAppState(state);

    // Режим камеры
    int cameraMode = CAMERA_FREE;
    DisableCursor();

    while (!WindowShouldClose())
    {
        // Управление камерой
        if (!state.guiMode)
        {
            UpdateCamera(&camera, cameraMode);
        }

        // Переключение между GUI и камерой
        if (IsKeyPressed(KEY_TAB))
        {
            state.guiMode = !state.guiMode;
            if (state.guiMode)
            {
                EnableCursor();
                cameraMode = CAMERA_CUSTOM;
            }
            else
            {
                DisableCursor();
                cameraMode = CAMERA_FREE;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Отрисовка 3D сцены
        BeginMode3D(camera);
        DrawGrid(20, 1.0f);
        DrawAllCurves(state.curves, state.selectedCurve, state.currentPoint, state.currentDerivative, state.calculated);
        EndMode3D();

        // Отрисовка GUI
        if (state.guiMode && state.panelVisible)
        {
            DrawMainPanel(state);
        }

        // Обработка окон
        HandleAddWindow(state);
        HandleEditWindow(state);
        HandleTaskWindow(state);

        // Отрисовка подсказок управления
        DrawText("Mouse wheel = Zoom in/out", 10, 10, 18, DARKGRAY);
        DrawText("Mouse right button = Rotate", 10, 35, 18, DARKGRAY);
        DrawText("TAB = Toggle GUI mode", 10, 60, 18, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}