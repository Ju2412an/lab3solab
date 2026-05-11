#!/usr/bin/env bash
# test_lab3.sh — Pruebas automatizadas para la práctica 3.
# Ejecutar desde el directorio dev/ con `bash test/test_lab3.sh` o `make test`.
# El script compila (si hace falta) y verifica las salidas esperadas
# de cada uno de los siete programas. No depende de Valgrind: las
# pruebas con Valgrind se invocan aparte con `make valgrind`.

set -u
cd "$(dirname "$0")/.."

PASS=0
FAIL=0
FAILED_TESTS=()

verde() { printf "\033[0;32m%s\033[0m" "$1"; }
rojo()  { printf "\033[0;31m%s\033[0m" "$1"; }

check() {
    # check <nombre> <archivo_salida> <patron_grep>
    local nombre="$1" archivo="$2" patron="$3"
    if grep -qE "$patron" "$archivo"; then
        echo "  $(verde 'PASS')  $nombre"
        PASS=$((PASS + 1))
    else
        echo "  $(rojo 'FAIL')  $nombre"
        echo "         patron esperado: $patron"
        FAIL=$((FAIL + 1))
        FAILED_TESTS+=("$nombre")
    fi
}

echo ">>> Compilando todos los binarios..."
# Usa make si está disponible; si no, cae a build_all.sh.
if command -v make >/dev/null 2>&1; then
    make all >/dev/null 2>&1 || { echo "$(rojo 'ERROR'): make all falló"; exit 2; }
else
    bash build_all.sh >/dev/null 2>&1 \
        || { echo "$(rojo 'ERROR'): build_all.sh falló"; exit 2; }
fi

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

# ------------------------------------------------------------------ 1
echo ""
echo "=== 1. mem_map (espacio de direcciones) ==="
# El programa hace getchar(), por lo que se le envía un ENTER por stdin.
echo "" | ./mem_map > "$TMP/mem_map.out"
check "imprime PID"          "$TMP/mem_map.out" "PID del proceso"
check "imprime dir codigo"   "$TMP/mem_map.out" "Dir. codigo \(main\)"
check "imprime dir global"   "$TMP/mem_map.out" "Dir. global_var"
check "imprime dir local"    "$TMP/mem_map.out" "Dir. local_var"
check "imprime dir heap"     "$TMP/mem_map.out" "Dir. heap_var"

# ------------------------------------------------------------------ 2
echo ""
echo "=== 2. heap_demo (malloc/realloc/free) ==="
./heap_demo > "$TMP/heap_demo.out"
check "arreglo original 0..81"    "$TMP/heap_demo.out" "Arreglo original: 0 1 4 9 16 25 36 49 64 81"
check "arreglo ampliado 100..361" "$TMP/heap_demo.out" "Arreglo ampliado:.* 100 121 144 169 196 225 256 289 324 361"

# ------------------------------------------------------------------ 3
echo ""
echo "=== 3. buggy_mem (errores intencionales) ==="
./buggy_mem > "$TMP/buggy_mem.out" 2>&1 || true
check "imprime hola mundo"      "$TMP/buggy_mem.out" "hola mundo"
# El programa imprime "p\[0\] = 0" tras el use-after-free; aceptamos
# cualquier número porque la memoria liberada puede contener basura.
check "intenta leer p[0]"       "$TMP/buggy_mem.out" "p\[0\] = "

# ------------------------------------------------------------------ 4
echo ""
echo "=== 4. buggy_mem_fixed (sin errores) ==="
./buggy_mem_fixed > "$TMP/buggy_mem_fixed.out"
check "imprime hola mundo"      "$TMP/buggy_mem_fixed.out" "hola mundo"
check "imprime p[0] = 0"        "$TMP/buggy_mem_fixed.out" "p\[0\] = 0"

# Si valgrind está disponible, verificamos que la versión arreglada NO reporta fugas.
if command -v valgrind >/dev/null 2>&1; then
    valgrind --leak-check=full --error-exitcode=99 ./buggy_mem_fixed \
        > "$TMP/vg.out" 2>&1
    rc=$?
    if [ $rc -eq 0 ]; then
        echo "  $(verde 'PASS')  valgrind sin errores en buggy_mem_fixed"
        PASS=$((PASS + 1))
    else
        echo "  $(rojo 'FAIL')  valgrind reportó errores en buggy_mem_fixed (rc=$rc)"
        FAIL=$((FAIL + 1))
        FAILED_TESTS+=("buggy_mem_fixed-valgrind")
    fi
else
    echo "  (valgrind no instalado, se omite la verificación)"
fi

# ------------------------------------------------------------------ 5
echo ""
echo "=== 5. base_bounds (Procesos A, B, C) ==="
./base_bounds > "$TMP/bb.out"
check "Proceso A header"     "$TMP/bb.out" "Proceso A \(base=32, bounds=64\)"
check "Proceso B header"     "$TMP/bb.out" "Proceso B \(base=128, bounds=80\)"
check "Proceso C header"     "$TMP/bb.out" "Proceso C \(base=0, bounds=32\)"
check "A: VA=0  -> PA=32"    "$TMP/bb.out" "VA=  0 -> PA= 32"
check "A: VA=10 -> PA=42"    "$TMP/bb.out" "VA= 10 -> PA= 42"
check "A: VA=63 -> PA=95"    "$TMP/bb.out" "VA= 63 -> PA= 95"
check "A: VA=64 EXCEPCION"   "$TMP/bb.out" "EXCEPCION\] VA=64 viola bounds=64"
check "A: VA=100 EXCEPCION"  "$TMP/bb.out" "EXCEPCION\] VA=100 viola bounds=64"
check "B: VA=0  -> PA=128"   "$TMP/bb.out" "VA=  0 -> PA=128"
check "C: VA=10 -> PA=10"    "$TMP/bb.out" "VA= 10 -> PA= 10"
check "C: VA=63 EXCEPCION"   "$TMP/bb.out" "EXCEPCION\] VA=63 viola bounds=32"

# ------------------------------------------------------------------ 6
echo ""
echo "=== 6. paging_sim (traducción VPN -> PFN) ==="
./paging_sim > "$TMP/ps.out"
check "VA=0x00 PA=0x30"      "$TMP/ps.out" "VA=0x00 .* PA=0x30"
check "VA=0x0F PA=0x3F"      "$TMP/ps.out" "VA=0x0F .* PA=0x3F"
check "VA=0x20 PA=0x70"      "$TMP/ps.out" "VA=0x20 .* PA=0x70"
check "VA=0x35 PA=0x25"      "$TMP/ps.out" "VA=0x35 .* PA=0x25"
check "VA=0x10 PAGE FAULT"   "$TMP/ps.out" "VA=0x10 .* PAGE FAULT"
check "VA=0xA3 PA=0x43"      "$TMP/ps.out" "VA=0xA3 .* PA=0x43"
check "VA=0xC8 PA=0x68"      "$TMP/ps.out" "VA=0xC8 .* PA=0x68"
check "VA=0xF0 PAGE FAULT"   "$TMP/ps.out" "VA=0xF0 .* PAGE FAULT"

# ------------------------------------------------------------------ 7
echo ""
echo "=== 7. fragmentation (10 mallocs + huecos + malloc grande) ==="
./fragmentation > "$TMP/fr.out"
n_assign=$(grep -c "^malloc(" "$TMP/fr.out" || true)
if [ "$n_assign" -ge 10 ]; then
    echo "  $(verde 'PASS')  realiza al menos 10 mallocs iniciales"
    PASS=$((PASS + 1))
else
    echo "  $(rojo 'FAIL')  esperaba >=10 mallocs iniciales, encontró $n_assign"
    FAIL=$((FAIL + 1))
    FAILED_TESTS+=("fragmentation-mallocs")
fi
check "intento de malloc(1500)"  "$TMP/fr.out" "malloc\(1500\) -> "

# ------------------------------------------------------------------ 8
echo ""
echo "=== 8. tlb_locality (secuencial vs aleatorio) ==="
./tlb_locality > "$TMP/tl.out"
check "imprime tiempo Secuencial" "$TMP/tl.out" "Secuencial : .* ms"
check "imprime tiempo Aleatorio"  "$TMP/tl.out" "Aleatorio  : .* ms"

# ------------------------------------------------------------------ resumen
echo ""
echo "==================================================="
echo "Resumen: $(verde "$PASS pasaron"), $(rojo "$FAIL fallaron")"
if [ $FAIL -gt 0 ]; then
    echo "Pruebas fallidas:"
    for t in "${FAILED_TESTS[@]}"; do echo "  - $t"; done
    exit 1
fi
exit 0
