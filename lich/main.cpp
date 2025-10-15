#define _USE_MATH_DEFINES
#include "raylib.h"
#include "Helix.h"
#include "Circle.h"
#include "Ellipse.h"
#include <vector>
#include <memory>
#include <cmath>

int main()
{
    InitWindow(1280, 720, "3D Curves Visualization");
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.position = { 10.0f, 10.0f, 10.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    std::vector<std::shared_ptr<Curve3D>> curves;

    // Спираль с 2 радиусом, шагом 0.5 и 10 витками
    curves.push_back(std::make_shared<Helix3D>(1.0, 1.0, 5));

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawGrid(40, 1.0f);

        // отрисовка
        for (auto& curve : curves)
        {
            const int segmentsPerTurn = 100;
            auto* helix = dynamic_cast<Helix3D*>(curve.get());
            double turns = helix ? helix->getTurns() : 1.0;
            int totalSegments = (int)(segmentsPerTurn * turns);

            Point3D prev = curve->getPoint(0);
            for (int i = 1; i <= totalSegments; i++)
            {
                double t = (2 * M_PI * turns) * i / totalSegments;
                Point3D p = curve->getPoint(t);
                DrawLine3D(
                    { (float)prev.x, (float)prev.y, (float)prev.z },
                    { (float)p.x, (float)p.y, (float)p.z },
                    BLUE
                );
                prev = p;
            }
        }

        EndMode3D();

        DrawText("Use mouse and WASD to move camera", 10, 10, 20, DARKGRAY);
        DrawText("Helix: radius=2, step=0.5, turns=10", 10, 40, 20, DARKBLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
