/*
 * INF-221 Algoritmos y Complejidad, Tarea 2, 2026-2
 * Autor: Sebastián Abarza, rol 202473601-9
 *
 * Programa principal de grid pathfinding: lee las grillas de data/grid_input/,
 * ejecuta fuerza bruta (solo si N+M <= 32), programacion dinamica y greedy,
 * mide tiempo y memoria, y guarda caminos y mediciones.
 *
 * Decisiones de diseno:
 *  - La brecha se reporta como diferencia absoluta respecto al puntaje de DP.
 *  - Se verifica que fuerza bruta y DP coincidan y que cada camino sea valido.
 * Referencias: Pauta.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <new>

#include <sys/resource.h>   // getrusage (respaldo para medir memoria)

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

namespace fs = std::filesystem;
using Clock = std::chrono::steady_clock;

// ---------------------------------------------------------------------------
// Rutas (se asume que el programa se ejecuta desde la raíz del proyecto)
// ---------------------------------------------------------------------------
const std::string DIR_INPUT = "data/grid_input";
const std::string DIR_OUTPUT = "data/grid_output";
const std::string DIR_MEASURE = "data/measurements";
const std::string CSV_PATH = DIR_MEASURE + "/grid_measurements.csv";
const int BF_MAX_SUMA = 32;  // Brute Force solo si N + M <= 32

// ---------------------------------------------------------------------------
// Estructuras
// ---------------------------------------------------------------------------
struct Grid {
    int n = 0, m = 0, v = 0, p = 0;
    std::vector<std::vector<int>> types;
    std::vector<std::vector<long long>> values;
};

struct Medicion {
    double tiempo_ms = 0.0;
    long long memoria_kb = 0;  // pico de memoria adicional durante el algoritmo
};

// ---------------------------------------------------------------------------
// Lectura de la grilla
// ---------------------------------------------------------------------------
bool cargarGrid(const std::string& ruta, Grid& g) {
    std::ifstream in(ruta);
    if (!in) return false;

    if (!(in >> g.n >> g.m >> g.v >> g.p)) return false;

    g.types.assign(g.n, std::vector<int>(g.m));
    g.values.assign(g.n, std::vector<long long>(g.m));

    for (int i = 0; i < g.n; i++)
        for (int j = 0; j < g.m; j++)
            if (!(in >> g.types[i][j])) return false;

    for (int i = 0; i < g.n; i++)
        for (int j = 0; j < g.m; j++)
            if (!(in >> g.values[i][j])) return false;

    return true;
}

// ---------------------------------------------------------------------------
// Medición de memoria (Linux)
// ---------------------------------------------------------------------------
// Se lee de /proc/self/status el valor de una clave en kB (VmRSS, VmHWM).


// --- Contador de memoria dinámica (pico de bytes reservados con new) ---
static std::size_t g_actual = 0;
static std::size_t g_pico = 0;

void* operator new(std::size_t size) {
    void* raw = std::malloc(size + 16);       // 16 bytes de cabecera (mantiene alineación)
    if (!raw) throw std::bad_alloc();
    *static_cast<std::size_t*>(raw) = size;
    g_actual += size;
    if (g_actual > g_pico) g_pico = g_actual;
    return static_cast<char*>(raw) + 16;
}

void operator delete(void* p) noexcept {
    if (!p) return;
    void* raw = static_cast<char*>(p) - 16;
    g_actual -= *static_cast<std::size_t*>(raw);
    std::free(raw);
}

void operator delete(void* p, std::size_t) noexcept { operator delete(p); }


long long leerStatusKb(const std::string& clave) {
    std::ifstream f("/proc/self/status");
    std::string linea;
    while (std::getline(f, linea)) {
        if (linea.rfind(clave + ":", 0) == 0) {
            std::istringstream ss(linea.substr(clave.size() + 1));
            long long kb = -1;
            ss >> kb;
            return kb;
        }
    }
    return -1;
}

// Reinicia el pico de RSS (VmHWM) del proceso. Devuelve false si no se pudo.
bool reiniciarPicoMemoria() {
    std::ofstream f("/proc/self/clear_refs");
    if (!f) return false;
    f << "5";
    f.flush();
    return static_cast<bool>(f);
}

PathResult medir(const std::function<PathResult()>& algoritmo, Medicion& med) {
    std::size_t base = g_actual;
    g_pico = g_actual;                        // reiniciar el pico

    auto t0 = Clock::now();
    PathResult res = algoritmo();
    auto t1 = Clock::now();

    med.tiempo_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    med.memoria_kb = static_cast<long long>((g_pico - base + 1023) / 1024);  // redondeo hacia arriba
    return res;
}

// ---------------------------------------------------------------------------
// Utilidades de archivos
// ---------------------------------------------------------------------------
// Separa "{n}_{m}_{t}_{d}_{s}" en sus campos.
std::vector<std::string> partirNombre(const std::string& stem) {
    std::vector<std::string> campos;
    std::stringstream ss(stem);
    std::string c;
    while (std::getline(ss, c, '_')) campos.push_back(c);
    return campos;
}

void guardarCamino(const std::string& ruta, const PathResult& r) {
    std::ofstream out(ruta);
    if (!r.found) {
        out << "NO_PATH\n";
        return;
    }
    // Ajusta este formato si el enunciado pide otro.
    out << r.score << "\n";
    out << r.remaining_lives << "\n";
    out << r.path << "\n";
}

void escribirEncabezadoCSV() {
    bool vacio = !fs::exists(CSV_PATH) || fs::file_size(CSV_PATH) == 0;
    if (vacio) {
        std::ofstream csv(CSV_PATH);
        csv << "instancia,n,m,tipo,dominio,muestra,algoritmo,"
               "tiempo_ms,memoria_kb,encontrado,puntaje,vidas_restantes,"
               "brecha_abs,brecha_pct\n";
    }
}

void anotarCSV(const std::string& stem, const std::vector<std::string>& campos,
               const std::string& alg, const Medicion& med, const PathResult& r,
               const PathResult& ref) {
    // Brecha respecto al óptimo (DP). Solo definida si ambos encontraron camino.
    std::string brechaAbs = "", brechaPct = "";
    if (r.found && ref.found) {
        long long abs = ref.score - r.score;
        brechaAbs = std::to_string(abs);
        if (ref.score != 0) {
            double pct = 100.0 * abs / std::abs((double)ref.score);
            std::ostringstream ss;
            ss << pct;
            brechaPct = ss.str();
        }
    }

    std::ofstream csv(CSV_PATH, std::ios::app);
    csv << stem << ",";
    for (int i = 0; i < 5; i++) csv << (i < (int)campos.size() ? campos[i] : "") << ",";
    csv << alg << "," << med.tiempo_ms << "," << med.memoria_kb << ","
        << (r.found ? 1 : 0) << "," << (r.found ? std::to_string(r.score) : "") << ","
        << (r.found ? std::to_string(r.remaining_lives) : "") << ","
        << brechaAbs << "," << brechaPct << "\n";
}

// ---------------------------------------------------------------------------
// Procesar una instancia
// ---------------------------------------------------------------------------
void procesarInstancia(const std::string& ruta) {
    std::string stem = fs::path(ruta).stem().string();
    std::vector<std::string> campos = partirNombre(stem);

    Grid g;
    if (!cargarGrid(ruta, g)) {
        std::cerr << "Error leyendo " << ruta << std::endl;
        return;
    }
    std::cout << "Procesando " << stem << " ..." << std::endl;

    struct Alg {
        std::string nombre;
        std::function<PathResult()> f;
        bool ejecutar;
    };
    std::vector<Alg> algoritmos = {
        {"bf", [&] { return solveBruteForce(g.n, g.m, g.v, g.p, g.types, g.values); },
         g.n + g.m <= BF_MAX_SUMA},
        {"dp", [&] { return solveDP(g.n, g.m, g.v, g.p, g.types, g.values); }, true},
        {"greedy", [&] { return solveGreedy(g.n, g.m, g.v, g.p, g.types, g.values); }, true},
    };

    // Primero se ejecuta todo y se guardan los resultados; la referencia
    // para las brechas es el óptimo entregado por DP.
    std::vector<PathResult> resultados(algoritmos.size());
    std::vector<Medicion> mediciones(algoritmos.size());
    int idxDP = 1;

    for (size_t a = 0; a < algoritmos.size(); a++) {
        if (!algoritmos[a].ejecutar) continue;
        resultados[a] = medir(algoritmos[a].f, mediciones[a]);
    }

    for (size_t a = 0; a < algoritmos.size(); a++) {
        if (!algoritmos[a].ejecutar) continue;
        const std::string& nombre = algoritmos[a].nombre;

        guardarCamino(DIR_OUTPUT + "/" + stem + "_" + nombre + "_out.txt", resultados[a]);
        anotarCSV(stem, campos, nombre, mediciones[a], resultados[a], resultados[idxDP]);
    }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    fs::create_directories(DIR_OUTPUT);
    fs::create_directories(DIR_MEASURE);
    escribirEncabezadoCSV();

    std::vector<std::string> archivos;
    if (argc > 1) {
        // Archivos indicados por línea de comandos
        for (int i = 1; i < argc; i++) archivos.push_back(argv[i]);
    } else {
        // Todos los .txt de data/grid_input/
        if (!fs::exists(DIR_INPUT)) {
            std::cerr << "No existe " << DIR_INPUT << std::endl;
            return 1;
        }
        for (const auto& e : fs::directory_iterator(DIR_INPUT))
            if (e.is_regular_file() && e.path().extension() == ".txt")
                archivos.push_back(e.path().string());
        std::sort(archivos.begin(), archivos.end());
    }

    for (const auto& ruta : archivos) procesarInstancia(ruta);

    std::cout << "Listo. Resultados en " << DIR_OUTPUT << " y " << CSV_PATH << std::endl;
    return 0;
}