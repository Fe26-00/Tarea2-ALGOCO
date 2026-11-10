#include "greedy.h"
#include <limits>
#include <algorithm>

using namespace std;

// función Greedy principal
PathResult solveGreedy(int n, int m, int v, int p,
                       const std::vector<std::vector<int>>& types,
                       const std::vector<std::vector<long long>>& values){

	// Lo que vamos a devolver
    PathResult mejor_resultado;

    mejor_resultado.found = false;
    mejor_resultado.score = 0;
    mejor_resultado.path = "";
    mejor_resultado.remaining_lives = v;

    // Verificamos que las dimensiones sean validas
    if (n <= 0 || m <= 0) {
        return mejor_resultado;
    }

    int i = 0;
    int j = 0;
    while(i<n-1 || j<m-1) { // devoramos jeje
        long long vali = numeric_limits<long long>::lowest();
        long long valj = numeric_limits<long long>::lowest();
        bool minai = false;
        bool minaj = false;
    	// revisar celdas contiguas
    	// Primero revisamos con i+1
        if(i + 1 < n) {
        	if(types[i+1][j] == 1) { // celda transitable
                vali = values[i+1][j];
        	} else if(types[i+1][j] == -1) { // mina
                vali = -p;
                minai = true;
                if(mejor_resultado.remaining_lives == 1) {
                    vali = numeric_limits<long long>::lowest();
                }
            } else { // muralla
                vali = numeric_limits<long long>::lowest();
            }
        }

        // Luego para revisamos con j+1

        if(j + 1 < m) {
            if(types[i][j+1] == 1) { // celda transitable
                valj = values[i][j+1];
            } else if(types[i][j+1] == -1) { // mina
                valj = -p;
                minaj = true;
                if(mejor_resultado.remaining_lives == 1) {
                    valj = numeric_limits<long long>::lowest();
                }
            } else { // muralla
                valj = numeric_limits<long long>::lowest();
            }
        }

        // Ahora comparamos y elejimos el mejor
        long long best = max(vali, valj);
        if(best == numeric_limits<long long>::lowest()) { // no hay donde ir
            return mejor_resultado; // no se encontró
        }

        if(vali > valj) { // mejor ir pa abajo
            mejor_resultado.score += vali;
            mejor_resultado.path += "D";
            if(minai) mejor_resultado.remaining_lives--;
            i++;
        } else { // mejor ir pa la derecha (nooooo XD)
            mejor_resultado.score += valj;
            mejor_resultado.path += "R";
            if(minaj) mejor_resultado.remaining_lives--;
            j++;
        }

    }

    // si llego vivo hasta aquí debe haber encontrado una sol.
    mejor_resultado.found = true;


    return mejor_resultado;

}