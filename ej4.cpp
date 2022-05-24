#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

struct actividad { int inicio, cierre; };

int n;
vector<actividad> actividades;


void leer_actividades() {
	// Leer encabezado
	cin >> n;
	actividades = vector<actividad>(n, actividad());

	// Leer actividades
	for (actividad & a: actividades) {
		cin >> a.inicio >> a.cierre;
		assert(0 <= a.inicio && a.inicio < a.cierre && a.cierre <= 2*n);
	}
}


int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);
	
	// Leer entrada
	leer_actividades();
	
	// Bucketsort según cierre (preservando orden original)
	vector<vector<int>> baldes(1+2*n, vector<int>());
	for (int i = 0; i < n; i++) {
		baldes[actividades[i].cierre].push_back(i);
	}
	vector<int> orden_por_cierre;
	for (vector<int> balde : baldes) {
		for (int i : balde) {
			orden_por_cierre.push_back(i);
		}
	}
	
	// Recorrer nueva secuencia, e ir comparando cierres contra inicios
	vector<int> actividades_realizadas;
	int ultimo_cierre = -1;
	for (int i = 0; i < n; i++) {
		int candidata = orden_por_cierre[i];
		if (actividades[candidata].inicio > ultimo_cierre) {
			actividades_realizadas.push_back(candidata);
			ultimo_cierre = actividades[candidata].cierre;
		}
	}
	
	// Reportar resultado
	printf("%i\n", actividades_realizadas.size());
		for (int id : actividades_realizadas) { printf("%i",id); }
	printf("\n");
}

