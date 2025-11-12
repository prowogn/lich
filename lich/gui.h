#pragma once
#include "Point3D.h"
#include "Curve3D.h"
#include "Helix.h"
#include "Circle.h"
#include "Ellipse.h"
#include <vector>
#include <memory>

struct AppState {
    std::vector<std::shared_ptr<Curve3D>> curves;
    int selectedCurve;
    bool panelVisible;
    bool guiMode;
    bool showAddWindow;
    bool showEditWindow;
    bool showTaskWindow;

    // Переменные для добавления/редактирования
    int curveType;
    char circleRadius[32];
    char ellipseA[32];
    char ellipseB[32];
    char helixRadius[32];
    char helixStep[32];
    char helixTurns[32];
    char posX[32];
    char posY[32];
    char posZ[32];
    char tValue[32];

    // Флаги редактирования
    bool editCircleRadius, editEllipseA, editEllipseB;
    bool editHelixRadius, editHelixStep, editHelixTurns;
    bool editPosX, editPosY, editPosZ;
    bool editTValue;
};

void InitializeAppState(AppState& state);
void FillEditFieldsFromSelectedCurve(AppState& state);
void DrawMainPanel(AppState& state);
void HandleAddWindow(AppState& state);
void HandleEditWindow(AppState& state);
void HandleTaskWindow(AppState& state);