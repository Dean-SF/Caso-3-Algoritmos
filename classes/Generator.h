#ifndef GENERATOR_H
#define GENERATOR_H

#include <queue>
#include <thread>
#include <string>
#include "Point.h"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include "TypeOfRoute.h"
#include "ObserverPattern.h"

using std::cout;
using std::endl;
using std::queue;
using std::thread;
using std::string;
using std::ofstream;
using std::to_string;
using pugi::xml_node;
using std::stringstream;
using pugi::xml_document;


/* Generator Process - Backtracking
N: Coordinates from router (equal to the amount of frames)

Solution Vector: Every posible frame in each coordinate calculated by the router

How to determine if a solution is valid: before generating a frame, we compare the last
used coordinate to generate a frame, and the next coordinates until we get a valid distance
between the two coordinates to generate the frames

*/
class Generator : public Observer {
private:

    int frames;
    int processId;
    int squareSize;
    TypeOfRoute route;
    xml_node svgMaskGroup;
    Resolution *canvasSize;
    string originalFileName;
    xml_document nodeCreator;
    xml_document *docPointer;
    bool keepRepetingConsumer;
    vector<Point*> *maskCoordinates;
    vector<xml_node> *pathCollection;
    vector<Point> *originalCoordinates;
    queue<xml_document*> producedFrames;

    /*
    This function creates a "easy to move" group of paths and mask which will be
    copied multiple times so we dont recalculate it.
    Time Complexity: we have two cycles with different sizes. They arent nested
    so we can conclude we have a formula like f(n) = O(n) + O(n) + O(C).
    Considering only the worse case, we have a O(n) (linear) complexity
    */
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

    /*
    This function is used to generate a frame. it's called multiple
    times, but only once it calls the function "createSVGMaskGroup()".
    Time Complexity: the first time we call this function it will have a time
    complexity of O(n) (linear) because of the funtion "createSVGMaskGroup()"
    being linear. But every other time we call this funtion we have a 
    O(C) (constant) time complexity because of the lack of cycles
    */
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

    /*
    This recursive function is the main backtracking generation process.
    for each successful frame, it search the next successful frame.

    Time complexity: This function being recursive, is like a cycle.
    and it will loop through all the coordinates calculated by the router
    once, including the for loop in it. so, considering the for loop is what
    makes possible the recursive calls, and is setup in a way, only once we
    go through a coordiante, we can considere the time complexity as O(n) (linear).
    We execute multiple times the function "createFrame()" but only the first time it runs
    is O(n), the rest is O(C), so it doesnt make it O(n^2). In conclusion we can say
    that this algorithm is as a whole, O(n) (linear) complexity
    */
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

    /*
    This function is in charge of writting to disk the files produce by the "backtrackerProducer()".
    It's time complexity is no really necessary. It's a infinite while loop that stops when
    "KeepRepetingConsumer" is false and "produceFrames.empty()" is true. It's executed in a different
    thread besides the main thread, and will write the files at the same time they are created.
    */
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
    
    /*
    In the "backtrackerProducer()" we concluded that the time complexity of the whole algorithm is linear.
    We dont take consumer into account because we can consider it as the same instructions being repeated 
    the same amount of times as there are files to be written. If we didn't need to include a Producer-Consumer
    Pattern, the same instructions executed when writting a file created in "backtrackerProducer()" would be 
    executed each time we created a frame, instead of adding it to a queue. So we can say that's constant time.
    In conclusion we could say, the algorithm including the consumer, would be, linear complexity ( O(n) )
    */

public:
    Generator() {
        processId = 2;
        frames = 0;
        mkdir("./Result/");
        docPointer = nullptr;
        route = straightRoute;
        originalFileName = "./Result/null";
        keepRepetingConsumer = true;
        originalCoordinates = nullptr;
    }

    Generator(TypeOfRoute pRoute, int pFrames, string pFileName, xml_document *pDocPointer, vector<Point> *pOriginalCoordinates) {
        processId = 2;
        route = pRoute;
        frames = pFrames;
        mkdir("./Result/");
        docPointer = pDocPointer;
        keepRepetingConsumer = true;
        originalFileName = pFileName;
        originalCoordinates = pOriginalCoordinates;
        originalFileName = "./Result/" + originalFileName.substr(0,originalFileName.size()-4);
    }

    ~Generator() {
        delete pathCollection;
    }

    int getProcessId() {
        return processId;
    }

    void work() {
        thread consumerThread(&Generator::consumer,this);
        backtrackerProducer(0,0);
        keepRepetingConsumer = false;
        consumerThread.join();
        cout << "Generation process finished" << endl;
        cout << "--------------------------" << endl;
        cout << "Animation complete" << endl;
        cout << "--------------------------" << endl;
    }

    void update(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) {
        cout << "Generation process started" << endl;
        canvasSize = pCanvasSize;
        pathCollection = pPathCollection;
        maskCoordinates = (vector<Point*>*) pCoordinates;
        squareSize = (canvasSize->getWidth() + canvasSize->getHeight())/40;
        work();
    }

};

#endif // GENERATOR_H
