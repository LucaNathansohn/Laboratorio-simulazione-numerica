#include <fstream>
#include <string>
#include "../random.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// Funzione per calcolare l'errore statistico sulle medie progressive
double error (vector <double> AV, vector <double> AV2, int n){
   if (n==0)
   return 0;
   else
   return sqrt((AV2[n]-AV[n]*AV[n])/n);
}

// Funzione che restituisce il valore dell'asset a tempo T secondo il modello di moto browniano geometrico (GBM)
double geoBM(double S0, double T, double r, double sigma, double Z){
   return S0*exp((r-sigma*sigma/2)*T+sigma*sqrt(T)*Z);
}

// Funzione max tra due double (per payoff call/put)
double max(double a, double b){
   if (a>b)
   return a;
   else 
   return b;
}

int main (int argc, char *argv[]){

   // Controllo argomento: 0 per continuo, 1 per discreto
   if (argv[1]==NULL) {
      cout << "Inserire come argomento '0' per continuo o '1' per discreto" << endl;
      return -1;
   }
   
   // Inizializzazione del generatore di numeri casuali
   Random rnd;
   int seed[4];
   int p1, p2;
   ifstream Primes("../Primes");
   if (Primes.is_open()){
      Primes >> p1 >> p2 ;
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

   // Parametri dell'opzione e del modello Black-Scholes
   double K=100; // Strike price
   double S0=100; // Asset price at t=0
   double T=1; // Delivery time
   double r=0.1; // Risk-free interest rate
   double sigma=0.25; // Volatility

   int M=1E5;  // Numero totale di simulazioni Monte Carlo
   int N=100;  // Numero di blocchi per il data blocking
   int L= int(M/N);  // Numero di simulazioni per blocco

   // Vettori per memorizzare i risultati dei blocchi (call e put)
   vector<double> C(N,0.0); 
   vector<double> C2(N,0.0); 
   vector<double> sum_prog(N,0.0); 
   vector<double> sum_prog2(N,0.0); 
   vector<double> err_prog(N,0.0); 

   vector<double> P(N,0.0); 
   vector<double> P2(N,0.0); 
   vector<double> sum_progP(N,0.0); 
   vector<double> sum_prog2P(N,0.0); 
   vector<double> err_progP(N,0.0);

   double Z=0;
   double Sum=0;
   double SumP=0;
   double S=0;

   // --- Metodo 1: Sampling diretto del prezzo finale S(T) (GBM continuo) ---
   if (atoi(argv[1])==0)
   {
      // Ciclo sui blocchi
      for (int i = 0; i < N; i++)
      {
         Sum=0;
         SumP=0;
         // Ciclo sulle simulazioni all'interno del blocco
         for (int j = 0; j < L; j++)
         {
            Z=rnd.Gauss(0,1); // Estrazione di una variabile gaussiana standard
            S=geoBM(S0,T,r,sigma,Z); // Calcolo S(T) secondo GBM
            // Calcolo payoff scontato per call e put
            Sum+=max(0,S-K)*exp(-r*T); // Call
            SumP+=max(0,K-S)*exp(-r*T); // Put
         }
         // Media del blocco
         C[i]=Sum/L;
         C2[i]=C[i]*C[i];
         P[i]=SumP/L;
         P2[i]=P[i]*P[i];
      }

      //apertura file di output
      ofstream fileout("Finanza.dat");
      if (!fileout){
         cerr << "PROBLEM: Unable to open the file" << endl;
      }

      // Calcolo delle medie progressive e degli errori statistici (data blocking)
      for(int i=0; i<N; i++){
         //calcolo somme progressive
         for (int j = 0; j < i+1; j++)
         {
            sum_prog[i]+=C[j];
            sum_prog2[i]+=C2[j];
            
            sum_progP[i]+=P[j];
            sum_prog2P[i]+=P2[j];
         }
         //calcolo medie progressive
         sum_prog[i]/=i+1;
         sum_prog2[i]/=i+1;
         err_prog[i]=error(sum_prog,sum_prog2,i);

         sum_progP[i]/=i+1;
         sum_prog2P[i]/=i+1;
         err_progP[i]=error(sum_progP,sum_prog2P,i);

         fileout << sum_prog[i] << "      " << err_prog[i] << "        " << sum_progP[i] << "      " << err_progP[i] << endl;
      
      }

      fileout.close();

      cout << "Metodo continuo eseguito, i dati di output sono in Finanza.dat" << endl;

   }
   // --- Metodo 2: Sampling della traiettoria discreta di GBM ---
   else if (atoi(argv[1])==1)
   {
      // Ciclo sui blocchi
      for (int i = 0; i < N; i++)
      {
         Sum=0;
         SumP=0;
         // Ciclo sulle simulazioni all'interno del blocco
         for (int j = 0; j < L; j++)
         {
            S=S0; //inizializzo S a S0
            // Evolvo S in 100 passi (discretizzazione di [0,T])
            for (int k = 0; k < 100; k++)
            {
               Z=rnd.Gauss(0,1);// Z va aggiornato a ogni passo
               S=geoBM(S,T/100,r,sigma,Z);// Evolvo di T/100 a partire dal valore S del passo precedente
            }
            // Calcolo payoff scontato per call e put
            Sum+=max(0,S-K)*exp(-r*T);
            SumP+=max(0,K-S)*exp(-r*T);
         }
         // Media del blocco
         C[i]=Sum/L;
         C2[i]=C[i]*C[i];
         P[i]=SumP/L;
         P2[i]=P[i]*P[i];
      }
   
      //apertura file di output
      ofstream fileout_dis("Finanza_discreta.dat");
      if (!fileout_dis){
         cerr << "PROBLEM: Unable to open the file" << endl;
      }
   
      // Calcolo delle medie progressive e degli errori statistici (data blocking)
      for(int i=0; i<N; i++){
         //calcolo somme progressive
         for (int j = 0; j < i+1; j++)
         {
            sum_prog[i]+=C[j];
            sum_prog2[i]+=C2[j];
            
            sum_progP[i]+=P[j];
            sum_prog2P[i]+=P2[j];
         }
         //calcolo medie progressive
         sum_prog[i]/=i+1;
         sum_prog2[i]/=i+1;
         err_prog[i]=error(sum_prog,sum_prog2,i);
   
         sum_progP[i]/=i+1;
         sum_prog2P[i]/=i+1;
         err_progP[i]=error(sum_progP,sum_prog2P,i);
   
         fileout_dis << sum_prog[i] << "      " << err_prog[i] << "        " << sum_progP[i] << "      " << err_progP[i] << endl;
      
      }
   
      fileout_dis.close();

      cout << "Metodo discreto eseguito, i dati di output sono in Finanza_discreta.dat" << endl;

   }
   return 0;
}