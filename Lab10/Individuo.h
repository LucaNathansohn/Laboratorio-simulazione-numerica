#ifndef __Individuo__
#define __Individuo__


#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <armadillo>
#include <algorithm>
#include "random.h"

using namespace std;
using namespace arma;


class Individuo
{
private:

    int _n_citta;           // Number of cities
    vec _tragitto;
    double _fitness; //lunghezza del percorso totale
public:

    
    Individuo() {};
    Individuo(int n_citta, Random&); // Constructor
    ~Individuo();
    int pbc (int i); // periodic boundary condition
    void mutazione_permutazione(double prob, Random &);
    void mutazione_permutazione_blocco(double prob, Random &);
    void mutazione_inversione_blocco(double prob, Random &);
    void mutazione_traslazione_blocco(double prob, Random &);
    bool check_tragitto();
    int get_tragitto_component(int i);
    int set_tragitto_component(int i, int j);
};



#endif // __Individuo__
