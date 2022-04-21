#ifndef GENERATOR_H
#define GENERATOR_H

#include <queue>
#include <thread>
#include <string>
#include "vector"
#include "Point.h"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include "TypeOfRoute.h"
#include "ObserverPattern.h"
#include "../libraries/pugixml/pugixml.hpp"

using std::cout;
using std::endl;
using std::queue;
using std::thread;
using std::string;
using std::vector;
using std::ofstream;
using std::to_string;
using pugi::xml_node;
using std::stringstream;
using pugi::xml_document;
using pugi::xml_node_iterator;

class Generator : public Observer {
private:

    int frames;
    int processId;
    bool keepRepetingConsumer;
    string originalFileName;
    TypeOfRoute route;
    xml_document *docPointer;
    vector<Point*> *distances;
    queue<xml_document*> docs;

    Point beizerCurve(Point originPoint, Point middlePoint, Point lastPoint, double percentage) {
        double xAxis = pow((1-percentage),2)*originPoint.getHorizontalAxis() + 
                       2*(1-percentage)*percentage*middlePoint.getHorizontalAxis() + 
                       pow(percentage,2)*lastPoint.getHorizontalAxis();

        double yAxis = pow((1-percentage),2)*originPoint.getVerticalAxis() + 
                       2*(1-percentage)*percentage*middlePoint.getVerticalAxis() + 
                       pow(percentage,2)*lastPoint.getVerticalAxis();

        return Point(xAxis-originPoint.getHorizontalAxis(),yAxis-originPoint.getVerticalAxis());
    }

    void makeFiles() {
        xml_node mainSvgNode = docPointer->child("svg").last_child();
        for(int i = 0; i < frames; i++) {
            xml_node_iterator svgIterator = mainSvgNode.begin();
            for(Point *actual : *distances) {
                svgIterator->attribute("x").set_value(actual->getHorizontalAxis()*(i+1));
                svgIterator->attribute("y").set_value(actual->getVerticalAxis()*(i+1));
                svgIterator++;
            }
            xml_document *copiedDoc = new xml_document();
            copiedDoc->reset(*docPointer);
            docs.push(copiedDoc);
        }
        keepRepetingConsumer = false;
    }

    void consumer() {
        int frameExported = 1;
        xml_document *extractedDoc;
        while (keepRepetingConsumer || !docs.empty()) {
            sleep(1);
            if(!docs.empty()) {
                extractedDoc = docs.front();
                docs.pop();
                string newFileName = originalFileName + "-" + to_string(frameExported) + ".svg";
                ofstream *copyFile = new ofstream(newFileName,ofstream::trunc);
                stringstream svgWritter;
                extractedDoc->save(svgWritter," ");
                string stringXML = svgWritter.str();
                *copyFile << stringXML;
                copyFile->close();
                delete extractedDoc;
                delete copyFile;
                frameExported++;
            }
        }
    }

public:
    Generator() {
        processId = 2;
    }

    Generator(TypeOfRoute pRoute, int pFrames, string pFileName) {
        processId = 2;
        route = pRoute;
        frames = pFrames;
        originalFileName = pFileName;
        originalFileName = "./Result/" + originalFileName.substr(0,originalFileName.size()-4);
        keepRepetingConsumer = true;
    }

    ~Generator() {}

    void work() {
        cout << "Working..." << endl;
        thread consumerThread(&Generator::consumer,this);
        makeFiles();
        consumerThread.join();
        cout << "Animation complete" << endl;
    }

    void update(xml_document* pDocPointer, void* pDistances) {
        cout << "Generator started working" << endl;
        distances = (vector<Point*>*) pDistances;
        docPointer = (xml_document*) pDocPointer;
        work();
    }

    int getProcessId() {
        return processId;
    }

};

#endif // GENERATOR_H
