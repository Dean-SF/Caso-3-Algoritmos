#ifndef GENERATOR_H
#define GENERATOR_H

#include "ObserverPattern.h"
#include <iostream>

using std::cout;
using std::endl;

class Generator : public Observer {
private:
    int processId;

public:
    Generator() {
        processId = 2;
    }
    ~Generator() {}

    void work(void* pDocPointer) {
        cout << "Working..." << endl;
        cout << "Animation complete" << endl;
    }

    void update(void* pDocPointer) {
        cout << "Generator started working" << endl;
        work(pDocPointer);
    }

    int getProcessId() {
        return processId;
    }

};

#endif // GENERATOR_H
