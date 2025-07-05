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
#include "mpi.h"

using namespace std;

int main (int argc, char *argv[]) {

    int size, rank;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n_citta = 110;
    int n_individui;
    if (size==1) n_individui = 11000;
    else n_individui = 1000;
    int n_generazioni = 4000;
    Random rnd;
 int p1, p2; // Read from Primes a pair of numbers to be used to initialize the RNG
    ifstream Primes("primes64001.in");
        Primes >> p1 >> p2;
    Primes.close();
    int seed[4]; // Read the seed of the RNG
    ifstream Seed("seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    rnd.SetRandom(seed,p1,p2);

    Popolazione pop(n_individui, n_citta, rank);

    ofstream fout_best;
    ofstream fout_ave;
    if (size == 1){
        fout_ave.open("OUTPUT_1_CORE/ave.dat");
        fout_best.open("OUTPUT_1_CORE/best.dat");
    }
    else {
    string bestdat= "best_" + to_string(rank) + ".dat";
    string avedat = "ave_" + to_string(rank) + ".dat";
    fout_ave.open(avedat);
    fout_best.open(bestdat);
    }
   // pop.set_citta_quadrato();

    pop.set_citta("cap_prov_ita.dat");

    pop.calcola_distanze();

    pop.check();

    //fout.open("citta.dat");

    pop.calcola_fitness();

    pop.calcolaRanking();

    for (int i = 0; i < n_generazioni; i++) {
        pop.calcola_fitness();
        pop.calcolaRanking();
        pop.check();
       if (i % 70 == 0 and size>1) {
            int tag1=1;
            int tag2=2;
            MPI_Request req;
            MPI_Status stat1, stat2;
            //fai una migrazione
		int a,b;
            do {
                a = rnd.Rannyu(0, size);
                b = rnd.Rannyu(0, size);
            } while (a == b);

            if(rank ==a or rank == b) {
                
            int send_vec[n_citta];

            for (int j = 0; j < n_citta; j++) {
                send_vec[j] = pop.get_best().get_tragitto_component(j);
            }
            if (rank == a) {
                MPI_Isend(&send_vec,n_citta, MPI_INTEGER,b,tag1, MPI_COMM_WORLD,&req);
                MPI_Recv(&send_vec,n_citta,MPI_INTEGER,b,tag2,MPI_COMM_WORLD,&stat2);
                MPI_Wait(&req, &stat1);
            }
            else if(rank==b){
                MPI_Send(&send_vec,n_citta,MPI_INTEGER,a,tag2,MPI_COMM_WORLD);
                MPI_Recv(&send_vec,n_citta,MPI_INTEGER,a,tag1,MPI_COMM_WORLD, &stat1);
            }
            pop.set_best(send_vec);
            }
        }

        for (int j = 0; j < n_citta; j++)
       {
            fout_best << pop.get_best().get_tragitto_component(j) << " ";
       }
       fout_best << endl;

       fout_ave << pop.media_fitness() << setw(15) << pop.get_fitness()(pop.get_ranking()(0)) << endl;
 
      pop.crossover(0.7);
      pop.mutazione_perm(0.1);
      pop.mutazione_trasl_blocco(0.1);
      pop.mutazione_perm_blocco(0.1);
      pop.mutazione_inv_blocco(0.1);
    }

    fout_best.close();
    fout_ave.close();
    MPI_Finalize();

    return 0;
}
