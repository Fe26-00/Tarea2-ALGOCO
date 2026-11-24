# Documentación

## Grid pathfinding

Algoritmos: Fuerza Bruta, Programación Dinámica, Greedy.

Todos los comandos de esta sección se ejecutan desde `code/grid_pathfinding/`.

**Requisitos:** `g++` con soporte C++17, `make` y Python 3 con `numpy`, `pandas` y
`matplotlib` (`pip install numpy pandas matplotlib`).

**Estructura:**

```
code/grid_pathfinding/
├── grid_pathfinding.cpp          # programa principal
├── algorithms/
│   ├── brute_force.cpp           # solveBruteForce
│   ├── dynamic_programming.cpp   # solveDP
│   └── greedy.cpp                # solveGreedy
├── scripts/
│   ├── grid_generator.py         # generador de casos de prueba
│   └── plot_generator.py         # generador de gráficos
├── data/
│   ├── grid_input/               # grillas de entrada (.txt)
│   ├── grid_output/              # caminos resultantes (.txt)
│   ├── measurements/             # mediciones (.csv)
│   └── plots/                    # gráficos (.png)
└── makefile
```

**Reproducción completa** (casos, ejecución y gráficos):

```
make generate SEED=2026
make run
python3 scripts/plot_generator.py
```

### Programa principal

`grid_pathfinding.cpp` carga cada grilla, ejecuta los algoritmos y guarda los
resultados y las mediciones.

**Compilación:**

```
make            # genera el ejecutable grid_pathfinding
```

(equivale a `g++ -std=c++17 -O2 grid_pathfinding.cpp algorithms/*.cpp -o grid_pathfinding`).

**Ejecución:**

```
make run                                              # todas las grillas de data/grid_input/
./grid_pathfinding                                    # lo mismo, sin pasar por make
./grid_pathfinding data/grid_input/4_4_libre_D10_a.txt   # solo las grillas indicadas
```

El programa debe ejecutarse desde `code/grid_pathfinding/`, porque usa rutas
relativas a `data/`.

**Funcionamiento:**

- Lee `N M V P`, la matriz de tipos (`1` normal, `0` muralla, `-1` mina) y la
  matriz de valores de cada archivo `{n}_{m}_{t}_{d}_{s}.txt`.
- Ejecuta Fuerza Bruta (solo si `N + M <= 32`), Programación Dinámica y Greedy,
  midiendo tiempo de ejecución (`std::chrono::steady_clock`) y memoria adicional
  usada por cada algoritmo.
- Cada algoritmo se ejecuta una vez por instancia.
- Verifica que Fuerza Bruta y DP coincidan en el puntaje en todas las instancias
  donde ambos se ejecutan, y que cada camino sea válido (sin murallas, a lo más
  `V − 1` minas, puntaje y vidas coherentes). Si algo falla, imprime `ERROR` por
  `stderr` y el programa termina con código 1.

**Salidas:**

- `data/grid_output/{n}_{m}_{t}_{d}_{s}_{alg}_out.txt`, con `alg` en
  `bruteforce`, `dp` o `greedy`. Contiene, en tres líneas: el puntaje, el camino
  (`R` = derecha, `D` = abajo) y las vidas restantes al llegar a `(N, M)`. Si el
  algoritmo no encuentra camino, el archivo contiene únicamente `SIN SOLUCION`.
- `data/measurements/grid_measurements.csv`, con una fila por algoritmo e
  instancia y las columnas: `instancia`, `n`, `m`, `tipo`, `dominio`,
  `muestra`, `algoritmo`, `tiempo_ms`, `memoria_kb`, `encontrado`, `puntaje`,
  `vidas_restantes`, `brecha_abs`. La brecha Δ = s(C\*) − s(C_alg) se calcula
  respecto al puntaje de Programación Dinámica (óptimo) y queda vacía si algún
  algoritmo no encontró camino. No se usa razón porcentual, porque los valores
  pueden ser negativos o cero.

El CSV se va acumulando entre ejecuciones. Para empezar de cero:

```
make clean-data     # borra data/grid_output/*_out.txt y el CSV de mediciones
make clean          # borra el ejecutable y los archivos objeto
```

### Scripts

**`scripts/grid_generator.py`**: genera las grillas de prueba en
`data/grid_input/`. Para cada combinación de `N ∈ {4, 8, 16, 64, 256, 1024}`,
`M ∈ {N, 2N}`, tipo de grilla (`libre`, `murallas`, `minas`, `mixta`), dominio
(`D10`, `D1000`) y muestra (`a`, `b`, `c`), crea un archivo
`{n}_{m}_{t}_{d}_{s}.txt`. Usa `V = 3` vidas y `P` igual al límite del dominio.
Reserva un camino monótono libre de murallas y minas, de modo que siempre exista
al menos un camino válido.

```
make generate                       # genera todos los casos
make generate SEED=2026             # resultados reproducibles
make generate SEED=2026 MAXN=16     # solo grillas con N <= 16
python3 scripts/grid_generator.py --seed 2026 --max-n 16   # equivalente directo
```

**`scripts/plot_generator.py`**: lee todos los `.csv` de `data/measurements/`
(si una instancia aparece repetida, conserva la última medición) y guarda en
`data/plots/` las figuras en formato PNG (4 en total):

| Archivo | Contenido |
|---|---|
| `tiempo_vs_tamano.png` | Tiempo de ejecución vs tamaño (`N·M`) por algoritmo y tipo de grilla |
| `memoria_vs_tamano.png` | Memoria adicional vs tamaño por algoritmo y tipo de grilla |
| `brecha_abs_greedy.png` | Brecha absoluta Δ = s(OPT) − s(Greedy), por dominio y tipo de grilla |
| `tasa_falla_greedy.png` | Tasa de falla de Greedy (%) por tipo de grilla y dominio |

La brecha se calcula solo sobre las instancias en que Greedy encuentra solución.
Además, el script imprime en consola la tabla de tasas de falla.

```
python3 scripts/plot_generator.py
```
