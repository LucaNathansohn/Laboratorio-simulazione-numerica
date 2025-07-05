#include <cmath>
#include <cstdlib>
#include <string>
#include "energia.h"

using namespace std;



Energia::Energia(Random &rnd) {
    _rnd = rnd;
}

Energia::~Energia(){}


double Energia::psi(double x1){
    return exp(-(x1-_mu)*(x1-_mu)/(2*_sigma*_sigma)) + exp(-(x1+_mu)*(x1+_mu)/(2.*_sigma*_sigma));
}
  
double Energia::E_Kin(){

    return -0.5 *(  -1/(_sigma*_sigma) *  exp(-(_x-_mu)*(_x-_mu)/(2*_sigma*_sigma)) -1/(_sigma*_sigma) *  exp(-(_x+_mu)*(_x+_mu)/(2*_sigma*_sigma)) + 1/(pow(_sigma,4)) * (_x-_mu)*(_x-_mu) * exp(-(_x-_mu)*(_x-_mu)/(2*_sigma*_sigma)) + 1/(pow(_sigma,4)) * (_x+_mu)*(_x+_mu) * exp(-(_x+_mu)*(_x+_mu)/(2*_sigma*_sigma)) )/psi(_x);
}

double Energia::E_Pot(){
    return ((pow(_x,4)-2.5*pow(_x,2))*psi(_x))/psi(_x);
}

double Energia:: Hpsi(){
    return E_Kin()+E_Pot();
}


void Energia::Equilibrio(){

    int iter=0;
    int iter_max=1000;
    int passi=1000;
    _delta = 1; // passo iniziale di Metropolis
    //Sto cercando il passo per avere un'accettanza del 50%
    while (iter<iter_max)
    {
        _accettati = 0; // resetto il contatore degli accettati
        for (int i = 0; i < passi; i++)
        {
            Move(); // eseguo un passo di Metropolis
        }
        
        _accettanza = (double)_accettati / (double)passi; // Calcolo l'accettanza
        if (_accettanza > 0.53) { // Se l'accettanza è troppo alta, aumento la delta
            _delta *= 1.1;
        }
        else if (_accettanza < 0.47) { // Se l'accettanza è troppo bassa, diminuisco la delta
            _delta *= 0.9;
        } 
        else break; // Se l'accettanza è intorno al 50%, esco dal ciclo
     iter++;  
    }

    }



void Energia::Move(){ // Metropolis algorithm
    
    _x_new = _x + _rnd.Rannyu(-_delta, _delta);
    double accept = min( 1. , psi(_x_new)*psi(_x_new) / (psi(_x)*psi(_x)) );
    if(_rnd.Rannyu() < accept ){// Accept the move
        _x = _x_new; 
        _accettati++;
    } 
    
}


void Energia::calcola_E_tot(){ // Calcolo dell'energia totale

   int n_steps=1000;
   int n_blocks=100;
   double E;

   double ave2_block=0.0; // media dei quadrati delle medie dei primi (i+1) blocchi
   double partial_sum=0.0; // accumula la somma delle medie dei primi (i+1) blocchi
   double partial_sum2=0.0; // accumula la somma delle medie dei quadrati 


   for (int i=0; i<n_blocks ; i++){ //loop over blocks
      E=0.0;
      for (int j=0; j<n_steps ; j++){ //loop over steps
        Move(); // eseguo il passo di Metropolis
        E += Hpsi(); // calcolo l'energia
      }
      partial_sum += E/n_steps; // somma delle medie
      partial_sum2 += (E/n_steps)*(E/n_steps); // somma dei quadrati delle medie
       // media dei quadrati delle medie

   }
    _E_tot = partial_sum/n_blocks; // media delle medie
    ave2_block = partial_sum2/n_blocks;
    _E_tot = _E_tot; // energia totale
   _errore = err(_E_tot, ave2_block, n_blocks);
}

void Energia::calcola_E_tot(ofstream & fout, ofstream & foutA){ // Calcolo dell'energia totale e stampa su file
    

   int n_steps=1000;
   int n_blocks=100;
   double E;

   // creo file per stampare la proprietà che mi interessa
   
   fout << "#     BLOCK:  ACTUAL_E:     E_AVE:      ERROR:" << endl;
   foutA << "#     BLOCK:  ACCEPTANCE:" << endl;


   double ave2_block=0.0; // media dei quadrati delle medie dei primi (i+1) blocchi
   double partial_sum=0.0; // accumula la somma delle medie dei primi (i+1) blocchi
   double partial_sum2=0.0; // accumula la somma delle medie dei quadrati 


   for (int i=0; i<n_blocks ; i++){ //loop over blocks
      E=0.0;
      _accettati = 0; // resetto il contatore degli accettati per poter calcolare l'accettanza ad ogni blocco
      for (int j=0; j<n_steps ; j++){ //loop over steps
        Move(); // eseguo il passo di Metropolis
        E += Hpsi(); // calcolo l'energia
      }
      partial_sum += E/n_steps; // somma delle medie
      partial_sum2 += (E/n_steps)*(E/n_steps); // somma dei quadrati delle medie
      _E_tot = partial_sum/(i+1); // media delle medie
      ave2_block = partial_sum2/(i+1); // media dei quadrati delle medie
    
      _errore = err(_E_tot, ave2_block, (i+1));

      fout << i+1 << setw(20) << E/n_steps << setw(20) << _E_tot << setw(20) << _errore << endl; // scrivo su file l'energia
      foutA << i+1 << setw(20) << (double)_accettati / (double)n_steps << endl; // scrivo su file l'accettanza

   }

    _E_tot = _E_tot; // energia totale
}

double Energia::err( double AV, double AV2 , int n ){
      if(n == 0){
          return 0;
      }
      return sqrt( (AV2 - AV*AV)/n );
  }