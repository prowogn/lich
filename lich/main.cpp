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
// Функции для задания
// ───────────────────────────────
void Task1_GenerateRandomCurves(std::vector<std::shared_ptr<Curve3D>>& curves, int count = 10)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> typeDist(0, 2);
    std::uniform_real_distribution<> paramDist(0.5, 5.0);
    std::uniform_real_distribution<> posDist(-3.0, 3.0);

    curves.clear();
    for (int i = 0; i < count; ++i)
    {
        int type = typeDist(gen);
        Point3D pos(posDist(gen), posDist(gen), posDist(gen));

        switch (type)
        {
        case 0: // Circle
        {
            double radius = paramDist(gen);
            auto circle = std::make_shared<Circle3D>(radius);
            circle->setPosition(pos);
            curves.push_back(circle);
            break;
        }
        case 1: // Ellipse
        {
            double a = paramDist(gen);
            double b = paramDist(gen);
            auto ellipse = std::make_shared<Ellipse3D>(a, b);
            ellipse->setPosition(pos);
            curves.push_back(ellipse);
            break;
        }
        case 2: // Helix
        {
            double radius = paramDist(gen);
            double step = paramDist(gen) * 0.5;
            int turns = std::uniform_int_distribution<>(3, 8)(gen);
            auto helix = std::make_shared<Helix3D>(radius, step, turns);
            helix->setPosition(pos);
            curves.push_back(helix);
            break;
        }
        }
    }
}

void Task3_PrintPointsAndDerivatives(const std::vector<std::shared_ptr<Curve3D>>& curves)
{
    std::cout << "=== Points and Derivatives at t = PI/4 ===" << std::endl;
    double t = M_PI / 4.0;

    for (int i = 0; i < curves.size(); ++i)
    {
        std::cout << "Curve " << i + 1 << ": ";
        if (auto circle = std::dynamic_pointer_cast<Circle3D>(curves[i]))
            std::cout << "Circle (r=" << circle->getRadius() << ")";
        else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[i]))
            std::cout << "Ellipse (a=" << ellipse->getA() << ", b=" << ellipse->getB() << ")";
        else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[i]))
            std::cout << "Helix (r=" << helix->getRadius() << ", step=" << helix->getStep() << ", turns=" << helix->getTurns() << ")";

        Point3D point = curves[i]->getPoint(t);
        Point3D derivative = curves[i]->getDerivative(t);

        std::cout << "\n  Point: " << point;
        std::cout << "\n  Derivative: " << derivative << std::endl << std::endl;
    }
}

void Task4_5_6_CirclesOperations(const std::vector<std::shared_ptr<Curve3D>>& curves)
{
    // Task 4: Create second container with circles only
    std::vector<std::shared_ptr<Circle3D>> circles;
    for (const auto& curve : curves)
    {
        if (auto circle = std::dynamic_pointer_cast<Circle3D>(curve))
        {
            circles.push_back(circle);
        }
    }

    // Task 5: Sort by radius
    std::sort(circles.begin(), circles.end(),
        [](const std::shared_ptr<Circle3D>& a, const std::shared_ptr<Circle3D>& b) {
            return a->getRadius() < b->getRadius();
        });

    // Task 6: Calculate sum of radii
    double sum = 0.0;
    for (const auto& circle : circles)
    {
        sum += circle->getRadius();
    }

    std::cout << "=== Circle Operations ===" << std::endl;
    std::cout << "Found " << circles.size() << " circles" << std::endl;
    std::cout << "Sorted radii: ";
    for (const auto& circle : circles)
    {
        std::cout << circle->getRadius() << " ";
    }
    std::cout << "\nSum of radii: " << sum << std::endl;
}

// ───────────────────────────────
// Основная функция
// ───────────────────────────────
int main()
{
    const int screenW = 1280, screenH = 720;
    InitWindow(screenW, screenH, "3D Curves Editor (Raylib + RayGUI)");
    SetTargetFPS(60);

    // Простая камера от Raylib (исправленный синтаксис)
    Camera3D camera = { 0 };
    camera.position = Vector3{ 10.0f, 10.0f, 10.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
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
    bool showTaskWindow = false;

    // Переменные для добавления/редактирования
    int curveType = 0; // 0=Circle, 1=Ellipse, 2=Helix
    char circleRadius[32] = "1.0";
    char ellipseA[32] = "1.2";
    char ellipseB[32] = "0.7";
    char helixRadius[32] = "1.0";
    char helixStep[32] = "0.4";
    char helixTurns[32] = "5";

    // Позиция по умолчанию
    char posX[32] = "0.0";
    char posY[32] = "0.0";
    char posZ[32] = "0.0";

    // Флаги для текстовых полей
    bool editCircleRadius = false;
    bool editEllipseA = false;
    bool editEllipseB = false;
    bool editHelixRadius = false;
    bool editHelixStep = false;
    bool editHelixTurns = false;
    bool editPosX = false, editPosY = false, editPosZ = false;

    // Режим камеры
    int cameraMode = CAMERA_FREE;
    DisableCursor();

    while (!WindowShouldClose())
    {
        // Управление камерой (встроенное в Raylib)
        if (!guiMode)
        {
            UpdateCamera(&camera, cameraMode);
        }

        // Переключение между GUI и камерой
        if (IsKeyPressed(KEY_TAB))
        {
            guiMode = !guiMode;
            if (guiMode)
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
                    snprintf(posX, 32, "%.2f", circle->getPosition().x);
                    snprintf(posY, 32, "%.2f", circle->getPosition().y);
                    snprintf(posZ, 32, "%.2f", circle->getPosition().z);
                }
                else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[selectedCurve]))
                {
                    curveType = 1;
                    snprintf(ellipseA, 32, "%.2f", ellipse->getA());
                    snprintf(ellipseB, 32, "%.2f", ellipse->getB());
                    snprintf(posX, 32, "%.2f", ellipse->getPosition().x);
                    snprintf(posY, 32, "%.2f", ellipse->getPosition().y);
                    snprintf(posZ, 32, "%.2f", ellipse->getPosition().z);
                }
                else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[selectedCurve]))
                {
                    curveType = 2;
                    snprintf(helixRadius, 32, "%.2f", helix->getRadius());
                    snprintf(helixStep, 32, "%.2f", helix->getStep());
                    snprintf(helixTurns, 32, "%d", helix->getTurns());
                    snprintf(posX, 32, "%.2f", helix->getPosition().x);
                    snprintf(posY, 32, "%.2f", helix->getPosition().y);
                    snprintf(posZ, 32, "%.2f", helix->getPosition().z);
                }
            }

            if (GuiButton({ 1120, 80, 120, 30 }, "Delete") && selectedCurve >= 0)
            {
                curves.erase(curves.begin() + selectedCurve);
                selectedCurve = -1;
            }

            if (GuiButton({ 980, 80, 120, 30 }, "Tasks 1-6"))
                showTaskWindow = true;

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
            Rectangle addWindow = { screenW / 2 - 200, screenH / 2 - 250, 400, 350 };
            GuiWindowBox(addWindow, "Add New Curve");

            // Выбор типа кривой
            GuiLabel({ addWindow.x + 20, addWindow.y + 40, 100, 25 }, "Curve Type:");
            GuiComboBox({ addWindow.x + 120, addWindow.y + 40, 200, 25 }, "Circle;Ellipse;Helix", &curveType);

            // Обработка фокуса для текстовых полей
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                editCircleRadius = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 80, 200, 25 });
                editEllipseA = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 80, 200, 25 });
                editEllipseB = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 120, 200, 25 });
                editHelixRadius = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 80, 200, 25 });
                editHelixStep = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 120, 200, 25 });
                editHelixTurns = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 160, 200, 25 });
                editPosX = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 200, 60, 25 });
                editPosY = CheckCollisionPointRec(mousePos, { addWindow.x + 210, addWindow.y + 200, 60, 25 });
                editPosZ = CheckCollisionPointRec(mousePos, { addWindow.x + 300, addWindow.y + 200, 60, 25 });
            }

            // Параметры в зависимости от типа
            if (curveType == 0) // Circle
            {
                GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "Radius:");
                if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, circleRadius, 32, editCircleRadius)) {
                    editCircleRadius = !editCircleRadius;
                }
            }
            else if (curveType == 1) // Ellipse
            {
                GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "A:");
                if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, ellipseA, 32, editEllipseA)) {
                    editEllipseA = !editEllipseA;
                }
                GuiLabel({ addWindow.x + 20, addWindow.y + 120, 100, 25 }, "B:");
                if (GuiTextBox({ addWindow.x + 120, addWindow.y + 120, 200, 25 }, ellipseB, 32, editEllipseB)) {
                    editEllipseB = !editEllipseB;
                }
            }
            else if (curveType == 2) // Helix
            {
                GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "Radius:");
                if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, helixRadius, 32, editHelixRadius)) {
                    editHelixRadius = !editHelixRadius;
                }
                GuiLabel({ addWindow.x + 20, addWindow.y + 120, 100, 25 }, "Step:");
                if (GuiTextBox({ addWindow.x + 120, addWindow.y + 120, 200, 25 }, helixStep, 32, editHelixStep)) {
                    editHelixStep = !editHelixStep;
                }
                GuiLabel({ addWindow.x + 20, addWindow.y + 160, 100, 25 }, "Turns:");
                if (GuiTextBox({ addWindow.x + 120, addWindow.y + 160, 200, 25 }, helixTurns, 32, editHelixTurns)) {
                    editHelixTurns = !editHelixTurns;
                }
            }

            // Позиция
            GuiLabel({ addWindow.x + 20, addWindow.y + 200, 100, 25 }, "Position:");
            if (GuiTextBox({ addWindow.x + 120, addWindow.y + 200, 60, 25 }, posX, 32, editPosX)) editPosX = !editPosX;
            GuiLabel({ addWindow.x + 190, addWindow.y + 200, 20, 25 }, "Y:");
            if (GuiTextBox({ addWindow.x + 210, addWindow.y + 200, 60, 25 }, posY, 32, editPosY)) editPosY = !editPosY;
            GuiLabel({ addWindow.x + 280, addWindow.y + 200, 20, 25 }, "Z:");
            if (GuiTextBox({ addWindow.x + 300, addWindow.y + 200, 60, 25 }, posZ, 32, editPosZ)) editPosZ = !editPosZ;

            // Кнопки OK/Cancel
            if (GuiButton({ addWindow.x + 100, addWindow.y + 280, 80, 30 }, "OK"))
            {
                try {
                    double x = std::stod(posX);
                    double y = std::stod(posY);
                    double z = std::stod(posZ);
                    Point3D position(x, y, z);

                    if (curveType == 0) {
                        double radius = std::stod(circleRadius);
                        auto circle = std::make_shared<Circle3D>(radius);
                        circle->setPosition(position);
                        curves.push_back(circle);
                    }
                    else if (curveType == 1) {
                        double a = std::stod(ellipseA);
                        double b = std::stod(ellipseB);
                        auto ellipse = std::make_shared<Ellipse3D>(a, b);
                        ellipse->setPosition(position);
                        curves.push_back(ellipse);
                    }
                    else if (curveType == 2) {
                        double radius = std::stod(helixRadius);
                        double step = std::stod(helixStep);
                        int turns = std::stoi(helixTurns);
                        auto helix = std::make_shared<Helix3D>(radius, step, turns);
                        helix->setPosition(position);
                        curves.push_back(helix);
                    }
                    showAddWindow = false;
                }
                catch (const std::exception& e) {
                    // Ошибка ввода
                }
            }

            if (GuiButton({ addWindow.x + 220, addWindow.y + 280, 80, 30 }, "Cancel"))
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

            // Обработка фокуса для текстовых полей в Edit Window
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                editCircleRadius = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 80, 200, 25 });
                editEllipseA = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 80, 200, 25 });
                editEllipseB = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 120, 200, 25 });
                editHelixRadius = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 80, 200, 25 });
                editHelixStep = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 120, 200, 25 });
                editHelixTurns = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 160, 200, 25 });
            }

            // Параметры в зависимости от типа
            if (auto circle = std::dynamic_pointer_cast<Circle3D>(curves[selectedCurve]))
            {
                GuiLabel({ editWindow.x + 20, editWindow.y + 80, 100, 25 }, "Radius:");
                if (GuiTextBox({ editWindow.x + 120, editWindow.y + 80, 200, 25 }, circleRadius, 32, editCircleRadius)) {
                    editCircleRadius = !editCircleRadius;
                }
            }
            else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[selectedCurve]))
            {
                GuiLabel({ editWindow.x + 20, editWindow.y + 80, 100, 25 }, "A:");
                if (GuiTextBox({ editWindow.x + 120, editWindow.y + 80, 200, 25 }, ellipseA, 32, editEllipseA)) {
                    editEllipseA = !editEllipseA;
                }
                GuiLabel({ editWindow.x + 20, editWindow.y + 120, 100, 25 }, "B:");
                if (GuiTextBox({ editWindow.x + 120, editWindow.y + 120, 200, 25 }, ellipseB, 32, editEllipseB)) {
                    editEllipseB = !editEllipseB;
                }
            }
            else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[selectedCurve]))
            {
                GuiLabel({ editWindow.x + 20, editWindow.y + 80, 100, 25 }, "Radius:");
                if (GuiTextBox({ editWindow.x + 120, editWindow.y + 80, 200, 25 }, helixRadius, 32, editHelixRadius)) {
                    editHelixRadius = !editHelixRadius;
                }
                GuiLabel({ editWindow.x + 20, editWindow.y + 120, 100, 25 }, "Step:");
                if (GuiTextBox({ editWindow.x + 120, editWindow.y + 120, 200, 25 }, helixStep, 32, editHelixStep)) {
                    editHelixStep = !editHelixStep;
                }
                GuiLabel({ editWindow.x + 20, editWindow.y + 160, 100, 25 }, "Turns:");
                if (GuiTextBox({ editWindow.x + 120, editWindow.y + 160, 200, 25 }, helixTurns, 32, editHelixTurns)) {
                    editHelixTurns = !editHelixTurns;
                }
            }

            // Кнопки Save/Cancel
            if (GuiButton({ editWindow.x + 100, editWindow.y + 220, 80, 30 }, "Save"))
            {
                try {
                    if (auto circle = std::dynamic_pointer_cast<Circle3D>(curves[selectedCurve]))
                    {
                        double radius = std::stod(circleRadius);
                        double x = std::stod(posX);
                        double y = std::stod(posY);
                        double z = std::stod(posZ);
                        Point3D position(x, y, z);

                        curves[selectedCurve] = std::make_shared<Circle3D>(radius);
                        curves[selectedCurve]->setPosition(position);
                    }
                    else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[selectedCurve]))
                    {
                        double a = std::stod(ellipseA);
                        double b = std::stod(ellipseB);
                        double x = std::stod(posX);
                        double y = std::stod(posY);
                        double z = std::stod(posZ);
                        Point3D position(x, y, z);

                        curves[selectedCurve] = std::make_shared<Ellipse3D>(a, b);
                        curves[selectedCurve]->setPosition(position);
                    }
                    else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[selectedCurve]))
                    {
                        double radius = std::stod(helixRadius);
                        double step = std::stod(helixStep);
                        int turns = std::stoi(helixTurns);
                        double x = std::stod(posX);
                        double y = std::stod(posY);
                        double z = std::stod(posZ);
                        Point3D position(x, y, z);

                        helix->setRadius(radius);
                        helix->setStep(step);
                        helix->setTurns(turns);
                        helix->setPosition(position);
                    }
                    showEditWindow = false;
                }
                catch (const std::exception& e) {
                    // В случае ошибки ввода просто закрываем окно
                    showEditWindow = false;
                }
            }

            if (GuiButton({ editWindow.x + 220, editWindow.y + 220, 80, 30 }, "Cancel"))
                showEditWindow = false;
        }

        // Окно заданий
        if (showTaskWindow)
        {
            Rectangle taskWindow = { screenW / 2 - 250, screenH / 2 - 200, 500, 400 };
            GuiWindowBox(taskWindow, "Tasks 1-6");

            GuiLabel({ taskWindow.x + 20, taskWindow.y + 40, 460, 25 }, "Task 1-2: Generate random curves");
            if (GuiButton({ taskWindow.x + 20, taskWindow.y + 70, 200, 30 }, "Generate 10 Random Curves"))
            {
                Task1_GenerateRandomCurves(curves, 10);
            }

            GuiLabel({ taskWindow.x + 20, taskWindow.y + 120, 460, 25 }, "Task 3: Print points and derivatives");
            if (GuiButton({ taskWindow.x + 20, taskWindow.y + 150, 200, 30 }, "Print to Console"))
            {
                Task3_PrintPointsAndDerivatives(curves);
            }

            GuiLabel({ taskWindow.x + 20, taskWindow.y + 200, 460, 25 }, "Task 4-6: Circle operations");
            if (GuiButton({ taskWindow.x + 20, taskWindow.y + 230, 200, 30 }, "Execute Circle Tasks"))
            {
                Task4_5_6_CirclesOperations(curves);
            }

            if (GuiButton({ taskWindow.x + 350, taskWindow.y + 350, 120, 30 }, "Close"))
                showTaskWindow = false;
        }

        DrawText("Mouse wheel = Zoom in/out", 10, 10, 18, DARKGRAY);
        DrawText("Mouse right button = Rotate", 10, 35, 18, DARKGRAY);
        DrawText("TAB = Toggle GUI mode", 10, 60, 18, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}