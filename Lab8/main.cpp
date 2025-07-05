/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include "energia.h"

using namespace std;

bool metro_boltzmann(Random& rnd, double E , double E_test , double beta );

int main (int argc, char *argv[]){

   
    // random number generator
    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open()){
       Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();
 
    ifstream input("seed.in");
    string property;
    if (input.is_open()){
       while ( !input.eof() ){
          input >> property;
          if( property == "RANDOMSEED" ){
             input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
             rnd.SetRandom(seed,p1,p2);
          }
       }
       input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;




   if (argv[1]==NULL) {
      cout << "Inserire come argomento '1' per 8.1 o '2' per 8.2" << endl;
      return -1;
   } 


   if (atoi(argv[1])==1) {
   cout << "Esercizio 8.1" << endl;
   
   Energia E(rnd);
   ofstream fout("energy.dat");
   ofstream foutA("acceptance.dat");
   E.set_x(0.0); // posizione iniziale
   E.set_mu(0.5); // valore iniziale di mu
   E.set_sigma(1.0); // valore iniziale di sigma
   E.Equilibrio(); // fase di equilibrio
   E.calcola_E_tot(fout,foutA); // calcolo dell'energia totale e dell'accettanza e stampa su file
   fout.close(); // chiudo il file
}



   if (atoi(argv[1])==2) {
   cout << "Esercizio 8.2" << endl;

   

   ofstream foutP ("parameters.dat");
   ofstream foutE ("energy_SA.dat");
   ofstream fout("final_energy.dat");
   ofstream foutpsi("psi.dat");
   ofstream foutA("acceptance_SA.dat");

   Energia E(rnd);
   E.set_mu(2.); // valore iniziale di mu
   E.set_sigma(2.0); // valore iniziale di sigma
   E.set_x(0.0); // posizione iniziale
   E.Equilibrio(); // fase di equilibrio
   E.calcola_E_tot(); // calcolo dell'energia totale
   int passi=0;
   foutE << "#Step:       Beta:       E_tot:       Err:" << endl;
   foutP << "#Step:       Beta:       mu:       sigma:" << endl;


   Energia E_test(rnd);
   E_test.set_x(0.0); // valore iniziale di mu

   for (double T=2; T>=0.01; T*=0.99){
      double beta=1/T;
      double delta_new=T; // passo di Metropolis diventa sempre più piccolo al decrescere della temperatura
      //cout << "T: " << T << endl;
      passi++;
      foutE << passi << setw(20) << beta << setw(20) << E.getE_tot() << setw(20) << E.getErrore() << endl; // scrivo su file i parametri e l'energia totale
      foutP << passi << setw(20) << beta << setw(20) << E.getMu() << setw(20) << E.getSigma() << endl; // scrivo su file i parametri



      for(int i=0; i<100; i++){

         //cout << "Step: " << i << endl;
         E.calcola_E_tot(); // calcolo dell'energia totale iniziale
         
         double mu_new=E.getMu() + rnd.Rannyu(-delta_new, delta_new);
         double sigma_new=E.getSigma() + rnd.Rannyu(-delta_new, delta_new);

         E_test.set_mu(mu_new);
         E_test.set_sigma(sigma_new);
         E_test.set_x(0); // mantengo la stessa posizione iniziale
         E_test.Equilibrio();
         E_test.calcola_E_tot(); // calcolo dell'energia totale della nuova configurazione

         if(metro_boltzmann(rnd, E.getE_tot(), E_test.getE_tot(), beta)){ // accetto o rifiuto la nuova configurazione
            E.set_mu(mu_new);
            E.set_sigma(sigma_new);
            E.set_Delta(E_test.get_Delta());
         }
      }


   }

   E.calcola_E_tot(fout, foutA); // calcolo dell'energia totale e dell'accettanza e stampa su file
   
   for (double x = -3; x <= 3; x += 0.1) {
      foutpsi << x << " " << E.psi(x)*E.psi(x) << endl; // stampa la funzione d'onda
   }
   

   fout.close();
   foutE.close();
   foutP.close();
   foutpsi.close();

   }
   return 0;
}


bool metro_boltzmann(Random& rnd, double E , double E_test , double beta ){
    double acceptance = exp(-beta * ( E_test - E ) );
    if(rnd.Rannyu() < acceptance ) return true; //Metropolis acceptance step
    return false; 
}