#ifndef OBSERVERPATTERN_H
#define OBSERVERPATTERN_H

class Observer {
public:
  virtual ~Observer(){};
  virtual void update(void* pElement) = 0;
  virtual int getProcessId() = 0;
};

class Subject {
public:
  virtual ~Subject(){};
  virtual void attach(Observer *pObserver) = 0;
  virtual void detach(Observer *pObserver) = 0;
  virtual void notify(void* pElement) = 0;
};

#endif // OBSERVERPATTERN_H
