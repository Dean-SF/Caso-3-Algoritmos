#ifndef SELECTOR_H
#define SELECTOR_H

#include <string>
#include <vector>
#include <sstream>
#include "Point.h"
#include <algorithm>
#include "Resolution.h"
#include <unordered_set>
#include "observerPattern.h"
#include "../libraries/pugixml/pugixml.hpp"


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
    int coordinateOffset;
    xml_document *svgFile;
    xml_node mainSvgGroup;
    xml_document nodeCreator;
    Resolution svgResolution;
    vector<Point> *coordinates;
    unordered_set<string> colors;

    /*
    Calculates the pointOffset depending on the square size 
    to put it on the center of a given point
    */
    void calculatePointOffset(int pSquareSize) {
        coordinateOffset = pSquareSize/2;
    }

    /*
    Selects all the original SVG Shapes that the color matches with one
    on the 'colorList'
    */
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
    */
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
    */
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
    */
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

        Point currentPoint = (*coordinates)[pCoordsIndex];

        SquareMask.attribute("x").set_value(currentPoint.getHorizontalAxis() - coordinateOffset);
        SquareMask.attribute("y").set_value(currentPoint.getVerticalAxis() - coordinateOffset);

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
    }

public:
    Selector(vector<string> pColors) {
        setColors(pColors);
        coordinateOffset = 0;
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
        selection();
        notify(svgFile, &coordinates);
    }

    void notify(xml_document* pDocPointer, void* pCoordinates) {
        cout << "Selector is done" << endl;
        animator->update(pDocPointer, pCoordinates);
    }

    void update(xml_document* pDocPointer, void* pCoordinates) {
        cout << "test" << endl;
        svgFile = pDocPointer;
        coordinates = (vector<Point>*) pCoordinates;
        work();
    }

};

#endif
