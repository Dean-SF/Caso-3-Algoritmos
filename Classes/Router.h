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
    vector<Point> coordinates; // coordinates from each mask->rect
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
                    coordinates.emplace_back(Point(coordinate,0));
                    break;
                case 2: // initial angle = 90
                    coordinate = stoi((*nodeIterator).child("mask").child("rect").attribute("y").value());
                    distances.emplace_back(new Point(0,-(coordinate/frames)));
                    coordinates.emplace_back(Point(0,coordinate));
                    break;
                case 3: // initial angle = 180
                    coordinate = stoi((*nodeIterator).child("mask").child("rect").attribute("x").value());
                    distances.emplace_back(new Point(-(coordinate/frames),0));
                    coordinates.emplace_back(Point(coordinate,0));
                    break;
                case 4: // initial angle = 270
                    coordinate = stoi((*nodeIterator).child("mask").child("rect").attribute("y").value());
                    distances.emplace_back(new Point(0,(canvasSize.getHeight() - coordinate)/frames));
                    coordinates.emplace_back(Point(0,coordinate));
                    break;
                default: 
                    break;
            }
        }
        printDistances(); // for debugging
        
        if (getTypeOfRoute() == TypeOfRoute::curvedRoute) 
            calculateCurvedRoute();
        
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
            coordinates.emplace_back(Point(xAxis,yAxis));

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
        printDistances(); // REMOVE LATER!

        if (getTypeOfRoute() == TypeOfRoute::curvedRoute) 
            calculateCurvedRoute();
        
        notify(docPointer, &distances);
    }


    void calculateCurvedRoute() {
        vector<vector<Point*>> distancesForCurvedRoute;
    
        for (int pointIterator = 0; pointIterator < coordinates.size(); pointIterator++) {
                int yAxisFinalPoint = coordinates[pointIterator].getVerticalAxis() + distances[pointIterator]->getVerticalAxis() * frames;
                
                if (distances[pointIterator]->getVerticalAxis() < 0) 
                    yAxisFinalPoint = coordinates[pointIterator].getVerticalAxis() + (distances[pointIterator]->getVerticalAxis() * -1) * frames;

                int xAxisFinalPoint = coordinates[pointIterator].getHorizontalAxis() + (distances[pointIterator]->getHorizontalAxis() * frames);
                
                Point *initialPoint = new Point(coordinates[pointIterator].getHorizontalAxis(), coordinates[pointIterator].getVerticalAxis());
                Point *finalPoint = new Point(xAxisFinalPoint,yAxisFinalPoint);

                double slope = ((double)finalPoint->getVerticalAxis() - (double)initialPoint->getVerticalAxis()) / 
                ((double)finalPoint->getHorizontalAxis() - (double)initialPoint->getHorizontalAxis());

                int xAxisMediumPoint = (initialPoint->getHorizontalAxis() + finalPoint->getHorizontalAxis()) / 2;
                int yAxisMediumPoint = (initialPoint->getVerticalAxis() + finalPoint->getVerticalAxis()) / 2;
                Point *mediumPoint = new Point(xAxisMediumPoint, yAxisMediumPoint);

                int squareSize = (getWidth() + 50 + getHeight() + 50) / 40;
                double perpendicularSlope = 1 - slope;
                double bFunction = mediumPoint->getVerticalAxis() - (perpendicularSlope * mediumPoint->getHorizontalAxis());
                
                int newXAxis = mediumPoint->getHorizontalAxis() + squareSize;
                int newYAxis = perpendicularSlope * newXAxis + bFunction;   // y = mx + b
            
                Point *guideForCurve = new Point(newXAxis, newYAxis);

                vector<Point*> pointTriplets;
                pointTriplets.emplace_back(initialPoint);
                pointTriplets.emplace_back(finalPoint);
                pointTriplets.emplace_back(guideForCurve);
                distancesForCurvedRoute.emplace_back(pointTriplets);
        }
        printPointsCurvedRoute(distancesForCurvedRoute);    // REMOVE LATER!
        notify(docPointer, &distancesForCurvedRoute);
    }

    // For debug only
    void printPointsCurvedRoute(vector<vector<Point*>> dist) {
        for (int i = 0; i < dist.size(); i++) {
            cout << "--------- Para el punto " << i << "--------" << endl;
            for (int j = 0; j < 3; j++) {
                cout << "(" << dist[i][j]->getHorizontalAxis() << "," << dist[i][j]->getVerticalAxis() << ")" << endl;
            }
        }
    }

public:
    Router(double pAngle, int pFrames, TypeOfRoute pTypeOfRoute) {
        animator = nullptr;
        processId = 1;
        angle = pAngle;
        frames = pFrames;
        typeOfRoute = pTypeOfRoute;
        docPointer = nullptr;
        
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

    vector<Point> getCoordinates() {
        return coordinates;
    }

    void setCoordinates(vector<Point> pCoordinates) {
        coordinates = pCoordinates;
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
        canvasSize.setViewBoxResolution(pDocPointer->child("svg").attribute("viewBox").value(),true); // set canvas size;
        // vector<Point> *originalPointsPointer = (vector<Point>*)pCoordinates;
        // vector<Point> originalPoints = originalPointsPointer[0];
        // setCoordinates(originalPoints);
        calculateQuadrantNormalCase();
    }

};

#endif // ROUTER_H
