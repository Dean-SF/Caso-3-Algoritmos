#include <iostream>
#include <list>
#include <vector>
#include <thread>

#include "patronObserver2.h"

using namespace std;
//class Profesor {
//private:
//    string nombre;
//public:
//    Profesor(string nombre) {
//        this->nombre = nombre;
//    }
//    ~Profesor() {}
//
//    string getNombre() {
//        return nombre;
//    }
//};

class Estudiante : public Observer {
public:
    Estudiante() {}
    ~Estudiante() {}

    void update(void* curso) {
        // (int*) = castear el puntero void a puntero a int
        // *(int*) = valor del int al que apunta
        int value = *(int*)curso;

        if (value == 0)
            cout << "Se ha publicado una nueva tarea del curso Analisis de Algoritmos" << endl;
        else if (value == 1)
            cout << "Se ha publicado una nueva tarea del curso Bases de Datos 1" << endl;
        else if (value == 2)
            cout << "Se ha publicado una nueva tarea del curso Ambiente Humano" << endl;
    }

//    void update(void* profe) {
//        Profesor value = *(Profesor*)profe;
//        cout << "El profesor " << value.getNombre() << " creo una tarea" << endl;
//    }
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

    void notify(void* curso) {
        for (Observer* actual : estudiantes) {
            thread t(&Observer::update, actual, curso); // parametros = (direccionDeMetodo, instancia/objeto, parametro)
            t.join(); // espere a que t termine
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

    int curso = 2;
    int* cursoPointer = &curso;
    plat->notify(cursoPointer);

    cout << endl;
    cout << "Estudiante 1 ha abandonado un curso" << endl;
    plat->detach(est1);
    cout << "Vamos a crear una tarea nueva" << endl;
    curso = 1;
    plat->notify(cursoPointer);

    // Prueba con un objeto
//    Profesor* profe = new Profesor("Pedro");
//    plat->notify(profe);

    return 0;
}
