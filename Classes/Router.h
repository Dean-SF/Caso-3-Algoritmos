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
        for (Point *current : distances) {
             cout << "(" << current->getHorizontalAxis() << "," << current->getVerticalAxis() << ")" << endl;
        }
    }


    void calculateQuadrant() {
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
        calculateRoute();
    }

    void calculateRoute() {
        cout << "quadrant = " << quadrant << endl;
        int pointIterator = 0;
        double partialAverageForXAxis = 0;
        double partialAverageForYAxis = 0;
        if (angle == 0 || angle == (double)(M_PI/2))
            //calculateRouteForSpecialCase(); para pruebas
            calculateRouteForSpecialCaseAux(pointIterator, partialAverageForXAxis, partialAverageForYAxis);
        else 
            //calculateRouteForNormalCase(); para pruebas
            calculateRouteForNormalCaseAux(pointIterator, partialAverageForXAxis, partialAverageForYAxis);
    }

    void* calculateRouteForNormalCaseAux(int pointIterator, int partialAverageForXAxis, int partialAverageForYAxis) {
        if (pointIterator == coordinates.size()) { 
            partialAverageForXAxis = partialAverageForXAxis/coordinates.size();
            partialAverageForYAxis = partialAverageForYAxis/coordinates.size();

            if(quadrant == 2 || quadrant == 3)
                partialAverageForXAxis *= -1;
            if(quadrant == 1 || quadrant == 2)
                partialAverageForYAxis *= -1;
        
            cout << "average for x = " << partialAverageForXAxis << endl;
            cout << "average for y = " << partialAverageForYAxis << endl;

            if (typeOfRoute == TypeOfRoute::curvedRoute) {
                calculateCurve(partialAverageForXAxis, partialAverageForYAxis);
                return nullptr;
            }
            
            for (int currentFrame = 1; currentFrame <= frames; currentFrame++) {
                distances.emplace_back(new Point(partialAverageForXAxis * currentFrame, partialAverageForYAxis * currentFrame));
            }
            notify(docPointer, &distances);
            return nullptr;
        }

        Point currentPoint = coordinates[pointIterator];
       
        cout << "(" << currentPoint.getHorizontalAxis() << "," << currentPoint.getVerticalAxis() << ")" << endl;
        
        int xAxis,yAxis; 
        double firstLeg,secondLeg,slope,linearConstant; // leg -> cateto
     
        xAxis = currentPoint.getHorizontalAxis();
        yAxis = currentPoint.getVerticalAxis();

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
        
        partialAverageForXAxis += firstLeg / frames;
        partialAverageForYAxis += secondLeg / frames;
        cout << "x = " << firstLeg / frames << endl;
        cout << "y = " << secondLeg / frames << endl;
        pointIterator++;
        calculateRouteForNormalCaseAux(pointIterator, partialAverageForXAxis, partialAverageForYAxis);
        return nullptr;
    }

    void* calculateRouteForSpecialCaseAux(int pointIterator, int partialAverageForXAxis, int partialAverageForYAxis) {
        if (pointIterator == coordinates.size()) { 
            partialAverageForXAxis = partialAverageForXAxis/coordinates.size();
            partialAverageForYAxis = partialAverageForYAxis/coordinates.size();

            if (quadrant == 2) // initial angle = 90
                partialAverageForYAxis *= -1;
            else if (quadrant == 3) // initial angle = 180
                partialAverageForXAxis *= -1;
    
            cout << "average for x Special Case = " << partialAverageForXAxis << endl;
            cout << "average for y Special Case = " << partialAverageForYAxis << endl;

            if (typeOfRoute == TypeOfRoute::curvedRoute) {
                calculateCurve(partialAverageForXAxis, partialAverageForYAxis);
                return nullptr;
            }
            
            for (int currentFrame = 1; currentFrame <= frames; currentFrame++) {
                distances.emplace_back(new Point(partialAverageForXAxis * currentFrame, partialAverageForYAxis * currentFrame));
            }
            notify(docPointer, &distances);
            return nullptr;
        }

        Point current = coordinates[pointIterator];

        int coordinate;
        switch (quadrant) {
            case 1: // initial angle = 0
                coordinate = current.getHorizontalAxis();
                //distances.emplace_back(new Point((canvasSize.getWidth() - coordinate)/frames,0));
                partialAverageForXAxis += (canvasSize.getWidth() - coordinate) / frames;
                break;
            case 2: // initial angle = 90
                coordinate = current.getVerticalAxis();
                cout << coordinate / frames << endl;
                //distances.emplace_back(new Point(0,-(coordinate/frames)));
                partialAverageForYAxis += coordinate / frames;
                break;
            case 3: // initial angle = 180
                coordinate = current.getHorizontalAxis();
                //distances.emplace_back(new Point(-(coordinate/frames),0));
                partialAverageForXAxis += coordinate/frames;
                break;
            case 4: // initial angle = 270
                coordinate = current.getVerticalAxis();
                //distances.emplace_back(new Point(0,(canvasSize.getHeight() - coordinate)/frames));
                partialAverageForYAxis += (canvasSize.getHeight() - coordinate) / frames;
                break;
            default: 
                break;
        }
        pointIterator++;
        calculateRouteForSpecialCaseAux(pointIterator, partialAverageForXAxis, partialAverageForYAxis);
        return nullptr;
    }

    void calculateCurve(double pAverageForXAxis, double pAverageForYAxis) {
        double newXAxis,newYAxis;
        int squareSize = 3*(getWidth() + 50 + getHeight() + 50) / 40;
        double xAxisOffset = squareSize * pow(((2.0*angle)/(double)M_PI),90.0/100.0);
        
        Point initialPoint(pAverageForXAxis, pAverageForYAxis);
        Point finalPoint(pAverageForXAxis * frames, pAverageForYAxis * frames);

        double xAxisMediumPoint = (initialPoint.getHorizontalAxis() + finalPoint.getHorizontalAxis()) / 2.0;
        double yAxisMediumPoint = (initialPoint.getVerticalAxis() + finalPoint.getVerticalAxis()) / 2.0;
        Point mediumPoint(xAxisMediumPoint, yAxisMediumPoint);

        if (angle > 1) {
            double slope = ((double)finalPoint.getVerticalAxis() - (double)initialPoint.getVerticalAxis()) / 
            ((double)finalPoint.getHorizontalAxis() - (double)initialPoint.getHorizontalAxis());
            
            double perpendicularSlope = -1.0/slope;
            double linearConstant = mediumPoint.getVerticalAxis() - (perpendicularSlope * mediumPoint.getHorizontalAxis());
            
            newXAxis = mediumPoint.getHorizontalAxis() + xAxisOffset;
            newYAxis = perpendicularSlope * newXAxis + linearConstant;   // y = mx + b
        } else {
            newXAxis = mediumPoint.getHorizontalAxis();
            newYAxis = mediumPoint.getVerticalAxis() + squareSize;
        }
        
        Point guideForCurve(newXAxis, newYAxis);

        for (int currentFrame = 0; currentFrame < frames; currentFrame++) {
            distances.emplace_back(beizerCurve(initialPoint, guideForCurve, finalPoint, ((1.0/frames)*(currentFrame+1))));
        }

        notify(docPointer, &distances);
    }

    Point* beizerCurve(Point originPoint, Point middlePoint, Point lastPoint, double percentage) {
        double xAxis = pow((1-percentage),2)*originPoint.getHorizontalAxis() + 
                       2*(1-percentage)*percentage*middlePoint.getHorizontalAxis() + 
                       pow(percentage,2)*lastPoint.getHorizontalAxis();

        double yAxis = pow((1-percentage),2)*originPoint.getVerticalAxis() + 
                       2*(1-percentage)*percentage*middlePoint.getVerticalAxis() + 
                       pow(percentage,2)*lastPoint.getVerticalAxis();

        cout << "(" << xAxis-originPoint.getHorizontalAxis() << " , " << yAxis-originPoint.getVerticalAxis() << ")" << endl;
        Point *point = new Point(xAxis-originPoint.getHorizontalAxis(), yAxis-originPoint.getVerticalAxis());
        return point;
    }

// ------------------------------------------------------------------------------------------------------------

    void calculateRouteForSpecialCase() {

        // Debugging
        cout << "Process for special case" << endl;
        
        cout << "Cuadrant = " << quadrant << endl;
        cout << "Angle = " << angle << endl;

        for(Point current : coordinates) {
            int coordinate;
            switch (quadrant) {
                case 1: // initial angle = 0
                    coordinate = current.getHorizontalAxis();
                    distances.emplace_back(new Point((canvasSize.getWidth() - coordinate)/frames,0));
                    break;
                case 2: // initial angle = 90
                    coordinate = current.getVerticalAxis();
                    distances.emplace_back(new Point(0,-(coordinate/frames)));
                    break;
                case 3: // initial angle = 180
                    coordinate = current.getHorizontalAxis();
                    distances.emplace_back(new Point(-(coordinate/frames),0));
                    break;
                case 4: // initial angle = 270
                    coordinate = current.getVerticalAxis();
                    distances.emplace_back(new Point(0,(canvasSize.getHeight() - coordinate)/frames));
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
        // cout << "Frames = " << frames << endl;
        // cout << "Cuadrant = " << quadrant << endl;
        // cout << "Angle = " << angle << endl;
        
        int xAxis,yAxis; 
        
        double firstLeg,secondLeg,slope,linearConstant; // leg -> cateto

        for(Point current : coordinates) {
            xAxis = current.getHorizontalAxis();
            yAxis = current.getVerticalAxis();

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
        double newXAxis,newYAxis;
        int squareSize = 3*(getWidth() + 50 + getHeight() + 50) / 40;
        double xAxisOffset = squareSize * pow(((2.0*angle)/(double)M_PI),90.0/100.0);
        vector<vector<Point*>> *distancesForCurvedRoute = new vector<vector<Point*>>();
        
        for (int pointIterator = 0; pointIterator < coordinates.size(); pointIterator++) {
            double yAxisFinalPoint = coordinates[pointIterator].getVerticalAxis() + distances[pointIterator]->getVerticalAxis() * frames;

            double xAxisFinalPoint = coordinates[pointIterator].getHorizontalAxis() + (distances[pointIterator]->getHorizontalAxis() * frames);
            
            Point *initialPoint = new Point(coordinates[pointIterator].getHorizontalAxis(), coordinates[pointIterator].getVerticalAxis());
            Point *finalPoint = new Point(xAxisFinalPoint,yAxisFinalPoint);

            double xAxisMediumPoint = (initialPoint->getHorizontalAxis() + finalPoint->getHorizontalAxis()) / 2.0;
            double yAxisMediumPoint = (initialPoint->getVerticalAxis() + finalPoint->getVerticalAxis()) / 2.0;
            Point *mediumPoint = new Point(xAxisMediumPoint, yAxisMediumPoint);

            if(angle > 1) {
                double slope = ((double)finalPoint->getVerticalAxis() - (double)initialPoint->getVerticalAxis()) / 
                ((double)finalPoint->getHorizontalAxis() - (double)initialPoint->getHorizontalAxis());
                
                double perpendicularSlope = -1.0/slope;
                double linearConstant = mediumPoint->getVerticalAxis() - (perpendicularSlope * mediumPoint->getHorizontalAxis());
                
                newXAxis = mediumPoint->getHorizontalAxis() + xAxisOffset;
                newYAxis = perpendicularSlope * newXAxis + linearConstant;   // y = mx + b

                cout << "y = " << perpendicularSlope << " * x + " << linearConstant << endl;
            } else {
                newXAxis = mediumPoint->getHorizontalAxis();
                newYAxis = mediumPoint->getVerticalAxis() + squareSize;
            }
            
            Point *guideForCurve = new Point(newXAxis, newYAxis);

            cout << initialPoint->getHorizontalAxis() << " " << initialPoint->getVerticalAxis() << endl;
            cout << finalPoint->getHorizontalAxis() << " " << finalPoint->getVerticalAxis() << endl;
            cout << guideForCurve->getHorizontalAxis() << " " << guideForCurve->getVerticalAxis() << endl;

            vector<Point*> pointTriplets;
            pointTriplets.emplace_back(initialPoint);
            pointTriplets.emplace_back(finalPoint);
            pointTriplets.emplace_back(guideForCurve);
            distancesForCurvedRoute->emplace_back(pointTriplets);
        }
        printPointsCurvedRoute(*distancesForCurvedRoute);    // REMOVE LATER!
        notify(docPointer, distancesForCurvedRoute);
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

        coordinates = *(vector<Point>*) pCoordinates;
        setDocPointer(pDocPointer);
        canvasSize.setViewBoxResolution(pDocPointer->child("svg").attribute("viewBox").value(),true); // set canvas size;
        // vector<Point> *originalPointsPointer = (vector<Point>*)pCoordinates;
        // vector<Point> originalPoints = originalPointsPointer[0];
        // setCoordinates(originalPoints);
        calculateQuadrant();
    }

};

#endif // ROUTER_H