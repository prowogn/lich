#define _USE_MATH_DEFINES

#include "tasks.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <cmath>

void Task1_GenerateRandomCurves(std::vector<std::shared_ptr<Curve3D>>& curves, int count)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> typeDist(0, 2);
    std::uniform_real_distribution<> paramDist(0.5, 5.0);
    std::uniform_real_distribution<> posDist(-3.0, 3.0);

    curves.clear();
    for (int i = 0; i < count; ++i)
    {
        int type = typeDist(gen);
        Point3D pos(posDist(gen), posDist(gen), posDist(gen));

        switch (type)
        {
        case 0: // Circle
        {
            double radius = paramDist(gen);
            auto circle = std::make_shared<Circle3D>(radius);
            circle->setPosition(pos);
            curves.push_back(circle);
            break;
        }
        case 1: // Ellipse
        {
            double a = paramDist(gen);
            double b = paramDist(gen);
            auto ellipse = std::make_shared<Ellipse3D>(a, b);
            ellipse->setPosition(pos);
            curves.push_back(ellipse);
            break;
        }
        case 2: // Helix
        {
            double radius = paramDist(gen);
            double step = paramDist(gen) * 0.5;
            int turns = std::uniform_int_distribution<>(3, 8)(gen);
            auto helix = std::make_shared<Helix3D>(radius, step, turns);
            helix->setPosition(pos);
            curves.push_back(helix);
            break;
        }
        }
    }
}

void Task3_PrintPointsAndDerivatives(const std::vector<std::shared_ptr<Curve3D>>& curves)
{
    std::cout << "=== Points and Derivatives at t = PI/4 ===" << std::endl;
    double t = M_PI / 4.0;

    for (int i = 0; i < curves.size(); ++i)
    {
        std::cout << "Curve " << i + 1 << ": ";
        if (auto circle = std::dynamic_pointer_cast<Circle3D>(curves[i]))
            std::cout << "Circle (r=" << circle->getRadius() << ")";
        else if (auto ellipse = std::dynamic_pointer_cast<Ellipse3D>(curves[i]))
            std::cout << "Ellipse (a=" << ellipse->getA() << ", b=" << ellipse->getB() << ")";
        else if (auto helix = std::dynamic_pointer_cast<Helix3D>(curves[i]))
            std::cout << "Helix (r=" << helix->getRadius() << ", step=" << helix->getStep() << ", turns=" << helix->getTurns() << ")";

        Point3D point = curves[i]->getPoint(t);
        Point3D derivative = curves[i]->getDerivative(t);

        std::cout << "\n  Point: " << point;
        std::cout << "\n  Derivative: " << derivative << std::endl << std::endl;
    }
}

void Task4_5_6_CirclesOperations(const std::vector<std::shared_ptr<Curve3D>>& curves)
{
    // Task 4: Create second container with circles only
    std::vector<std::shared_ptr<Circle3D>> circles;
    for (const auto& curve : curves)
    {
        if (auto circle = std::dynamic_pointer_cast<Circle3D>(curve))
        {
            circles.push_back(circle);
        }
    }

    // Task 5: Sort by radius
    std::sort(circles.begin(), circles.end(),
        [](const std::shared_ptr<Circle3D>& a, const std::shared_ptr<Circle3D>& b) {
            return a->getRadius() < b->getRadius();
        });

    // Task 6: Calculate sum of radii
    double sum = 0.0;
    for (const auto& circle : circles)
    {
        sum += circle->getRadius();
    }

    std::cout << "=== Circle Operations ===" << std::endl;
    std::cout << "Found " << circles.size() << " circles" << std::endl;
    std::cout << "Sorted radii: ";
    for (const auto& circle : circles)
    {
        std::cout << circle->getRadius() << " ";
    }
    std::cout << "\nSum of radii: " << sum << std::endl;
}