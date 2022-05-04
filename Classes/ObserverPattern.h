#ifndef OBSERVERPATTERN_H
#define OBSERVERPATTERN_H

#include "Resolution.h"
#include <vector>
#include "../libraries/pugixml/pugixml.hpp"

using std::vector;
using pugi::xml_node;

// This abstract class observes a subject and it gets notifications from that subject.
class Observer {
public:
  virtual ~Observer(){};
  virtual void update(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) = 0;
  virtual int getProcessId() = 0;
};

// This abstract class notifies its observers when needed
class Subject {
public:
  virtual ~Subject(){};
  virtual void attach(Observer *pObserver) = 0;
  virtual void detach(Observer *pObserver) = 0;
  virtual void notify(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) = 0;
};

#endif // OBSERVERPATTERN_H
