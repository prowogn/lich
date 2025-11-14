#pragma once
#include "Point3D.h"
#include "Curve3D.h"
#include "Helix.h"
#include "Circle.h"
#include "Ellipse.h"
#include <vector>
#include <memory>
#include <string>

struct AppState {
    std::vector<std::shared_ptr<Curve3D>> curves;
    int selectedCurve;
    bool panelVisible;
    bool guiMode;
    bool showAddWindow;
    bool showEditWindow;
    bool showTaskWindow;

    int curveType;
    std::string circleRadius;
    std::string ellipseA;
    std::string ellipseB;
    std::string helixRadius;
    std::string helixStep;
    std::string helixTurns;
    std::string posX;
    std::string posY;
    std::string posZ;
    std::string rotX;
    std::string rotY;
    std::string rotZ;
    std::string tValue;

    bool editCircleRadius, editEllipseA, editEllipseB;
    bool editHelixRadius, editHelixStep, editHelixTurns;
    bool editPosX, editPosY, editPosZ;
    bool editRotX, editRotY, editRotZ;
    bool editTValue;

    Point3D currentPoint;
    Point3D currentDerivative;
    bool calculated;
};

void InitializeAppState(AppState& state);
void FillEditFieldsFromSelectedCurve(AppState& state);
void DrawMainPanel(AppState& state);
void HandleAddWindow(AppState& state);
void HandleEditWindow(AppState& state);
void HandleTaskWindow(AppState& state);