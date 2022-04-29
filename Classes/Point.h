#ifndef POINT_H
#define POINT_H

#include <tgmath.h>

using std::sqrt;
using std::pow;

class Point {
private:
    double horizontalAxis;
    double verticalAxis;
public:
    Point() {
        horizontalAxis = 0;
        verticalAxis = 0;
    }

    Point(double pHorizontalAxis,double pVerticalAxis) {
        horizontalAxis = pHorizontalAxis;
        verticalAxis = pVerticalAxis;
    }

    double getDistanceBetweenPoints(Point pOtherPoint) {
        return sqrt(pow(horizontalAxis-pOtherPoint.horizontalAxis,2)+
                    pow(verticalAxis-pOtherPoint.verticalAxis,2));
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

};


#endif