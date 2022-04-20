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

    points.emplace_back(Point(50,100));
    points.emplace_back(Point(100,200));
    points.emplace_back(Point(100,320));
    points.emplace_back(Point(350,400));
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

    xml_document doc;
    doc.load_file("./SVGPLANTILLA.svg");
    xml_document *docPointer = &doc;

    Administrator *admin = new Administrator(docPointer, &points, &colors, angle, frames);
    admin->startAnimationProcess();

    return 0;
}
