#ifndef ROUTER_H
#define ROUTER_H
#define _USE_MATH_DEFINES

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include "Point.h"
#include "TypeOfRoute.h"
#include "ObserverPattern.h"
#include "../../libraries/pugixml/pugixml.hpp"

using std::cout;
using std::endl;
using std::list;
using std::string;
using std::vector;
using namespace pugi;
using std::stringstream;

class Router : public Subject, public Observer {
private:
    Observer* animator;
    TypeOfRoute typeOfRoute;
    int processId;
    float angle;
    int width;
    int height;
    int frames;

public:
    Router(float pAngle, int pFrames, TypeOfRoute pTypeOfRoute, xml_document* pDocPointer) {
        animator = nullptr;
        processId = 1;
        angle = pAngle;
        frames = pFrames;
        typeOfRoute = pTypeOfRoute;
        calculateWidthAndHeight(pDocPointer);
    }
    ~Router() {} 

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }

    int getFrames() {
        return frames;
    }

    float getAngle() {
        return angle;
    }

    int getProcessId() {
        return processId;
    }

    void setWidth(int pWidth) {
        width = pWidth;
    }

    void setHeight(int pHeight) {
        height = pHeight;
    }

    void setAngle(float pAngle) {
        angle = pAngle;
    }

    void setFrames(int pFrames) {
        frames = pFrames;
    }

    TypeOfRoute getTypeOfRoute() {
        return typeOfRoute;
    }

    void setProcessId(int pProcessId) {
        processId = pProcessId;
    }

    // Assign given pointer to an observer as the class' animator
    void attach(Observer* pAnimator) {
        animator = pAnimator;
    }

    // Delete class' animator
    void detach(Observer* pAnimator) {
        delete animator;
    }

    void setTypeOfRoute(TypeOfRoute pTypeOfRoute) {
        typeOfRoute = pTypeOfRoute;
    }

    // Notify the animator that Router finished it's job 
    void notify(xml_document* pDocPointer) {
        cout << "Router is done" << endl;
        cout << "--------------------------" << endl;
        animator->update(pDocPointer);
    }

    void update(xml_document* pDocPointer) {
        cout << "--------------------------" << endl;
        cout << "Router started working" << endl;
        verifySpecialCase(angle, pDocPointer);
    }

    void calculateWidthAndHeight(xml_document* pDocPointer) {
        string strViewbox = pDocPointer->child("svg").attribute("viewBox").value();
        stringstream stringManipulator(strViewbox);
        string extractedString;

        stringManipulator >> extractedString; // min-x
        stringManipulator >> extractedString; // min-y

        stringManipulator >> extractedString;
        int widthResult = stoi(extractedString);
        stringManipulator >> extractedString;
        int heightResult = stoi(extractedString);

        width = widthResult;
        height = heightResult;
    }

    void verifySpecialCase(float pAngle, xml_document* pDocPointer) {
        bool isSpecial = false;
        int quadrant;
    
        if (pAngle == 0) {
            quadrant = 1;
            isSpecial = true;
        } else if (pAngle == (float)(M_PI)) {
            angle = 0;
            quadrant = 2;
            isSpecial = true;
        } else if (pAngle == (float)(3*M_PI/2)) {
            angle = pAngle - M_PI;
            quadrant = 4;
            isSpecial = true;
        } else if (pAngle == (float)(M_PI/2)) {
            quadrant = 1;
            isSpecial = true;
        }
    
        if (isSpecial)
            calculateRouteForSpecialCase(quadrant, pDocPointer);
        else
            calculateQuadrantNormalCase(angle, pDocPointer);
    }

    int calculateQuadrantNormalCase(float pAngle, xml_document* pDocPointer) {
        float rightLimit, leftLimit, upperLimit, lowerLimit;
        leftLimit = M_PI;                   // 180°
        rightLimit = 2 * M_PI;              // 360° 
        upperLimit = (3 * M_PI) / 2;        // 270°
        lowerLimit = M_PI / 2;              // 90°
        int quadrant;

        if (pAngle > 0 && pAngle < lowerLimit) // 0° < pAngle < 90°
            quadrant = 1;  // (+,+) -> Is treated as (+,-)

        else if (pAngle > lowerLimit && pAngle < leftLimit) // 90° < pAngle < 180°
            quadrant = 2;  // (-,+) -> Is treated as (-,-)

        else if (pAngle > leftLimit && pAngle < upperLimit) // 180° < pAngle < 270°
            quadrant = 3;  // (-,-) -> Is treated as (-,+)

        else if (pAngle > upperLimit && pAngle < rightLimit) // 270° < pAngle < 360°
            quadrant = 4;   // (+,-) -> Is treated as (+,+)

        // Dependiendo del cuadrante se va a obtener el ángulo adyacente al eje x
        switch (quadrant) {  
            case 3:
                angle = leftLimit - pAngle;
                break;
            case 2:
                angle = pAngle - leftLimit;
                break;
            case 1:
                angle = rightLimit - pAngle;
                break;
            default: 
                break; 
        }
        calculateRouteForNormalCase(quadrant, pDocPointer);
    }

    void calculateRouteForSpecialCase(int quadrant, xml_document* pDocPointer) {
        cout << "Process for special case" << endl;
        // to do
        notify(pDocPointer);
    }

    void calculateRouteForNormalCase(int quadrant, xml_document* pDocPointer) {        
        vector<int> distances;

        cout << "Cuadrant = " << quadrant << endl;
        cout << "Angle = " << angle << endl;

        int xAxis, yAxis;
        string strXAxis, strYAxis;
        xml_node node;
        auto nodeIteratorParent = pDocPointer->child("svg").begin();
        
        for(; nodeIteratorParent != pDocPointer->child("svg").end(); nodeIteratorParent++) {
            if (string(nodeIteratorParent->name()) == "svg") {
                node = *nodeIteratorParent;
                strXAxis = node.child("mask").child("rect").attribute("x").value();
                strYAxis = node.child("mask").child("rect").attribute("y").value();

                xAxis = stoi(strXAxis);
                yAxis = stoi(strYAxis);

                cout << "(" << strXAxis << "," << strYAxis << ")" << endl;
            }
        }

        notify(pDocPointer);
    }

};

#endif // ROUTER_H
