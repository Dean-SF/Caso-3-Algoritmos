#include <iostream>
#include <list>

#include "observerPattern.h"

using namespace std;

class Estudiante : public Observer {
public:
    Estudiante() {}
    ~Estudiante() {}

    void update() {
        cout << "Se ha publicado una nueva tarea" << endl;
    }
} ;

class Plataforma : public Subject {
private:
    list<Observer*> estudiantes;

public:
    Plataforma() {}
    ~Plataforma() {}

    void attach(Observer* est) {
        estudiantes.emplace_back(est);
    }

    void detach(Observer* est) {
        estudiantes.remove(est);
    }

    void notify() {
        for (Observer* actual : estudiantes) {
            actual->update();
        }
    }
};


main () {
    Observer* est1 = new Estudiante();
    Observer* est2 = new Estudiante();
    Observer* est3 = new Estudiante();

    Subject* plat = new Plataforma();
    plat->attach(est1);
    plat->attach(est2);
    plat->attach(est3);

    plat->notify();

    cout << endl;
    cout << "Estudiante 1 ha abandonado un curso" << endl;
    plat->detach(est1);
    cout << "Vamos a crear una tarea nueva" << endl;
    plat->notify();

    return 0;
}
