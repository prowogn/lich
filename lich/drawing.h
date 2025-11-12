#pragma once
#include "Point3D.h"
#include "Curve3D.h"
#include <vector>
#include <memory>
#include "raylib.h"

Vector3 ToVec3(const Point3D& p);
Point3D ToPoint3D(const Vector3& v);
void DrawCurve3D(const std::shared_ptr<Curve3D>& curve, int segments = 200, Color col = BLUE);
void DrawAllCurves(const std::vector<std::shared_ptr<Curve3D>>& curves, int selectedCurve);