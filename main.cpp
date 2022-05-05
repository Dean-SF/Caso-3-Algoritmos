#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include "Classes/Point.h"
#include "Classes/Administrator.h"
#include "Classes/Router.h"

using namespace std;

int main() {
    int angleGrad = 60;        // 60 -> 120 -> 240 -> 300
    double angle = M_PI / 3;   //60° Grados
    double angleTransformed = angleGrad *(M_PI/180);

    int frames = 4;
    
    string fileName = "svgcaso3.svg";

    vector<Point> points;
    vector<string> colors;

    points.emplace_back(Point(50,100));
    points.emplace_back(Point(100,200));
    points.emplace_back(Point(100,320));
    points.emplace_back(Point(350,200));
    points.emplace_back(Point(350,400));
    points.emplace_back(Point(250,400));

    colors.emplace_back("#FF2970");
    colors.emplace_back("#D10257");
    colors.emplace_back("#5349B7");
    colors.emplace_back("#3B38A5");
    colors.emplace_back("#3B38A5");
    colors.emplace_back("#D10257");
    colors.emplace_back("#37167F");
    colors.emplace_back("#2EB9FF");
    colors.emplace_back("#0097F4");

    Administrator *admin = new Administrator(fileName, points, colors, angleTransformed, frames);
    
    admin->startAnimationProcess();

    return 0;
}
