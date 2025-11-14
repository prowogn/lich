#pragma once
#include "Point3D.h"

class Curve3D
{
public:
    virtual ~Curve3D() = default;

    virtual Point3D getPoint(double t) const = 0;
    virtual Point3D getDerivative(double t) const = 0;

    virtual void setPosition(const Point3D& pos) = 0;

    virtual void setRotation(const Point3D& rot) = 0;

    virtual Point3D getRotation() const = 0;
};