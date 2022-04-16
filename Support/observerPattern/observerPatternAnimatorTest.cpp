#define _USE_MATH_DEFINES
#include <list>
#include <iostream>
#include <fstream>
#include <cmath>
#include "../../Classes/Animator.h"
#include "../../Classes/Selector.h"
#include "../../Classes/Router.h"
#include "../../Classes/Generator.h"
#include "../../Classes/TypeOfRoute.h"
#include "../../libraries/pugixml/pugixml.hpp"

using namespace std;
using namespace pugi;

int main() {
    xml_document doc;
    doc.load_file("./SVGPLANTILLA.svg");
    
    xml_document *docPointer = &doc;

    Selector* selection = new Selector();

    int angleGrad = 350;        // 60 -> 120 -> 240 -> 300

    double angle = M_PI / 3;   //60° Grados

    double angleTransformed = angleGrad *(M_PI/180);
    

    Router* routing = new Router(angleTransformed, 4, TypeOfRoute::straightRoute, docPointer);

    Generator* generation = new Generator();
    Animator* animator = new Animator();

    selection->attach(animator); // animator observa a selection
    routing->attach(animator);   // animator observa a routing

    animator->attach(selection);     // selection observa a animator
    animator->attach(routing);       // routing observa a animator
    animator->attach(generation);    // generation observa a animator

    animator->startProcess(docPointer);

    return 0;
}
