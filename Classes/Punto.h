#ifndef PUNTO_H
#define PUNTO_H

class Punto {
private:
    int horizontalAxis;
    int verticalAxis;
public:
    Punto(int horizontalAxis,int verticalAxis) {
        this->horizontalAxis = horizontalAxis;
        this->verticalAxis = verticalAxis;
    }

    int getHorizontalAxis() {
        return horizontalAxis;
    }

    int getVerticalAxis() {
        return verticalAxis;
    }

    void setHorizontalAxis(int horizontalAxis) {
        this->horizontalAxis = horizontalAxis;
    }

    void setVerticalAxis(int vertialAxis) {
        this->verticalAxis = verticalAxis;
    }

};


#endif