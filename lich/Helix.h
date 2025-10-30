#pragma once
#include "Curve3D.h"

#define _USE_MATH_DEFINES
#include <cmath>

class Helix3D : public Curve3D
{
private:
    double radius;
    double step;
    int turns;
    Point3D position; // Добавлена позиция

public:
    Helix3D(double radius, double step, int turns = 5);

    Point3D getPoint(double t) const override;
    Point3D getDerivative(double t) const override;
    void setPosition(const Point3D& pos) override { position = pos; } // Реализация

    double getRadius() const { return radius; }
    double getStep() const { return step; }
    int getTurns() const { return turns; }
    Point3D getPosition() const { return position; }

    void setRadius(double r) { radius = r; }
    void setStep(double s) { step = s; }
    void setTurns(int t) { turns = t; }
};