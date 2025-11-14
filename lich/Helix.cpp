#define _USE_MATH_DEFINES
#include "Helix.h"
#include <cmath>

Helix3D::Helix3D(double radius, double step, int turns)
    : radius(radius), step(step), turns(turns), position(0, 0, 0), rotation(0, 0, 0) {
}

Point3D Helix3D::getPoint(double t) const
{
    double scaled_t = t * turns;
    double x = radius * cos(scaled_t);
    double y = radius * sin(scaled_t);
    double z = step * scaled_t / (2 * M_PI);

    double cosX = cos(rotation.x), sinX = sin(rotation.x);
    double cosY = cos(rotation.y), sinY = sin(rotation.y);
    double cosZ = cos(rotation.z), sinZ = sin(rotation.z);

    double x1 = x * cosZ - y * sinZ;
    double y1 = x * sinZ + y * cosZ;
    double z1 = z;

    double x2 = x1 * cosY + z1 * sinY;
    double y2 = y1;
    double z2 = -x1 * sinY + z1 * cosY;

    double x3 = x2;
    double y3 = y2 * cosX - z2 * sinX;
    double z3 = y2 * sinX + z2 * cosX;

    return Point3D(x3 + position.x, y3 + position.y, z3 + position.z);
}

Point3D Helix3D::getDerivative(double t) const
{
    double scaled_t = t * turns;
    double dx = -radius * turns * sin(scaled_t);
    double dy = radius * turns * cos(scaled_t);
    double dz = (step * turns) / (2 * M_PI);

    double cosX = cos(rotation.x), sinX = sin(rotation.x);
    double cosY = cos(rotation.y), sinY = sin(rotation.y);
    double cosZ = cos(rotation.z), sinZ = sin(rotation.z);

    double dx1 = dx * cosZ - dy * sinZ;
    double dy1 = dx * sinZ + dy * cosZ;
    double dz1 = dz;

    double dx2 = dx1 * cosY + dz1 * sinY;
    double dy2 = dy1;
    double dz2 = -dx1 * sinY + dz1 * cosY;

    double dx3 = dx2;
    double dy3 = dy2 * cosX - dz2 * sinX;
    double dz3 = dy2 * sinX + dz2 * cosX;

    return Point3D(dx3, dy3, dz3);
}