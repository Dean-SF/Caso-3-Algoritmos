#ifndef ROUTER_H
#define ROUTER_H
#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>
#include "Point.h"
#include <iostream>
#include "TypeOfRoute.h"
#include "ObserverPattern.h"

using std::tan;
using std::cout;
using std::endl;

/* Routing process - Dynamic Programming
N: Coordinates given by the user.
Memorization: It memorizes the previous partial sum.
Stages: Each point given by the user.
Local optimum: The ideal distance that the mask has to move per frame.
Global optimum: Vector of points where the mask is going to move.
*/

class Router : public Subject, public Observer {
private:
    int quadrant; 
    double angle;
    int processId;
    double frames;
    Observer* animator;
    Resolution *canvasSize; 
    TypeOfRoute typeOfRoute;
    vector<Point*> distances; 
    vector<Point> coordinates;
    vector<xml_node> *pathCollection;

    /* This is where the routing process begins. It calculates the given angle's quadrant and the referential angle.
    Lastly, it calls the function that calculates the route. 
    O(C): It doens't depend on the size of the input, it'll always calculate the quadrant of one angle.
    */
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

        // Get referential angle depending on the quadrant
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

    /* This function initializes the variables that will be used for the recursive functions.
    If the given angle is 0 or PI/2 (90°), it means it is a special case. On the contrary, it means it is a normal case.
    O(C): It doesn't depend on the size of the input, it'll always call one of two functions.
    */
    void calculateRoute() {
        int pointIterator = 0;
        double partialAverageForXAxis = 0;
        double partialAverageForYAxis = 0;
        
        if (angle == 0 || angle == (double)(M_PI/2))
            calculateRouteForSpecialCaseAux(pointIterator, partialAverageForXAxis, partialAverageForYAxis);
        else 
            calculateRouteForNormalCaseAux(pointIterator, partialAverageForXAxis, partialAverageForYAxis);
    }

    /* This function calculates the route dynamically for angles that aren't equal to 0 nor PI/2.
    It calls itself recursively using the sum of the ideal distances calculated as the next call's input. So it memorizes
    the previous partial sum.
    The last call's output is the global optimum, which the vector of points where the mask is going to move.
    O(n): It depends on the points given by the user.
    */
    void calculateRouteForNormalCaseAux(int pointIterator, double partialAverageForXAxis, double partialAverageForYAxis) {
        if (pointIterator == coordinates.size()) { //If this is true, it means it is the last call
            partialAverageForXAxis = partialAverageForXAxis/coordinates.size(); // Get final average
            partialAverageForYAxis = partialAverageForYAxis/coordinates.size();

            if(quadrant == 2 || quadrant == 3)  // Depending on the quadrant, we'll need a negative axis
                partialAverageForXAxis *= -1;
            if(quadrant == 1 || quadrant == 2)
                partialAverageForYAxis *= -1;
        
            if (typeOfRoute == TypeOfRoute::curvedRoute) {  // Process if the given type of route is a curve
                calculateCurve(partialAverageForXAxis, partialAverageForYAxis); //In this function we'll get the global optimum
                return;
            }
            // Process if the given type of route is straight. Get the global optimum
            for (int currentFrame = 1; currentFrame <= frames; currentFrame++) {    
                distances.emplace_back(new Point(partialAverageForXAxis * currentFrame, partialAverageForYAxis * currentFrame));
            }
            notify(pathCollection,&distances,canvasSize);
            return;
        }

        Point currentPoint = coordinates[pointIterator];
        
        /* Calculate the distance between the initial point and the final point, by calculating a slope with 
        trigonometric identities and the given angle. 
        */
        int xAxis,yAxis; 
        double firstLeg,secondLeg,slope,linearConstant; // leg -> cateto
     
        xAxis = currentPoint.getHorizontalAxis();
        yAxis = currentPoint.getVerticalAxis();

        if (quadrant == 1 || quadrant == 4) {
            firstLeg = canvasSize->getWidth() - xAxis;
            secondLeg = tan(angle)*firstLeg;
        } else {
            firstLeg = xAxis;
            secondLeg = tan(angle)*firstLeg;
        }

        if ((secondLeg > yAxis && (quadrant == 1 || quadrant == 2)) || 
            (secondLeg + yAxis > canvasSize->getHeight() && (quadrant == 3 || quadrant == 4))) {
            slope = (secondLeg)/(firstLeg);     
            linearConstant = yAxis-(slope*xAxis);
            if(quadrant == 1 || quadrant == 2) {
                secondLeg = yAxis;
                firstLeg = ((((yAxis*2)-linearConstant)/slope) - xAxis);
            } else {
                secondLeg = canvasSize->getHeight() - yAxis;
                firstLeg = (((canvasSize->getHeight()-linearConstant)/slope) - xAxis);
            }
        }
        // Sum the local optimum to the partial sum
        partialAverageForXAxis += firstLeg / frames;
        partialAverageForYAxis += secondLeg / frames;
        pointIterator++;    // To get next point
        calculateRouteForNormalCaseAux(pointIterator, partialAverageForXAxis, partialAverageForYAxis);
        return;
    }

    /* This function calculates the route dynamically for angles that are equal to 0 or PI/2.
    It calls itself recursively using the sum of the ideal distances calculated as the next call's input. So it memorizes
    the previous partial sum.
    The last call's output is the global optimum, which the vector of points where the mask is going to move.
    O(n): It depends on the points given by the user.
    */
    void calculateRouteForSpecialCaseAux(int pointIterator, double partialAverageForXAxis, double partialAverageForYAxis) {
        if (pointIterator == coordinates.size()) { //If this is true, it means it is the last call
            partialAverageForXAxis = partialAverageForXAxis/coordinates.size(); // Get final average
            partialAverageForYAxis = partialAverageForYAxis/coordinates.size();

            // Depending on the quadrant, we'll need a negative axis
            if (quadrant == 2) // initial angle = 90
                partialAverageForYAxis *= -1;
            else if (quadrant == 3) // initial angle = 180
                partialAverageForXAxis *= -1;

            if (typeOfRoute == TypeOfRoute::curvedRoute) { // Process if the given type of route is a curve
                calculateCurve(partialAverageForXAxis, partialAverageForYAxis); //In this function we'll get the global optimum
                return;
            }

            // Process if the given type of route is straight. Get the global optimum.
            // This process is executed once after the main recursive cycle. Its not a nested loop
            for (int currentFrame = 1; currentFrame <= frames; currentFrame++) {
                distances.emplace_back(new Point(partialAverageForXAxis * currentFrame, partialAverageForYAxis * currentFrame));
            }
            notify(pathCollection,&distances,canvasSize);
            return;
        }

        Point current = coordinates[pointIterator];
        
        /* Depending on the quadrant, the point will move in the X axis or the Y axis. The ideal distance is
        added to the correspondent partial sum.
        */
        int coordinate;
        switch (quadrant) {
            case 1: // initial angle = 0
                coordinate = current.getHorizontalAxis();
                partialAverageForXAxis += (canvasSize->getWidth() - coordinate) / frames;
                break;
            case 2: // initial angle = 90
                coordinate = current.getVerticalAxis();
                partialAverageForYAxis += coordinate / frames;
                break;
            case 3: // initial angle = 180
                coordinate = current.getHorizontalAxis();
                partialAverageForXAxis += coordinate/frames;
                break;
            case 4: // initial angle = 270
                coordinate = current.getVerticalAxis();
                partialAverageForYAxis += (canvasSize->getHeight() - coordinate) / frames;
                break;
            default: 
                break;
        }
        pointIterator++; // To get next point
        calculateRouteForSpecialCaseAux(pointIterator, partialAverageForXAxis, partialAverageForYAxis);
        return;
    }

    /* This function calculates a point of a beizer curve, given an initial, middle and final point, the coordinates
    will depend on the given curve's progress.
    O(C): The input will not affect it's time since it only implements 2 formulas.
    */
    Point* beizerCurve(Point originPoint, Point middlePoint, Point lastPoint, double percentage) {
        double xAxis = pow((1-percentage),2)*originPoint.getHorizontalAxis() + 
                       2*(1-percentage)*percentage*middlePoint.getHorizontalAxis() + 
                       pow(percentage,2)*lastPoint.getHorizontalAxis();

        double yAxis = pow((1-percentage),2)*originPoint.getVerticalAxis() + 
                       2*(1-percentage)*percentage*middlePoint.getVerticalAxis() + 
                       pow(percentage,2)*lastPoint.getVerticalAxis();

        Point *point = new Point(xAxis - originPoint.getHorizontalAxis(), yAxis - originPoint.getVerticalAxis());
        return point;
    }

    /* This function calculates the points needed for the beizer curve.
    This calculates the global optimum based on the local optimums produced by the calculateRouteForSpecialCaseAux or
    the calculateRouteForNormalCaseAux function.
    The  global optimum is the vector of points where the mask is going to move.
    O(n): It depends on the amount of frames given by the user.
    */
    void calculateCurve(double pAverageForXAxis, double pAverageForYAxis) {
        double newXAxis, newYAxis;
        int squareSize = 3*(getWidth() + 50 + getHeight() + 50) / 40;
        double xAxisOffset = squareSize * pow(((2.0*angle)/(double)M_PI), 90.0/100.0);
        
        // Calculate initial and final point with the given average distances
        Point initialPoint(pAverageForXAxis, pAverageForYAxis);
        Point finalPoint(pAverageForXAxis * frames, pAverageForYAxis * frames);

        // Get the point between the initial and final point
        double xAxisMediumPoint = (initialPoint.getHorizontalAxis() + finalPoint.getHorizontalAxis()) / 2.0;
        double yAxisMediumPoint = (initialPoint.getVerticalAxis() + finalPoint.getVerticalAxis()) / 2.0;
        Point mediumPoint(xAxisMediumPoint, yAxisMediumPoint);
        
        // Calculate the axis for the point that will be used as a guide for the beizer curve.
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
        
        Point guideForCurve(newXAxis, newYAxis); // This point will be used to create a beizer curve

        for (int currentFrame = 0; currentFrame < frames; currentFrame++) { // Get global optimum
            distances.emplace_back(beizerCurve(initialPoint, guideForCurve, finalPoint, ((1.0/frames)*(currentFrame+1))));
        }
        notify(pathCollection,&distances,canvasSize);
    }

    /*
    Each function has it's own time complexity described in a comment.
    for the full algorithm we can make the next analysis:
    calculateQuadrant() -> First executed to initialize the algorithm, its instructions are always the same
                           and always executed, we can conclude it is constant time O(C)

        calculateRoute() -> Second executed fuction, its instructions are always the same without
                            cycles, we can conclude it is constant time O(C)

            From the last fuction, we have two other functions to be executed
            calculateRouteForNormalCaseAux() or calculateRouteForNormalCaseAux() -> both have diferent instructions, but both loop through the
                                                                                    same "vector<Point>", if the route is straight, both have
                                                                                    a for loop to create all the coordinates, but they are NOT
                                                                                    nested loops, for instance we can conclude it has a time
                                                                                    complexity of O(n) + O(n) because of two separated loops
                                                                                    running one after another so the time complexity for worst
                                                                                    case scenario is O(n).
            
                From the last funtion we have one final function to execute, in case of a Curve Route we execute another function
                instead of the for loop describe in the last two functions:
                calculateCurve(): -> this funtion just makes some extra steps and then executes a similar for loop ( with O(N) ) to the last two functions
                                     but executing another function in each loop iteration:
                    
                    beizerCurve() -> its just a formula, and its time complexity is O(C), constant time.

    In conclution we have in the WORST CASE SCENARIO some formula like: f(n)= O(C) + O(C) + O(n) + O(n) + O(n) * O(C)
    we can conclude that in worse case we have a complexity of O(n), in other words, linear.
    */

public:
    Router() {
        angle = 0; 
        frames = 0;
        quadrant = 1;
        processId = 1; 
        animator = nullptr;
        canvasSize = nullptr;
        pathCollection = nullptr;
        typeOfRoute = straightRoute;
    }

    Router(TypeOfRoute pTypeOfRoute, double pAngle, int pFrames) {
        quadrant = 1;
        processId = 1;
        angle = pAngle;
        frames = pFrames;
        animator = nullptr;
        canvasSize = nullptr;
        pathCollection = nullptr;
        typeOfRoute = pTypeOfRoute;
    }

    int getWidth() {
        return canvasSize->getWidth();
    }

    int getHeight() {
        return canvasSize->getHeight();
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
        canvasSize->setWidth(pWidth);    
    }

    void setHeight(int pHeight) {
        canvasSize->setHeight(pHeight);
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
    void notify(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) {
        cout << "Routing process finished" << endl;
        cout << "--------------------------" << endl;
        animator->update(pPathCollection, pCoordinates, pCanvasSize);
    }

    // This is executed by the Animator when it notifies that the Selector finished it's job
    void update(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) {
        cout << "--------------------------" << endl;
        cout << "Routing process started" << endl;
        pathCollection = pPathCollection;
        coordinates = *(vector<Point> *)pCoordinates;
        canvasSize = pCanvasSize;
        calculateQuadrant();
    }

};

#endif // ROUTER_H