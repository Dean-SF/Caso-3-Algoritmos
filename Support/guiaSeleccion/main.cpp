#include <iostream>
#include <fstream>
#include <sstream>
#include "../../libraries/pugixml/pugixml.hpp"

using namespace std;
using namespace pugi;

int main() {
   
    xml_document doc;
    xml_parse_result result = doc.load_file("./svgcaso3.svg");

    xml_node newSvg = doc.child("svg").append_child("svg");
    xml_node newG = newSvg.append_child("g");
    newG.append_attribute("mask").set_value("url(#punto0)");
    //xml_node newMask = newSvg.append_child("mask");

    auto iterator = doc.child("svg").begin();
    for(; iterator != doc.child("svg").end(); iterator++) {
        if(string(iterator->name()) != "svg") {
            newG.append_copy(*iterator);
        }
    }
    for(int i = 1; i < 6; i++) {
        string hola = "url(#punto" + to_string(i) + ")";
        xml_node a = doc.child("svg").append_copy(newSvg);
        a.child("g").attribute("mask").set_value(&hola[0]);
    }

    

    ofstream *copyFile = new ofstream("Newsvg.svg",fstream::trunc);
    stringstream ss;
    doc.save(ss," ");
    string stringXML = ss.str();
    *copyFile << stringXML;
    copyFile->close();
    
    delete copyFile;

}