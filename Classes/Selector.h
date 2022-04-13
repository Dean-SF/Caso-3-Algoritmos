#ifndef SELECTOR_H
#define SELECTOR_H

#include <string>
#include <vector>
#include <sstream>
#include "Point.h"
#include <algorithm>
#include "Resolution.h"
#include <unordered_set>
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


/*
Class used for the selecting process, creates a "SVG" Node with a "g" Node and a "mask" node.
In the g node goes all the shapes from the original SVG with matching color. In the mask node goes
a square in a given position. this svg node is copied by the amount of given points and changes the
square in mask to the position of a point.
*/
class Selector {
private:
    int coordinateOffset;
    xml_document svgFile;
    xml_node svgNodeGroup;
    xml_document nodeCreator;
    Resolution svgResolution;
    vector<Point> coordinates;
    unordered_set<string> colors;
    xml_parse_result svgLoadingResult;

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
    void selectShapes() {
        xml_node_iterator iterator;
        xml_node mainSvgNode = svgFile.child("svg");
        xml_node newSvgG = svgNodeGroup.append_child("g");
        newSvgG.append_attribute("mask");
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
    void generateMask() {
        xml_node newMask = svgNodeGroup.append_child("mask");
        newMask.append_attribute("id");

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
    void generateSvgNodeGroup() {
        svgNodeGroup.append_attribute("x").set_value(0);
        svgNodeGroup.append_attribute("y").set_value(0);
        generateMask();
        selectShapes();
    }

    /*
    Main algorithm for the selection process. Calls a function to create a svg node with a
    mask and the shapes that corresponds with the given colors. Then the algorithm makes
    copies of the svg node in the main document, with the square mask in the position of
    each point given to the algorithm
    */
    void selection() {
        svgResolution.setViewBoxResolution(svgFile.child("svg").attribute("viewBox").value());
        generateSvgNodeGroup();
        for(int pointPosition = 0; pointPosition < coordinates.size(); pointPosition++) {
            
            xml_node newSvgNodeGroup = svgFile.child("svg").append_copy(svgNodeGroup);

            string maskId = "punto" + to_string(pointPosition);
            xml_node newMask = newSvgNodeGroup.child("mask");
            newMask.attribute("id").set_value(&maskId[0]);
            xml_node newSquareMask = newMask.child("rect");

            Point currentPoint = coordinates[pointPosition];

            newSquareMask.attribute("x").set_value(currentPoint.getHorizontalAxis() - coordinateOffset);
            newSquareMask.attribute("y").set_value(currentPoint.getVerticalAxis() - coordinateOffset);

            xml_node newGSvg = newSvgNodeGroup.child("g");
            maskId = "url(#punto" + to_string(pointPosition) + string(")");
            newGSvg.attribute("mask").set_value(&maskId[0]);
        }
    }

public:
    Selector(string pPath) {
        svgLoadingResult = svgFile.load_file(&pPath[0]);
        svgNodeGroup = nodeCreator.append_child("svg");
        coordinateOffset = 0;
    }

    // Starts the selection process
    xml_document *start() {
        if(!svgLoadingResult) {
            return nullptr;
        }
        selection();
        return &svgFile;
    }

    // setters and getters:

    vector<Point> getCoordinates() {
        return coordinates;
    }

    Resolution getResolution() {
        return svgResolution;
    }

    void setColors(vector<string> pColorList) {
        for(string color : pColorList) {
            colors.insert(color);
        }
    }

    void setCoordinates(vector<Point> pCoordinates) {
        coordinates = pCoordinates;
    }

    
};

#endif