#ifndef POINT_H
#define POINT_H

class Point {
private:
    int horizontalAxis;
    int verticalAxis;
public:
    Point() {
        horizontalAxis = 0;
        verticalAxis = 0;
    }

    Point(int pHorizontalAxis,int pVerticalAxis) {
        horizontalAxis = pHorizontalAxis;
        verticalAxis = pVerticalAxis;
    }

    int getHorizontalAxis() {
        return horizontalAxis;
    }

    int getVerticalAxis() {
        return verticalAxis;
    }

    void setHorizontalAxis(int pHorizontalAxis) {
        horizontalAxis = pHorizontalAxis;
    }

    void setVerticalAxis(int pVerticalAxis) {
        verticalAxis = pVerticalAxis;
    }

};


#endif