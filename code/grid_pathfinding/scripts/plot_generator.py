"""
Script para generar gráficos a partir de las mediciones experimentales.
Lee los resultados desde data/measurements/ y guarda los gráficos en data/plots/.

Métricas a graficar:
  - Tiempo de ejecución vs tamaño (N * M) para cada algoritmo y tipo de grilla.
  - Uso de memoria vs tamaño para cada algoritmo.
  - Brecha de puntaje Delta = s(OPT) - s(Greedy) para casos donde greedy encuentra solución.
  - Tasa de falla de Greedy (% de casos con 'SIN SOLUCION') según tipo de grilla y dominio.
"""

import os
import matplotlib.pyplot as plt
import pandas as pd

MEASUREMENTS_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "measurements")
PLOTS_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "plots")

# --- Configuración de los gráficos ---
TIPOS = ["libre", "murallas", "minas", "mixta"]
DOMINIOS = ["D10", "D1000"]
NOMBRES_ALG = {"bf": "Fuerza bruta", "dp": "Prog. dinámica", "greedy": "Greedy"}
COLUMNAS_NUMERICAS = ["n", "m", "tiempo_ms", "memoria_kb", "encontrado",
                      "puntaje", "vidas_restantes", "brecha_abs", "brecha_pct"]


def cargar_mediciones():
    """
    Lee todos los CSV de data/measurements/ y retorna un DataFrame limpio
    con una columna adicional 'tamano' = N * M.
    """
    archivos = sorted(
        os.path.join(MEASUREMENTS_DIR, f)
        for f in os.listdir(MEASUREMENTS_DIR)
        if f.endswith(".csv")
    )
    if not archivos:
        raise FileNotFoundError(f"No hay archivos .csv en {MEASUREMENTS_DIR}")

    df = pd.concat([pd.read_csv(a) for a in archivos], ignore_index=True)
    for col in COLUMNAS_NUMERICAS:
        df[col] = pd.to_numeric(df[col], errors="coerce")

    # Si se corrió el programa más de una vez, se conserva la última medición.
    df = df.drop_duplicates(subset=["instancia", "algoritmo"], keep="last")
    df["tamano"] = df["n"] * df["m"]
    return df


def guardar(fig, nombre):
    """Guarda la figura como PNG en data/plots/ y la cierra."""
    ruta = os.path.join(PLOTS_DIR, nombre)
    fig.tight_layout()
    fig.savefig(ruta, dpi=150)
    plt.close(fig)
    print("Guardado:", os.path.normpath(ruta))


def grafico_por_tipo(df, columna, ylabel, titulo, nombre_archivo):
    """
    Una figura 2x2 (un panel por tipo de grilla) con una curva por algoritmo.
    Eje X: tamaño N*M. Eje Y: promedio de 'columna' sobre dominios y muestras.
    Ambos ejes en escala logarítmica.
    """
    fig, axes = plt.subplots(2, 2, figsize=(12, 9), sharex=True)
    for ax, tipo in zip(axes.flat, TIPOS):
        sub = df[df["tipo"] == tipo]
        for alg, g in sub.groupby("algoritmo"):
            serie = g.groupby("tamano")[columna].mean()
            ax.plot(serie.index, serie.values, marker="o",
                    label=NOMBRES_ALG.get(alg, alg))
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_title(f"Grilla {tipo}")
        ax.set_xlabel("Tamaño (N · M)")
        ax.set_ylabel(ylabel)
        ax.grid(True, which="both", alpha=0.3)
        if ax.get_legend_handles_labels()[0]:
            ax.legend()
    fig.suptitle(titulo)
    guardar(fig, nombre_archivo)


def grafico_tiempo(df):
    grafico_por_tipo(df, "tiempo_ms", "Tiempo promedio (ms)",
                     "Tiempo de ejecución vs tamaño", "tiempo_vs_tamano.png")


def grafico_memoria(df):
    grafico_por_tipo(df, "memoria_kb", "Memoria adicional promedio (kB)",
                     "Uso de memoria vs tamaño", "memoria_vs_tamano.png")


def grafico_brecha(df, columna, ylabel, titulo, nombre_archivo):
    """
    Brecha de Greedy respecto al óptimo (DP), solo en instancias donde Greedy
    encontró solución. Un panel por dominio y una curva por tipo de grilla.
    """
    g = df[(df["algoritmo"] == "greedy") & (df["encontrado"] == 1)]
    g = g.dropna(subset=[columna])

    fig, axes = plt.subplots(1, 2, figsize=(13, 5), sharex=True)
    for ax, dom in zip(axes, DOMINIOS):
        for tipo in TIPOS:
            sub = g[(g["dominio"] == dom) & (g["tipo"] == tipo)]
            if sub.empty:
                continue
            serie = sub.groupby("tamano")[columna].mean()
            ax.plot(serie.index, serie.values, marker="o", label=tipo)
        ax.set_xscale("log")
        ax.set_title(f"Dominio {dom}")
        ax.set_xlabel("Tamaño (N · M)")
        ax.set_ylabel(ylabel)
        ax.grid(True, which="both", alpha=0.3)
        if ax.get_legend_handles_labels()[0]:
            ax.legend(title="Tipo de grilla")
    fig.suptitle(titulo)
    guardar(fig, nombre_archivo)


def grafico_tasa_falla(df):
    """
    Tasa de falla de Greedy: % de instancias donde no encontró solución
    ('SIN SOLUCION'), agrupado por tipo de grilla y dominio.
    """
    g = df[df["algoritmo"] == "greedy"]
    tasa = (1 - g.groupby(["tipo", "dominio"])["encontrado"].mean()) * 100
    tabla = tasa.unstack("dominio").reindex(index=TIPOS, columns=DOMINIOS)

    fig, ax = plt.subplots(figsize=(8, 5))
    tabla.plot(kind="bar", ax=ax, rot=0)
    ax.set_xlabel("Tipo de grilla")
    ax.set_ylabel("Tasa de falla (%)")
    ax.set_title("Tasa de falla de Greedy")
    ax.set_ylim(0, 100)
    ax.grid(True, axis="y", alpha=0.3)
    ax.legend(title="Dominio")
    guardar(fig, "tasa_falla_greedy.png")

    print("\nTasa de falla de Greedy (%):")
    print(tabla.round(1).to_string())


def main():
    os.makedirs(PLOTS_DIR, exist_ok=True)
    df = cargar_mediciones()
    print(f"Mediciones cargadas: {len(df)} filas desde {os.path.normpath(MEASUREMENTS_DIR)}")

    grafico_tiempo(df)
    grafico_memoria(df)
    grafico_brecha(df, "brecha_abs", "Brecha Δ = s(OPT) − s(Greedy)",
                   "Brecha absoluta de Greedy respecto al óptimo",
                   "brecha_abs_greedy.png")
    grafico_brecha(df, "brecha_pct", "Brecha relativa (%)",
                   "Brecha relativa de Greedy respecto al óptimo",
                   "brecha_pct_greedy.png")
    grafico_tasa_falla(df)

    print("Figuras guardadas en:", os.path.normpath(PLOTS_DIR))

if __name__ == "__main__":
    main()