#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "ObserverPattern.h"
#include <iostream>
#include <list>
#include <vector>

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
        observers = list<Observer*>();
    }
    ~Animator() {}

    void attach(Observer* pNewObserver) {
        observers.emplace_back(pNewObserver);
    }

    void detach(Observer* pNewObserver) {
        observers.remove(pNewObserver);
    }

    void notify(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) {
    /* 
       a. start process = -1
       b. Selector = 0 -> Router = 1
       c. Router = 1 -> Generator = 2

       b. Selector notified animator, so animator notifies router
       c. Router notified animator, so animator notifies generator */
    
        stage++;
        for (Observer* observer : observers) {
            if (observer->getProcessId() == stage) {
                cout << observer->getProcessId() << endl;
                observer->update(pPathCollection, pCoordinates, pCanvasSize);
                break;
            }
        }
    }

    void update(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) {
        notify(pPathCollection, pCoordinates, pCanvasSize);
    }

    int getProcessId() {
        return -1;
    }

};

#endif // ANIMATOR_H
