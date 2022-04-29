#include <iostream>
#include "classes/Point.h"
#include <vector>

using namespace std;
vector<Point*> puntos;
double distanceForReference = 5.0;

/*
bool backtracking(int pointIndexTest, int lastSuccessfulIndex) {
    Point *current = puntos[pointIndexTest];
    Point *lastSuccessful = puntos[lastSuccessfulIndex];
    double distance = current->getDistanceBetweenPoints(*lastSuccessful);

    if(pointIndexTest != 0) {
        if(distance >= distanceForReference)
            return true;
        else 
            return false;
    }

    for(int pointIndex = pointIndexTest + 1; pointIndex < puntos.size(); pointIndex++) {
        cout << "pIndex: " << pointIndex << " lastSuccessfulIndex: " << lastSuccessfulIndex << endl;
        if(backtracking(pointIndex,lastSuccessfulIndex)) {
            lastSuccessfulIndex = pointIndex;
            cout << "resultado: " << pointIndex << endl;
        }
    }
    return false;
}*/

void backtracking(int pointIndexTest, int lastSuccessfulIndex) {
    if(pointIndexTest >= puntos.size())
        return;
    Point *current = puntos[pointIndexTest];
    Point *lastSuccessful = puntos[lastSuccessfulIndex];
    double distance = current->getDistanceBetweenPoints(*lastSuccessful);

    if(distance >= distanceForReference || pointIndexTest == 0) {
        int lastSuccessfulIndex = pointIndexTest;
        cout << "Frame con el punto: " << pointIndexTest << endl;
        return backtracking(pointIndexTest + 1,lastSuccessfulIndex);  
    } else
        return backtracking(pointIndexTest + 1,lastSuccessfulIndex);
}


bool backtracking2(int pointIndexTest, int lastSuccessfulIndex) {
    Point *current = puntos[pointIndexTest];
    Point *lastSuccessful = puntos[lastSuccessfulIndex];
    double distance = current->getDistanceBetweenPoints(*lastSuccessful);

    if(distance >= distanceForReference || pointIndexTest == 0) {
        int lastSuccessfulIndex = pointIndexTest;
        
        //CREAR FRAME FUNCION

        for(int pointIndex = pointIndexTest + 1; pointIndex < puntos.size(); pointIndex++) {
            if(backtracking2(pointIndex,lastSuccessfulIndex)) {
                return true;
            }
        }
        return true;
    } else
        return false;
}

int main() {
    cout << "HOLAAAAAAAAAAAAA soy un programa consciente" << endl;
    puntos.emplace_back(new Point(0,0)); //0
    puntos.emplace_back(new Point(1,1)); //1
    puntos.emplace_back(new Point(2,2)); //2
    puntos.emplace_back(new Point(3,3)); //3
    puntos.emplace_back(new Point(4,4)); //4
    puntos.emplace_back(new Point(5,5)); //5
    puntos.emplace_back(new Point(6,6)); //6
    puntos.emplace_back(new Point(7,7)); //7
    puntos.emplace_back(new Point(8,8)); //8
    puntos.emplace_back(new Point(9,9)); //9
    backtracking2(0,0);
    return 0;
}