#pragma once
#include "Curve3D.h"
#include <cmath>

class Helix3D : Curve3D
{
private:
	double radius;
	double step;

public:
	Helix3D(double radius, double step);

	Point3D getPoint(double t) const override;
	Point3D getDerivative(double t) const override;

	double getRadius(double t) const { return radius; }
	double getStep(double t) const { return step; }
};
