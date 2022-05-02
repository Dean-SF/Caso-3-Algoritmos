#ifndef SELECTOR_H
#define SELECTOR_H

#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "Point.h"
#include <algorithm>
#include "Resolution.h"
#include <unordered_set>
#include "observerPattern.h"
#include "../libraries/pugixml/pugixml.hpp"

using std::cout;
using std::endl;
using std::stod;
using std::find;
using std::regex;
using std::smatch;
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

/* Selector process - Greedy Algorithm
N: Paths in a given SVG
Stages: A Path
Criterion:
1. A path that has a color in the list given by the user, is selected as a candidate path
2. Using the coordinates given by the user, get an aproximated area where every 
"M" coordinate in a candiadate path (the first coordinates that appear after the M), that is in the area,
is selected to save in a vector for the generator process.
Local optimum: A path close to the given coordinates
Global optimum: Vector of paths close to the given coordinates
*/
class Selector : public Subject, public Observer {
private:
    Observer* animator;
    int processId;
    xml_document *svgFile;
    double coordinateOffset;
    Point maxProximityCoords;
    Point minProximityCoords;
    Resolution canvasSize;
    vector<Point> *coordinates;
    unordered_set<string> colors;
    vector<xml_node> *pathCollection;
    
    /*
    This method calculates the diagonal coordinates of a rectangular
    area, where every path inside of it is consider as a path close to
    the coordinates given by the user.
    Time Complexity: This fuction loops through all of the user given points
    onces, without any nested cycles, in conclusion, is O(n) (linear)
    */
    void getProximityCoords() {
        double maxXAxis = 0,maxYAxis = 0;
        double minXAxis = canvasSize.getWidth();
        double minYAxis = canvasSize.getHeight();
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

    /*
    This method verifies if a path is in the area calculated in the
    "getProximityCoords()" method.
    Time Complexity: We could said that this method is O(C) (Constant time) because
    of the lack of any cycles.
    */

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

    /*
    This method is in charge of the whole selection process. It calls "getProximityCoords()"
    once, and calls "checkIfValidPath()" multiple times to verify if a path is valid for selection.
    Time complexity: We could say that from the first function call we get an O(n) and then we loop through
    all the available paths which is O(n) because the checks of color and position in the path are constant.
    For instance we hace an O(n) + O(n), two independent loops with linear complexity, which makes the union
    of both, O(n) (linear).
    */
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

    /*
    For my final analysis, I conclude that the selection process has a
    time complexity of "O(n)" (linear), because the union of all the methods
    make so that there is no nested cycle that can change its complexity
    */

public:
    Selector(vector<string> pColors, xml_document *pDocPointer) {
        setColors(pColors);
        pathCollection = new vector<xml_node>();
        processId = 0;
        svgFile = pDocPointer;
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
        return canvasSize;
    }

    int getProcessId() {
        return processId;
    }

     void setResolution() {
        canvasSize.setViewBoxResolution(svgFile->child("svg").attribute("viewBox").value());
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
        coordinateOffset = (canvasSize.getWidth() + canvasSize.getHeight())/40;
        setResolution();
        selection();
        notify(pathCollection,coordinates,&canvasSize);
    }

    void notify(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) {
        cout << "Selector is done" << endl;
        animator->update(pPathCollection,pCoordinates,pCanvasSize);
    }

    void update(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) {
        coordinates = (vector<Point>*) pCoordinates;
        work();
    }

};

#endif
