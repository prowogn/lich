#pragma once
#include "Curve3D.h"

#define _USE_MATH_DEFINES
#include <cmath>

class Helix3D : public Curve3D
{
private:
    double radius; // радиус ВСЕЙ спирали
    double step; // длина одного витка спирали
    int turns; // количество витков ВСЕЙ спирали

public:
    Helix3D(double radius, double step, int turns = 5);

    Point3D getPoint(double t) const override;
    Point3D getDerivative(double t) const override;

    double getRadius() const { return radius; }
    double getStep() const { return step; }
    int getTurns() const { return turns; }

    void setRadius(double r) { radius = r; }
    void setStep(double s) { step = s; }
    void setTurns(int t) { turns = t; }
};
