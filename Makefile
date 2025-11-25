# Makefile para compilar arquivos C++ com FrequencyTable (Linux/Windows)

# Compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Diretórios de headers
INCLUDES = -I. -Iarithmetic -Ippm -Igf

# Arquivos fonte
SOURCES = src/main.cpp \
          src/ReadData.cpp \
          arithmetic/FrequencyTable.cpp \
          arithmetic/BitIoStream.cpp \
          arithmetic/ArithmeticCoder.cpp \
          ppm/PatriciaTree.cpp \
          gf/generalFunctions.cpp

# Objetos compilados
OBJECTS = $(SOURCES:.cpp=.o)

# Nome do executável
TARGET = main

# Regra padrão
all: $(TARGET)

# Compilar o executável
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Regra para compilar objetos
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Executar o programa (com arquivo de teste)
run: $(TARGET)
	./$(TARGET) teste.txt

# Executar com arquivo específico
run-file: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Uso: make run-file FILE=<arquivo>"; \
		echo "Exemplo: make run-file FILE=teste.txt"; \
	else \
		./$(TARGET) $(FILE); \
	fi

# Limpar arquivos gerados
clean:
	rm -f $(TARGET) $(OBJECTS) output.bin
	rm -rf obj/

# Limpar apenas objetos
clean-obj:
	rm -f $(OBJECTS)

# Mostrar ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make              - Compilar o projeto"
	@echo "  make run          - Compilar e executar com teste.txt"
	@echo "  make run-file     - Executar com arquivo específico"
	@echo "  make clean        - Limpar todos os arquivos gerados"
	@echo "  make clean-obj    - Limpar apenas arquivos objeto"
	@echo "  make help         - Mostrar esta ajuda"
	@echo ""
	@echo "Exemplos de uso:"
	@echo "  make run-file FILE=teste.txt"
	@echo "  make run-file FILE=silesia/dickens"

.PHONY: all run run-file clean clean-obj help
