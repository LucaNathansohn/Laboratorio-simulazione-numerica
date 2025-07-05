#include <fstream>
#include <string>
#include "../random.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

double error (vector <double> AV, vector <double> AV2, int n){
   if (n==0)
   return 0;
   else
   return sqrt((AV2[n]-AV[n]*AV[n])/n);
}

double compute_Chi2 (double expected, vector <int> observed){
   double chi2=0;
   for (unsigned int i = 0; i < observed.size(); i++)
   {
      chi2+=(observed[i]-expected)*(observed[i]-expected)/expected;
   }
   return chi2;
}
 
int main (int argc, char *argv[]){

   Random rnd;
   int seed[4];
   int p1, p2;
   ifstream Primes("../Primes");
   if (Primes.is_open()){
      for (int i = 0; i < 5; i++)
      {
         Primes >> p1 >> p2 ;
      }
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   ifstream input("../seed.in");
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
   
   rnd.SaveSeed();

   // Inizializzazione dei parametri
   int n_aghi=1E5;  // Numero totale di aghi lanciati
   int N=100;  // Numero di blocchi
   int n_thr= int(n_aghi/N);  // numero di aghi lanciati per ogni blocco

   double d=1; // Distanza tra le righe
   double L=0.7; // Lunghezza dell'ago
   double z; //ordinata del centro dell'ago
   double theta; //angolo di inclinazione dell'ago

   vector<double> av(N,0.0); 
   vector<double> av2(N,0.0); 
   vector<double> sum_prog(N,0.0); 
   vector<double> sum_prog2(N,0.0); 
   vector<double> err_prog(N,0.0); 
   double n_hit;
   double pi;


   for(int i=0; i<N; i++){ //ciclo sui blocchi
       n_hit=0;
       for(int j=0; j<n_thr; j++){ //ciclo sugli n_thr elementi di ciascun blocco
           z= rnd.Rannyu(0.,d/2); //generazione ordinata del centro dell'ago
           double x=0,y=0; //coordinate per campionare l'angolo
           do
           {
               x=rnd.Rannyu(-1.,1.); //generazione di un numero casuale tra -1 e 1
               y=rnd.Rannyu(0.,1.); //generazione di un numero casuale tra 0 e 1
           } while (x*x+y*y>1);

           theta=acos(x/sqrt(x*x+y*y)); //calcolo dell'angolo di inclinazione dell'ago
           if(z<=L/2*sin(theta)) n_hit++; //controllo se l'ago interseca una riga
       }

       pi=(2*L*n_thr)/(d*n_hit); //calcolo di pi
       av[i]=pi;   //riempimento dei vettori delle medie e delle medie al quadrato di ciascun blocco
       av2[i]=(pi)*(pi);
   }

   //apertura file di output
   ofstream fileout("buffon.dat");
   if (!fileout){
      cerr << "PROBLEM: Unable to open the file" << endl;
   }

   // Calcolo delle medie progressive e degli errori statistici
   for(int i=0; i<N; i++){
      //calcolo somme progressive
      for (int j = 0; j < i+1; j++)
      {
         sum_prog[i]+=av[j];
         sum_prog2[i]+=av2[j];
      }
      //calcolo medie progressive
      sum_prog[i]/=i+1;
      sum_prog2[i]/=i+1;
      err_prog[i]=error(sum_prog,sum_prog2,i);
      fileout << i << "      " << sum_prog[i] << "      " << err_prog[i] << endl;
   }

   fileout.close();

   return 0;
}

