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
    vector<Point*> distances; // resulting vector as a attribute
    int processId;
    
    Resolution canvasSize; // width and height as a Resolution object
    double angle;
    int frames;
    int quadrant; // Quadrant as atribute

    /*
    Resumen:
    verifySpecialCase() -> elimitado, en cambio primera etapa es en calculateQuadrantNormalCase(), donde
    se va a transformar el caso especial en un angulo de 0 o 90, y se le asigna un quadrante para identificar
    si se mueve hacia arriba, abajo, izquierda o derecha. En caso de ser un caso especial ( que ahora es reducido
    a un angulo de 0 o 90), se ejecuta calculateRouteForSpecialCase(), si no calculateRouteForNormalCase(). ambas
    funciones se completaron y estan explicadas en la debida funcion.
    Ademas se redujeron los parametros en funciones y se cambiaron a atributos de la clase para que puedan ser
    accedidos de cualquier lado
    */

    // Debug only, remove for final release just to print the resulting vector
    void printDistances() {
        for(Point *actual : distances) {
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
        if(angle == 0 || angle == (double)(M_PI/2))
            calculateRouteForSpecialCase();
        else
            calculateRouteForNormalCase();
    }

    void calculateRouteForSpecialCase() {

        // Debugging
        cout << "Process for special case" << endl;
        
        cout << "Cuadrant = " << quadrant << endl;
        cout << "Angle = " << angle << endl;

        // Empieza la funcion

        /*
        CAMBIO DE PLANTILLA: Se puso un nodo SVG al final del nodo SVG principal, entonces 
        puede ser accedido al instante y todo lo que esta dentro de ese nodo va ser un nodo SVG con mask 
        y coordenadas para evitar comparar strings, lo que es muy lento
        */

        // mainSvgNode contiene el nodo con todos los nodos que traen los datos que se necesitan
        xml_node mainSvgNode = docPointer->child("svg").last_child(); 


        xml_node_iterator nodeIterator; // recorrido por los nodos para obtener coordenadas y calcular direcciones
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
        notify(docPointer);
    }



    void calculateRouteForNormalCase() {        
        
        cout << "--- CASO NORMAL ---" << endl;
        cout << "Frames = " << frames << endl;
        cout << "Cuadrant = " << quadrant << endl;
        cout << "Angle = " << angle << endl;
        
        int xAxis,yAxis; 
        
        // leg significa cateto, por lo tanto las primeras variables son "primer cateto y segundo cateto"
        // slope va ser la pendiente(m) y linearConstant la b en la funcion (lineal) y = mx + b 
        double firstLeg,secondLeg,slope,linearConstant;; // leg -> cateto

        xml_node node;

        // Esta parte y los parametro del for son iguales a la anterior
        xml_node mainSvgNode = docPointer->child("svg").last_child();
        xml_node_iterator nodeIteratorParent; ;
        for(nodeIteratorParent = mainSvgNode.begin(); nodeIteratorParent != mainSvgNode.end(); nodeIteratorParent++) {

            // Extraccion de coordenadas fue cambiado, originalmente necesitaba de dos variables extras
            node = *nodeIteratorParent;
            xAxis = stoi(node.child("mask").child("rect").attribute("x").value());
            yAxis = stoi(node.child("mask").child("rect").attribute("y").value());

            /*
            Para el primer y cuarto cuadrante siempre se usa la misma distancia en el eje X para el primer
            cateto puesto que estan reflejados en dicho eje
            */

            /*
            Para el segundo y tercer cuadrante (parte del "else") siempre se usa la misma distancia del eje x
            la cual es el valor del xAxis
            */
        
            if(quadrant == 1 || quadrant == 4) {
                firstLeg = canvasSize.getWidth() - xAxis;
                secondLeg = tan(angle)*firstLeg;
            } else {
                firstLeg = xAxis;
                secondLeg = tan(angle)*firstLeg;
            }

            /*
            Para los cuadrantes 1 y 2 si el triangulo cuadrado se sale de los limites se verifica
            si la distancia del segundo cateto (opuesto) es mayor a la del valor de yAxis del punto actual

            Para los cuadrantes 3 y 4 si el triangulo cuadrado se sale de los limites se verifica que 
            la coordenada 'y' del punto de llegada no sobrepase el limite del canvas

            En cualquier caso si se supera el limite, se calcula la pendiente y la 'b' de la funcion
            lineal para usar la funcion inversa y obtener una coordenada en el eje X que choque con
            el limite del canvas, una vez se hace eso:

            Para cuadrante 1 y 2 el segundo cateto (opuesto) va ser del tamaño del yAxis, y
            el primer cateto (adyacente) va ser la distancia entre la coordenada del eje x encontrada
            con la funcion inversa y xAxis que era el punto inicial

            Para cuadrante 3 y 4 al pasarse el limite significa que la distancia del segundo cateto
            (opuesto) es la distancia entre el final del canvas y el yAxis del punto actual, mientras
            que el primer cateto (adyacente) es la distancia entre la coordenada del eje x encontrada
            con la funcion inversa, pero esta vez se usa el valor maximo de altura del canvas, y 
            el xAxis del punto inicial
            */
            if((secondLeg > yAxis && (quadrant == 1 || quadrant == 2)) || 
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
            

            // Esto es para acomodar los signo de los valores, segun el cuadrante
            if(quadrant == 2 || quadrant == 3)
                firstLeg *= -1;
            if(quadrant == 1 || quadrant == 2)
                secondLeg *=-1;

            distances.emplace_back(new Point(firstLeg/frames,secondLeg/frames));
        }
        printDistances(); // remove letter in final release
        notify(docPointer);
    }

public:
    Router(double pAngle, int pFrames, TypeOfRoute pTypeOfRoute, xml_document* pDocPointer) {
        animator = nullptr;
        processId = 1;
        angle = pAngle;
        frames = pFrames;
        typeOfRoute = pTypeOfRoute;
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
    void notify(xml_document* pDocPointer) {
        cout << "Router is done" << endl;
        cout << "--------------------------" << endl;
        animator->update(pDocPointer);
    }

    void update(xml_document* pDocPointer) {
        cout << "--------------------------" << endl;
        cout << "Router started working" << endl;
        docPointer = pDocPointer;
        calculateQuadrantNormalCase();

    }

};

#endif // ROUTER_H
