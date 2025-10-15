#include "Circle.h"

Circle3D::Circle3D(double radius)
    : radius(radius) {  }

Point3D Circle3D::getPoint(double t) const
{
    double x = radius * cos(t);
    double y = radius * sin(t);
    double z = 0.0;
    return Point3D(x, y, z);
}

Point3D Circle3D::getDerivative(double t) const
{
    double dx = -radius * sin(t);
    double dy = radius * cos(t);
    double dz = 0.0;
    return Point3D(dx, dy, dz);
}