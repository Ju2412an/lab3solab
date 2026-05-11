# Makefile — Práctica 3: Gestión de Memoria
# Compila los siete programas de la guía con las banderas exigidas por
# el curso: -Wall -Wextra -std=c99 (y -g para que Valgrind muestre
# nombres de archivo y líneas en sus reportes).

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -g

# Lista de programas (cada uno es un .c en este mismo directorio).
PROGS   := mem_map heap_demo buggy_mem buggy_mem_fixed \
           base_bounds paging_sim fragmentation tlb_locality

.PHONY: all clean test valgrind

all: $(PROGS)

# Regla genérica: cada programa se construye desde su .c homónimo.
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Banco de pruebas automatizado.
test: all
	bash test/test_lab3.sh

# Atajo para ejecutar Valgrind sobre los dos programas de la sección 2.
valgrind: heap_demo buggy_mem buggy_mem_fixed
	@echo "===== heap_demo ====="
	-valgrind --leak-check=full --track-origins=yes ./heap_demo
	@echo "===== buggy_mem ====="
	-valgrind --leak-check=full --track-origins=yes ./buggy_mem
	@echo "===== buggy_mem_fixed ====="
	-valgrind --leak-check=full --track-origins=yes ./buggy_mem_fixed

clean:
	rm -f $(PROGS) *.o
