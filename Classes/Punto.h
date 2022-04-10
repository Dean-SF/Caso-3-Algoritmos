#ifndef PUNTO_H
#define PUNTO_H

class Punto {
private:
    int horizontalAxis;
    int verticalAxis;
public:
    Punto(int pHorizontalAxis,int pVerticalAxis) {
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