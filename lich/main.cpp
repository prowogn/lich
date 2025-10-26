#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <cmath>
#include <memory>
#include <vector>
#include <iostream>
#include <string>

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "Point3D.h"
#include "Curve3D.h"
#include "Helix.h"
#include "Circle.h"
#include "Ellipse.h"

// ───────────────────────────────
// Конвертеры между Point3D и Vector3
// ───────────────────────────────
static Vector3 ToVec3(const Point3D& p)
{
    return { (float)p.x, (float)p.y, (float)p.z };
}

static Point3D ToPoint3D(const Vector3& v)
{
    return { v.x, v.y, v.z };
}

// ───────────────────────────────
// Камера от 1-го лица (свободное движение)
// ───────────────────────────────
void UpdateFreeCamera(Camera3D& camera, float deltaTime, float sensitivity = 0.0035f, float speed = 5.0f)
{
    Vector2 md = GetMouseDelta();

    Vector3 dir = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(dir, { 0.0f, 1.0f, 0.0f }));

    float yaw = -md.x * sensitivity;
    dir = Vector3RotateByAxisAngle(dir, { 0.0f, 1.0f, 0.0f }, yaw);

    float pitch = -md.y * sensitivity;
    dir = Vector3RotateByAxisAngle(dir, right, pitch);

    camera.target = Vector3Add(camera.position, dir);

    Vector3 move = { 0.0f, 0.0f, 0.0f };
    if (IsKeyDown(KEY_W)) move = Vector3Add(move, Vector3Scale(dir, speed * deltaTime));
    if (IsKeyDown(KEY_S)) move = Vector3Add(move, Vector3Scale(dir, -speed * deltaTime));
    if (IsKeyDown(KEY_D)) move = Vector3Add(move, Vector3Scale(right, speed * deltaTime));
    if (IsKeyDown(KEY_A)) move = Vector3Add(move, Vector3Scale(right, -speed * deltaTime));
    if (IsKeyDown(KEY_SPACE)) move = Vector3Add(move, { 0.0f, speed * deltaTime, 0.0f });
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
        move = Vector3Add(move, { 0.0f, -speed * deltaTime, 0.0f });

    camera.position = Vector3Add(camera.position, move);
    camera.target = Vector3Add(camera.target, move);
}

// ───────────────────────────────
// Отрисовка кривой
// ───────────────────────────────
void DrawCurve3D(const std::shared_ptr<Curve3D>& curve, int segments = 200, Color col = BLUE)
{
    if (!curve) return;
    double t0 = 0.0;
    double t1 = 2.0 * M_PI;
    Point3D prev = curve->getPoint(t0);
    for (int i = 1; i <= segments; ++i)
    {
        double tt = t0 + (t1 - t0) * ((double)i / segments);
        Point3D p = curve->getPoint(tt);
        DrawLine3D(ToVec3(prev), ToVec3(p), col);
        prev = p;
    }
}

// ───────────────────────────────
// Основная функция
// ───────────────────────────────
int main()
{
    const int screenW = 1280, screenH = 720;
    InitWindow(screenW, screenH, "3D Curves Editor (Raylib + RayGUI)");
    SetTargetFPS(60);
    DisableCursor(); // стартуем в режиме камеры

    // Камера
    Camera3D camera;
    camera.position = { 6.0f, 4.0f, 6.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Контейнер кривых
    std::vector<std::shared_ptr<Curve3D>> curves;

    // GUI-переменные
    int selectedCurve = -1;
    bool panelVisible = true;
    bool guiMode = false; // по умолчанию - режим камеры

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Переключение между GUI и камерой
        if (IsKeyPressed(KEY_TAB))
        {
            guiMode = !guiMode;
            if (guiMode)
                EnableCursor();
            else
                DisableCursor();
        }

        // Камеру двигаем только если GUI выключен
        if (!guiMode)
            UpdateFreeCamera(camera, dt, 0.0045f, 6.0f);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawGrid(20, 1.0f);

        // Рисуем все кривые
        for (int i = 0; i < curves.size(); ++i)
        {
            auto& c = curves[i];
            Color col = (i == selectedCurve) ? ORANGE : GRAY;
            if (std::dynamic_pointer_cast<Circle3D>(c)) col = RED;
            if (std::dynamic_pointer_cast<Ellipse3D>(c)) col = GREEN;
            if (std::dynamic_pointer_cast<Helix3D>(c)) col = BLUE;
            DrawCurve3D(c, 300, col);
        }

        EndMode3D();

        // === GUI ===
        if (guiMode && panelVisible)
        {
            Rectangle panel = { 960, 0, 320, (float)screenH };
            GuiPanel(panel, "Curve Editor");

            // Добавление
            if (GuiButton({ 980, 40, 120, 30 }, "Add Circle"))
                curves.push_back(std::make_shared<Circle3D>(1.0));
            if (GuiButton({ 980, 80, 120, 30 }, "Add Ellipse"))
                curves.push_back(std::make_shared<Ellipse3D>(1.2, 0.7));
            if (GuiButton({ 980, 120, 120, 30 }, "Add Helix"))
                curves.push_back(std::make_shared<Helix3D>(1.0, 0.4));

            // Список кривых
            DrawText("Curves:", 980, 170, 20, DARKGRAY);
            for (int i = 0; i < curves.size(); ++i)
            {
                std::string label = std::to_string(i + 1) + ". ";
                if (std::dynamic_pointer_cast<Circle3D>(curves[i])) label += "Circle";
                else if (std::dynamic_pointer_cast<Ellipse3D>(curves[i])) label += "Ellipse";
                else if (std::dynamic_pointer_cast<Helix3D>(curves[i])) label += "Helix";

                if (GuiButton({ 980, 200.0f + i * 40, 140, 30 }, label.c_str()))
                    selectedCurve = i;
            }

            // Удаление выбранной
            if (selectedCurve >= 0 && selectedCurve < (int)curves.size())
            {
                if (GuiButton({ 1120, 40, 120, 30 }, "Delete"))
                {
                    curves.erase(curves.begin() + selectedCurve);
                    selectedCurve = -1;
                }
            }
        }

        DrawText("WASD + Mouse = Move camera", 10, 10, 18, DARKGRAY);
        DrawText("SPACE/Ctrl = Up/Down", 10, 30, 18, DARKGRAY);
        DrawText("[TAB] Switch GUI/Camera mode", 10, 50, 18, DARKGRAY);
        DrawText("[P] Toggle panel", 10, 70, 18, DARKGRAY);

        if (IsKeyPressed(KEY_P)) panelVisible = !panelVisible;

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
