#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "Point.h"
#include "Router.h"
#include "Animator.h"
#include "Selector.h"
#include "Generator.h"
#include "TypeOfRoute.h"
#include <string>
#include <random>
#include <time.h>
#include <iostream>
#include "../libraries/pugixml/pugixml.hpp"

using namespace pugi;
using std::rand;
using std::cout;
using std::endl;

// This class handles the initialization of the Animator, Selector, Router and Generator 
class Administrator {
private:
    Animator *animator;
    Selector *selector;
    Router *router;
    Generator *generator;
    xml_document *docPointer;
    vector<Point> points;

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
    Administrator(string fileName, vector<Point> pPoints, vector<string> pColors, double pAngle, int pFrames) {
        docPointer = new xml_document();
        docPointer->load_file(&fileName[0]);

        points = pPoints;

        TypeOfRoute typeOfRoute = chooseRandomTypeOfRoute();

        selector = new Selector(pColors,docPointer);
        router = new Router(pAngle, pFrames, typeOfRoute);
        generator = new Generator(typeOfRoute, pFrames, fileName, docPointer, &points);
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
        delete docPointer;
    }

    void startAnimationProcess() {
        animator->notify(nullptr, &points, nullptr);    // animator starts the animation process
    }

};

#endif