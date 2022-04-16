#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "ObserverPattern.h"
#include <iostream>
#include <list>

using std::cout;
using std::endl;
using std::list;
using namespace pugi;

class Animator : public Subject, public Observer {
private:
    list<Observer*> observers;
    int stage;

public:
    Animator() {
        stage = -1;
    }
    ~Animator() {}

    void attach(Observer* pNewObserver) {
        observers.emplace_back(pNewObserver);
    }

    void detach(Observer* pNewObserver) {
        observers.remove(pNewObserver);
    }

    void notify(xml_document* pDocPointer) {
    /* a. Selector = 0 -> Router = 1
       b. Router = 1 -> Generator = 2

       a. Selector notified animator, so animator notifies router
       b. Router notified animator, so animator notifies generator */
        //list<Observer*> iterator observerIterator;
        stage++;
        for (Observer* observer : observers) {
            if (observer->getProcessId() == stage) {
                observer->update(pDocPointer);
                break;
            }
        }
    }

    void startProcess(xml_document* pDocPointer) {
        notify(pDocPointer);
    }

    void update(xml_document* pDocPointer) {
        notify(pDocPointer);
    }

    int getProcessId() {
        return -1;
    }

};

#endif // ANIMATOR_H
