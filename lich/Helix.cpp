#define _USE_MATH_DEFINES
#include "Helix.h"
#include <cmath>

Helix3D::Helix3D(double radius, double step, int turns)
    : radius(radius), step(step), turns(turns), position(0, 0, 0) {
}

Point3D Helix3D::getPoint(double t) const
{
    // Фикс витков!!!
    double scaled_t = t * turns;
    double x = radius * cos(scaled_t) + position.x;
    double y = radius * sin(scaled_t) + position.y;
    double z = step * scaled_t / (2 * M_PI) + position.z;
    return Point3D(x, y, z);
}

Point3D Helix3D::getDerivative(double t) const
{
    double scaled_t = t * turns;
    double dx = -radius * turns * sin(scaled_t);
    double dy = radius * turns * cos(scaled_t);
    double dz = (step * turns) / (2 * M_PI);
    return Point3D(dx, dy, dz);
}