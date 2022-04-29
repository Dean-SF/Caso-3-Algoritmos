#ifndef SELECTOR_H
#define SELECTOR_H

#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include "Point.h"
#include <algorithm>
#include "Resolution.h"
#include <unordered_set>
#include "observerPattern.h"
#include "../libraries/pugixml/pugixml.hpp"

using std::stod;
using std::find;
using std::string;
using std::vector;
using pugi::xml_node;
using std::to_string;
using std::stringstream;
using pugi::xml_document;
using std::unordered_set;
using pugi::xml_object_range;
using pugi::xml_parse_result;
using pugi::xml_node_iterator;

using std::regex;
using std::smatch;

#include <iostream>
using std::cout;
using std::endl;

/*
Class used for the selecting process, creates a "SVG" Node with a "g" Node and a "mask" node.
In the g node goes all the shapes from the original SVG with matching color. In the mask node goes
a square in a given position. this svg node is copied by the amount of given points and changes the
square in mask to the position of a point.
*/
class Selector : public Subject, public Observer {
private:
    Observer* animator;
    int processId;
    xml_document *svgFile;
    double coordinateOffset;
    Point maxProximityCoords;
    Point minProximityCoords;
    Resolution svgResolution;
    vector<Point> *coordinates;
    unordered_set<string> colors;
    vector<xml_node> *pathCollection;
    


    /*
    Calculates the pointOffset depending on the square size 
    to put it on the center of a given point
    
    void calculatePointOffset(int pSquareSize) {
        coordinateOffset = pSquareSize/2;
    }

    /*
    Selects all the original SVG Shapes that the color matches with one
    on the 'colorList'
    
    void selectShapes(xml_node pSvgNodeGroup) {
        xml_node_iterator iterator;
        xml_node mainSvgNode = svgFile->child("svg");
        xml_node newSvgG = pSvgNodeGroup.append_child("g");
        newSvgG.append_attribute("mask").set_value("url(#punto");
        for(iterator = mainSvgNode.begin(); iterator != mainSvgNode.end(); iterator++) {
            string colorCode = iterator->attribute("fill").value();
            if(colors.find(colorCode) != colors.end()) {
                newSvgG.append_copy(*iterator);
            }
        }
    }

    /*
    Creates a mask with a square, the sizes depends on the viewBox size of
    the SVG. The square is white so that everything on it is
    
    void generateMask(xml_node pSvgNodeGroup) {
        xml_node newMask = pSvgNodeGroup.append_child("mask");
        newMask.append_attribute("id").set_value("punto");

        xml_node squareMask = newMask.append_child("rect");

        int squareSize = (svgResolution.getWidth() + svgResolution.getHeight())/40;
        squareMask.append_attribute("width").set_value(squareSize);
        squareMask.append_attribute("height").set_value(squareSize);
        squareMask.append_attribute("x");
        squareMask.append_attribute("y");
        squareMask.append_attribute("fill").set_value("white");

        calculatePointOffset(squareSize);
    }

    /*
    Generates all the data needed in the svg node created at the creation of the\
    class, so it can be use to complete the selection process
    
    xml_node generateSvgNodeGroup() {
        xml_node svgNodeGroup = nodeCreator.append_child("svg");

        svgNodeGroup.append_attribute("x").set_value(0);
        svgNodeGroup.append_attribute("y").set_value(0);
        generateMask(svgNodeGroup);
        selectShapes(svgNodeGroup);

        return svgNodeGroup;
    }

    /*
    Main algorithm for the selection process. Calls a function to create a svg node with a
    mask and the shapes that corresponds with the given colors. Then the algorithm makes
    copies of the svg node in the main document, with the square mask in the position of
    each point given to the algorithm
    
    void selectionAux(xml_node previousSvgNode, int pCoordsIndex) {
        if(pCoordsIndex >= coordinates->size()) {
            return;
        }
        if(previousSvgNode.empty()) {
            previousSvgNode = generateSvgNodeGroup();
        }
        xml_node newSvgNodeGroup = mainSvgGroup.append_copy(previousSvgNode);
        xml_node SvgNodeGroupMask = newSvgNodeGroup.child("mask");
        string maskId = SvgNodeGroupMask.attribute("id").value();
        maskId = maskId.substr(0,5) + to_string(pCoordsIndex);
        SvgNodeGroupMask.attribute("id").set_value(&maskId[0]);
        xml_node SquareMask = SvgNodeGroupMask.child("rect");

        Point *currentPoint = &((*coordinates)[pCoordsIndex]);
        currentPoint->setHorizontalAxis(currentPoint->getHorizontalAxis() - coordinateOffset);
        currentPoint->setVerticalAxis(currentPoint->getVerticalAxis() - coordinateOffset);

        SquareMask.attribute("x").set_value(currentPoint->getHorizontalAxis());
        SquareMask.attribute("y").set_value(currentPoint->getVerticalAxis());
        

        xml_node gNodeSvg = newSvgNodeGroup.child("g");
        maskId = gNodeSvg.attribute("mask").value();
        maskId = maskId.substr(0,10) + to_string(pCoordsIndex) + string(")");
        gNodeSvg.attribute("mask").set_value(&maskId[0]);

        pCoordsIndex++;

        selectionAux(newSvgNodeGroup,pCoordsIndex);
    }

    void selection() {
        xml_node previousSvgNode;
        mainSvgGroup = svgFile->child("svg").append_child("svg");
        svgResolution.setViewBoxResolution(svgFile->child("svg").attribute("viewBox").value(),false);
        selectionAux(previousSvgNode, 0);
    }*/
    
    void getProximityCoords() {
        double maxXAxis = 0,maxYAxis = 0;
        double minXAxis = svgResolution.getWidth();
        double minYAxis = svgResolution.getHeight();
        for(Point current : *coordinates) {
            double currentXAxis = current.getHorizontalAxis();
            double currentYAxis = current.getVerticalAxis();
            if(currentXAxis > maxXAxis )
                maxXAxis = current.getHorizontalAxis();
            if(currentYAxis > maxYAxis)
                maxYAxis = current.getVerticalAxis();
            if(currentXAxis < minXAxis)
                minXAxis = currentXAxis;
            if(currentYAxis < minYAxis)
                minYAxis = currentYAxis; 
        }
        
        maxProximityCoords = Point(maxXAxis+coordinateOffset,maxYAxis+coordinateOffset);
        minProximityCoords = Point(minXAxis-coordinateOffset,minYAxis-coordinateOffset);
    }

    bool checkIfValidPath(xml_node path) {
        smatch match;

        string pathDrawnValue = path.attribute("d").value();

        regex yAxisRegex(",(\\d+(\\.\\d+)?)"); // match[1]
        regex xAxisRegex("(\\d+(\\.\\d+)?)(?=,)"); // match[0]
        
        if(pathDrawnValue[0] != 'M')
            return false; 

        regex_search(pathDrawnValue,match,xAxisRegex);
        double pathXAxis = stod(match[0]);
        regex_search(pathDrawnValue,match,yAxisRegex);
        double pathYAxis = stod(match[1]);

        if(pathXAxis <= maxProximityCoords.getHorizontalAxis() && pathXAxis >= minProximityCoords.getHorizontalAxis() &&
           pathYAxis <= maxProximityCoords.getVerticalAxis() && pathYAxis >= minProximityCoords.getVerticalAxis())
            return true;
        else
            return false;
    }

    void selection() {
        getProximityCoords();
        xml_node_iterator iterator;
        xml_node mainSvgNode = svgFile->child("svg");
        for(iterator = mainSvgNode.begin(); iterator != mainSvgNode.end(); iterator++) {
            string colorCode = iterator->attribute("fill").value();
            if(colors.find(colorCode) != colors.end()) {
                if(checkIfValidPath(*iterator))
                    pathCollection->emplace_back(*iterator);
            }
        }
    }

public:
    Selector(vector<string> pColors) {
        setColors(pColors);
        pathCollection = new vector<xml_node>();
        processId = 0;
    }

    void attach(Observer* pAnimator) {
        animator = pAnimator;
    }

    void detach(Observer* pAnimator) {
        delete animator;
    }

    // setters and getters:

    vector<Point> *getCoordinates() {
        return coordinates;
    }

    Resolution getResolution() {
        return svgResolution;
    }

    int getProcessId() {
        return processId;
    }

    void setCoordinates(vector<Point> *pCoordinates) {
        coordinates = pCoordinates;
    }

    void setColors(vector<string> pColorList) {
        for(string color : pColorList) {
            colors.insert(color);
        }
    }

    void work() {  
        cout << "Selector is working..." << endl;
        svgResolution.setViewBoxResolution(svgFile->child("svg").attribute("viewBox").value(),false);
        coordinateOffset = (svgResolution.getWidth() + svgResolution.getHeight())/40;
        selection();
        notify(pathCollection, coordinates);
    }

    void notify(void* pathCollection, void* pCoordinates) {
        cout << "Selector is done" << endl;
        animator->update(pathCollection, pCoordinates);
    }

    void update(void* pDocPointer, void* pCoordinates) {
        svgFile = (xml_document *)pDocPointer;
        coordinates = (vector<Point>*) pCoordinates;
        work();
    }

};

#endif
