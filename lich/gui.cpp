#define _CRT_SECURE_NO_WARNINGS

#include "gui.h"
#include "drawing.h"
#include "tasks.h"
#include "raylib.h"
#include "raygui.h"
#include <cmath>
#include <iostream>
#include <cstring>
#include <string>

// Инициализация состояния приложения с пустыми полями
void InitializeAppState(AppState& state)
{
    state.selectedCurve = -1;
    state.panelVisible = true;
    state.guiMode = false;
    state.showAddWindow = false;
    state.showEditWindow = false;
    state.showTaskWindow = false;

    state.curveType = 0;

    // Инициализируем все поля пустыми строками
    strcpy(state.circleRadius, "");
    strcpy(state.ellipseA, "");
    strcpy(state.ellipseB, "");
    strcpy(state.helixRadius, "");
    strcpy(state.helixStep, "");
    strcpy(state.helixTurns, "");
    strcpy(state.posX, "");
    strcpy(state.posY, "");
    strcpy(state.posZ, "");
    strcpy(state.tValue, "0.0");

    state.editCircleRadius = state.editEllipseA = state.editEllipseB = false;
    state.editHelixRadius = state.editHelixStep = state.editHelixTurns = false;
    state.editPosX = state.editPosY = state.editPosZ = false;
    state.editTValue = false;
}

// Заполнение полей редактирования значениями из выбранной кривой
void FillEditFieldsFromSelectedCurve(AppState& state)
{
    if (state.selectedCurve < 0 || state.selectedCurve >= state.curves.size()) return;

    auto& curve = state.curves[state.selectedCurve];

    // Заполняем позицию
    Point3D position;
    if (auto circle = std::dynamic_pointer_cast<Circle3D>(curve)) {
        position = circle->getPosition();
        state.curveType = 0;
        snprintf(state.circleRadius, 32, "%.2f", circle->getRadius());
    }
    else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curve)) {
        position = ellipse->getPosition();
        state.curveType = 1;
        snprintf(state.ellipseA, 32, "%.2f", ellipse->getA());
        snprintf(state.ellipseB, 32, "%.2f", ellipse->getB());
    }
    else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curve)) {
        position = helix->getPosition();
        state.curveType = 2;
        snprintf(state.helixRadius, 32, "%.2f", helix->getRadius());
        snprintf(state.helixStep, 32, "%.2f", helix->getStep());
        snprintf(state.helixTurns, 32, "%d", helix->getTurns());
    }

    // Заполняем координаты позиции
    snprintf(state.posX, 32, "%.2f", position.x);
    snprintf(state.posY, 32, "%.2f", position.y);
    snprintf(state.posZ, 32, "%.2f", position.z);
}

void DrawMainPanel(AppState& state)
{
    Rectangle panel = { 960, 0, 320, (float)720 };
    GuiPanel(panel, "Curve Editor");

    // Основные кнопки
    if (GuiButton({ 980, 40, 120, 30 }, "Add Curve"))
    {
        state.showAddWindow = true;
        // Очищаем поля при открытии окна добавления
        strcpy(state.circleRadius, "");
        strcpy(state.ellipseA, "");
        strcpy(state.ellipseB, "");
        strcpy(state.helixRadius, "");
        strcpy(state.helixStep, "");
        strcpy(state.helixTurns, "");
        strcpy(state.posX, "0.0");
        strcpy(state.posY, "0.0");
        strcpy(state.posZ, "0.0");
        state.curveType = 0;
    }

    if (GuiButton({ 1120, 40, 120, 30 }, "Edit Curve") && state.selectedCurve >= 0)
    {
        state.showEditWindow = true;
        // Заполняем поля значениями выбранной кривой
        FillEditFieldsFromSelectedCurve(state);
    }

    if (GuiButton({ 1120, 80, 120, 30 }, "Delete") && state.selectedCurve >= 0)
    {
        state.curves.erase(state.curves.begin() + state.selectedCurve);
        state.selectedCurve = -1;
    }

    if (GuiButton({ 980, 80, 120, 30 }, "Tasks 1-6"))
        state.showTaskWindow = true;

    // Список кривых
    DrawText("Curves:", 980, 120, 20, DARKGRAY);
    for (int i = 0; i < state.curves.size(); ++i)
    {
        std::string label = std::to_string(i + 1) + ". ";
        if (std::dynamic_pointer_cast<Circle3D>(state.curves[i])) label += "Circle";
        else if (std::dynamic_pointer_cast<Ellipse3D>(state.curves[i])) label += "Ellipse";
        else if (std::dynamic_pointer_cast<Helix3D>(state.curves[i])) label += "Helix";

        if (GuiButton({ 980, 150.0f + i * 40, 140, 30 }, label.c_str()))
            state.selectedCurve = i;
    }

    // Поле для ввода t и кнопка Calculate
    float tInputY = 150.0f + state.curves.size() * 40 + 20;
    GuiLabel({ 980, tInputY, 100, 25 }, "t value:");
    if (GuiTextBox({ 980, tInputY + 30, 100, 25 }, state.tValue, 32, state.editTValue)) {
        state.editTValue = !state.editTValue;
    }

    if (GuiButton({ 1090, tInputY + 30, 120, 30 }, "Calculate") && state.selectedCurve >= 0) {
        try {
            double t = std::stod(state.tValue);
            Point3D point = state.curves[state.selectedCurve]->getPoint(t);
            Point3D derivative = state.curves[state.selectedCurve]->getDerivative(t);

            std::cout << "=== Calculation for t = " << t << " ===" << std::endl;
            std::cout << "Selected curve " << state.selectedCurve + 1 << ": ";
            if (auto circle = std::dynamic_pointer_cast<Circle3D>(state.curves[state.selectedCurve]))
                std::cout << "Circle (r=" << circle->getRadius() << ")";
            else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(state.curves[state.selectedCurve]))
                std::cout << "Ellipse (a=" << ellipse->getA() << ", b=" << ellipse->getB() << ")";
            else if (auto helix = std::dynamic_pointer_cast<Helix3D>(state.curves[state.selectedCurve]))
                std::cout << "Helix (r=" << helix->getRadius() << ", step=" << helix->getStep() << ", turns=" << helix->getTurns() << ")";

            std::cout << "\n  Point: " << point;
            std::cout << "\n  Derivative: " << derivative << std::endl << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "Error: Invalid t value" << std::endl;
        }
    }
}

void HandleAddWindow(AppState& state)
{
    if (!state.showAddWindow) return;

    Rectangle addWindow = { 1280 / 2 - 200, 720 / 2 - 250, 400, 350 };
    GuiWindowBox(addWindow, "Add New Curve");

    // Выбор типа кривой
    GuiLabel({ addWindow.x + 20, addWindow.y + 40, 100, 25 }, "Curve Type:");
    GuiComboBox({ addWindow.x + 120, addWindow.y + 40, 200, 25 }, "Circle;Ellipse;Helix", &state.curveType);

    // Обработка фокуса для текстовых полей
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        state.editCircleRadius = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 80, 200, 25 });
        state.editEllipseA = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 80, 200, 25 });
        state.editEllipseB = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 120, 200, 25 });
        state.editHelixRadius = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 80, 200, 25 });
        state.editHelixStep = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 120, 200, 25 });
        state.editHelixTurns = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 160, 200, 25 });
        state.editPosX = CheckCollisionPointRec(mousePos, { addWindow.x + 120, addWindow.y + 200, 60, 25 });
        state.editPosY = CheckCollisionPointRec(mousePos, { addWindow.x + 210, addWindow.y + 200, 60, 25 });
        state.editPosZ = CheckCollisionPointRec(mousePos, { addWindow.x + 300, addWindow.y + 200, 60, 25 });
    }

    // Параметры в зависимости от типа
    if (state.curveType == 0) // Circle
    {
        GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "Radius:");
        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, state.circleRadius, 32, state.editCircleRadius)) {
            state.editCircleRadius = !state.editCircleRadius;
        }
    }
    else if (state.curveType == 1) // Ellipse
    {
        GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "A:");
        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, state.ellipseA, 32, state.editEllipseA)) {
            state.editEllipseA = !state.editEllipseA;
        }
        GuiLabel({ addWindow.x + 20, addWindow.y + 120, 100, 25 }, "B:");
        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 120, 200, 25 }, state.ellipseB, 32, state.editEllipseB)) {
            state.editEllipseB = !state.editEllipseB;
        }
    }
    else if (state.curveType == 2) // Helix
    {
        GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "Radius:");
        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, state.helixRadius, 32, state.editHelixRadius)) {
            state.editHelixRadius = !state.editHelixRadius;
        }
        GuiLabel({ addWindow.x + 20, addWindow.y + 120, 100, 25 }, "Step:");
        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 120, 200, 25 }, state.helixStep, 32, state.editHelixStep)) {
            state.editHelixStep = !state.editHelixStep;
        }
        GuiLabel({ addWindow.x + 20, addWindow.y + 160, 100, 25 }, "Turns:");
        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 160, 200, 25 }, state.helixTurns, 32, state.editHelixTurns)) {
            state.editHelixTurns = !state.editHelixTurns;
        }
    }

    // Позиция
    GuiLabel({ addWindow.x + 20, addWindow.y + 200, 100, 25 }, "Position:");
    if (GuiTextBox({ addWindow.x + 120, addWindow.y + 200, 60, 25 }, state.posX, 32, state.editPosX)) state.editPosX = !state.editPosX;
    GuiLabel({ addWindow.x + 190, addWindow.y + 200, 20, 25 }, "Y:");
    if (GuiTextBox({ addWindow.x + 210, addWindow.y + 200, 60, 25 }, state.posY, 32, state.editPosY)) state.editPosY = !state.editPosY;
    GuiLabel({ addWindow.x + 280, addWindow.y + 200, 20, 25 }, "Z:");
    if (GuiTextBox({ addWindow.x + 300, addWindow.y + 200, 60, 25 }, state.posZ, 32, state.editPosZ)) state.editPosZ = !state.editPosZ;

    // Кнопки OK/Cancel
    if (GuiButton({ addWindow.x + 100, addWindow.y + 280, 80, 30 }, "OK"))
    {
        try {
            double x = std::stod(state.posX);
            double y = std::stod(state.posY);
            double z = std::stod(state.posZ);
            Point3D position(x, y, z);

            if (state.curveType == 0) {
                double radius = std::stod(state.circleRadius);
                auto circle = std::make_shared<Circle3D>(radius);
                circle->setPosition(position);
                state.curves.push_back(circle);
            }
            else if (state.curveType == 1) {
                double a = std::stod(state.ellipseA);
                double b = std::stod(state.ellipseB);
                auto ellipse = std::make_shared<Ellipse3D>(a, b);
                ellipse->setPosition(position);
                state.curves.push_back(ellipse);
            }
            else if (state.curveType == 2) {
                double radius = std::stod(state.helixRadius);
                double step = std::stod(state.helixStep);
                int turns = std::stoi(state.helixTurns);
                auto helix = std::make_shared<Helix3D>(radius, step, turns);
                helix->setPosition(position);
                state.curves.push_back(helix);
            }
            state.showAddWindow = false;
        }
        catch (const std::exception& e) {
            // Ошибка ввода
        }
    }

    if (GuiButton({ addWindow.x + 220, addWindow.y + 280, 80, 30 }, "Cancel"))
        state.showAddWindow = false;
}

void HandleEditWindow(AppState& state)
{
    if (!state.showEditWindow || state.selectedCurve < 0) return;

    Rectangle editWindow = { 1280 / 2 - 200, 720 / 2 - 250, 400, 350 };
    GuiWindowBox(editWindow, "Edit Curve");

    // Отображаем тип текущей кривой
    std::string typeLabel = "Type: ";
    if (auto circle = std::dynamic_pointer_cast<Circle3D>(state.curves[state.selectedCurve])) typeLabel += "Circle";
    else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(state.curves[state.selectedCurve])) typeLabel += "Ellipse";
    else if (auto helix = std::dynamic_pointer_cast<Helix3D>(state.curves[state.selectedCurve])) typeLabel += "Helix";

    GuiLabel({ editWindow.x + 20, editWindow.y + 40, 360, 25 }, typeLabel.c_str());

    // Обработка фокуса для текстовых полей в Edit Window
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        state.editCircleRadius = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 80, 200, 25 });
        state.editEllipseA = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 80, 200, 25 });
        state.editEllipseB = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 120, 200, 25 });
        state.editHelixRadius = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 80, 200, 25 });
        state.editHelixStep = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 120, 200, 25 });
        state.editHelixTurns = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 160, 200, 25 });
        state.editPosX = CheckCollisionPointRec(mousePos, { editWindow.x + 120, editWindow.y + 200, 60, 25 });
        state.editPosY = CheckCollisionPointRec(mousePos, { editWindow.x + 210, editWindow.y + 200, 60, 25 });
        state.editPosZ = CheckCollisionPointRec(mousePos, { editWindow.x + 300, editWindow.y + 200, 60, 25 });
    }

    // Параметры в зависимости от типа
    int yOffset = 80;
    if (auto circle = std::dynamic_pointer_cast<Circle3D>(state.curves[state.selectedCurve]))
    {
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Radius:");
        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, state.circleRadius, 32, state.editCircleRadius)) {
            state.editCircleRadius = !state.editCircleRadius;
        }
        yOffset += 40;
    }
    else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(state.curves[state.selectedCurve]))
    {
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "A:");
        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, state.ellipseA, 32, state.editEllipseA)) {
            state.editEllipseA = !state.editEllipseA;
        }
        yOffset += 40;
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "B:");
        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, state.ellipseB, 32, state.editEllipseB)) {
            state.editEllipseB = !state.editEllipseB;
        }
        yOffset += 40;
    }
    else if (auto helix = std::dynamic_pointer_cast<Helix3D>(state.curves[state.selectedCurve]))
    {
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Radius:");
        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, state.helixRadius, 32, state.editHelixRadius)) {
            state.editHelixRadius = !state.editHelixRadius;
        }
        yOffset += 40;
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Step:");
        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, state.helixStep, 32, state.editHelixStep)) {
            state.editHelixStep = !state.editHelixStep;
        }
        yOffset += 40;
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Turns:");
        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, state.helixTurns, 32, state.editHelixTurns)) {
            state.editHelixTurns = !state.editHelixTurns;
        }
        yOffset += 40;
    }

    // Позиция
    GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Position:");
    if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 60, 25 }, state.posX, 32, state.editPosX)) state.editPosX = !state.editPosX;
    GuiLabel({ editWindow.x + 190, editWindow.y + yOffset, 20, 25 }, "Y:");
    if (GuiTextBox({ editWindow.x + 210, editWindow.y + yOffset, 60, 25 }, state.posY, 32, state.editPosY)) state.editPosY = !state.editPosY;
    GuiLabel({ editWindow.x + 280, editWindow.y + yOffset, 20, 25 }, "Z:");
    if (GuiTextBox({ editWindow.x + 300, editWindow.y + yOffset, 60, 25 }, state.posZ, 32, state.editPosZ)) state.editPosZ = !state.editPosZ;

    // Кнопки Save/Cancel
    if (GuiButton({ editWindow.x + 100, editWindow.y + 300, 80, 30 }, "Save"))
    {
        try {
            double x = std::stod(state.posX);
            double y = std::stod(state.posY);
            double z = std::stod(state.posZ);
            Point3D position(x, y, z);

            if (auto circle = std::dynamic_pointer_cast<Circle3D>(state.curves[state.selectedCurve]))
            {
                double radius = std::stod(state.circleRadius);
                auto newCircle = std::make_shared<Circle3D>(radius);
                newCircle->setPosition(position);
                state.curves[state.selectedCurve] = newCircle;
            }
            else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(state.curves[state.selectedCurve]))
            {
                double a = std::stod(state.ellipseA);
                double b = std::stod(state.ellipseB);
                auto newEllipse = std::make_shared<Ellipse3D>(a, b);
                newEllipse->setPosition(position);
                state.curves[state.selectedCurve] = newEllipse;
            }
            else if (auto helix = std::dynamic_pointer_cast<Helix3D>(state.curves[state.selectedCurve]))
            {
                double radius = std::stod(state.helixRadius);
                double step = std::stod(state.helixStep);
                int turns = std::stoi(state.helixTurns);
                helix->setRadius(radius);
                helix->setStep(step);
                helix->setTurns(turns);
                helix->setPosition(position);
            }
            state.showEditWindow = false;
        }
        catch (const std::exception& e) {
            state.showEditWindow = false;
        }
    }

    if (GuiButton({ editWindow.x + 220, editWindow.y + 300, 80, 30 }, "Cancel"))
        state.showEditWindow = false;
}

void HandleTaskWindow(AppState& state)
{
    if (!state.showTaskWindow) return;

    Rectangle taskWindow = { 1280 / 2 - 250, 720 / 2 - 200, 500, 400 };
    GuiWindowBox(taskWindow, "Tasks 1-6");

    GuiLabel({ taskWindow.x + 20, taskWindow.y + 40, 460, 25 }, "Task 1-2: Generate random curves");
    if (GuiButton({ taskWindow.x + 20, taskWindow.y + 70, 200, 30 }, "Generate 10 Random Curves"))
    {
        Task1_GenerateRandomCurves(state.curves, 10);
    }

    GuiLabel({ taskWindow.x + 20, taskWindow.y + 120, 460, 25 }, "Task 3: Print points and derivatives");
    if (GuiButton({ taskWindow.x + 20, taskWindow.y + 150, 200, 30 }, "Print to Console"))
    {
        Task3_PrintPointsAndDerivatives(state.curves);
    }

    GuiLabel({ taskWindow.x + 20, taskWindow.y + 200, 460, 25 }, "Task 4-6: Circle operations");
    if (GuiButton({ taskWindow.x + 20, taskWindow.y + 230, 200, 30 }, "Execute Circle Tasks"))
    {
        Task4_5_6_CirclesOperations(state.curves);
    }

    if (GuiButton({ taskWindow.x + 350, taskWindow.y + 350, 120, 30 }, "Close"))
        state.showTaskWindow = false;
}