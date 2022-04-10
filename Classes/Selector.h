#ifndef SELECTOR_H
#define SELECTOR_H

#include <string>
#include <sstream>
#include <vector>
#include "Point.h"
#include "Resolution.h"
#include "../libraries/pugixml/pugixml.hpp"

using pugi::xml_document;
using pugi::xml_parse_result;
using pugi::xml_node_iterator;
using pugi::xml_object_range;
using pugi::xml_node;
using std::string;
using std::vector;
using std::to_string;
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;


class Selector {
private:
    xml_document svgFile;
    xml_document nodeCreator;
    xml_parse_result svgLoadingResult;
    Resolution svgResolution;
    vector<Point> pointList;
    vector<string> colorList;
    xml_node svgNodeGroup;
    int pointOffset;

    void calculatePointOffset(int pSquareSize) {
        pointOffset = pSquareSize/2;
    }

    void shapeSelection() {
        xml_node_iterator iterator;
        xml_node mainSvgNode = svgFile.child("svg");
        xml_node newSvgG = svgNodeGroup.append_child("g");
        newSvgG.append_attribute("mask");
        for(iterator = mainSvgNode.begin(); iterator != mainSvgNode.end(); iterator++) {
            string colorCode = iterator->attribute("fill").value();
            for(string color : colorList) {
                if(color == colorCode) {
                    newSvgG.append_copy(*iterator);
                }
            }
            
        }
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
        shapeSelection();
    }

    void selection() {
        svgResolution.setViewBoxResolution(svgFile.child("svg").attribute("viewBox").value());
        generateSvgNodeGroup();
        for(int pointPosition = 0; pointPosition < pointList.size(); pointPosition++) {
            
            xml_node newSvgNodeGroup = svgFile.child("svg").append_copy(svgNodeGroup);

            string maskId = "punto" + to_string(pointPosition);
            xml_node newMask = newSvgNodeGroup.child("mask");
            newMask.attribute("id").set_value(&maskId[0]);
            xml_node newSquareMask = newMask.child("rect");

            Point currentPoint = pointList[pointPosition];

            newSquareMask.attribute("x").set_value(currentPoint.getHorizontalAxis() - pointOffset);
            newSquareMask.attribute("y").set_value(currentPoint.getVerticalAxis() - pointOffset);

            xml_node newGSvg = newSvgNodeGroup.child("g");
            maskId = "url(#punto" + to_string(pointPosition) + string(")");
            newGSvg.attribute("mask").set_value(&maskId[0]);
        }
    }

public:
    Selector(string pPath) {
        svgLoadingResult = svgFile.load_file(&pPath[0]);
        svgNodeGroup = nodeCreator.append_child("svg");
        pointOffset = 0;
    }

    vector<string> getColorList() {
        return colorList;
    }

    void setColorList(vector<string> pColorList) {
        colorList = pColorList;
    }

    vector<Point> getPointList() {
        return pointList;
    }

    void setPointList(vector<Point> pPointList) {
        pointList = pPointList;
    }

    xml_document *start() {
        if(!svgLoadingResult) {
            return nullptr;
        }
        selection();
        return &svgFile;
    }

    xml_node *svg() {
        return &svgNodeGroup;
    }

};

#endif