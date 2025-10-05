#pragma once
#include <iostream>

class Point3D {
public:
    double x, y, z;

    Point3D();
    Point3D(double x, double y, double z);

    friend std::ostream& operator<<(std::ostream& os, const Point3D& p);
};
