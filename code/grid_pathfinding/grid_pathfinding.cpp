#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

#ifndef PATH_RESULT_H
#define PATH_RESULT_H
struct PathResult {
    bool found;
    long long score;
    std::string path;
    int remaining_lives;
};
#endif

// Firmas de los algoritmos definidos en algorithms/
PathResult solveBruteForce(int n, int m, int v, int p,
                          const std::vector<std::vector<int>>& types,
                          const std::vector<std::vector<long long>>& values);

PathResult solveDP(int n, int m, int v, int p,
                   const std::vector<std::vector<int>>& types,
                   const std::vector<std::vector<long long>>& values);

PathResult solveGreedy(int n, int m, int v, int p,
                       const std::vector<std::vector<int>>& types,
                       const std::vector<std::vector<long long>>& values);

int main(int argc, char* argv[]) {
    // TODO:
    // 1. Leer argumentos por línea de comandos o iterar sobre data/grid_input/.
    // 2. Cargar grilla (N, M, V, P, matriz de tipos, matriz de valores).
    // 3. Ejecutar Brute Force (si N+M <= 32), DP y Greedy midiendo tiempo y memoria.
    // 4. Guardar caminos resultantes en data/grid_output/{n}_{m}_{t}_{d}_{s}_{alg}_out.txt
    // 5. Guardar tiempos, memoria, puntajes y brechas en data/measurements/.
    
    std::cout << "Programa principal de Grid Pathfinding" << std::endl;
    return 0;
}
