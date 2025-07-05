#include "Individuo.h"

Individuo::Individuo(int n_citta, Random& _rnd)
{
    _n_citta = n_citta;
    _tragitto.resize(_n_citta);

    for (int i = 0; i < _n_citta; i++)
    {
        _tragitto(i) = i + 1;
    }

    for (int i = 0; i < _n_citta; i++)
    {
        mutazione_permutazione(1.0, _rnd);
    }
}


bool Individuo :: check_tragitto(){

    // Check if the individual is valid
    for (int i=0; i< _n_citta; i++) {
        for (int j=i+1; j<_n_citta; j++) {
            if (_tragitto(i)==_tragitto(j)) {
                return false;
            }
        }
    }
    if (_tragitto(0)!=1){return false;}
    return true;
}

int Individuo::get_tragitto_component(int i)
{
    return _tragitto(i);
}

int Individuo::set_tragitto_component(int i, int j)
{
    _tragitto(i) = j;
    return _tragitto(i);
}

int Individuo::pbc(int i)
{
    if (i >= _n_citta)
    {
        return i - _n_citta +1;
    }
    else if (i < 0)
    {
        return i + _n_citta-1;
    }
    else
    {
        return i;
    }
}



void Individuo::mutazione_permutazione(double prob, Random& _rnd)
{
    if (_rnd.Rannyu()<prob)
{
int i1=_rnd.Rannyu(1,_n_citta);
int i2;
do i2=_rnd.Rannyu(1,_n_citta);
while (i1==i2);
    swap(_tragitto(i1),_tragitto(i2));
}
}

void Individuo::mutazione_permutazione_blocco(double prob, Random& _rnd)
{
    if (_rnd.Rannyu() < prob)
    {
        int m;    //dimensione del blocco che scambi
        if (_n_citta%2 != 0)
        {m = _rnd.Rannyu(2,( _n_citta - 1) / 2);}
        else
        {m = _rnd.Rannyu(2, (_n_citta - 2) / 2);}

        int i1 = _rnd.Rannyu(1, _n_citta); //primo indice del primo blocco
        int i2;
        do{
        i2 = _rnd.Rannyu(1, _n_citta); //primo indice del secondo blocco
        
        }while((i1-m <= i2 and i2<= i1 + m));//non voglio che i blocchi si sovrappongano
//non è un problema se i1-m < 0 o i1+m>n_citta perchè i2 è tra 1 e n_citta-1
        for (int i = 0; i < m; i++)
        {
            swap(_tragitto(pbc(i1 + i)), _tragitto(pbc(i2 + i)));
        }
    } 
}

void Individuo::mutazione_inversione_blocco(double prob, Random& _rnd)
{

        if (_rnd.Rannyu() < prob)
    {
        int i1, i2;
        // Scegli due indici distinti (almeno distanza 2) per delimitare il blocco da invertire
        do {
            i1 = _rnd.Rannyu(1, _n_citta);
            i2 = _rnd.Rannyu(1, _n_citta);
        } while (abs(i2 - i1) < 2);

        // Ordina gli indici in modo che i1 < i2
        if (i1 > i2) swap(i1, i2);

        // Inverti il blocco compreso tra i1 e i2 (estremi inclusi)
        for (int i = 0; i < (i2 - i1 + 1) / 2; i++)
        {
            swap(_tragitto(i1 + i), _tragitto(i2 - i));
        }
    }
}

void Individuo::mutazione_traslazione_blocco(double prob, Random& _rnd) {
    if (_rnd.Rannyu() < prob) {
        int size = _n_citta;

        int m = _rnd.Rannyu(1, size - 1);                // lunghezza blocco da traslare
        int start = _rnd.Rannyu(1, size - m);            // inizio del blocco (escludendo la prima città)
        int max_n = size - (start + m);                  // massimo spostamento possibile
        int n = _rnd.Rannyu(1, max_n + 1);               // spostamento effettivo

        std::vector<int> nuovo_tragitto;
        nuovo_tragitto.reserve(size);

        // 1) Copia parte iniziale
        for (int i = 0; i < start; ++i)
            nuovo_tragitto.push_back(_tragitto[i]);

        // 2) Copia la parte tra blocco e posizione d'inserimento
        for (int i = start + m; i < start + m + n; ++i)
            nuovo_tragitto.push_back(_tragitto[i]);

        // 3) Copia il blocco da spostare
        for (int i = start; i < start + m; ++i)
            nuovo_tragitto.push_back(_tragitto[i]);

        // 4) Copia il resto del tragitto
        for (int i = start + m + n; i < size; ++i)
            nuovo_tragitto.push_back(_tragitto[i]);

        // Sostituisci il tragitto originale
        for (int i = 0; i < size; ++i)
            _tragitto[i] = nuovo_tragitto[i];
    }
}





Individuo::~Individuo(){}

