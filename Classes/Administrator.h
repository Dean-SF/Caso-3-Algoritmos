#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "Animator.h"
#include "TypeOfRoute.h"
#include "Selector.h"
#include "Router.h"
#include "Generator.h"
#include "Point.h"
#include "../../libraries/pugixml/pugixml.hpp"
#include <string>
#include <random>
#include <iostream>
#include <time.h>

using namespace pugi;
using std::rand;
using std::cout;
using std::endl;

class Administrator {
private:
    Animator *animator;
    Selector *selector;
    Router *router;
    Generator *generator;
    xml_document *docPointer;
    vector<Point> *points;

    // Choose a type of route randomly, generate random number from 0 to 1 -> 0 = straight, 1 = curved
    TypeOfRoute chooseRandomTypeOfRoute() {
        srand (time(0));
        int chooseRandomRoute = rand() % 2;
        TypeOfRoute typeOfRoute;
        if (chooseRandomRoute == 0) 
            typeOfRoute = TypeOfRoute::straightRoute;
        else
            typeOfRoute = TypeOfRoute::curvedRoute;
        return typeOfRoute;
    }

public:
    Administrator(xml_document* pDocPointer, vector<Point> *pPoints, vector<string> *pColors, double pAngle, int pFrames) {
        docPointer = pDocPointer;
        points = pPoints;
        TypeOfRoute typeOfRoute = chooseRandomTypeOfRoute();

        selector = new Selector();
        router = new Router(pAngle, pFrames, typeOfRoute, pDocPointer);
        generator = new Generator();
        animator = new Animator();
        
        selector->attach(animator); // animator observes selector
        router->attach(animator);   // animator observes router

        animator->attach(selector);     // selector observes animator
        animator->attach(router);       // router observes animator
        animator->attach(generator);    // generator observes animator
    }

    ~Administrator() {
        delete selector;
        delete router;
        delete generator;
        delete animator;
    }

    void startAnimationProcess() {
        animator->notify(docPointer, points);
    }

};

#endif