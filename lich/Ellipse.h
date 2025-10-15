#pragma once
#include "Curve3D.h"
#include <cmath>

class Ellipse3D : public Curve3D
{
private:
	double a;
	double b;

public: 
	Ellipse3D(double a, double b);

	Point3D getPoint(double t) const override;
	Point3D getDerivative(double t) const override;

	double getA() const { return a; }
	double getB() const { return b; }
};
