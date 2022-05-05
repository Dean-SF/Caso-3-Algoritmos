#ifndef POINT_H
#define POINT_H

#include <tgmath.h>

using std::pow;
using std::sqrt;

/*
This clase is made to store "x" and "y" coordinates
*/
class Point {
private:
    double verticalAxis;
    double horizontalAxis;
public:

    // Default constructor
    Point() {
        verticalAxis = 0;
        horizontalAxis = 0;
    }

    Point(double pHorizontalAxis,double pVerticalAxis) {
        verticalAxis = pVerticalAxis;
        horizontalAxis = pHorizontalAxis;
    }

    double getHorizontalAxis() {
        return horizontalAxis;
    }

    double getVerticalAxis() {
        return verticalAxis;
    }

    void setHorizontalAxis(double pHorizontalAxis) {
        horizontalAxis = pHorizontalAxis;
    }

    void setVerticalAxis(double pVerticalAxis) {
        verticalAxis = pVerticalAxis;
    }
    
    // given a point, calculates the distance between the two
    double getDistanceBetweenPoints(Point pOtherPoint) {
        return sqrt(pow(horizontalAxis-pOtherPoint.horizontalAxis,2)+
                    pow(verticalAxis-pOtherPoint.verticalAxis,2));
    }
};


#endif