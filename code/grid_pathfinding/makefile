
# Compilador
CXX = g++

# Opciones de compilacion
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic

# Nombre del ejecutable
TARGET = grid_pathfinding

# Archivos fuente
SOURCES = grid_pathfinding.cpp \
          algorithms/brute_force.cpp \
          algorithms/dynamic_programming.cpp \
          algorithms/greedy.cpp

# Directorio de mediciones
MEASUREMENTS = data/measurements

# Regla principal
all: $(TARGET)

# Compilar todos los archivos
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

# Preparar directorio de mediciones
directories:
	mkdir -p $(MEASUREMENTS)

# Compilar y preparar directorios
build: $(TARGET) directories

# Ejecutar el programa
run: build
	./$(TARGET)

# Eliminar ejecutable y archivos objeto
clean:
	rm -f $(TARGET)
	rm -f algorithms/*.o *.o

# Evitar conflictos con archivos que tengan estos nombres
.PHONY: all build run clean directories

