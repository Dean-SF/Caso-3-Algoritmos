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
#include "Resolution.h"
#include "Point.h"
#include "TypeOfRoute.h"
#include "ObserverPattern.h"

using std::cout;
using std::endl;
using std::list;
using std::stoi;
using std::string;
using std::vector;
using namespace pugi;
using std::stringstream;

class Router : public Subject, public Observer {
private:
    Observer* animator;
    TypeOfRoute typeOfRoute;
    xml_document* docPointer;
    vector<Point*> distances; 
    int processId;
    
    Resolution canvasSize; 
    double angle;
    int frames;
    int quadrant; 


    // Debug only, remove for final release just to print the resulting vector
    void printDistances() {
        for (Point *actual : distances) {
             cout << "(" << actual->getHorizontalAxis() << "," << actual->getVerticalAxis() << ")" << endl;
        }
    }


    void calculateQuadrantNormalCase() {
        double rightLimit, leftLimit, upperLimit, lowerLimit;
        leftLimit = M_PI;                   // 180°
        rightLimit = 2 * M_PI;              // 360° 
        upperLimit = (3 * M_PI) / 2;        // 270°
        lowerLimit = M_PI / 2;              // 90°

        if (angle >= 0 && angle < lowerLimit) // 0° <= angle < 90°
            quadrant = 1;  // (+,+) -> Is treated as (+,-)

        else if (angle >= lowerLimit && angle < leftLimit) // 90° <= angle < 180°
            quadrant = 2;  // (-,+) -> Is treated as (-,-)

        else if (angle >= leftLimit && angle < upperLimit) // 180° <= angle < 270°
            quadrant = 3;  // (-,-) -> Is treated as (-,+)

        else if (angle >= upperLimit && angle < rightLimit) // 270° <= angle < 360°
            quadrant = 4;   // (+,-) -> Is treated as (+,+)

        // Dependiendo del cuadrante se va a obtener el ángulo adyacente al eje x ( Angulo Referencial :> )
        switch (quadrant) {  
            case 2:
                angle = leftLimit - angle;
                break;
            case 3:
                angle = angle - leftLimit;
                break;
            case 4:
                angle = rightLimit - angle;
                break;
            default: 
                break; 
        }
        if (angle == 0 || angle == (double)(M_PI/2))
            calculateRouteForSpecialCase();
        else
            calculateRouteForNormalCase();
    }

    void calculateRouteForSpecialCase() {

        // Debugging
        cout << "Process for special case" << endl;
        
        cout << "Cuadrant = " << quadrant << endl;
        cout << "Angle = " << angle << endl;

        xml_node mainSvgNode = docPointer->child("svg").last_child(); 
        xml_node_iterator nodeIterator; 
        for(nodeIterator = mainSvgNode.begin(); nodeIterator != mainSvgNode.end(); nodeIterator++) {
            int coordinate;

            switch (quadrant) {
                case 1: // initial angle = 0
                    coordinate = stoi((*nodeIterator).child("mask").child("rect").attribute("x").value());
                    distances.emplace_back(new Point((canvasSize.getWidth() - coordinate)/frames,0));
                    break;
                case 2: // initial angle = 90
                    coordinate = stoi((*nodeIterator).child("mask").child("rect").attribute("y").value());
                    distances.emplace_back(new Point(0,-(coordinate/frames)));
                    break;
                case 3: // initial angle = 180
                    coordinate = stoi((*nodeIterator).child("mask").child("rect").attribute("x").value());
                    distances.emplace_back(new Point(-(coordinate/frames),0));
                    break;
                case 4: // initial angle = 270
                    coordinate = stoi((*nodeIterator).child("mask").child("rect").attribute("y").value());
                    distances.emplace_back(new Point(0,(canvasSize.getHeight() - coordinate)/frames));
                    break;
                default: 
                    break;
            }
        }
        printDistances(); // for debugging
        notify(docPointer, &distances);
    }

    void calculateRouteForNormalCase() {        
        
        cout << "--- CASO NORMAL ---" << endl;
        cout << "Frames = " << frames << endl;
        cout << "Cuadrant = " << quadrant << endl;
        cout << "Angle = " << angle << endl;
        
        int xAxis,yAxis; 
        
        double firstLeg,secondLeg,slope,linearConstant; // leg -> cateto

        xml_node node;
        xml_node mainSvgNode = docPointer->child("svg").last_child();
        xml_node_iterator nodeIteratorParent;
        for(nodeIteratorParent = mainSvgNode.begin(); nodeIteratorParent != mainSvgNode.end(); nodeIteratorParent++) {
            node = *nodeIteratorParent;
            xAxis = stoi(node.child("mask").child("rect").attribute("x").value());
            yAxis = stoi(node.child("mask").child("rect").attribute("y").value());
        
            if (quadrant == 1 || quadrant == 4) {
                firstLeg = canvasSize.getWidth() - xAxis;
                secondLeg = tan(angle)*firstLeg;
            } else {
                firstLeg = xAxis;
                secondLeg = tan(angle)*firstLeg;
            }

            if ((secondLeg > yAxis && (quadrant == 1 || quadrant == 2)) || 
               (secondLeg + yAxis > canvasSize.getHeight() && (quadrant == 3 || quadrant == 4))) {
                slope = (secondLeg)/(firstLeg);     
                linearConstant = yAxis-(slope*xAxis);
                if(quadrant == 1 || quadrant == 2) {
                    secondLeg = yAxis;
                    firstLeg = ((((yAxis*2)-linearConstant)/slope) - xAxis);
                } else {
                    secondLeg = canvasSize.getHeight() - yAxis;
                    firstLeg = (((canvasSize.getHeight()-linearConstant)/slope) - xAxis);
                }
            }
            
            if(quadrant == 2 || quadrant == 3)
                firstLeg *= -1;
            if(quadrant == 1 || quadrant == 2)
                secondLeg *=-1;

            distances.emplace_back(new Point(firstLeg/frames,secondLeg/frames));
        }
        printDistances(); // remove letter in final release
        notify(docPointer, &distances);
    }

public:
    Router(double pAngle, int pFrames, TypeOfRoute pTypeOfRoute, xml_document* pDocPointer) {
        animator = nullptr;
        processId = 1;
        angle = pAngle;
        frames = pFrames;
        typeOfRoute = pTypeOfRoute;
        docPointer = nullptr;
        canvasSize.setViewBoxResolution(pDocPointer->child("svg").attribute("viewBox").value(),true); // set canvas size;
    }
    ~Router() {} 

    int getWidth() {
        return canvasSize.getWidth();
    }

    int getHeight() {
        return canvasSize.getHeight();
    }

    int getFrames() {
        return frames;
    }

    double getAngle() {
        return angle;
    }

    int getProcessId() {
        return processId;
    }

    void setWidth(int pWidth) {
        canvasSize.setWidth(pWidth);    
    }

    void setHeight(int pHeight) {
        canvasSize.setHeight(pHeight);
    }

    void setAngle(double pAngle) {
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

    xml_document* getDocPointer() {
        return docPointer;
    }

    void setDocPointer(xml_document* pDocPointer) {
        docPointer = pDocPointer;
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
    void notify(xml_document* pDocPointer, void* pCoordinates) {
        cout << "Router is done" << endl;
        cout << "--------------------------" << endl;

        animator->update(pDocPointer, pCoordinates);
    }

    void update(xml_document* pDocPointer, void* pCoordinates) {
        cout << "--------------------------" << endl;
        cout << "Router started working" << endl;

        setDocPointer(pDocPointer);
        calculateQuadrantNormalCase();
    }

};

#endif // ROUTER_H
