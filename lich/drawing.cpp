#define _USE_MATH_DEFINES

#include "drawing.h"
#include "raylib.h"
#include <cmath>
#include "Circle.h"
#include "Ellipse.h"
#include "Helix.h"

Vector3 ToVec3(const Point3D& p)
{
    return { (float)p.x, (float)p.y, (float)p.z };
}

Point3D ToPoint3D(const Vector3& v)
{
    return { v.x, v.y, v.z };
}

void DrawCurve3D(const std::shared_ptr<Curve3D>& curve, int segments, Color col)
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

void DrawAllCurves(const std::vector<std::shared_ptr<Curve3D>>& curves, int selectedCurve)
{
    for (int i = 0; i < curves.size(); ++i)
    {
        auto& c = curves[i];
        Color col = (i == selectedCurve) ? ORANGE : GRAY;
        if (std::dynamic_pointer_cast<Circle3D>(c)) col = RED;
        if (std::dynamic_pointer_cast<Ellipse3D>(c)) col = GREEN;
        if (std::dynamic_pointer_cast<Helix3D>(c)) col = BLUE;
        DrawCurve3D(c, 300, col);

        // Для выбранной кривой дополнительно рисуем маркеры
        if (i == selectedCurve)
        {
            for (int j = 0; j < 8; j++)
            {
                double t = (2.0 * M_PI * j) / 8.0;
                Point3D p = c->getPoint(t);
                DrawSphere(ToVec3(p), 0.1f, PURPLE);
            }
        }
    }
}