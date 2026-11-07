#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <string>
#include <vector>

// Estructura del resultado
#ifndef PATH_RESULT_H
#define PATH_RESULT_H
struct PathResult {
    bool found;
    long long score;
    std::string path;
    int remaining_lives;
};
#endif

// Funcion de fuerza bruta
PathResult solveBruteForce(
    int n, int m, int v, int p,
    const std::vector<std::vector<int>>& types,
    const std::vector<std::vector<long long>>& values
);

#endif
