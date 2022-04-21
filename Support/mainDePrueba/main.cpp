#include <iostream>
#include "Classes/Selector.h"
#include "Classes/Point.h"
#include <vector>
#include <fstream>

using std::cout;
using std::endl;
using std::ofstream;
using std::vector;
using pugi::xml_writer;

int main() {
    Selector selector("svgcaso3.svg");

    vector<Point> puntos;
    vector<string> colores;

    puntos.emplace_back(Point(50,100));
    puntos.emplace_back(Point(100,200));
    puntos.emplace_back(Point(100,320));
    puntos.emplace_back(Point(350,400));
    puntos.emplace_back(Point(350,400));
    puntos.emplace_back(Point(250,400));
    
    colores.emplace_back("#FF2970");
    colores.emplace_back("#D10257");
    colores.emplace_back("#5349B7");
    colores.emplace_back("#3B38A5");
    colores.emplace_back("#3B38A5");
    colores.emplace_back("#D10257");
    colores.emplace_back("#37167F");
    colores.emplace_back("#2EB9FF");
    colores.emplace_back("#0097F4");

    selector.setColors(colores);
    selector.setCoordinates(puntos);

    xml_document *doc = selector.start();

    if(doc == nullptr) {
        cout << "error" << endl;
        return 0;
    }
    ofstream *copyFile = new ofstream("Newsvg.svg",ofstream::trunc);
    stringstream ss;
    doc->save(ss," ");
    string stringXML = ss.str();
    *copyFile << stringXML;
    copyFile->close();

    return 0;
}