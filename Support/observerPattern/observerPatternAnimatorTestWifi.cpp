#define _USE_MATH_DEFINES
#include <string>
#include <cmath>
#include "../../Classes/Point.h"
#include "../../Classes/Administrator.h"
#include "../../Classes/Router.h"

using namespace std;
using namespace pugi;

int main() {
    int angleGrad = 350;        // 60 -> 120 -> 240 -> 300
    double angle = M_PI / 3;   //60° Grados
    double angleTransformed = angleGrad *(M_PI/180);

    int frames = 4;
    
    vector<Point> points;
    vector<string> colors;

    points.emplace_back(Point(1000,500));
    points.emplace_back(Point(2500,1000));
    points.emplace_back(Point(2500,3500));

    colors.emplace_back("#000000");

    string fileName = "wifi.svg";

    Administrator *admin = new Administrator(fileName, points, colors, angleTransformed, frames);
    
    admin->startAnimationProcess();

    return 0;
}
