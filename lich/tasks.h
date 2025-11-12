#pragma once
#include "Point3D.h"
#include "Curve3D.h"
#include "Helix.h"
#include "Circle.h"
#include "Ellipse.h"
#include <vector>
#include <memory>

void Task1_GenerateRandomCurves(std::vector<std::shared_ptr<Curve3D>>& curves, int count = 10);
void Task3_PrintPointsAndDerivatives(const std::vector<std::shared_ptr<Curve3D>>& curves);
void Task4_5_6_CirclesOperations(const std::vector<std::shared_ptr<Curve3D>>& curves);