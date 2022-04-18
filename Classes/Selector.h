#ifndef SELECTOR_H
#define SELECTOR_H

#include "ObserverPattern.h"

#include <iostream>

using std::cout;
using std::endl;
using namespace pugi;

class Selector : public Subject, public Observer {
private:
    Observer* animator;
    int processId;

public:
    Selector() {
        processId = 0;
    }
    ~Selector() {}

    void attach(Observer* pAnimator) {
        animator = pAnimator;
    }

    void detach(Observer* pAnimator) {
        delete animator;
    }

    void work(void* pDocPointer, void* pCoordinates) {  
        cout << "Selector is working..." << endl;
        notify(pDocPointer, pCoordinates);
    }

    void notify(void* pDocPointer, void* pCoordinates) {
        cout << "Selector is done" << endl;
        animator->update(pDocPointer, pCoordinates);
    }

    void update(void* pDocPointer, void* pCoordinates) {
        work(pDocPointer, pCoordinates);
    }

    int getProcessId() {
        return processId;
    }
};

#endif // SELECTOR_H
