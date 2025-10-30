#include "Ellipse.h"

Ellipse3D::Ellipse3D(double a, double b)
    : a(a), b(b), position(0, 0, 0) {
}

Point3D Ellipse3D::getPoint(double t) const
{
    double x = a * cos(t) + position.x;
    double y = b * sin(t) + position.y;
    double z = 0.0 + position.z;
    return Point3D(x, y, z);
}

Point3D Ellipse3D::getDerivative(double t) const
{
    double dx = -a * sin(t);
    double dy = b * cos(t);
    double dz = 0.0;
    return Point3D(dx, dy, dz);
}