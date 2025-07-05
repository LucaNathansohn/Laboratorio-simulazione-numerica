#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "random.h"
#include "individuo.h"
#include "popolazione.h"

using namespace std;

int main(){
    int n_ind=100;
    int n_cit=34;
    ofstream out;
    cout<<"suca";
    Popolazione corridori(n_ind,n_cit);
    cout<<"suca";
    corridori.setCittaCirconferenza();
    cout<<"suca";
    corridori.calcolaDistanza();
    cout<<"suca";
    out.open("dati/citta.dat");
    out <<"x y"<<endl;
    for (int i=0; i<n_cit; i++) {
        out << corridori.GetCitta()(i,0) << " " << corridori.GetCitta()(i,1) << endl;
    }
    out.close();
    out.open("dati/Best.dat");
    for (int i=0; i<100; i++) {
        corridori.check();
        corridori.calcolaFitness();
        corridori.calcolaRanking();
        corridori.crossover(1);
        corridori.mutazionePerm(0.01);

        for (int j=0; j<n_cit; j++) {
            out << corridori.GetBest()(j) << " ";
        }
        out<<endl;

    }

return 0;
}