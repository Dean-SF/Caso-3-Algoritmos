#ifndef ROUTER_H
#define ROUTER_H

#include "ObserverPattern.h"
#include <iostream>

using std::cout;
using std::endl;

class Router : public Subject, public Observer {
private:
    Observer* animator;
    int processId;

public:
    Router() {
        processId = 1;
    }
    ~Router() {}

    void attach(Observer* pAnimator) {
        animator = pAnimator;
    }

    void detach(Observer* pAnimator) {
        animator = nullptr;
    }

    void notify(void* pDocPointer) {
        cout << "Router is done" << endl;
        animator->update(pDocPointer);
    }

    void work(void* pDocPointer) {
        cout << "Working..." << endl;
        notify(pDocPointer);
    }

    void update(void* pDocPointer) {
        cout << "Router started working" << endl;
        work(pDocPointer);
    }

    int getProcessId() {
        return processId;
    }
};

#endif // ROUTER_H
