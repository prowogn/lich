#define _USE_MATH_DEFINES
#include "Helix.h"
#include <cmath>

Helix3D::Helix3D(double radius, double step, int turns)
    : radius(radius), step(step), turns(turns) {
}

Point3D Helix3D::getPoint(double t) const
{
    double x = radius * cos(t);
    double y = radius * sin(t);
    double z = step * t / (2 * M_PI);
    return Point3D(x, y, z);
}

Point3D Helix3D::getDerivative(double t) const
{
    double dx = -radius * sin(t);
    double dy = radius * cos(t);
    double dz = step / (2 * M_PI);
    return Point3D(dx, dy, dz);
}
