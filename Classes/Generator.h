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
#include <sys/stat.h>
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
    int squareSize;
    TypeOfRoute route;
    xml_node svgMaskGroup;
    string originalFileName;
    Resolution *canvasSize;
    xml_document nodeCreator;
    xml_document *docPointer;
    queue<xml_document*> producedFrames;
    bool keepRepetingConsumer;
    vector<Point*> *maskCoordinates;
    vector<xml_node> *pathCollection;
    vector<Point> *originalCoordinates;

    /*
    void makeFiles() {
        xml_node mainSvgNode = docPointer->child("svg").last_child();
        for(int i = 0; i < frames; i++) {
            xml_node_iterator svgIterator = mainSvgNode.begin();
            if(route == TypeOfRoute::straightRoute) {
                for(Point * current : *(vector<Point*>*)distances) {
                    svgIterator->attribute("x").set_value( current->getHorizontalAxis()*(i+1));
                    svgIterator->attribute("y").set_value( current->getVerticalAxis()*(i+1));
                    svgIterator++;
                }
            } else {
                for(vector<Point*>  current : *((vector<vector<Point*>>*)distances)) {
                    Point newOffset = beizerCurve(* current[0],* current[2],* current[1],(1.0/frames)*(i+1));
                    svgIterator->attribute("x").set_value(newOffset.getHorizontalAxis());
                    svgIterator->attribute("y").set_value(newOffset.getVerticalAxis());
                    svgIterator++;
                }
            }
            xml_document *copiedDoc = new xml_document();
            copiedDoc->reset(*docPointer);
            producedFrames.push(copiedDoc);
        }
        keepRepetingConsumer = false;
    }*/

    void createSVGMaskGroup() {
        svgMaskGroup = nodeCreator.append_child("svg");

        xml_node maskNode = svgMaskGroup.append_child("mask");
        maskNode.append_attribute("id").set_value("maskForSvg");
        for(Point currentPoint : *originalCoordinates) {
            xml_node squaredMask = maskNode.append_child("rect");
            squaredMask.append_attribute("x").set_value(currentPoint.getHorizontalAxis()-squareSize);
            squaredMask.append_attribute("y").set_value(currentPoint.getVerticalAxis()-squareSize);
            squaredMask.append_attribute("height").set_value(squareSize);
            squaredMask.append_attribute("width").set_value(squareSize);
            squaredMask.append_attribute("fill").set_value("white");
        }

        xml_node pathGroup = svgMaskGroup.append_child("g");
        pathGroup.append_attribute("mask").set_value("url(#maskForSvg)");
        for(xml_node currentPath : *pathCollection) {
            pathGroup.append_copy(currentPath);
        }
    }

    void createFrame(int pointIndex) {
        
        if(svgMaskGroup.empty()) {
            createSVGMaskGroup();
        }
        
        Point *currentPoint = (*maskCoordinates)[pointIndex];
        xml_document *newFrame = new xml_document();
        newFrame->reset(*docPointer);
        xml_node copiedSVGMaskGroup = newFrame->child("svg").append_copy(svgMaskGroup);
        copiedSVGMaskGroup.append_attribute("x").set_value(currentPoint->getHorizontalAxis());
        copiedSVGMaskGroup.append_attribute("y").set_value(currentPoint->getVerticalAxis());
        producedFrames.push(newFrame);
    }

    bool backtrackerProducer(int pointIndexTest, int lastSuccessfulIndex) {
        Point *current = (*maskCoordinates)[pointIndexTest];
        Point *lastSuccessful = (*maskCoordinates)[lastSuccessfulIndex];
        double distance = current->getDistanceBetweenPoints(*lastSuccessful);
        if(distance >= squareSize || pointIndexTest == 0) {
            int lastSuccessfulIndex = pointIndexTest;
            createFrame(pointIndexTest);

            for(int pointIndex = pointIndexTest + 1; pointIndex < maskCoordinates->size(); pointIndex++) {
                if(backtrackerProducer(pointIndex,lastSuccessfulIndex)) {
                    return true;
                }
            }
            return true;
        } else
            return false;
    }

    void consumer() {
        int frameExported = 1;
        xml_document *extractedDoc;
        while (keepRepetingConsumer || !producedFrames.empty()) {
            sleep(1);
            if(!producedFrames.empty()) {
                extractedDoc = producedFrames.front();
                producedFrames.pop();
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

    Generator(TypeOfRoute pRoute, int pFrames, string pFileName, xml_document *pDocPointer, vector<Point> *pOriginalCoordinates) {
        processId = 2;
        route = pRoute;
        frames = pFrames;
        originalFileName = pFileName;
        originalFileName = "./Result/" + originalFileName.substr(0,originalFileName.size()-4);\
        mkdir("./Result/");
        keepRepetingConsumer = true;
        docPointer = pDocPointer;
        originalCoordinates = pOriginalCoordinates;
    }

    ~Generator() {}

    void work() {
        cout << "Working..." << endl;
        thread consumerThread(&Generator::consumer,this);
        backtrackerProducer(0,0);
        keepRepetingConsumer = false;
        consumerThread.join();
        cout << "Animation complete" << endl;
    }

    void update(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) {
        cout << "Generator started working" << endl;
        maskCoordinates = (vector<Point*>*) pCoordinates;
        pathCollection = pPathCollection;
        canvasSize = pCanvasSize;
        squareSize = (canvasSize->getWidth() + canvasSize->getHeight())/40;
        work();
    }

    int getProcessId() {
        return processId;
    }

};

#endif // GENERATOR_H
