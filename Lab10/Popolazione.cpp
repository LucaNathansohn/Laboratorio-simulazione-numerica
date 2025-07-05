#include "Popolazione.h"

using namespace arma;


Popolazione::Popolazione(int n_individui, int n_citta, int rank)
{
    _n_ind = n_individui;
    _n_citta = n_citta;

    int p1, p2; // Read from Primes a pair of numbers to be used to initialize the RNG
    ifstream Primes("primes64001.in");
for(int i=0; i<rank+2;i++){
        Primes >> p1 >> p2;
}
    Primes.close();
    int seed[4]; // Read the seed of the RNG
    ifstream Seed("seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];



    _generazione.set_size(_n_ind);
    _rnd.SetRandom(seed,p1,p2);

    _citta.set_size(_n_citta, 2);
    _distanze.set_size(_n_citta, _n_citta);
    _fitness.set_size(_n_ind);
    _ranking.set_size(_n_ind);
    _rank = rank; //quale processore

    for (int i = 0; i < _n_ind; i++) 
    {
        _generazione(i) = Individuo(_n_citta, _rnd);
    }
}

Popolazione::~Popolazione(){}

void Popolazione::set_citta_circonferenza ()
{
    for (int i = 0; i < _n_citta; i++)
    {
        double theta = _rnd.Rannyu(0, 2 * M_PI);
        _citta(i, 0) = cos(theta);
        _citta(i, 1) = sin(theta);
    }
}

void Popolazione::set_citta_quadrato ()
{
    for (int i = 0; i < _n_citta; i++)
    {
        _citta(i, 0) = _rnd.Rannyu(0, 1);
        _citta(i, 1) = _rnd.Rannyu(0, 1);
    }
}

void Popolazione::calcola_distanze() // da controllare l'ottimizzazione ma mi sembra giusta
{
    for (int i = 0; i < _n_citta; i++) 
    {
        for (int j = i; j < _n_citta; j++)
        {
            if (i != j)
            {
                _distanze(i, j) = sqrt(pow(_citta(i, 0) - _citta(j, 0), 2) + pow(_citta(i, 1) - _citta(j, 1), 2));
                _distanze(j, i) = _distanze(i, j); // matrice simmetrica
            }
            else
            {
                _distanze(i, j) = 0;
            }
        }
    }
}

void Popolazione::calcola_fitness()
{
    // Calcola il fitness per ogni individuo

    _fitness.zeros();


    for (int j=0; j < _n_ind; j++)
    {
        for (int i = 0; i < _n_citta - 1; i++)
        {
            _fitness(j) += pow(_distanze(_generazione(j).get_tragitto_component(i) - 1, _generazione(j).get_tragitto_component(i + 1) - 1),1);
        }
        _fitness(j) += pow(_distanze(_generazione(j).get_tragitto_component(_n_citta - 1) - 1, _generazione(j).get_tragitto_component(0) - 1),1); //chiudo il giro
    }
}

void Popolazione::calcolaRanking()
{
    _ranking = sort_index(_fitness, "ascend"); // ordinamento decrescente

    //vettore di indici secondo cui dovrebbero esseree ordinati gli individui
}
void Popolazione :: crossover(double prob){

   //crossover per tutta la popolazione (finche non creo _n_ind figli)
    
    int i1,i2,i3;
    mat figli;
    figli.set_size(_n_ind,_n_citta);
    for (int j=0;j< _n_ind/2;j++){

        i1=int(_n_ind*pow(_rnd.Rannyu(),2.5));
        i2=int(_n_ind*pow(_rnd.Rannyu(),2.5));
        // Scelgo due individui a "caso" roulette truccata
        for (int s=0; s<_n_citta;s++){
                figli(2*j,s)=_generazione(_ranking(i1)).get_tragitto_component(s);
                figli(2*j+1,s)=_generazione(_ranking(i2)).get_tragitto_component(s);
            }
        if(_rnd.Rannyu() < prob){//accetto lo scambio con probabilità prob
            int t=_rnd.Rannyu(1,_n_citta-1); // Scelgo un punto a caso dove tagliare
            int appo=t;
            int appo2=t;
            for (int i = 0; i < _n_citta; i++) {
                for (int k = t; k < _n_citta; k++) {
                     // Trova l'indice dell'elemento di vec2 nell'ordine di vec1
                    if (_generazione(_ranking(i1)).get_tragitto_component(k) == _generazione(_ranking(i2)).get_tragitto_component(i)) {
                        figli(2*j,appo)=_generazione(_ranking(i2)).get_tragitto_component(i);
                        appo++;
                    }
                    // Trova l'indice dell'elemento di vec1 nell'ordine di vec2
                    if (_generazione(_ranking(i2)).get_tragitto_component(k) == _generazione(_ranking(i1)).get_tragitto_component(i)) {
                        figli(2*j+1,appo2)=_generazione(_ranking(i1)).get_tragitto_component(i);
                        appo2++;
                     }
                }
            }
        }
    }
    // Gestione del caso dispari: l'ultimo individuo viene copiato senza crossover
    if (_n_ind % 2 != 0) {
        i3 = int(_n_ind * pow(_rnd.Rannyu(), 2.5));
        int last_index = _n_ind - 1;
        for (int s = 0; s < _n_citta; s++) {
            figli(last_index, s) = _generazione(_ranking(i3)).get_tragitto_component(s);
        }
    }
    for (int i=0; i<_n_ind; i++){
        for (int j=0; j<_n_citta; j++){
            _generazione(i).set_tragitto_component(j,figli(i,j));
        }
    }
}


mat Popolazione::get_citta(){
    return _citta;
}

uvec Popolazione::get_ranking()
{
    return _ranking;
}

Individuo Popolazione :: get_best() {
    // Get the best individual
    return _generazione(_ranking(0));
}
//restituisce il miglior individuo della generazione

vec Popolazione::get_fitness()
{
    return _fitness;
} 

void Popolazione::mutazione_perm(double prob)
{
    for (int i = 0; i < _n_ind; i++)
    {
        _generazione(i).mutazione_permutazione(prob, _rnd);
    }
}

void Popolazione::mutazione_perm_blocco(double prob)
{
    for (int i = 0; i < _n_ind; i++)
    {
        _generazione(i).mutazione_permutazione_blocco(prob, _rnd);
    }
}
void Popolazione::mutazione_inv_blocco(double prob)
{
    for (int i = 0; i < _n_ind; i++)
    {
        _generazione(i).mutazione_inversione_blocco(prob, _rnd);
    }
}
void Popolazione::mutazione_trasl_blocco(double prob)
{
    for (int i = 0; i < _n_ind; i++)
    {
        _generazione(i).mutazione_traslazione_blocco(prob, _rnd);
    }
}

void Popolazione :: check() {
    // Check if the individual is valid
    for (int i=0; i<_n_ind; i++) {
        if (_generazione(i).check_tragitto() == false) {
            cout << "Individuo non valido" << endl;
    }
    }
}

void Popolazione::set_best(int best[])
{
    // Set the best individual in the population
    for (int i = 0; i < _n_citta; i++) {
        _generazione(_ranking(0)).set_tragitto_component(i, best[i]);
    }
}

void Popolazione::set_citta(string filename)
{
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    for (int i = 0; i < _n_citta; i++) {
        fin >> _citta(i, 0) >> _citta(i, 1);
    }
    fin.close();
}

double Popolazione::media_fitness() //calcola la media del fitness della migliore metà di ciascuna generazione
{
    double media = 0;
    for (int i = 0; i < _n_ind / 2; i++)
    {
        media += _fitness(_ranking(i));
    }
    return media / (_n_ind / 2);
}
