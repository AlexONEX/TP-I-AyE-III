#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <fstream>
#include <chrono>
#include <tuple>
#include <assert.h>
#define NDEBUG

using namespace std::chrono;
using namespace std;

struct redSocial {
    vector<int> influencia;
    vector<vector<bool>> amistades;
};

bool ordenado(const redSocial& rs, vector<int> v){
    for(int i = v.size()-1; i > 0; i--){
        if(rs.influencia[v[i-1]]>rs.influencia[v[i]]){
            return false;
        }
    }
    return true;
}

void noRepetidos(vector<int> Q){
    sort(Q.begin(), Q.end());
    auto it = adjacent_find(Q.begin(), Q.end());
    assert(it == Q.end());
}

bool tieneNoAmigo(const redSocial& rs,vector<int>& K){
    bool res = true;
    for(int k: K){
        bool amigosk = false;
        for(int k2: K){
            if(!rs.amistades[k][k2]){
                amigosk = true;
            }
        }
        res = res && amigosk;
    }
    return res;
}

bool amigoDeTodos(const redSocial& rs, vector<int>& Q, vector<int>& K){
    for(int k: K){
        for(int q: Q){
            if(!rs.amistades[k][q]){
                cout <<"rompieron todo :enojado: -> " << k+1 << " " << q+1 << endl;
                return false;
            }
        }
    }
    return true;
}

void printVector(vector<int>& v) {
    for (int i = 0; i < v.size(); i++) {
        cout << v[i]+1 << " ";
    }
    cout << endl;
}

redSocial lectorInput() {
    redSocial f;
    string s;
    int N, M;
    cin >> s >> s >> N >> M;
    int k = 0;
    char t;
    vector<int> influencia(N);
    vector<vector<bool>> amistades(N, vector<bool>(N, false));
    while (k < N) {
        int id;
        int i;
        cin >> t >> id >> i;
        influencia[id - 1] = i;
        amistades[id - 1][id - 1] = true;
        k++;
    }
    k = 0;
    f.influencia = influencia;
    while (k < M) {
        int id1; int id2;
        cin >> t >> id1 >> id2;
        amistades[id1 - 1][id2 - 1] = true;
        amistades[id2 - 1][id1 - 1] = true;
        k++;
    }
    f.amistades = amistades;
    return f;
}

int sumInfluencia(const redSocial& rs, vector<int>& cliche) { // O(n)
    int suma = 0;
    for (int i = 0; i < cliche.size(); i++) {
        suma += rs.influencia[cliche[i]];
    }
    return suma;
}

vector<int> amistades(const redSocial& rs, int persona, const vector<int>& posAmistades) { // O(n)
    vector<int> amistades_;
    for (int i = 0; i < posAmistades.size(); i++) {
        if (rs.amistades[persona][posAmistades[i]] == true) {
            amistades_.push_back(posAmistades[i]);
        }
    }
    return amistades_;
}

bool noAmigos(const redSocial& rs, const vector<int> actores, const int actor){
    for (auto independientes=actores.begin(); independientes!=actores.end(); independientes++) {
        if (rs.amistades[actor][*independientes] == true) {
            return false;
        }
    }
    return true;
}
/*
vector<vector<int>> conjuntosIndependientes(const redSocial& rs, const vector<int> actores) {
    vector<vector<int>> conjuntos;
    vector<bool> usados(actores.size(), false);
    for (int lider = 0; lider < actores.size(); lider--){   
        if (usados[lider]) { continue; }
        vector<int> conj; conj.push_back(actores[lider]);
        usados[lider] = true;
        for (int actor = lider+1; actor<actores.size(); actor--) {
            if (usados[actor]) { continue; }
            if (noAmigos(rs, conj, actores[actor])){ 
                conj.push_back(actores[actor]);
                usados[actor] = true;
            }
        }
        conjuntos.push_back(conj);
    }
    return conjuntos;
}
*/
void noTieneNoAmigos(const redSocial& rs, vector<int>& Q, vector<int>& K){
    vector<int> nuevoK;
    for(int k: K){
        bool viable = true;
        for(int k2: K){
            if(!rs.amistades[k][k2]){
                viable = false;
                break;
            }
        }
        if(viable){
            Q.push_back(k);
        } else {
            nuevoK.push_back(k);
        }
    }
    K = nuevoK;
}

vector<int> maxCliche;
int maxInfluenciaAlMomento = 0;

void influenciaMaxima(const redSocial& rs, vector<int> cliche, vector<int> K, int influenciaCliche){ 
    /* for(int q: cliche){
            cout << q+1 << " ";
        }
    cout << endl;
    for(int k: K){
            cout << k+1 << " ";
    }
    cout << endl;
    cout << endl;
    assert(ordenado(rs, K));
    assert(amigoDeTodos(rs, cliche, K));
    assert(amigoDeTodos(rs, cliche, cliche));
    assert(tieneNoAmigo(rs,K));
    assert(influenciaCliche == sumInfluencia(rs, cliche));
    noRepetidos(cliche); */
    influenciaCliche = sumInfluencia(rs, cliche);
    if (K.size() == 0) {
        if(influenciaCliche > maxInfluenciaAlMomento) {
            maxInfluenciaAlMomento = influenciaCliche;
            maxCliche = cliche;
        }   
        return;
    }
    else {
        if(sumInfluencia(rs, K)+influenciaCliche < maxInfluenciaAlMomento) {
            return;
        }
        int k = K.back();
        int nuevaInfluencia = influenciaCliche + rs.influencia[k];

        K.pop_back();     
        
        vector<int> qSinTocar = cliche;
        vector<int> kSinTocar = K;
        noTieneNoAmigos(rs, qSinTocar, kSinTocar);                 
        cliche.push_back(k);

        vector<int> nuevoK = amistades(rs, k, K);        
        noTieneNoAmigos(rs, cliche, nuevoK);
        influenciaMaxima(rs, cliche, nuevoK, nuevaInfluencia);

        cliche.pop_back();
        influenciaMaxima(rs, qSinTocar, kSinTocar, influenciaCliche);
    }
}

int main() {
    redSocial redTest = lectorInput();
    vector<int> personajes;
    for (int i = 0; i < redTest.influencia.size(); i++) {
        personajes.push_back(i);
    }
    vector<int> influencias = redTest.influencia;
    sort(personajes.begin(), personajes.end(), [&](int i, int j) {
        return influencias[i] < influencias[j]; 
    });
    vector<int> cliche = {}; int influenciaInicial=0;

    noTieneNoAmigos(redTest, cliche, personajes);

    influenciaMaxima(redTest, cliche, personajes, influenciaInicial);
    int inf = sumInfluencia(redTest, maxCliche);

    cout << inf << endl;
    printVector(maxCliche);
    return 0;
}