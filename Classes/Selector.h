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

    void work(xml_document* pDocPointer) {
        cout << "Selector is working..." << endl;
        notify(pDocPointer);
    }

    void notify(xml_document* pDocPointer) {
        cout << "Selector is done" << endl;
        animator->update(pDocPointer);
    }

    void update(xml_document* pDocPointer) {
        work(pDocPointer);
    }

    int getProcessId() {
        return processId;
    }
};

#endif // SELECTOR_H
