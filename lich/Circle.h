#pragma once
#include "Curve3D.h"
#include <cmath>

class Circle3D : public Curve3D
{
private:
    double radius;
    Point3D position;
    Point3D rotation;

public:
    Circle3D(double radius);

    Point3D getPoint(double t) const override;
    Point3D getDerivative(double t) const override;
    void setPosition(const Point3D& pos) override { position = pos; }
    void setRotation(const Point3D& rot) override { rotation = rot; }
    Point3D getRotation() const override { return rotation; }

    double getRadius() const { return radius; }
    Point3D getPosition() const { return position; }
};