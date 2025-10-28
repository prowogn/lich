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

    // Окна
    bool showAddWindow = false;
    bool showEditWindow = false;

    // Переменные для добавления/редактирования
    int curveType = 0; // 0=Circle, 1=Ellipse, 2=Helix
    char circleRadius[32] = "1.0";
    char ellipseA[32] = "1.2";
    char ellipseB[32] = "0.7";
    char helixRadius[32] = "1.0";
    char helixStep[32] = "0.4";
    char helixTurns[32] = "5";

    // Флаги для текстовых полей
    bool editCircleRadius = false;
    bool editEllipseA = false;
    bool editEllipseB = false;
    bool editHelixRadius = false;
    bool editHelixStep = false;
    bool editHelixTurns = false;

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

            // Основные кнопки
            if (GuiButton({ 980, 40, 120, 30 }, "Add Curve"))
                showAddWindow = true;

            if (GuiButton({ 1120, 40, 120, 30 }, "Edit Curve") && selectedCurve >= 0)
            {
                showEditWindow = true;
                // Загружаем данные выбранной кривой
                if (auto circle = std::dynamic_pointer_cast<Circle3D>(curves[selectedCurve]))
                {
                    curveType = 0;
                    snprintf(circleRadius, 32, "%.2f", circle->getRadius());
                }
                else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[selectedCurve]))
                {
                    curveType = 1;
                    snprintf(ellipseA, 32, "%.2f", ellipse->getA());
                    snprintf(ellipseB, 32, "%.2f", ellipse->getB());
                }
                else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[selectedCurve]))
                {
                    curveType = 2;
                    snprintf(helixRadius, 32, "%.2f", helix->getRadius());
                    snprintf(helixStep, 32, "%.2f", helix->getStep());
                    snprintf(helixTurns, 32, "%d", helix->getTurns());
                }
            }

            if (GuiButton({ 1120, 80, 120, 30 }, "Delete") && selectedCurve >= 0)
            {
                curves.erase(curves.begin() + selectedCurve);
                selectedCurve = -1;
            }

            // Список кривых
            DrawText("Curves:", 980, 120, 20, DARKGRAY);
            for (int i = 0; i < curves.size(); ++i)
            {
                std::string label = std::to_string(i + 1) + ". ";
                if (std::dynamic_pointer_cast<Circle3D>(curves[i])) label += "Circle";
                else if (std::dynamic_pointer_cast<Ellipse3D>(curves[i])) label += "Ellipse";
                else if (std::dynamic_pointer_cast<Helix3D>(curves[i])) label += "Helix";

                if (GuiButton({ 980, 150.0f + i * 40, 140, 30 }, label.c_str()))
                    selectedCurve = i;
            }
        }

        // Окно добавления кривой
        if (showAddWindow)
        {
            Rectangle addWindow = { screenW / 2 - 200, screenH / 2 - 200, 400, 300 };
            GuiWindowBox(addWindow, "Add New Curve");

            // Выбор типа кривой
            GuiLabel({ addWindow.x + 20, addWindow.y + 40, 100, 25 }, "Curve Type:");
            GuiComboBox({ addWindow.x + 120, addWindow.y + 40, 200, 25 }, "Circle;Ellipse;Helix", &curveType);

            // Параметры в зависимости от типа
            if (curveType == 0) // Circle
            {
                GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "Radius:");
                GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, circleRadius, 32, editCircleRadius);
            }
            else if (curveType == 1) // Ellipse
            {
                GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "A:");
                GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, ellipseA, 32, editEllipseA);
                GuiLabel({ addWindow.x + 20, addWindow.y + 120, 100, 25 }, "B:");
                GuiTextBox({ addWindow.x + 120, addWindow.y + 120, 200, 25 }, ellipseB, 32, editEllipseB);
            }
            else if (curveType == 2) // Helix
            {
                GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "Radius:");
                GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, helixRadius, 32, editHelixRadius);
                GuiLabel({ addWindow.x + 20, addWindow.y + 120, 100, 25 }, "Step:");
                GuiTextBox({ addWindow.x + 120, addWindow.y + 120, 200, 25 }, helixStep, 32, editHelixStep);
                GuiLabel({ addWindow.x + 20, addWindow.y + 160, 100, 25 }, "Turns:");
                GuiTextBox({ addWindow.x + 120, addWindow.y + 160, 200, 25 }, helixTurns, 32, editHelixTurns);
            }

            // Кнопки OK/Cancel
            if (GuiButton({ addWindow.x + 100, addWindow.y + 250, 80, 30 }, "OK"))
            {
                try {
                    if (curveType == 0) // Circle
                    {
                        double radius = std::stod(circleRadius);
                        curves.push_back(std::make_shared<Circle3D>(radius));
                    }
                    else if (curveType == 1) // Ellipse
                    {
                        double a = std::stod(ellipseA);
                        double b = std::stod(ellipseB);
                        curves.push_back(std::make_shared<Ellipse3D>(a, b));
                    }
                    else if (curveType == 2) // Helix
                    {
                        double radius = std::stod(helixRadius);
                        double step = std::stod(helixStep);
                        int turns = std::stoi(helixTurns);
                        curves.push_back(std::make_shared<Helix3D>(radius, step, turns));
                    }
                    showAddWindow = false;
                }
                catch (const std::exception& e) {
                    // В случае ошибки ввода просто закрываем окно
                    showAddWindow = false;
                }
            }

            if (GuiButton({ addWindow.x + 220, addWindow.y + 250, 80, 30 }, "Cancel"))
                showAddWindow = false;
        }

        // Окно редактирования кривой
        if (showEditWindow && selectedCurve >= 0)
        {
            Rectangle editWindow = { screenW / 2 - 200, screenH / 2 - 200, 400, 300 };
            GuiWindowBox(editWindow, "Edit Curve");

            // Отображаем тип текущей кривой
            std::string typeLabel = "Type: ";
            if (auto circle = std::dynamic_pointer_cast<Circle3D>(curves[selectedCurve])) typeLabel += "Circle";
            else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[selectedCurve])) typeLabel += "Ellipse";
            else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[selectedCurve])) typeLabel += "Helix";

            GuiLabel({ editWindow.x + 20, editWindow.y + 40, 360, 25 }, typeLabel.c_str());

            // Параметры в зависимости от типа
            if (auto circle = std::dynamic_pointer_cast<Circle3D>(curves[selectedCurve]))
            {
                GuiLabel({ editWindow.x + 20, editWindow.y + 80, 100, 25 }, "Radius:");
                GuiTextBox({ editWindow.x + 120, editWindow.y + 80, 200, 25 }, circleRadius, 32, editCircleRadius);
            }
            else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[selectedCurve]))
            {
                GuiLabel({ editWindow.x + 20, editWindow.y + 80, 100, 25 }, "A:");
                GuiTextBox({ editWindow.x + 120, editWindow.y + 80, 200, 25 }, ellipseA, 32, editEllipseA);
                GuiLabel({ editWindow.x + 20, editWindow.y + 120, 100, 25 }, "B:");
                GuiTextBox({ editWindow.x + 120, editWindow.y + 120, 200, 25 }, ellipseB, 32, editEllipseB);
            }
            else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[selectedCurve]))
            {
                GuiLabel({ editWindow.x + 20, editWindow.y + 80, 100, 25 }, "Radius:");
                GuiTextBox({ editWindow.x + 120, editWindow.y + 80, 200, 25 }, helixRadius, 32, editHelixRadius);
                GuiLabel({ editWindow.x + 20, editWindow.y + 120, 100, 25 }, "Step:");
                GuiTextBox({ editWindow.x + 120, editWindow.y + 120, 200, 25 }, helixStep, 32, editHelixStep);
                GuiLabel({ editWindow.x + 20, editWindow.y + 160, 100, 25 }, "Turns:");
                GuiTextBox({ editWindow.x + 120, editWindow.y + 160, 200, 25 }, helixTurns, 32, editHelixTurns);
            }

            // Кнопки Save/Cancel
            if (GuiButton({ editWindow.x + 100, editWindow.y + 250, 80, 30 }, "Save"))
            {
                try {
                    if (auto circle = std::dynamic_pointer_cast<Circle3D>(curves[selectedCurve]))
                    {
                        double radius = std::stod(circleRadius);
                        curves[selectedCurve] = std::make_shared<Circle3D>(radius);
                    }
                    else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[selectedCurve]))
                    {
                        double a = std::stod(ellipseA);
                        double b = std::stod(ellipseB);
                        curves[selectedCurve] = std::make_shared<Ellipse3D>(a, b);
                    }
                    else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[selectedCurve]))
                    {
                        double radius = std::stod(helixRadius);
                        double step = std::stod(helixStep);
                        int turns = std::stoi(helixTurns);
                        helix->setRadius(radius);
                        helix->setStep(step);
                        helix->setTurns(turns);
                    }
                    showEditWindow = false;
                }
                catch (const std::exception& e) {
                    // В случае ошибки ввода просто закрываем окно
                    showEditWindow = false;
                }
            }

            if (GuiButton({ editWindow.x + 220, editWindow.y + 250, 80, 30 }, "Cancel"))
                showEditWindow = false;
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