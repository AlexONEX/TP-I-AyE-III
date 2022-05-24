#include <vector>
#include <stack>
#include <numeric>
#include <algorithm>
#include <iostream>

using namespace std;


struct RedSocial {
	int tamanio;
	vector<int> influencias;
	vector<vector<bool>> amistades;
	vector<int> aOrdenOriginal;
};

RedSocial leerRedSocial() {
	string s; char c; int n; cin >> s >> s;
	
	RedSocial red;
	
	cin >> red.tamanio;
	int cantidadAmistades; cin >> cantidadAmistades;
	
	vector<int> influenciasOriginales(red.tamanio);
	for (int i = 0; i < red.tamanio; i++) {
		cin >> c >> n >> influenciasOriginales[i];
	}
	
	red.aOrdenOriginal = vector<int>(red.tamanio);
	iota(red.aOrdenOriginal.begin(), red.aOrdenOriginal.end(), 0);
	sort(red.aOrdenOriginal.begin(), red.aOrdenOriginal.end(), [&](int a,int b){
		return influenciasOriginales[a] > influenciasOriginales[b];
	});
	
	vector<int> aOrdenNuevo(red.tamanio);
	for (int i = 0; i < red.tamanio; i++) {
		aOrdenNuevo[red.aOrdenOriginal[i]] = i;
	}

	red.influencias = vector<int>(red.tamanio);
	for (int i = 0; i < red.tamanio; i++) {
		red.influencias[i] = influenciasOriginales[red.aOrdenOriginal[i]];
	}

	red.amistades = vector<vector<bool>>(red.tamanio,vector<bool>(red.tamanio));
	for (int i = 0; i < cantidadAmistades; i++) {
		int actorA, actorB;
		cin >> c >> actorA >> actorB;
		red.amistades[aOrdenNuevo[actorA-1]][aOrdenNuevo[actorB-1]] = true;
		red.amistades[aOrdenNuevo[actorB-1]][aOrdenNuevo[actorA-1]] = true;
	}
	
	return red;
}

bool sonAmigos(int actorA, int actorB, const RedSocial & red) {
	return red.amistades[actorA-1][actorB-1];
}

int influencia(int actor, const RedSocial & red) {
	return red.influencias[actor-1];
}

int influenciaCombinada(const vector<int> & actores, const RedSocial & red) {
	int acumulada = 0;
	for (int actor : actores) { acumulada += influencia(actor, red); }
	return acumulada;
}

struct K {
	const int FIN = -1;
	vector<stack<int>> siguientes;  // Los siguientes a quienes apuntó cada nodo
	stack<vector<int>*> modificaciones;  // Nodos reapuntados con cada extración
	
	// Inicialmente cada nodo apunta al inmediatamente siguiente ([0] es inicio)
	K(int tamanio) : siguientes(tamanio+1) {
		for (int i = 0; i < tamanio+1; i++) { siguientes[i].push(i+1); }
		siguientes.back().top() = FIN;
	}
	
	void restaurarExtraccion() {
		for (int modificado : *modificaciones.top()) {
			siguientes[modificado].pop();
		}
		delete modificaciones.top();
		modificaciones.pop();
	}
	
	const int primero() { return siguientes[0].top(); }
	const int siguiente(int actor) { return siguientes[actor].top(); }
	
	int extraerPrimero() {
		int actor = primero();
		vector<int> * modificacion = new vector<int>{0};
		modificaciones.push(modificacion);
		siguientes[0].push(siguiente(actor));
		return actor;
	}
	
	void extraerGrupo(const vector<int> & grupo) {
		vector<int> * modificacion = new vector<int>();
		int aReapuntar = 0, aExtraer = 0;
		while (aExtraer < grupo.size()) {
			// Avanzar en la secuencia hasta tener que extraer algo
			while (siguiente(aReapuntar) != grupo[aExtraer]) {
				aReapuntar = siguiente(aReapuntar);
			}
			
			// Reapuntar a luego del continuo
			int nuevaPunta = siguiente(aReapuntar);
			while (aExtraer < grupo.size() && nuevaPunta == grupo[aExtraer]) {
				nuevaPunta = siguiente(nuevaPunta);
				aExtraer++;
			}
			siguientes[aReapuntar].push(nuevaPunta);
			(*modificacion).push_back(aReapuntar);
			
			// Reposicionar para procesar lo que siga
			aReapuntar = nuevaPunta;
		}
		modificaciones.push(modificacion);
	}
	
	vector<int> extraerNoAmigos(int elActor, const RedSocial & red) {
		vector<int> noAmigos;
		int otroActor = primero();
		while (otroActor != FIN) {
			if (!sonAmigos(elActor, otroActor, red)) {
				noAmigos.push_back(otroActor);
			}
			otroActor = siguiente(otroActor);
		}
		
		extraerGrupo(noAmigos);
		
		return noAmigos;
	}
	
	vector<int> extraerAmigosDeTodos(const RedSocial & red) {
		vector<int> amigosDeTodos;
		vector<int> * modificacion = new vector<int>();
		
		bool elAnteriorFueAmigoDeTodos = false;
		int previoAlActor = 0, actor = primero();
		while (actor != FIN) {
			bool esAmigoDeTodosLosOtros = true;
			int otroActor = primero();
			while (otroActor != FIN) {
				if (actor != otroActor && !sonAmigos(actor, otroActor, red)) {
					esAmigoDeTodosLosOtros = false;	break;
				}
				otroActor = siguiente(otroActor);
			}
			if (esAmigoDeTodosLosOtros) {
				amigosDeTodos.push_back(actor);
				// Extraerlo ahora para evitar repetir comparaciones
				if (elAnteriorFueAmigoDeTodos) {
					siguientes[previoAlActor].top() = siguiente(actor);
				} else {				
					(*modificacion).push_back(previoAlActor);
					siguientes[previoAlActor].push(siguiente(actor));
				}
			} else {
				previoAlActor = siguiente(previoAlActor);
			}
			actor = siguiente(previoAlActor);
			elAnteriorFueAmigoDeTodos = esAmigoDeTodosLosOtros;
		}
		
		modificaciones.push(modificacion);
		return amigosDeTodos;
	}
};

int influenciaTrasParticionK(const RedSocial & red, K & k) {
	vector<vector<int>> fragmentos;
	for (int actor = k.primero(); actor != k.FIN; actor = k.siguiente(actor)) {
		int fragmentoAsignado = 0;
		while (fragmentoAsignado < fragmentos.size()) {
			bool independienteAlFragmento = true;
			for (int otroActor : fragmentos[fragmentoAsignado]) {
				if (sonAmigos(actor, otroActor, red)) {
					independienteAlFragmento = false; break;
				}
			}
			if (independienteAlFragmento) {
				fragmentos[fragmentoAsignado].push_back(actor); break;
			} else { fragmentoAsignado++; }
		}
		if (fragmentoAsignado == fragmentos.size()) {
			fragmentos.push_back({actor});
		}
	}
	
	vector<int> lideresDeFragmento;
	for (vector<int> fragmento : fragmentos) {
		lideresDeFragmento.push_back(fragmento.front());
	}
	
	return influenciaCombinada(lideresDeFragmento, red);
}


// Explora ambos subárboles de decisión, recuerda si llegó a un clique relevante
void buscarCliqueDeMayorInfluencia(
		const RedSocial & red, K & aunNoDecididosK, int influenciaEnK,
		vector<vector<int>> & cliqueParcialQ, int influenciaDeQ,
		vector<int> & mejorCliqueVisto, int & influenciaDeMejorCliqueVisto) {
	if (influenciaDeQ + influenciaEnK < influenciaDeMejorCliqueVisto) { return;}
	
	int queda = influenciaTrasParticionK(red, aunNoDecididosK);
	if (influenciaDeQ + queda < influenciaDeMejorCliqueVisto) { return; }
	
	if (influenciaEnK == 0) {
		// En caso de serlo, recordar nuevo clique óptimo
		if (influenciaDeQ > influenciaDeMejorCliqueVisto) {
			mejorCliqueVisto.clear();
			for (vector<int> agregado : cliqueParcialQ) {
				for (int actor : agregado) {
					mejorCliqueVisto.push_back(actor);
				}
			}
			influenciaDeMejorCliqueVisto = influenciaDeQ;
		}
		return;
	}
	
	// Casos recursivos (/**/ indíca modificaciones y restauraciones del estado)
/**/int actorPorDecidir = aunNoDecididosK.extraerPrimero();
	vector<bool> decisiones = {true, false};
	for (bool elActorEntraEnQ : decisiones) {
		// Reducir K
		vector<int> noAmigosDelActorDecididoQueDebenSalirDeK = elActorEntraEnQ ?
/**/			aunNoDecididosK.extraerNoAmigos(actorPorDecidir, red) :
				vector<int>();
		vector<int> amigosDeTodosLosQueTambienQuedaronEnK = aunNoDecididosK.extraerAmigosDeTodos(red); 
		//vector<int> amigosDeTodosLosQueTambienQuedaronEnK = vector<int>();
		
		int nuevaInfluenciaEnK = influenciaEnK;
		int nuevaInfluenciaDeQ = influenciaDeQ;
		nuevaInfluenciaEnK -= influencia(actorPorDecidir,red);
		nuevaInfluenciaDeQ += influencia(actorPorDecidir,red)*(elActorEntraEnQ);
		int influeciaDeCliqueExtraidoDeK = 
				influenciaCombinada(amigosDeTodosLosQueTambienQuedaronEnK, red);
		nuevaInfluenciaEnK -= influeciaDeCliqueExtraidoDeK;
		nuevaInfluenciaDeQ += influeciaDeCliqueExtraidoDeK;
		nuevaInfluenciaEnK -=
			influenciaCombinada(noAmigosDelActorDecididoQueDebenSalirDeK, red);
		
		// Crecer Q
/**/	if (elActorEntraEnQ) { cliqueParcialQ.push_back({actorPorDecidir}); }
/**/	cliqueParcialQ.push_back(amigosDeTodosLosQueTambienQuedaronEnK);
		
		// Buscar recursivamente
		buscarCliqueDeMayorInfluencia(
			red, aunNoDecididosK, nuevaInfluenciaEnK,
			cliqueParcialQ, nuevaInfluenciaDeQ,
			mejorCliqueVisto, influenciaDeMejorCliqueVisto);
		
		// Restaurar Q
/**/	cliqueParcialQ.pop_back();
/**/	if (elActorEntraEnQ) { cliqueParcialQ.pop_back(); }
		
		// Restaurar K
		aunNoDecididosK.restaurarExtraccion();
/**/	if (elActorEntraEnQ) { aunNoDecididosK.restaurarExtraccion(); }
	}
/**/aunNoDecididosK.restaurarExtraccion();
}


int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);
	
	// Leer entrada
	RedSocial red = leerRedSocial();
	
	// Contabilizar influencia disponible
	vector<int> actores(red.tamanio);
	iota(actores.begin(), actores.end(), 1);
	int influenciaExistente = influenciaCombinada(actores, red);
	
	// Buscar el mejor "clique"
	K noYaDecididosK(red.tamanio);
	vector<vector<int>> cliqueParcialQ;
	vector<int> mejorCliqueVisto; int influenciaDeMejorCliqueVisto = 0;
	buscarCliqueDeMayorInfluencia(
		red, noYaDecididosK, influenciaExistente,
		cliqueParcialQ, 0,
		mejorCliqueVisto, influenciaDeMejorCliqueVisto);
	
	// Reportar resultado
	cout << influenciaDeMejorCliqueVisto << endl;
	for (int actor : mejorCliqueVisto) {
		cout << red.aOrdenOriginal[actor-1]+1 << ' '; }
	cout << endl;
}
