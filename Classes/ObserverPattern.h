#ifndef OBSERVERPATTERN_H
#define OBSERVERPATTERN_H

#include "Resolution.h"
#include <vector>
#include "../libraries/pugixml/pugixml.hpp"

using std::vector;
using pugi::xml_node;

class Observer {
public:
  virtual ~Observer(){};
  virtual void update(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) = 0;
  virtual int getProcessId() = 0;
};

class Subject {
public:
  virtual ~Subject(){};
  virtual void attach(Observer *pObserver) = 0;
  virtual void detach(Observer *pObserver) = 0;
  virtual void notify(vector<xml_node> *pPathCollection, void *pCoordinates, Resolution *pCanvasSize) = 0;
};

#endif // OBSERVERPATTERN_H
