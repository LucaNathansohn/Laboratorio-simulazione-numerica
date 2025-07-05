#include <cmath>
#include <cstdlib>
#include <string>
#include "individuo.h"
#include "popolazione.h"

using namespace std;
using namespace arma;


Popolazione::Popolazione(int nind, int ncit) {
    n_ind = nind;
    n_cit = ncit;

    int p1, p2;
    ifstream Primes("../INPUT/Primes");
    for (int i = 0; i < n_ind + 1; i++) {
        Primes >> p1 >> p2;
    }
    Primes.close();

    int seed[4];
    ifstream Seed("../INPUT/seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    _rnd1.SetRandom(seed, p1, p2);

    _generazione.set_size(n_ind);
    for (int i = 0; i < n_ind; i++) {
        _generazione(i) = Individuo(n_cit, i);
    }
    _citta.set_size(n_cit, 2);
    _distanze.set_size(n_cit, n_cit);
    _fitness.set_size(n_ind);
    _ranking.set_size(n_ind, 2);
}

Popolazione::~Popolazione()
{
}

void Popolazione :: setCittaCirconferenza() {
    
    for (int i=0; i<n_cit; i++) {
      double theta = _rnd1.Rannyu(0,2.0*M_PI);
      _citta(i,0)=cos(theta); // x coordinate
      _citta(i,1)=sin(theta); // y coordinate
      //cout << _citta(i,0) << " " << _citta(i,1) << endl;
    }

}

void Popolazione :: setCittaQuadrato() {
    
    for (int i=0; i<n_cit; i++) {
      _citta(i,0)=_rnd1.Rannyu(); // x coordinate
      _citta(i,1)=_rnd1.Rannyu(); // y coordinate
    }

}

void Popolazione::calcolaDistanza() {
    if (_citta.n_rows != n_cit || _citta.n_cols != 2)
        throw std::runtime_error("Errore: _citta non inizializzata correttamente!");
    _distanze.set_size(n_cit, n_cit);
    for (int i = 0; i < n_cit; i++) {
        for (int j = 0; j < n_cit; j++) {
            _distanze(i, j) = sqrt(pow(_citta(i, 0) - _citta(j, 0), 2) + pow(_citta(i, 1) - _citta(j, 1), 2));
        }
    }
}

void Popolazione :: calcolaFitness() {
    // Calculate the fitness of the individual
    for (int j=0; j<n_ind-1; j++) {
        for (int i=0; i<n_cit-1; i++) {

            _fitness(j)+=pow(_distanze(_generazione(j).GetTragitto(i)-1,_generazione(j).GetTragitto(i+1)-1),2);
        }
        _fitness(j)+=pow(_distanze(_generazione(j).GetTragitto(n_cit-1)-1,_generazione(j).GetTragitto(0)-1),2); //chiudo il giro
    }
    
}//////attenzione non so se si usa cosi field



void Popolazione::calcolaRanking() {
    // Riempie la matrice _ranking con fitness e indici
    for (int i = 0; i < n_ind; i++) {
        _ranking(i, 0) = _fitness(i); // Fitness dell'individuo
        _ranking(i, 1) = i;          // Indice dell'individuo
    }

    // Converte _ranking in un vettore di coppie per ordinamento
    std::vector<std::pair<double, int>> ranking_vec;
    for (int i = 0; i < n_ind; i++) {
        ranking_vec.emplace_back(_ranking(i, 0), _ranking(i, 1));
    }

    // Ordina il vettore in base alla fitness (prima colonna)
    std::sort(ranking_vec.begin(), ranking_vec.end(), [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
        return a.first < b.first; // Ordine crescente in base alla fitness
    });

    // Copia i dati ordinati di nuovo in _ranking
    for (int i = 0; i < n_ind; i++) {
        _ranking(i, 0) = ranking_vec[i].first;  // Fitness ordinata
        _ranking(i, 1) = ranking_vec[i].second; // Indice corrispondente
    }
}



void Popolazione :: crossover(int p){

   //crossover per tutta la popolazione (finche non creo n_ind figli)
    
    int i1,i2;
    int j=0;
    mat figli;
    figli.set_size(n_ind,n_cit);
    while (j< n_ind/2){

        i1=int(n_ind*pow(_rnd1.Rannyu(),0.5));
        i2=int(n_ind*pow(_rnd1.Rannyu(),0.5));
        // Scelgo due individui a "caso" rulette truccata

       

        if(_rnd1.Rannyu() < p){//accetto lo scambio con prob p
            
            int t=_rnd1.Rannyu(1,n_cit); // Scelgo un punto a caso dove tagliare

            for (int s=0; s<n_cit;s++){
                figli(2*j,s)=_generazione(_ranking(i1,1)).GetTragitto(s);
                figli(2*j+1,s)=_generazione(_ranking(i2,1)).GetTragitto(s);
            }

            for (int i = 0; i < n_cit; i++) {
                    // Trova l'indice dell'elemento di vec2 nell'ordine di vec1
                    for (int k = 0; k < n_cit; k++) {
                        if (_generazione(_ranking(i1,1)).GetTragitto(k) == _generazione(_ranking(i2,1)).GetTragitto(i)) {
                            figli(2*j,i)=_generazione(_ranking(i1,1)).GetTragitto(k);
                            break;
                        }
                    }

                    // Trova l'indice dell'elemento di vec1 nell'ordine di vec2
                    for (int k = 0; k < n_cit; k++) {
                        if (_generazione(_ranking(i2,1)).GetTragitto(k) == _generazione(_ranking(i1,1)).GetTragitto(i)) {
                            figli(2*j+1,i)=_generazione(_ranking(i2,1)).GetTragitto(k);
                            break;
                        }
                    }
                }

            j++;
        }
    }
    for (int i=0; i<n_ind; i++){
        for (int j=0; j<n_cit; j++){
            _generazione(i).SetTragittoTarghet(j,figli(i,j));
        }
    }
}
*/
void Popolazione::crossover(int p) {
    int i1, i2;
    int j = 0;
    mat figli(n_ind, n_cit);

    while (j < n_ind / 2) {
        i1 = int(n_ind * pow(_rnd1.Rannyu(), 0.5));
        i2 = int(n_ind * pow(_rnd1.Rannyu(), 0.5));

        // Controlla che gli indici siano validi
        if (i1 < 0 || i1 >= n_ind || i2 < 0 || i2 >= n_ind) {
            throw std::out_of_range("Indice i1 o i2 fuori dai limiti in _ranking");
        }

        if (_rnd1.Rannyu() < p) {
            int t = _rnd1.Rannyu(1, n_cit);

            for (int s = 0; s < n_cit; s++) {
                if (2 * j >= n_ind || 2 * j + 1 >= n_ind || s >= n_cit) {
                    throw std::out_of_range("Indice fuori dai limiti in figli");
                }
                figli(2 * j, s) = _generazione(_ranking(i1, 1)).GetTragitto(s);
                figli(2 * j + 1, s) = _generazione(_ranking(i2, 1)).GetTragitto(s);
            }

            for (int i = t; i < n_cit; i++) {
                for (int k = 0; k < n_cit; k++) {
                    if (_generazione(_ranking(i1, 1)).GetTragitto(k) == _generazione(_ranking(i2, 1)).GetTragitto(i)) {
                        figli(2 * j, i) = _generazione(_ranking(i1, 1)).GetTragitto(k);
                        break;
                    }
                }

                for (int k = 0; k < n_cit; k++) {
                    if (_generazione(_ranking(i2, 1)).GetTragitto(k) == _generazione(_ranking(i1, 1)).GetTragitto(i)) {
                        figli(2 * j + 1, i) = _generazione(_ranking(i2, 1)).GetTragitto(k);
                        break;
                    }
                }
            }

            j++;
        }
    }

    for (int i = 0; i < n_ind; i++) {
        for (int j = 0; j < n_cit; j++) {
            _generazione(i).SetTragittoTarghet(j, figli(i, j));
        }
    }
}

void Popolazione :: mutazionePerm(double pmut) {
 
    for (int i=0; i<n_ind; i++) {
        _generazione(i).mutazionePerm(pmut); // Mutate the vector of cities
    }
}


mat Popolazione :: GetRanking() {
    // Get the ranking of the individual
    return _ranking;
}


vec Popolazione :: GetBest() {
    // Get the best individual
    vec best_tragitto(n_cit);
    for (int i = 0; i < n_cit; i++) {
        best_tragitto(i) = _generazione(_ranking(0,1)).GetTragitto(i);
    }
    return best_tragitto;
}

mat Popolazione :: GetCitta() {
    // Get the cities of the individual
    return _citta;
}


void Popolazione :: check() {
    // Check if the individual is valid
    for (int i=0; i<n_ind; i++) {
        if (_generazione(i).chek() == false) {
            cout << "Individuo non valido" << endl;
        }
    }
}