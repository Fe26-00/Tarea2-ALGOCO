/*
 * INF-221 Algoritmos y Complejidad, Tarea 2, 2026-2
 * Autor: <nombre>, rol <rol>
 *
 * Fuerza bruta para grid pathfinding con murallas y minas.
 *
 * Decisiones de diseño:
 *  - Búsqueda exhaustiva recursiva desde (1,1): en cada celda se prueba primero
 *    avanzar a la derecha y luego hacia abajo, y se conserva el camino de mayor
 *    puntaje al llegar a (N,M). En empates se mantiene el primero encontrado.
 *  - Única poda permitida por el enunciado: se abandona un camino en cuanto pisa
 *    una muralla o se queda sin vidas (a lo más V-1 minas detonadas). No hay
 *    cotas sobre el puntaje ni memorización.
 *  - El camino parcial se pasa por valor (string) para no tener que deshacer
 *    movimientos al retroceder, a costa de una copia de O(N+M) por llamada.
 *
 * Complejidad: tiempo O(C(N+M-2, N-1) * (N+M)), exponencial en el tamaño de la
 * grilla; memoria O((N+M)^2) en el peor caso (profundidad de recursión N+M-2,
 * con una copia del camino en cada nivel). Por su costo solo se ejecuta cuando
 * N + M <= 32.
 */

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