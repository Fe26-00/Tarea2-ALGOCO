/*
 * INF-221 Algoritmos y Complejidad, Tarea 2, 2026-2
 * Autor: <nombre>, rol <rol>
 *
 * Programación dinámica (bottom-up) para grid pathfinding con murallas y minas.
 *
 * Decisiones de diseño:
 *  - Estado: dp[i][j][k] = máximo puntaje al llegar a (i,j) habiendo detonado
 *    exactamente k minas, con 0 <= k <= V-1 (con V minas el rover queda sin vidas
 *    y el camino es inválido). Las vidas restantes al final son V - k.
 *  - Transición: dp[i][j][k] = aporte + max(dp[i-1][j][k-d], dp[i][j-1][k-d]),
 *    con d = 1 si la celda es mina (aporte = -P) y d = 0 si es normal
 *    (aporte = g[i][j]). Las murallas no se calculan y los estados inalcanzables
 *    quedan en un valor centinela muy negativo.
 *  - Reconstrucción: par[i][j][k] guarda de dónde vino cada estado ('D' o 'R').
 *    El k anterior se deduce de si la celda actual es mina, por lo que no se
 *    almacena.
 *  - Empates: se prefiere el menor k (más vidas restantes) en la celda final y,
 *    entre predecesores con igual puntaje, el que viene de arriba ('D').
 *
 * Complejidad: tiempo O(N*M*V) y memoria O(N*M*V) (8 bytes de puntaje y 1 byte de
 * predecesor por estado).
 * Asistencia: la formulación del estado y el código base se desarrollaron con
 * ayuda de Claude (Anthropic)
 */

#include "dynamic_programming.h"
#include <algorithm>
#include <climits>

using namespace std;

PathResult solveDP(int n, int m, int v, int p,
                   const std::vector<std::vector<int>>& types,
                   const std::vector<std::vector<long long>>& values) {

	const long long NEG = LLONG_MIN / 4;

	// dp[i][j][k]: mejor puntaje en (i,j) con k minas explotadas
	std::vector<std::vector<std::vector<long long>>> dp(n, std::vector<std::vector<long long>>(m, std::vector<long long>(v, NEG)));
	

	// par[i][j][k]: 'D' si vino de arriba, 'R' si vino de la izquierda
	std::vector<std::vector<std::vector<char>>> par(n, std::vector<std::vector<char>>(m, std::vector<char>(v, 0)));

	// Caso base
	dp[0][0][0] = values[0][0];

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (i == 0 && j == 0) continue;
			if (types[i][j] == 0) continue;  // muralla

			int d = (types[i][j] == -1) ? 1 : 0;
			long long aporte = (d == 1) ? -(long long)p : values[i][j];

			for(int k = d; k < v; k++) {
				int kp = k - d;  // minas usadas antes de entrar a esta celda
				long long best = NEG;
				char from = 0;

				if(i >0 && dp[i-1][j][kp] > NEG) {
					best = dp[i-1][j][kp];
					from = 'D';
				}
				if(j > 0 && dp[i][j-1][kp] > best) {
					best = dp[i][j-1][kp];
					from = 'R';
				}

				if(from != 0) {
					dp[i][j][k] = best + aporte;
					par[i][j][k] = from;
				}
			}
		}
	}

	// Elegir el mejor k en la celda final
	long long bestScore = NEG;
	int bestK = -1;
	for (int k = 0; k < v; k++) {
	    if (dp[n-1][m-1][k] > bestScore) {
	        bestScore = dp[n-1][m-1][k];
	        bestK = k;
	    }
	}

	PathResult res;
	if (bestK == -1) {
	    res.found = false;
	    res.score = 0;
	    res.path = "";
	    res.remaining_lives = 0;
	    return res;
	}

	// Reconstrucción
	std::string path;
	int i = n - 1, j = m - 1, k = bestK;
	while (i != 0 || j != 0) {

	    char c = par[i][j][k];
	    path.push_back(c);

	    if(types[i][j] == -1) k--;  // deshacer la mina de esta celda
	    if(c == 'D') i--;
	    else j--;
	}
	std::reverse(path.begin(), path.end());

	res.found = true;
	res.score = bestScore;
	res.path = path;
	res.remaining_lives = v - bestK;
	return res;



}