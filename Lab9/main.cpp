/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include "Individuo.h"
#include "Popolazione.h"

using namespace std;

 
int main (int argc, char *argv[]){

   // Controllo argomento per la disposizione delle città
   if (argv[1]==NULL) {
      cout << "Inserire come argomento '0' per disporre le città su una circonferenza o '1' per disporre le città all'interno di un quadrato" << endl;
      return -1;
   }
   
   // Parametri principali del problema
   int n_citta = 34;
   int n_individui = 1000;
   int n_generazioni = 180;

   ofstream fout;
   ofstream fout_best;
   ofstream fout_ave; // per salvare la media dei fitness della migliore metà di ciascuna generazione

   // Inizializzazione popolazione
   Popolazione pop(n_individui, n_citta);

   // Scelta della disposizione delle città e apertura file di output
   if (atoi(argv[1])==0)
   {
      cout << "Disposizione circonferenza" << endl;
      pop.set_citta_circonferenza();
      fout.open("citta_circonferenza.dat");
      fout_best.open("best_circonferenza.dat");
      fout_ave.open("ave_circonferenza.dat");
   }
   else if (atoi(argv[1])==1)
   {
      cout << "Disposizione quadrato" << endl;
      pop.set_citta_quadrato();
      fout.open("citta_quadrato.dat");
      fout_best.open("best_quadrato.dat");
      fout_ave.open("ave_quadrato.dat");
   }
   else
   {
      cout << "Argomento non valido" << endl;
      return -1;
   }

   pop.calcola_distanze(); // Calcola la matrice delle distanze tra le città
   pop.check();            // Controlla la validità della popolazione
   pop.calcola_fitness();  // Calcola il fitness di ogni individuo
   pop.calcolaRanking();   // Ordina la popolazione per fitness

   // Salva le coordinate delle città su file
   for (int i=0; i<n_citta; i++)
   {
       fout << i+1 << " " << pop.get_citta()(i,0) << " " << pop.get_citta()(i,1) << endl;
   }
   fout.close();

   // Ciclo sulle generazioni evolutive
   for (int i=0; i<n_generazioni; i++)
   {
      pop.calcola_fitness();
      pop.calcolaRanking();
      pop.check();

      // Salva il percorso migliore della generazione
      for (int j = 0; j < n_citta; j++)
      {
         fout_best << pop.get_best().get_tragitto_component(j) << " ";
      }
      fout_best << endl;

      // Salva la media dei migliori fitness e il fitness del migliore
      fout_ave << pop.media_fitness() << setw(15) << pop.get_fitness()(pop.get_ranking()(0)) << endl;

      // Operatori genetici: crossover e mutazioni
      pop.crossover(0.7);
      pop.mutazione_perm(0.1);
      pop.mutazione_perm_blocco(0.1);
      pop.mutazione_trasl_blocco(0.1);
      pop.mutazione_inv_blocco(0.1);
   }

   fout_best.close();
   fout_ave.close();

   return 0;
}

