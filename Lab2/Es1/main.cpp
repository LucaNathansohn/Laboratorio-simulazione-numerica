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
 
int main (int argc, char *argv[]){

   Random rnd;
   int seed[4];
   int p1, p2;
   ifstream Primes("../Primes");

   if (Primes.is_open()){
   for (size_t i = 0; i < 4; i++)
   {
      
         Primes >> p1 >> p2 ;
      }
   }

   else cerr << "PROBLEM: Unable to open Primes" << endl;
   
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
   int M=1E5;  // Numero totale di numeri generati
   int N=100;  // Numero di blocchi
   int L= int(M/N);  // Numeri per ogni blocco

   vector<double> av(N,0.0); 
   vector<double> av2(N,0.0); 
   vector<double> sum_prog(N,0.0); 
   vector<double> sum_prog2(N,0.0);
   vector<double> err_prog(N,0.0); 
   double sum;

   vector<double> avI(N,0.0); 
   vector<double> av2I(N,0.0); 
   vector<double> sum_progI(N,0.0); 
   vector<double> sum_prog2I(N,0.0);
   vector<double> err_progI(N,0.0); 
   double sumI;

   for(int i=0; i<N; i++){ //ciclo sui blocchi
       sum=0;
       sumI=0;
       for(int j=0; j<L; j++){ //ciclo sugli L elementi di ciascun blocco
            
         sum+=M_PI/2*cos(rnd.Rannyu()*M_PI/2); // Calcola integranda con metodo della media

         //importance sampling
         double x=1-sqrt(1-rnd.Rannyu()); //genero una x distribuita secondo p(x)=-2x+2
         sumI+=((M_PI/2)*cos(x*M_PI/2))/(2*(1-x)); // Calcola integranda con metodo dell'inversione della funzione cumulativa

       }

       av[i]=(sum/L); //riempimento dei vettori delle medie e delle medie al quadrato di ciascun blocco
       av2[i]=(sum/L)*(sum/L);

       avI[i]=(sumI/L); //riempimento dei vettori delle medie e delle medie al quadrato di ciascun blocco
       av2I[i]=(sumI/L)*(sumI/L);
   }

   //apertura file di output
   ofstream fileoutV ("integrale.dat");
   ofstream fileoutI ("integraleIS.dat");
   if (!fileoutV or !fileoutI){
      cerr << "PROBLEM: Unable to open the file" << endl;
   }

   // Calcolo delle medie progressive e degli errori statistici
   for(int i=0; i<N; i++){
      //calcolo somme progressive
      for (int j = 0; j < i+1; j++)
      {
         sum_prog[i]+=av[j];
         sum_prog2[i]+=av2[j];

         sum_progI[i]+=avI[j];
         sum_prog2I[i]+=av2I[j];
      }
      //calcolo medie progressive
      sum_prog[i]/=i+1;
      sum_prog2[i]/=i+1;
      err_prog[i]=error(sum_prog,sum_prog2,i);
      fileoutV << sum_prog[i] << "      " << err_prog[i] << endl;

      sum_progI[i]/=i+1;
      sum_prog2I[i]/=i+1;
      err_progI[i]=error(sum_progI,sum_prog2I,i);
      fileoutI << sum_progI[i] << "      " << err_progI[i] << endl;
   
   }

   fileoutV.close();

   

   return 0;
}

//puoi non fare secondo for a casa rendilo più efficiente