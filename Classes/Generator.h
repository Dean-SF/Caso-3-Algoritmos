#ifndef GENERATOR_H
#define GENERATOR_H

#include "ObserverPattern.h"
#include "../../libraries/pugixml/pugixml.hpp"

#include <iostream>

using std::cout;
using std::endl;
using pugi::xml_document;

class Generator : public Observer {
private:
    int processId;

public:
    Generator() {
        processId = 2;
    }
    ~Generator() {}

    void work(xml_document* pDocPointer) {
        cout << "Working..." << endl;
        cout << "Animation complete" << endl;
    }

    void update(xml_document* pDocPointer) {
        cout << "Generator started working" << endl;
        work(pDocPointer);
    }

    int getProcessId() {
        return processId;
    }

};

#endif // GENERATOR_H
