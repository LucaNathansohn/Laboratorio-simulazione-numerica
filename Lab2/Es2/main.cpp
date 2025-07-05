#include <fstream>
#include <string>
#include "../random.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

double Modulo2(vector<int> pos) {
    return pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];
}
double Modulo2(vector<double> pos) {
    return pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];
}


void step_discreto(Random& rnd, vector<int>& pos) {
    // implemento ricerca sequenziale e non binary perchè l'efficienza in questo caso è confrontabile
    double r = rnd.Rannyu();

    if (r < 1.0 / 6) {
        pos[0]++;
    }
    else if (r < 2.0 / 6) {
        pos[0]--;
    }
    else if (r < 3.0 / 6) {
        pos[1]++;
    }
    else if (r < 4.0 / 6) {
        pos[1]--;
    }
    else if (r < 5.0 / 6) {
        pos[2]++;
    }
    else {
        pos[2]--;
    }
    return;
}

void step_continuo(Random& rnd, vector<double>& pos) {

   double x=rnd.Rannyu(); // numero casuale tra 0 e 1
   double phi= rnd.Rannyu(0, 2 * M_PI); // angolo azimutale
   double theta= acos(1-2*x); //distribuzione uniforme sulla sfera

   //passo di lunghezza a in direzione theta e phi
   pos[0] += sin(theta) * cos(phi); // x
   pos[1] += sin(theta) * sin(phi); // y
   pos[2] += cos(theta); // z
   return;
}

int main(int argc, char* argv[]) {

    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("../Primes");

    if (Primes.is_open()) {
        for (size_t i = 0; i < 1; i++) {
            Primes >> p1 >> p2;
        }
    }
    else
        cerr << "PROBLEM: Unable to open Primes" << endl;

    Primes.close();

    ifstream input("../seed.in");
    string property;
    if (input.is_open()) {
        while (!input.eof()) {
            input >> property;
            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
        input.close();
    }
    else
        cerr << "PROBLEM: Unable to open seed.in" << endl;

    rnd.SaveSeed();

    // random walk discreto
    // in ogni blocco devo fare L passeggiate di n passi
    // Inizializzazione dei parametri
    int M = 1E4;  // Numero totale di random walk
    int N = 100;  // Numero di blocchi
    int L = int(M / N);  // Random walk per ogni blocco
    int n = 100; // passi di ogni random walk


    vector<double> sum(N, 0.0);
    vector<double> sum2(N, 0.0);
    vector<double> err(N, 0.0);

    if (argv[1] == NULL) {
        cout << "Inserire come argomento '0' per discreto o '1' per continuo" << endl;
        return -1;
    }

    if (atoi(argv[1]) == 0) { // discreto
        cout << "Random walk discreto" << endl;
        vector<int> pos(3, 0); // posizione del punto (x,y,z)

        for (int i = 0; i < N; i++) {
            vector<double> v(n, 0.0); // vettore che memorizza le distanze quadratiche per ogni passo
            // lo azzero ad ogni random walk
            for (int j = 0; j < L; j++) {
                fill(pos.begin(), pos.end(), 0); // resetto la posizione
                for (int k = 0; k < n; k++) {
                    step_discreto(rnd, pos); // passo del random walk
                    v[k] += Modulo2(pos); // calcolo la distanza quadratica dal punto di partenza
                }
            }
            for (int k = 0; k < n; k++) {
                v[k] /= L; // media della distanza quadratica per ogni passo
                v[k] = sqrt(v[k]); // radice quadrata della distanza quadratica (la faccio ora per minimizzare l'errore(prova a vedere se è vero))
                sum[k] += v[k]; // somma progressiva delle medie delle distanze associate ad ogni passo
                sum2[k] += v[k] * v[k]; // somma progressiva al quadrato delle medie delle distanze associate ad ogni passo
            }
        }

        // apertura file di output
        ofstream fileout("RW_discreto.dat");
        if (!fileout) {
            cerr << "PROBLEM: Unable to open the file" << endl;
        }
        for (int i = 0; i < N; i++) {
            sum[i] /= (L); // media delle distanze per ogni passo
            sum2[i] /= (L); // media al quadrato delle distanze per ogni passo
            err[i] = sqrt((sum2[i] - pow(sum[i], 2)) / N);
            fileout << sum[i] << "       " << err[i] << endl; // scrivo su file
        }
    }
    else if (atoi(argv[1]) == 1) { // continuo
        cout << "Random walk continuo" << endl;
        vector<double> pos(3, 0); // posizione del punto (x,y,z)
        for (int i = 0; i < N; i++) {
            vector<double> v(n, 0.0); // vettore che memorizza le distanze quadratiche per ogni passo
            // lo azzero ad ogni random walk
            for (int j = 0; j < L; j++) {
                fill(pos.begin(), pos.end(), 0); // resetto la posizione
                for (int k = 0; k < n; k++) {
                    step_continuo(rnd, pos); // passo del random walk
                    v[k] += Modulo2(pos); // calcolo la distanza quadratica dal punto di partenza
                }
            }
            for (int k = 0; k < n; k++) {
                v[k] /= L; // media della distanza quadratica per ogni passo
                v[k] = sqrt(v[k]); // radice quadrata della distanza quadratica (la faccio ora per minimizzare l'errore(prova a vedere se è vero))
                sum[k] += v[k]; // somma progressiva delle medie delle distanze associate ad ogni passo
                sum2[k] += v[k] * v[k]; // somma progressiva al quadrato delle medie delle distanze associate ad ogni passo
            }
        }

        // apertura file di output
        ofstream fileout("RW_continuo.dat");
        if (!fileout) {
            cerr << "PROBLEM: Unable to open the file" << endl;
        }
        for (int i = 0; i < N; i++) {
            sum[i] /= (L); // media delle distanze per ogni passo
            sum2[i] /= (L); // media al quadrato delle distanze per ogni passo
            err[i] = sqrt((sum2[i] - pow(sum[i], 2)) / N);
            fileout << sum[i] << "       " << err[i] << endl; // scrivo su file
        }
    }
    else {
        cerr << "Argomento non valido!" << endl;
        return -1;
    }

    return 0;
}