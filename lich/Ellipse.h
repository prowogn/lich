#pragma once
#include "Curve3D.h"
#include <cmath>

class Ellipse3D : public Curve3D
{
private:
    double a;
    double b;
    Point3D position; // Добавлена позиция

public:
    Ellipse3D(double a, double b);

    Point3D getPoint(double t) const override;
    Point3D getDerivative(double t) const override;
    void setPosition(const Point3D& pos) override { position = pos; } // Реализация

    double getA() const { return a; }
    double getB() const { return b; }
    Point3D getPosition() const { return position; }
};