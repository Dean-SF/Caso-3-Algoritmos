#ifndef SELECTOR_H
#define SELECTOR_H

#include "ObserverPattern.h"
#include <iostream>

using std::cout;
using std::endl;

class Selector : public Subject {
private:
    Observer* animator;

public:
    Selector() {}
    ~Selector() {}

    void attach(Observer* pAnimator) {
        animator = pAnimator;
    }

    void detach(Observer* pAnimator) {
        animator = nullptr;
    }

    void work(void* pDocPointer) {
        cout << "Selector is working..." << endl;
        notify(pDocPointer);
    }

    void notify(void* pDocPointer) {
        cout << "Selector is done" << endl;
        animator->update(pDocPointer);
    }
};

#endif // SELECTOR_H
