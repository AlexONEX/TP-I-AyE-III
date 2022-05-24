//Intento Implementacion 2
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

redSocial lectorInput2(string archivo) {
    redSocial f;
    ifstream parametros;
    parametros.open(archivo, ios::in);
    if (!parametros) {
        exit(1);
    }
    string s;
    int N, M;
    parametros >> s >> s >> N >> M;
    int k = 0;
    char t;
    vector<int> influencia(N);
    while (k < N) {
        int id;
        int i;
        parametros >> t >> id >> i;
        influencia[id - 1] = i;
        k++;
    }
    k = 0;
    f.influencia = influencia;
    vector<vector<bool>> amistades(N, vector<bool>(N, false));
    while (k < M) {
        int id1; int id2;
        parametros >> t >> id1 >> id2;
        amistades[id1 - 1][id2 - 1] = true;
        amistades[id2 - 1][id1 - 1] = true;
        k++;
    }
    f.amistades = amistades;

    return f;
}

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
    /*string archivo; cin >> archivo;
    ifstream parametros;
    parametros.open(archivo, ios::in);
    if (!parametros) {
        exit(1);
    }
    */
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

//Si la recorro desde adelante hacia atrás más rapido pero poda mal
bool noAmigos(const redSocial& rs, const vector<int> actores, const int actor){
    for (auto independientes=actores.begin(); independientes!=actores.end(); independientes++) {
        if (rs.amistades[actor][*independientes] == true) {
            return false;
        }
    }
    return true;
}

void noTieneNoAmigosF(const redSocial& rs, vector<int>& Q, vector<int>& K) { // O(n^2)
   vector<int> nuevoK; 
   for (int i = 0; i < K.size(); i++) {
        bool tiene_noamigo = false;
        for (int j = 0; j < K.size() && !tiene_noamigo; j++){
            if (!rs.amistades[K[i]][K[j]]) { //Si es no amigo
                tiene_noamigo = true;
                break;
            }
        }
        if(!tiene_noamigo) {
            Q.push_back(K[i]);
        }
        else{
            nuevoK.push_back(K[i]);
        }
    }
    K=nuevoK;
}

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

vector<vector<int>> conjuntosIndependientes(const redSocial& rs, const vector<int> actores) {
    vector<vector<int>> conjuntos;
    vector<bool> usados(actores.size(), false);
    for (int lider = actores.size()-1; lider >= 0; lider--){   
        if (usados[lider]) { continue; }
        vector<int> conj; conj.push_back(actores[lider]);
        usados[lider] = true;
        for (int actor = actores.size()-1; actor>=0; actor--) {
            if (usados[actor]) { continue; }
            if (noAmigos(rs, conj, actores[actor])){ //No hace falta checkear lider!=actor, porque la guarda noAmigos falla. 
                conj.push_back(actores[actor]);
                usados[actor] = true;
            }
        }
        conjuntos.push_back(conj);
    }
    return conjuntos;
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
        vector<vector<int>> conjuntos = conjuntosIndependientes(rs, K);
        int acc=0;
        for(auto conj: conjuntos){
            acc+=rs.influencia[conj[0]];
        }
        if(acc+influenciaCliche < maxInfluenciaAlMomento) {
            return;
        }
        int k = K.back();
        int nuevaInfluencia = influenciaCliche + rs.influencia[k];

        K.pop_back();     
        //noTieneNoAmigos(rs, cliche, K); //Podo de K los que no tienen unNoAmigo
        
        vector<int> qSinTocar = cliche;
        vector<int> kSinTocar = K;
        noTieneNoAmigos(rs, qSinTocar, kSinTocar);                 //Podando todo noAmigo.

        cliche.push_back(k);


        vector<int> nuevoK = amistades(rs, k, K);       //Todo K puede pertenecer a Q.  
        noTieneNoAmigos(rs, cliche, nuevoK);

        influenciaMaxima(rs, cliche, nuevoK, nuevaInfluencia);

        cliche.pop_back();
        //Acá K, vale que para todo k-ésimo puede pertenecer a Q? 
        influenciaMaxima(rs, qSinTocar, kSinTocar, influenciaCliche);
    }
}


int main() {
    redSocial redTest = lectorInput2("instancias-1/brock200_1.clq");
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