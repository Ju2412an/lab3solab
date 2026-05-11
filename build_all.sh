#!/usr/bin/env bash
# Build helper para compilar los siete programas de la práctica 3
# directamente con gcc cuando `make` no está instalado.
set -e
cd "$(dirname "$0")"
for p in mem_map heap_demo buggy_mem buggy_mem_fixed \
         base_bounds paging_sim fragmentation tlb_locality; do
    echo "==> $p"
    gcc -Wall -Wextra -std=c99 -g -o "$p" "$p.c"
done
echo "OK: 8 binarios compilados (incluye buggy_mem_fixed)"
