#ifndef GENERATOR_H
#define GENERATOR_H

#include "ObserverPattern.h"
#include "TypeOfRoute.h"
#include "vector"
#include "Point.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
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
    string originalFileName;
    TypeOfRoute route;
    xml_document *docPointer;
    vector<Point*> *distances;

    void makeFiles() {
        xml_node mainSvgNode = docPointer->child("svg").last_child();
        xml_node_iterator svgIterator = mainSvgNode.begin();
        for(int i = 0; i < frames; i++) {
            for(Point *actual : *distances) {
                svgIterator->attribute("x").set_value(actual->getHorizontalAxis()*(i+1));
                svgIterator->attribute("y").set_value(actual->getVerticalAxis()*(i+1));
                svgIterator++;
            }
            docPointer->name();
            originalFileName += "-" + to_string(i);
            ofstream *copyFile = new ofstream(originalFileName,ofstream::trunc);
            stringstream svgWritter;
            docPointer->save(svgWritter," ");
            string stringXML = svgWritter.str();
            *copyFile << stringXML;
            copyFile->close();
            delete copyFile;
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
    }

    ~Generator() {}

    void work() {
        cout << "Working..." << endl;
        makeFiles();
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
