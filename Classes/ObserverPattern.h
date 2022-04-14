#ifndef OBSERVERPATTERN_H
#define OBSERVERPATTERN_H

#include "../../libraries/pugixml/pugixml.hpp"

using pugi::xml_document;

class Observer {
public:
  virtual ~Observer(){};
  virtual void update(xml_document* pElement) = 0;
  virtual int getProcessId() = 0;
};

class Subject {
public:
  virtual ~Subject(){};
  virtual void attach(Observer *pObserver) = 0;
  virtual void detach(Observer *pObserver) = 0;
  virtual void notify(xml_document* pElement) = 0;
};

#endif // OBSERVERPATTERN_H
