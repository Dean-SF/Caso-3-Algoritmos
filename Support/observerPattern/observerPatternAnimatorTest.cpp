#include "Animator.h"
#include "Selector.h"
#include "Router.h"
#include "Generator.h"

#include <list>
#include <iostream>

using namespace std;


int main() {
    cout << "Selector started" << endl;
    Selector* selection = new Selector();
    Router* routing = new Router();
    Generator* generation = new Generator();
    Animator* animator = new Animator();

    selection->attach(animator); // animator observa a selection

    routing->attach(animator);   // animator observa a routing

    animator->attach(routing);       // Routing observa a animator
    animator->attach(generation);    // generation observa a animator

    int test = 10;
    selection->notify(&test);
    return 0;
}
