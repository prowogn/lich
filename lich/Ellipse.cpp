#include "Ellipse.h"

Ellipse3D::Ellipse3D(double a, double b)
	: a(a), b(b) { }

Point3D Ellipse3D::getPoint(double t) const
{
	double x = a * cos(t);
	double y = b * sin(t);
	double z = 0.0;
	return Point3D(x, y, z);
}

Point3D Ellipse3D::getDerivative(double t) const
{
	double dx = -a * sin(t);
	double dy = b * cos(t);
	double dz = 0.0;
	return Point3D(dx, dy, dz);
}