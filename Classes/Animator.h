#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "ObserverPattern.h"
#include <list>

using std::list;

class Animator : public Observer {
private:
    list<Observer*> observers;
    int stage;

public:
    Animator() {
        stage = 0;
    }
    ~Animator() {}

    void attach(Observer* pNewObserver) {
        observers.emplace_back(pNewObserver);
    }

    void detach(Observer* pNewObserver) {
        observers.remove(pNewObserver);
    }

    void update(void* pDocPointer) {
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

    int getProcessId() {
        return 0;
    }
};

#endif // ANIMATOR_H
