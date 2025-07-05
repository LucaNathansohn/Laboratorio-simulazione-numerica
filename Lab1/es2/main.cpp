#include <fstream>
#include <string>
#include "../random.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

int main (int argc, char *argv[]){

   if (argv[1]==NULL) {
   cout << "Inserire come argomento N" << endl;
   return -1;
   }

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
   

   int N=atoi(argv[1]);
   int n=1e4;
   double sum;
   double sum_exp;
   double sum_lorentz;
   
   ostringstream filename;
   filename << "histo_N" << N << ".dat";
   ofstream fileout(filename.str());

   if (!fileout){
      cerr << "PROBLEM: Unable to open the file" << endl;
   }
   for (int i = 0; i < n; i++)
   {
      sum=0;
      sum_exp=0;
      sum_lorentz=0;

      for (int j = 0; j < N; j++)
      {
         sum+=rnd.Rannyu();
         sum_exp+=rnd.Exp(1);
         sum_lorentz+=rnd.Lorentz(1,0);
      }
         
      fileout << sum/N << "      " << sum_exp/N << "      " << sum_lorentz/N << endl;
   }
   fileout.close();

   return 0;
}

