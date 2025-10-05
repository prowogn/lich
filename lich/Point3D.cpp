#include "Point3D.h"

Point3D::Point3D() : x(0), y(0), z(0) {}

Point3D::Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

std::ostream& operator<<(std::ostream& os, const Point3D& p)
{
    os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}
