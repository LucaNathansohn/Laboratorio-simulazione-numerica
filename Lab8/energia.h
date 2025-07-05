#ifndef _energia_
#define _energia_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <armadillo>
#include <stdlib.h> //exit
#include "random.h"

using namespace std;
using namespace arma;


class Energia 
{

    private:

    Random _rnd;

    double _mu ;
    double _sigma ;

    double _x ; // posizione iniziale
    double _x_new;
    double _delta;

    double _E_tot;
    double _errore;
    
    double _accettati;
    double _accettanza;

    public:

    Energia(Random &rnd);
    ~Energia();

    void Equilibrio();
    void Move();
    double Hpsi();


    void set_x(double x){_x=x;}
    void set_mu(double mu){_mu=mu;}
    void set_sigma(double sigma){_sigma=sigma;}
    void set_Delta(double delta){_delta=delta;};
    double getMu(){return _mu;};
    double getSigma(){return _sigma;};
    double get_Delta(){return _delta;};
    double getE_tot(){return _E_tot;};
    double getErrore(){return _errore;};
    double getAccettanza(){return _accettanza;};



    double psi(double);
    double E_Kin();
    double E_Pot();

    void calcola_E_tot();  
    void calcola_E_tot(ofstream &,ofstream &);



    double err(double, double, int);


};


#endif // _Energia_
