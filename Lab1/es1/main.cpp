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
   int M=1E5;  // Numero totale di numeri generati
   int N=100;  // Numero di blocchi
   int L= int(M/N);  // Numeri per ogni blocco

   vector<double> av(N,0.0); 
   vector<double> av2(N,0.0); 
   vector<double> sum_prog(N,0.0); 
   vector<double> sum_prog2(N,0.0); 
   vector<double> err_prog(N,0.0); 
   double sum;

   vector<double> avV(N,0.0); 
   vector<double> av2V(N,0.0); 
   vector<double> sum_progV(N,0.0); 
   vector<double> sum_prog2V(N,0.0); 
   vector<double> err_progV(N,0.0); 
   double sumV;

   for(int i=0; i<N; i++){ //ciclo sui blocchi
       sum=0;
       sumV=0;
       for(int j=0; j<L; j++){ //ciclo sugli L elementi di ciascun blocco
           sum+=rnd.Rannyu(); //genera numero casuale tra 0 e 1

           sumV+=pow(rnd.Rannyu()-0.5,2); // Calcola (x-0.5)^2 per la varianza
       }
       av[i]=(sum/L);   //riempimento dei vettori delle medie e delle medie al quadrato di ciascun blocco
       av2[i]=(sum/L)*(sum/L);

       avV[i]=(sumV/L); //analogo a sopra per il calcolo della varianza
       av2V[i]=(sumV/L)*(sumV/L);
   }

   //apertura file di output
   ofstream fileout("medie.dat");
   ofstream fileoutV ("varianza.dat");
   if (!fileout or !fileoutV){
      cerr << "PROBLEM: Unable to open the file" << endl;
   }

   // Calcolo delle medie progressive e degli errori statistici
   for(int i=0; i<N; i++){
      //calcolo somme progressive
      for (int j = 0; j < i+1; j++)
      {
         sum_prog[i]+=av[j];
         sum_prog2[i]+=av2[j];

         sum_progV[i]+=avV[j];
         sum_prog2V[i]+=av2V[j];
      }
      //calcolo medie progressive
      sum_prog[i]/=i+1;
      sum_prog2[i]/=i+1;
      err_prog[i]=error(sum_prog,sum_prog2,i);
      fileout << sum_prog[i] << "      " << err_prog[i] << endl;

      
      sum_progV[i]/=i+1;
      sum_prog2V[i]/=i+1;
      err_progV[i]=error(sum_progV,sum_prog2V,i);
      fileoutV << sum_progV[i] << "      " << err_progV[i] << endl;
   
   }

   fileout.close();
   fileoutV.close();

   ofstream fileoutChi("Chi2.dat");
   if (!fileoutChi){
      cerr << "PROBLEM: Unable to open the file" << endl;
   }

   int m = 100; // Number of intervals
   int n = 10000; // Number of throws
   double expected = double(n) / m; // Expected count in each interval

   for (int i = 0; i < 100; i++) {
      vector<double> ni(m, 0); // Vector for observed frequencies
      double chi2_ = 0;

      for (int j = 0; j < n; j++) {
         double x = rnd.Rannyu(); // Generate a random number
         int interval = floor(x * m); // Determine which interval it falls into
         ni[interval]++; // Increment observed frequency
      }

      // Compute Chi-squared
      for (int j = 0; j < m; j++) {
         chi2_ += pow(ni[j] - expected, 2) / expected;
      }


      fileoutChi << chi2_ << endl;
   }

   fileoutChi.close();

   return 0;
}

