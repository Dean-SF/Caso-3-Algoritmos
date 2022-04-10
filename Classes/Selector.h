#ifndef SELECTOR_H
#define SELECTOR_H

#include <string>
#include <sstream>
#include <List>
#include "Point.h"
#include "Resolution.h"
#include "libraries/pugixml/pugixml.hpp"

using pugi::xml_document;
using pugi::xml_parse_result;
using pugi::xml_node;
using std::string;
using std::list;
using std::stoi;
using std::to_string;
using std::stringstream;

class Selector {
private:
    xml_document *svgFile;
    xml_parse_result svgLoadingResult;
    Resolution svgResolution;
    list<Point> pointList;
    list<string> colorList;
    xml_node svgNodeGroup;
    int pointOffset;
    /*
    void makeMask() {
        xml_node squareMask = svgMask.append_child("rect");

        int squareSize = (svgResolution.getWidth() + svgResolution.getHeight())/40;
        squareMask.append_attribute("width").set_value(squareSize); // posible cambio a string
        squareMask.append_attribute("height").set_value(squareSize);
        squareMask.append_attribute("x").set_value(0);
        squareMask.append_attribute("y").set_value(0);
        squareMask.append_attribute("fill").set_value("white");
    }
    */

    void calculatePointOffset(int pSquareSize) {
        pointOffset = pSquareSize/2;
    }

    void setResolution() { 
        string svgViewBox = svgFile->child("svg").attribute("viewBox").value();
        stringstream stringManipulator(svgViewBox);
        string extractedString;
        for(int cycles = 0; cycles < 3; cycles++) {
            stringManipulator >> extractedString;
        }
        svgResolution.setWidth(stoi(extractedString));
        stringManipulator >> extractedString;
        svgResolution.setHeight(stoi(extractedString));

    }

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

    void generateSvgNodeGroup() {
        svgNodeGroup.append_attribute("x").set_value(0);
        svgNodeGroup.append_attribute("y").set_value(0);
        generateMask();
    }

    void selection() {
        xml_node newSvgNode = svgFile->child("svg").append_child("svg");
        xml_node newGNode = newSvgNode.append_child("g");
        newGNode.append_attribute("mask").set_value("url(#punto0)");
    }

public:
    Selector(string pPath) {
        svgFile = new xml_document();
        svgLoadingResult = svgFile->load_file(&pPath[0]);
        svgNodeGroup.set_name("svg");
        pointOffset = 0;
    }

    list<string> getColorList() {
        return colorList;
    }

    void setColorList(list<string> pColorList) {
        colorList = pColorList;
    }

    list<Point> getPointList() {
        return pointList;
    }

    void setPointList(list<Point> pPointList) {
        pointList = pPointList;
    }

    xml_document *start() {
        if(svgLoadingResult) {
            return nullptr;
        }
    }

};

#endif