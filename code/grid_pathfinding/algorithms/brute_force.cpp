#include "brute_force.h"
#include <limits>

using namespace std;

// Funcion recursiva para explorar todos los caminos
void explorar(int i, int j, int vidas, long long puntaje, int P, string camino, int n, int m,
    const std::vector<std::vector<int>>& types, const std::vector<std::vector<long long>>& values, PathResult& mejor_resultado) {
	
	if(types[i][j] == 0) { // encontramos una muralla
		return; // adios para siempre ;c
	}

	if(types[i][j] == -1) { // encontramos una mina! :o
		vidas--;
		puntaje -= P;

		if(vidas <= 0) { // muerte xo
			return;
		}
	} else { // es una celda normal :p
		puntaje += values[i][j];
	}


	// LLegamos al final! B]
	if(i == n-1 && j == m-1) {
		if (!mejor_resultado.found || puntaje > mejor_resultado.score) {
			mejor_resultado.found = true;
			mejor_resultado.score = puntaje;
			mejor_resultado.path = camino;
			mejor_resultado.remaining_lives = vidas;
		}
		return;
	}


	// intentamos ir a la derecha
	if(j+1 < m) {
		explorar(i, j + 1, vidas, puntaje, P, camino + "R", n, m, types, values, mejor_resultado);
	}

	// intentamos ir hacia abajo
	if(i+1 < n) {
		explorar(i+1, j, vidas, puntaje, P, camino + "D", n, m, types, values, mejor_resultado);
	}


}

// Funcion principal :o
PathResult solveBruteForce(
    int n, int m, int v, int p,
    const std::vector<std::vector<int>>& types,
    const std::vector<std::vector<long long>>& values
) {

    // Resultado inicial: no se ha encontrado solucion
    PathResult mejor_resultado;

    mejor_resultado.found = false;
    mejor_resultado.score = 0;
    mejor_resultado.path = "";
    mejor_resultado.remaining_lives = v;

    // Verificamos que las dimensiones sean validas
    if (n <= 0 || m <= 0) {
        return mejor_resultado;
    }

    // Comenzamos desde la posicion inicial (0,0)
    explorar(0, 0, v, 0, p, "", n, m, types, values, mejor_resultado);

    return mejor_resultado;
}