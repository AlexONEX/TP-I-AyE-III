/*

1.
Actividad → es vertice
Beneficio → influencia
No solapa → adyacencia

2.
b(i) = { si i=n, 0
       { si i<n, max(b(i+1), b({Aᵢ}) + b(siguiente(i))
       donde siguiente(i) min{n, j∈ℕ / i<j≤n ∧ (j=n ∨ᴸ tᵢ<sⱼ)}

3.
¿?
Dentro del conjunto de partes de A,
se busca el subconjunto de actividades compatibles (S),
que maximice b(S).
"partes" → combinatoria
"maximice" → optimización

4. 5. y 6.
Se precomputan todos los siguiente(i), ordenando por los tᵢ con bucketsort (≤2n)

*/

bool MODO_JUSTIFICATIVO = false;

#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

struct actividad { int inicio, cierre, beneficio; };

int n;
vector<actividad> actividades;
vector<int> b_alcanzables;
vector<bool> ya_computado;
vector<int> computado_siguientes;


void leer_actividades() {
	// Leer encabezado
	cin >> n;
	actividades = vector<actividad>(n, actividad());

	// Leer actividades
	for (actividad & a: actividades) {
		cin >> a.inicio >> a.cierre >> a.beneficio;
		assert(0 <= a.inicio && a.inicio < a.cierre && a.cierre <= 2*n);
	}
}


void precomputar_compatibilidades() {
	computado_siguientes = vector<int>(n);
	
	// Bucketsort según cierre (preservando orden por inicio)
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
	for (int i = 0, j = 0; i < n; i++) {
		int cierre = actividades[orden_por_cierre[i]].cierre;
		while (j < n) {
			int candidato_inicio_posterior = actividades[j].inicio;
			if (candidato_inicio_posterior > cierre) { break; }
			j++;
		}
		
		computado_siguientes[orden_por_cierre[i]] = j;
	}
}


int beneficio_alcanzable(int i) {
	assert(i <= n);
	if (i == n) { return 0; }  // Caso base
	if (ya_computado[i]) { return b_alcanzables[i]; }  // Caso memoizado
	
	int alcanzable = max (
		beneficio_alcanzable(i+1),
		actividades[i].beneficio + beneficio_alcanzable(computado_siguientes[i])
	);
	ya_computado[i] = true;
	b_alcanzables[i] = alcanzable;
	
	return alcanzable;
}


int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);
	
	// Leer entrada
	leer_actividades();
	
	// Preparar memoización	
	b_alcanzables = vector<int>(n);
	ya_computado = vector<bool>(n, false);
	
	// Precomputar las actividades compatibles luego de cada cual
	precomputar_compatibilidades();
	
	// Computar mayor beneficio alcanzable con actividades compatibles
	//for (int i = n; i >= 0; i--) { beneficio_alcanzable(i); }  // bottom-up 5.
	int mayor_beneficio_alcanzable = beneficio_alcanzable(0);
	
	// Reconstruir conjunto más beneficioso de actividades compatibles (item 6.)
	vector<int> mejor_conjunto_de_actividades;
	int i = 0;
	while (i < n) {
		if (beneficio_alcanzable(i) > beneficio_alcanzable(i+1)) {
			mejor_conjunto_de_actividades.push_back(i);
			i = computado_siguientes[i];
		} else { i++; }
	}
	
	// Reportar resultado
	printf("%i\n", mayor_beneficio_alcanzable);
		for (int id : mejor_conjunto_de_actividades) { printf("%i",id); }
	printf("\n");
	
	if (MODO_JUSTIFICATIVO) {
		printf("// Subconjunto de mayor beneficio:\n");
		for (int i = 0; i < mejor_conjunto_de_actividades.size(); i++) {
			actividad a = actividades[mejor_conjunto_de_actividades[i]];
			printf("// %i: %i→%i (%i)\n", mejor_conjunto_de_actividades[i],
				a.inicio, a.cierre, a.beneficio);
		}
	}
}

