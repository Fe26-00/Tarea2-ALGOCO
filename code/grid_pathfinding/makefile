# Compilador
CXX = g++

# Opciones de compilacion
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic

# Nombre del ejecutable
TARGET = grid_pathfinding

# Archivos fuente y objetos
SOURCES = grid_pathfinding.cpp \
          algorithms/brute_force.cpp \
          algorithms/dynamic_programming.cpp \
          algorithms/greedy.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = $(wildcard algorithms/*.h)

# Directorios de datos
INPUT_DIR    = data/grid_input
OUTPUT_DIR   = data/grid_output
MEASUREMENTS = data/measurements

# Generador de casos (ajusta la ruta a donde esté tu script)
GENERATOR = scripts/grid_generator.py
PYTHON    = python3

# Regla principal
all: $(TARGET)

# Enlazar
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Compilar cada .cpp a .o (se recompila si cambia algun header)
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Crear directorios de datos
directories:
	mkdir -p $(INPUT_DIR) $(OUTPUT_DIR) $(MEASUREMENTS)

# Compilar y preparar directorios
build: $(TARGET) directories

# Generar los casos de prueba (ej: make generate SEED=2026 MAXN=16)
generate: directories
	$(PYTHON) $(GENERATOR) \
		$(if $(SEED),--seed $(SEED)) \
		$(if $(MAXN),--max-n $(MAXN))

# Ejecutar el programa sobre todos los inputs
run: build
	./$(TARGET)

# Eliminar ejecutable y objetos
clean:
	rm -f $(TARGET) $(OBJECTS)

# Eliminar resultados y mediciones (el CSV acumula filas entre corridas)
clean-data:
	rm -f $(OUTPUT_DIR)/*_out.txt
	rm -f $(MEASUREMENTS)/grid_measurements.csv

.PHONY: all build run clean clean-data directories generate