#ifndef __Popolazione__
#define __Popolazione__


#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <armadillo>
#include <stdlib.h>
#include "random.h"
#include "Individuo.h"


using namespace std;
using namespace arma;



class Popolazione {

    private:

    field<Individuo> _generazione; // matrice di individui
    int _n_citta;           // Number of cities
    mat _citta; // coordinate delle citta (n_citta x 2)
    mat _distanze; //matrice delle distanze tra le citta (n_citta x n_citta)
    Random _rnd;
    vec _fitness;
    int _n_ind; // Number of individuals
    uvec _ranking; 


    public:

    Popolazione(){} // Default constructor

    Popolazione(int,int); // Constructor

    ~Popolazione(); // Destructor

    void set_citta_circonferenza();

    void set_citta_quadrato();

    void calcola_distanze();

    void calcola_fitness();

    void calcolaRanking();

    void crossover(double);

    void mutazione_perm(double);

    void mutazione_perm_blocco(double);

    void mutazione_inv_blocco(double);

    void mutazione_trasl_blocco(double);

    uvec get_ranking();

    Individuo get_best();

    vec get_fitness() { return _fitness; };

    mat get_citta();

    Random get_rnd() { return _rnd; };

    void check();

    double media_fitness();

};

#endif // _Popolazione_