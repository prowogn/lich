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

    // Инициализируем строки пустыми значениями
    state.circleRadius = "";
    state.ellipseA = "";
    state.ellipseB = "";
    state.helixRadius = "";
    state.helixStep = "";
    state.helixTurns = "";
    state.posX = "0.0";
    state.posY = "0.0";
    state.posZ = "0.0";
    state.rotX = "0.0";
    state.rotY = "0.0";
    state.rotZ = "0.0";
    state.tValue = "0.0";

    // флаги редактирования
    state.editCircleRadius = state.editEllipseA = state.editEllipseB = false;
    state.editHelixRadius = state.editHelixStep = state.editHelixTurns = false;
    state.editPosX = state.editPosY = state.editPosZ = false;
    state.editRotX = state.editRotY = state.editRotZ = false;
    state.editTValue = false;

    // Инициализация новых полей
    state.currentPoint = Point3D();
    state.currentDerivative = Point3D();
    state.calculated = false;
}

// Заполнение полей редактирования значениями из выбранной кривой
void FillEditFieldsFromSelectedCurve(AppState& state)
{
    if (state.selectedCurve < 0 || state.selectedCurve >= state.curves.size()) return;

    auto& curve = state.curves[state.selectedCurve];

    // Заполняем позицию и вращение
    Point3D position;
    Point3D rotation;
    if (auto circle = std::dynamic_pointer_cast<Circle3D>(curve)) {
        position = circle->getPosition();
        rotation = circle->getRotation();
        state.curveType = 0;
        state.circleRadius = std::to_string(circle->getRadius());
    }
    else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curve)) {
        position = ellipse->getPosition();
        rotation = ellipse->getRotation();
        state.curveType = 1;
        state.ellipseA = std::to_string(ellipse->getA());
        state.ellipseB = std::to_string(ellipse->getB());
    }
    else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curve)) {
        position = helix->getPosition();
        rotation = helix->getRotation();
        state.curveType = 2;
        state.helixRadius = std::to_string(helix->getRadius());
        state.helixStep = std::to_string(helix->getStep());
        state.helixTurns = std::to_string(helix->getTurns());
    }

    // Заполняем координаты позиции
    state.posX = std::to_string(position.x);
    state.posY = std::to_string(position.y);
    state.posZ = std::to_string(position.z);

    // Заполняем координаты вращения
    state.rotX = std::to_string(rotation.x);
    state.rotY = std::to_string(rotation.y);
    state.rotZ = std::to_string(rotation.z);
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
        state.circleRadius = "";
        state.ellipseA = "";
        state.ellipseB = "";
        state.helixRadius = "";
        state.helixStep = "";
        state.helixTurns = "";
        state.posX = "0.0";
        state.posY = "0.0";
        state.posZ = "0.0";
        state.rotX = "0.0";
        state.rotY = "0.0";
        state.rotZ = "0.0";
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
        state.calculated = false; // Сбрасываем расчет при удалении кривой
    }

    if (GuiButton({ 980, 80, 120, 30 }, "Tasks 1-6"))
        state.showTaskWindow = true;

    // Список кривых
    DrawText("Curves:", 980, 120, 20, DARKGRAY);
    for (int i = 0; i < (int)state.curves.size(); ++i)
    {
        std::string label = std::to_string(i + 1) + ". ";
        if (std::dynamic_pointer_cast<Circle3D>(state.curves[i])) label += "Circle";
        else if (std::dynamic_pointer_cast<Ellipse3D>(state.curves[i])) label += "Ellipse";
        else if (std::dynamic_pointer_cast<Helix3D>(state.curves[i])) label += "Helix";

        if (GuiButton({ 980, 150.0f + i * 40, 140, 30 }, label.c_str()))
        {
            state.selectedCurve = i;
            state.calculated = false; // Сбрасываем расчет при выборе новой кривой
        }
    }

    // Поле для ввода t и кнопка Calculate
    float tInputY = 150.0f + state.curves.size() * 40 + 20;
    GuiLabel({ 980, tInputY, 100, 25 }, "t value:");

    // Используем статический буфер для t value
    static char tBuffer[32] = "0.0";

    // Копируем значение из state только при первом отображении или при изменении фокуса
    if (!state.editTValue && state.tValue != tBuffer) {
        strncpy(tBuffer, state.tValue.c_str(), sizeof(tBuffer) - 1);
        tBuffer[sizeof(tBuffer) - 1] = '\0';
    }

    if (GuiTextBox({ 980, tInputY + 30, 100, 25 }, tBuffer, sizeof(tBuffer) - 1, state.editTValue)) {
        state.editTValue = !state.editTValue;
        if (!state.editTValue) { // Когда поле теряет фокус, сохраняем значение
            state.tValue = tBuffer;
        }
    }

    if (GuiButton({ 1090, tInputY + 30, 120, 30 }, "Calculate") && state.selectedCurve >= 0) {
        try {
            // Используем актуальное значение из state
            double t = std::stod(state.tValue);
            state.currentPoint = state.curves[state.selectedCurve]->getPoint(t);
            state.currentDerivative = state.curves[state.selectedCurve]->getDerivative(t);
            state.calculated = true;

            std::cout << "=== Calculation for t = " << t << " ===" << std::endl;
            std::cout << "Selected curve " << state.selectedCurve + 1 << ": ";
            if (auto circle = std::dynamic_pointer_cast<Circle3D>(state.curves[state.selectedCurve]))
                std::cout << "Circle (r=" << circle->getRadius() << ")";
            else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(state.curves[state.selectedCurve]))
                std::cout << "Ellipse (a=" << ellipse->getA() << ", b=" << ellipse->getB() << ")";
            else if (auto helix = std::dynamic_pointer_cast<Helix3D>(state.curves[state.selectedCurve]))
                std::cout << "Helix (r=" << helix->getRadius() << ", step=" << helix->getStep() << ", turns=" << helix->getTurns() << ")";

            std::cout << "\n  Point: " << state.currentPoint;
            std::cout << "\n  Derivative: " << state.currentDerivative << std::endl << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "Error: Invalid t value" << std::endl;
        }
    }

    // Отображение результатов расчета в интерфейсе
    if (state.calculated && state.selectedCurve >= 0) {
        float resultY = tInputY + 70;

        // Отображаем точку
        std::string pointText = "Point: (" +
            std::to_string(state.currentPoint.x).substr(0, 5) + ", " +
            std::to_string(state.currentPoint.y).substr(0, 5) + ", " +
            std::to_string(state.currentPoint.z).substr(0, 5) + ")";
        GuiLabel({ 980, resultY, 300, 25 }, pointText.c_str());

        // Отображаем производную
        std::string derivText = "Derivative: (" +
            std::to_string(state.currentDerivative.x).substr(0, 5) + ", " +
            std::to_string(state.currentDerivative.y).substr(0, 5) + ", " +
            std::to_string(state.currentDerivative.z).substr(0, 5) + ")";
        GuiLabel({ 980, resultY + 25, 300, 25 }, derivText.c_str());
    }
}

void HandleAddWindow(AppState& state)
{
    if (!state.showAddWindow) return;

    Rectangle addWindow = { 1280 / 2 - 200, 720 / 2 - 250, 400, 400 };
    GuiWindowBox(addWindow, "Add New Curve");

    // Выбор типа кривой
    GuiLabel({ addWindow.x + 20, addWindow.y + 40, 100, 25 }, "Curve Type:");
    GuiComboBox({ addWindow.x + 120, addWindow.y + 40, 200, 25 }, "Circle;Ellipse;Helix", &state.curveType);

    // Статические буферы для всех полей ввода в Add Window
    static char circleRadiusBuffer[32] = "";
    static char ellipseABuffer[32] = "";
    static char ellipseBBuffer[32] = "";
    static char helixRadiusBuffer[32] = "";
    static char helixStepBuffer[32] = "";
    static char helixTurnsBuffer[32] = "";
    static char posXBuffer[32] = "0.0";
    static char posYBuffer[32] = "0.0";
    static char posZBuffer[32] = "0.0";
    static char rotXBuffer[32] = "0.0";
    static char rotYBuffer[32] = "0.0";
    static char rotZBuffer[32] = "0.0";

    // Параметры в зависимости от типа
    if (state.curveType == 0) // Circle
    {
        GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "Radius:");

        // Синхронизация для Circle Radius
        if (!state.editCircleRadius && state.circleRadius != circleRadiusBuffer) {
            strncpy(circleRadiusBuffer, state.circleRadius.c_str(), sizeof(circleRadiusBuffer) - 1);
            circleRadiusBuffer[sizeof(circleRadiusBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, circleRadiusBuffer, sizeof(circleRadiusBuffer) - 1, state.editCircleRadius)) {
            state.editCircleRadius = !state.editCircleRadius;
            if (!state.editCircleRadius) {
                state.circleRadius = circleRadiusBuffer;
            }
        }
    }
    else if (state.curveType == 1) // Ellipse
    {
        GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "A:");

        // Синхронизация для Ellipse A
        if (!state.editEllipseA && state.ellipseA != ellipseABuffer) {
            strncpy(ellipseABuffer, state.ellipseA.c_str(), sizeof(ellipseABuffer) - 1);
            ellipseABuffer[sizeof(ellipseABuffer) - 1] = '\0';
        }

        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, ellipseABuffer, sizeof(ellipseABuffer) - 1, state.editEllipseA)) {
            state.editEllipseA = !state.editEllipseA;
            if (!state.editEllipseA) {
                state.ellipseA = ellipseABuffer;
            }
        }

        GuiLabel({ addWindow.x + 20, addWindow.y + 120, 100, 25 }, "B:");

        // Синхронизация для Ellipse B
        if (!state.editEllipseB && state.ellipseB != ellipseBBuffer) {
            strncpy(ellipseBBuffer, state.ellipseB.c_str(), sizeof(ellipseBBuffer) - 1);
            ellipseBBuffer[sizeof(ellipseBBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 120, 200, 25 }, ellipseBBuffer, sizeof(ellipseBBuffer) - 1, state.editEllipseB)) {
            state.editEllipseB = !state.editEllipseB;
            if (!state.editEllipseB) {
                state.ellipseB = ellipseBBuffer;
            }
        }
    }
    else if (state.curveType == 2) // Helix
    {
        GuiLabel({ addWindow.x + 20, addWindow.y + 80, 100, 25 }, "Radius:");

        // Синхронизация для Helix Radius
        if (!state.editHelixRadius && state.helixRadius != helixRadiusBuffer) {
            strncpy(helixRadiusBuffer, state.helixRadius.c_str(), sizeof(helixRadiusBuffer) - 1);
            helixRadiusBuffer[sizeof(helixRadiusBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 80, 200, 25 }, helixRadiusBuffer, sizeof(helixRadiusBuffer) - 1, state.editHelixRadius)) {
            state.editHelixRadius = !state.editHelixRadius;
            if (!state.editHelixRadius) {
                state.helixRadius = helixRadiusBuffer;
            }
        }

        GuiLabel({ addWindow.x + 20, addWindow.y + 120, 100, 25 }, "Step:");

        // Синхронизация для Helix Step
        if (!state.editHelixStep && state.helixStep != helixStepBuffer) {
            strncpy(helixStepBuffer, state.helixStep.c_str(), sizeof(helixStepBuffer) - 1);
            helixStepBuffer[sizeof(helixStepBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 120, 200, 25 }, helixStepBuffer, sizeof(helixStepBuffer) - 1, state.editHelixStep)) {
            state.editHelixStep = !state.editHelixStep;
            if (!state.editHelixStep) {
                state.helixStep = helixStepBuffer;
            }
        }

        GuiLabel({ addWindow.x + 20, addWindow.y + 160, 100, 25 }, "Turns:");

        // Синхронизация для Helix Turns
        if (!state.editHelixTurns && state.helixTurns != helixTurnsBuffer) {
            strncpy(helixTurnsBuffer, state.helixTurns.c_str(), sizeof(helixTurnsBuffer) - 1);
            helixTurnsBuffer[sizeof(helixTurnsBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ addWindow.x + 120, addWindow.y + 160, 200, 25 }, helixTurnsBuffer, sizeof(helixTurnsBuffer) - 1, state.editHelixTurns)) {
            state.editHelixTurns = !state.editHelixTurns;
            if (!state.editHelixTurns) {
                state.helixTurns = helixTurnsBuffer;
            }
        }
    }

    // Позиция
    GuiLabel({ addWindow.x + 20, addWindow.y + 200, 100, 25 }, "Position:");

    // Синхронизация для Position X
    if (!state.editPosX && state.posX != posXBuffer) {
        strncpy(posXBuffer, state.posX.c_str(), sizeof(posXBuffer) - 1);
        posXBuffer[sizeof(posXBuffer) - 1] = '\0';
    }

    if (GuiTextBox({ addWindow.x + 120, addWindow.y + 200, 60, 25 }, posXBuffer, sizeof(posXBuffer) - 1, state.editPosX)) {
        state.editPosX = !state.editPosX;
        if (!state.editPosX) {
            state.posX = posXBuffer;
        }
    }

    GuiLabel({ addWindow.x + 190, addWindow.y + 200, 20, 25 }, "Y:");

    // Синхронизация для Position Y
    if (!state.editPosY && state.posY != posYBuffer) {
        strncpy(posYBuffer, state.posY.c_str(), sizeof(posYBuffer) - 1);
        posYBuffer[sizeof(posYBuffer) - 1] = '\0';
    }

    if (GuiTextBox({ addWindow.x + 210, addWindow.y + 200, 60, 25 }, posYBuffer, sizeof(posYBuffer) - 1, state.editPosY)) {
        state.editPosY = !state.editPosY;
        if (!state.editPosY) {
            state.posY = posYBuffer;
        }
    }

    GuiLabel({ addWindow.x + 280, addWindow.y + 200, 20, 25 }, "Z:");

    // Синхронизация для Position Z
    if (!state.editPosZ && state.posZ != posZBuffer) {
        strncpy(posZBuffer, state.posZ.c_str(), sizeof(posZBuffer) - 1);
        posZBuffer[sizeof(posZBuffer) - 1] = '\0';
    }

    if (GuiTextBox({ addWindow.x + 300, addWindow.y + 200, 60, 25 }, posZBuffer, sizeof(posZBuffer) - 1, state.editPosZ)) {
        state.editPosZ = !state.editPosZ;
        if (!state.editPosZ) {
            state.posZ = posZBuffer;
        }
    }

    // Вращение
    GuiLabel({ addWindow.x + 20, addWindow.y + 240, 100, 25 }, "Rotation:");

    // Синхронизация для Rotation X
    if (!state.editRotX && state.rotX != rotXBuffer) {
        strncpy(rotXBuffer, state.rotX.c_str(), sizeof(rotXBuffer) - 1);
        rotXBuffer[sizeof(rotXBuffer) - 1] = '\0';
    }
    GuiLabel({ addWindow.x + 120, addWindow.y + 240, 20, 25 }, "X:");
    if (GuiTextBox({ addWindow.x + 140, addWindow.y + 240, 60, 25 }, rotXBuffer, sizeof(rotXBuffer) - 1, state.editRotX)) {
        state.editRotX = !state.editRotX;
        if (!state.editRotX) {
            state.rotX = rotXBuffer;
        }
    }

    // Синхронизация для Rotation Y
    if (!state.editRotY && state.rotY != rotYBuffer) {
        strncpy(rotYBuffer, state.rotY.c_str(), sizeof(rotYBuffer) - 1);
        rotYBuffer[sizeof(rotYBuffer) - 1] = '\0';
    }
    GuiLabel({ addWindow.x + 210, addWindow.y + 240, 20, 25 }, "Y:");
    if (GuiTextBox({ addWindow.x + 230, addWindow.y + 240, 60, 25 }, rotYBuffer, sizeof(rotYBuffer) - 1, state.editRotY)) {
        state.editRotY = !state.editRotY;
        if (!state.editRotY) {
            state.rotY = rotYBuffer;
        }
    }

    // Синхронизация для Rotation Z
    if (!state.editRotZ && state.rotZ != rotZBuffer) {
        strncpy(rotZBuffer, state.rotZ.c_str(), sizeof(rotZBuffer) - 1);
        rotZBuffer[sizeof(rotZBuffer) - 1] = '\0';
    }
    GuiLabel({ addWindow.x + 300, addWindow.y + 240, 20, 25 }, "Z:");
    if (GuiTextBox({ addWindow.x + 320, addWindow.y + 240, 60, 25 }, rotZBuffer, sizeof(rotZBuffer) - 1, state.editRotZ)) {
        state.editRotZ = !state.editRotZ;
        if (!state.editRotZ) {
            state.rotZ = rotZBuffer;
        }
    }

    // Кнопки OK/Cancel
    if (GuiButton({ addWindow.x + 100, addWindow.y + 320, 80, 30 }, "OK"))
    {
        try {
            double x = std::stod(state.posX);
            double y = std::stod(state.posY);
            double z = std::stod(state.posZ);
            Point3D position(x, y, z);

            double rx = std::stod(state.rotX);
            double ry = std::stod(state.rotY);
            double rz = std::stod(state.rotZ);
            Point3D rotation(rx, ry, rz);

            if (state.curveType == 0) {
                double radius = std::stod(state.circleRadius);
                auto circle = std::make_shared<Circle3D>(radius);
                circle->setPosition(position);
                circle->setRotation(rotation);
                state.curves.push_back(circle);
            }
            else if (state.curveType == 1) {
                double a = std::stod(state.ellipseA);
                double b = std::stod(state.ellipseB);
                auto ellipse = std::make_shared<Ellipse3D>(a, b);
                ellipse->setPosition(position);
                ellipse->setRotation(rotation);
                state.curves.push_back(ellipse);
            }
            else if (state.curveType == 2) {
                double radius = std::stod(state.helixRadius);
                double step = std::stod(state.helixStep);
                int turns = std::stoi(state.helixTurns);
                auto helix = std::make_shared<Helix3D>(radius, step, turns);
                helix->setPosition(position);
                helix->setRotation(rotation);
                state.curves.push_back(helix);
            }
            state.showAddWindow = false;
        }
        catch (const std::exception& e) {
            (void)e;
            state.showAddWindow = false;
        }
    }

    if (GuiButton({ addWindow.x + 220, addWindow.y + 320, 80, 30 }, "Cancel"))
        state.showAddWindow = false;
}

void HandleEditWindow(AppState& state)
{
    if (!state.showEditWindow || state.selectedCurve < 0) return;

    Rectangle editWindow = { 1280 / 2 - 200, 720 / 2 - 250, 400, 400 };
    GuiWindowBox(editWindow, "Edit Curve");

    // Отображаем тип текущей кривой
    std::string typeLabel = "Type: ";
    if (auto circle = std::dynamic_pointer_cast<Circle3D>(state.curves[state.selectedCurve])) typeLabel += "Circle";
    else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(state.curves[state.selectedCurve])) typeLabel += "Ellipse";
    else if (auto helix = std::dynamic_pointer_cast<Helix3D>(state.curves[state.selectedCurve])) typeLabel += "Helix";

    GuiLabel({ editWindow.x + 20, editWindow.y + 40, 360, 25 }, typeLabel.c_str());

    // Статические буферы для всех полей ввода в Edit Window
    static char circleRadiusBuffer[32] = "";
    static char ellipseABuffer[32] = "";
    static char ellipseBBuffer[32] = "";
    static char helixRadiusBuffer[32] = "";
    static char helixStepBuffer[32] = "";
    static char helixTurnsBuffer[32] = "";
    static char posXBuffer[32] = "0.0";
    static char posYBuffer[32] = "0.0";
    static char posZBuffer[32] = "0.0";
    static char rotXBuffer[32] = "0.0";
    static char rotYBuffer[32] = "0.0";
    static char rotZBuffer[32] = "0.0";

    // Параметры в зависимости от типа
    int yOffset = 80;
    if (auto circle = std::dynamic_pointer_cast<Circle3D>(state.curves[state.selectedCurve]))
    {
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Radius:");

        // Синхронизация для Circle Radius
        if (!state.editCircleRadius && state.circleRadius != circleRadiusBuffer) {
            strncpy(circleRadiusBuffer, state.circleRadius.c_str(), sizeof(circleRadiusBuffer) - 1);
            circleRadiusBuffer[sizeof(circleRadiusBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, circleRadiusBuffer, sizeof(circleRadiusBuffer) - 1, state.editCircleRadius)) {
            state.editCircleRadius = !state.editCircleRadius;
            if (!state.editCircleRadius) {
                state.circleRadius = circleRadiusBuffer;
            }
        }
        yOffset += 40;
    }
    else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(state.curves[state.selectedCurve]))
    {
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "A:");

        // Синхронизация для Ellipse A
        if (!state.editEllipseA && state.ellipseA != ellipseABuffer) {
            strncpy(ellipseABuffer, state.ellipseA.c_str(), sizeof(ellipseABuffer) - 1);
            ellipseABuffer[sizeof(ellipseABuffer) - 1] = '\0';
        }

        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, ellipseABuffer, sizeof(ellipseABuffer) - 1, state.editEllipseA)) {
            state.editEllipseA = !state.editEllipseA;
            if (!state.editEllipseA) {
                state.ellipseA = ellipseABuffer;
            }
        }
        yOffset += 40;

        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "B:");

        // Синхронизация для Ellipse B
        if (!state.editEllipseB && state.ellipseB != ellipseBBuffer) {
            strncpy(ellipseBBuffer, state.ellipseB.c_str(), sizeof(ellipseBBuffer) - 1);
            ellipseBBuffer[sizeof(ellipseBBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, ellipseBBuffer, sizeof(ellipseBBuffer) - 1, state.editEllipseB)) {
            state.editEllipseB = !state.editEllipseB;
            if (!state.editEllipseB) {
                state.ellipseB = ellipseBBuffer;
            }
        }
        yOffset += 40;
    }
    else if (auto helix = std::dynamic_pointer_cast<Helix3D>(state.curves[state.selectedCurve]))
    {
        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Radius:");

        // Синхронизация для Helix Radius
        if (!state.editHelixRadius && state.helixRadius != helixRadiusBuffer) {
            strncpy(helixRadiusBuffer, state.helixRadius.c_str(), sizeof(helixRadiusBuffer) - 1);
            helixRadiusBuffer[sizeof(helixRadiusBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, helixRadiusBuffer, sizeof(helixRadiusBuffer) - 1, state.editHelixRadius)) {
            state.editHelixRadius = !state.editHelixRadius;
            if (!state.editHelixRadius) {
                state.helixRadius = helixRadiusBuffer;
            }
        }
        yOffset += 40;

        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Step:");

        // Синхронизация для Helix Step
        if (!state.editHelixStep && state.helixStep != helixStepBuffer) {
            strncpy(helixStepBuffer, state.helixStep.c_str(), sizeof(helixStepBuffer) - 1);
            helixStepBuffer[sizeof(helixStepBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, helixStepBuffer, sizeof(helixStepBuffer) - 1, state.editHelixStep)) {
            state.editHelixStep = !state.editHelixStep;
            if (!state.editHelixStep) {
                state.helixStep = helixStepBuffer;
            }
        }
        yOffset += 40;

        GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Turns:");

        // Синхронизация для Helix Turns
        if (!state.editHelixTurns && state.helixTurns != helixTurnsBuffer) {
            strncpy(helixTurnsBuffer, state.helixTurns.c_str(), sizeof(helixTurnsBuffer) - 1);
            helixTurnsBuffer[sizeof(helixTurnsBuffer) - 1] = '\0';
        }

        if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 200, 25 }, helixTurnsBuffer, sizeof(helixTurnsBuffer) - 1, state.editHelixTurns)) {
            state.editHelixTurns = !state.editHelixTurns;
            if (!state.editHelixTurns) {
                state.helixTurns = helixTurnsBuffer;
            }
        }
        yOffset += 40;
    }

    // Позиция
    GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Position:");

    // Синхронизация для Position X
    if (!state.editPosX && state.posX != posXBuffer) {
        strncpy(posXBuffer, state.posX.c_str(), sizeof(posXBuffer) - 1);
        posXBuffer[sizeof(posXBuffer) - 1] = '\0';
    }

    if (GuiTextBox({ editWindow.x + 120, editWindow.y + yOffset, 60, 25 }, posXBuffer, sizeof(posXBuffer) - 1, state.editPosX)) {
        state.editPosX = !state.editPosX;
        if (!state.editPosX) {
            state.posX = posXBuffer;
        }
    }

    GuiLabel({ editWindow.x + 190, editWindow.y + yOffset, 20, 25 }, "Y:");

    // Синхронизация для Position Y
    if (!state.editPosY && state.posY != posYBuffer) {
        strncpy(posYBuffer, state.posY.c_str(), sizeof(posYBuffer) - 1);
        posYBuffer[sizeof(posYBuffer) - 1] = '\0';
    }

    if (GuiTextBox({ editWindow.x + 210, editWindow.y + yOffset, 60, 25 }, posYBuffer, sizeof(posYBuffer) - 1, state.editPosY)) {
        state.editPosY = !state.editPosY;
        if (!state.editPosY) {
            state.posY = posYBuffer;
        }
    }

    GuiLabel({ editWindow.x + 280, editWindow.y + yOffset, 20, 25 }, "Z:");

    // Синхронизация для Position Z
    if (!state.editPosZ && state.posZ != posZBuffer) {
        strncpy(posZBuffer, state.posZ.c_str(), sizeof(posZBuffer) - 1);
        posZBuffer[sizeof(posZBuffer) - 1] = '\0';
    }

    if (GuiTextBox({ editWindow.x + 300, editWindow.y + yOffset, 60, 25 }, posZBuffer, sizeof(posZBuffer) - 1, state.editPosZ)) {
        state.editPosZ = !state.editPosZ;
        if (!state.editPosZ) {
            state.posZ = posZBuffer;
        }
    }
    yOffset += 40;

    // Вращение
    GuiLabel({ editWindow.x + 20, editWindow.y + yOffset, 100, 25 }, "Rotation:");

    // Синхронизация для Rotation X
    if (!state.editRotX && state.rotX != rotXBuffer) {
        strncpy(rotXBuffer, state.rotX.c_str(), sizeof(rotXBuffer) - 1);
        rotXBuffer[sizeof(rotXBuffer) - 1] = '\0';
    }
    GuiLabel({ editWindow.x + 120, editWindow.y + yOffset, 20, 25 }, "X:");
    if (GuiTextBox({ editWindow.x + 140, editWindow.y + yOffset, 60, 25 }, rotXBuffer, sizeof(rotXBuffer) - 1, state.editRotX)) {
        state.editRotX = !state.editRotX;
        if (!state.editRotX) {
            state.rotX = rotXBuffer;
        }
    }

    // Синхронизация для Rotation Y
    if (!state.editRotY && state.rotY != rotYBuffer) {
        strncpy(rotYBuffer, state.rotY.c_str(), sizeof(rotYBuffer) - 1);
        rotYBuffer[sizeof(rotYBuffer) - 1] = '\0';
    }
    GuiLabel({ editWindow.x + 210, editWindow.y + yOffset, 20, 25 }, "Y:");
    if (GuiTextBox({ editWindow.x + 230, editWindow.y + yOffset, 60, 25 }, rotYBuffer, sizeof(rotYBuffer) - 1, state.editRotY)) {
        state.editRotY = !state.editRotY;
        if (!state.editRotY) {
            state.rotY = rotYBuffer;
        }
    }

    // Синхронизация для Rotation Z
    if (!state.editRotZ && state.rotZ != rotZBuffer) {
        strncpy(rotZBuffer, state.rotZ.c_str(), sizeof(rotZBuffer) - 1);
        rotZBuffer[sizeof(rotZBuffer) - 1] = '\0';
    }
    GuiLabel({ editWindow.x + 300, editWindow.y + yOffset, 20, 25 }, "Z:");
    if (GuiTextBox({ editWindow.x + 320, editWindow.y + yOffset, 60, 25 }, rotZBuffer, sizeof(rotZBuffer) - 1, state.editRotZ)) {
        state.editRotZ = !state.editRotZ;
        if (!state.editRotZ) {
            state.rotZ = rotZBuffer;
        }
    }

    // Кнопки Save/Cancel
    if (GuiButton({ editWindow.x + 100, editWindow.y + 350, 80, 30 }, "Save"))
    {
        try {
            double x = std::stod(state.posX);
            double y = std::stod(state.posY);
            double z = std::stod(state.posZ);
            Point3D position(x, y, z);

            double rx = std::stod(state.rotX);
            double ry = std::stod(state.rotY);
            double rz = std::stod(state.rotZ);
            Point3D rotation(rx, ry, rz);

            if (auto circle = std::dynamic_pointer_cast<Circle3D>(state.curves[state.selectedCurve]))
            {
                double radius = std::stod(state.circleRadius);
                auto newCircle = std::make_shared<Circle3D>(radius);
                newCircle->setPosition(position);
                newCircle->setRotation(rotation);
                state.curves[state.selectedCurve] = newCircle;
            }
            else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(state.curves[state.selectedCurve]))
            {
                double a = std::stod(state.ellipseA);
                double b = std::stod(state.ellipseB);
                auto newEllipse = std::make_shared<Ellipse3D>(a, b);
                newEllipse->setPosition(position);
                newEllipse->setRotation(rotation);
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
                helix->setRotation(rotation);
            }
            state.showEditWindow = false;
        }
        catch (const std::exception& e) {
            (void)e;
            state.showEditWindow = false;
        }
    }

    if (GuiButton({ editWindow.x + 220, editWindow.y + 350, 80, 30 }, "Cancel"))
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