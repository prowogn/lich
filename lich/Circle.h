#pragma once
#include "Curve3D.h"
#include <cmath>

class Circle3D : public Curve3D
{
private:
    double radius;

public:
    Circle3D(double radius);

    Point3D getPoint(double t) const override;
    Point3D getDerivative(double t) const override;

    double getRadius() const { return radius; }
};
