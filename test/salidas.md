# Salidas reales capturadas (evidencia)

Salidas obtenidas en Ubuntu 24 / WSL con `gcc 13.3.0` y `Valgrind 3.22.0`.
Estas son las salidas que respaldan las afirmaciones del README.

---

## 1. mem_map — `/proc/[pid]/maps` y `pmap -x`

**Salida del programa** (un PID y direcciones):

```
PID del proceso     : 383
Dir. codigo (main)  : 0x5f4baf77f229     (zona r-x, ejecutable)
Dir. global_var     : 0x5f4baf782010     (zona rw-, segmento .data)
Dir. local_var      : 0x7ffc14fa972c     (zona rw- [stack])
Dir. heap_var       : 0x5f4bd266a2a0     (zona rw- [heap])
```

**`/proc/383/maps`** (regiones relevantes):

```
5f4baf77e000-5f4baf77f000 r--p          ./mem_map     <- .rodata + ELF header
5f4baf77f000-5f4baf780000 r-xp          ./mem_map     <- .text (codigo, ejecutable)
5f4baf780000-5f4baf781000 r--p          ./mem_map
5f4baf781000-5f4baf782000 r--p          ./mem_map
5f4baf782000-5f4baf783000 rw-p          ./mem_map     <- .data (globales)
5f4bd266a000-5f4bd268b000 rw-p          [heap]        <- 132 KB de heap
7f8357800000-7f83579b0000 r--p / r-xp   libc.so.6     <- libc compartida
7f8357b70000-7f8357baa000 r--p / r-xp   ld-linux.so.2 <- loader dinamico
7ffc14f89000-7ffc14faa000 rw-p          [stack]       <- 132 KB de stack
7ffc14fe2000-7ffc14fe6000 r--p          [vvar]
7ffc14fe6000-7ffc14fe8000 r-xp          [vdso]
```

**Mapeo dirección impresa → región:**

| Variable        | Dirección        | Región en /proc/maps                  | Permisos |
| :-------------- | :--------------- | :------------------------------------ | :------- |
| `main`          | 0x5f4baf77f229   | 5f4baf77f000-5f4baf780000 (.text)     | r-xp     |
| `global_var`    | 0x5f4baf782010   | 5f4baf782000-5f4baf783000 (.data)     | rw-p     |
| `heap_var`      | 0x5f4bd266a2a0   | 5f4bd266a000-5f4bd268b000 [heap]      | rw-p     |
| `local_var`     | 0x7ffc14fa972c   | 7ffc14f89000-7ffc14faa000 [stack]     | rw-p     |

> Observación: este kernel WSL no expone `[vsyscall]` (las MMU recientes lo
> reemplazaron por `[vdso]`). Esto se discute en la respuesta a la pregunta 3
> de la sección 1.3.

---

## 2. heap_demo — Valgrind limpio

```
==393== HEAP SUMMARY:
==393==     in use at exit: 0 bytes in 0 blocks
==393==   total heap usage: 3 allocs, 3 frees, 4,216 bytes allocated
==393== All heap blocks were freed -- no leaks are possible
==393== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

---

## 3. buggy_mem — Valgrind detecta los 3 errores

```
==395== Invalid write of size 4
==395==    at 0x109232: buggy_mem (buggy_mem.c:37)        <- ERROR 1: buffer overflow
==395==  Address 0x4a74054 is 0 bytes after a block of size 20 alloc'd

==395== Invalid read of size 4
==395==    at 0x1092A9: buggy_mem (buggy_mem.c:52)        <- ERROR 3: use-after-free
==395==  Address 0x4a74040 is 0 bytes inside a block of size 20 free'd

==395== 100 bytes in 1 blocks are definitely lost           <- ERROR 2: memory leak
==395==    at 0x4846828: malloc
==395==    by 0x109247: buggy_mem (buggy_mem.c:41)

==395== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)
==395== LEAK SUMMARY: definitely lost: 100 bytes in 1 blocks
```

## 4. buggy_mem_fixed — Valgrind limpio

```
==397== HEAP SUMMARY:
==397==     in use at exit: 0 bytes in 0 blocks
==397==   total heap usage: 3 allocs, 3 frees, 4,216 bytes allocated
==397== All heap blocks were freed -- no leaks are possible
==397== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

---

## 5. base_bounds (Procesos A, B y C)

```
--- Proceso A (base=32, bounds=64) ---
  VA=  0 -> PA= 32
  VA= 10 -> PA= 42
  VA= 63 -> PA= 95
  [EXCEPCION] VA=64 viola bounds=64
  [EXCEPCION] VA=100 viola bounds=64
--- Proceso B (base=128, bounds=80) ---
  VA=  0 -> PA=128
  VA= 10 -> PA=138
  VA= 63 -> PA=191
  VA= 64 -> PA=192
  [EXCEPCION] VA=100 viola bounds=80
--- Proceso C (base=0, bounds=32) ---
  VA=  0 -> PA=  0
  VA= 10 -> PA= 10
  [EXCEPCION] VA=63 viola bounds=32
  [EXCEPCION] VA=64 viola bounds=32
  [EXCEPCION] VA=100 viola bounds=32
```

---

## 6. paging_sim

```
VA                     VPN    Offset   PFN    PA
-----------------------------------------------------
VA=0x00  VPN= 0  Offset= 0  -> PFN= 3  PA=0x30
VA=0x0F  VPN= 0  Offset=15  -> PFN= 3  PA=0x3F
VA=0x20  VPN= 2  Offset= 0  -> PFN= 7  PA=0x70
VA=0x35  VPN= 3  Offset= 5  -> PFN= 2  PA=0x25
VA=0x10  VPN= 1  Offset= 0  -> PAGE FAULT (pagina no presente)
VA=0xA3  VPN=10  Offset= 3  -> PFN= 4  PA=0x43
VA=0xC8  VPN=12  Offset= 8  -> PFN= 6  PA=0x68
VA=0xF0  VPN=15  Offset= 0  -> PAGE FAULT (pagina no presente)
```

Page faults observados: VPN=1 y VPN=15 (entradas con valor -1).

---

## 7. fragmentation

```
malloc(  16) -> 0x59960d20d2a0
malloc(  32) -> 0x59960d20e2d0
malloc(  64) -> 0x59960d20e300
malloc( 128) -> 0x59960d20e350
malloc( 256) -> 0x59960d20e3e0
malloc( 512) -> 0x59960d20e4f0
malloc(1024) -> 0x59960d20e700
malloc( 512) -> 0x59960d20eb10
malloc( 256) -> 0x59960d20ed20
malloc( 128) -> 0x59960d20ee30

Liberando bloques en indices pares...

malloc(1500) -> 0x59960d20eec0 [exito]
```

Observaciones:
- Los bloques pequeños (≥32 B) son contiguos en pasos predecibles que reflejan
  el tamaño solicitado más la cabecera del chunk de glibc (típicamente 16 B
  por chunk + alineación a 16).
- El primer bloque (16 B) está separado del resto: glibc lo coloca en un
  *tcache bin* distinto.
- La asignación final de 1500 B tiene **éxito** porque glibc fusionó los
  huecos liberados (coalescing) y/o extendió el heap con `sbrk()`.

---

## 8. tlb_locality (3 corridas, mismo binario y semilla)

```
Secuencial :     9.90 ms (sum=8796090925056)
Aleatorio  :   113.01 ms (sum=8796090925056)

Secuencial :    20.55 ms (sum=8796090925056)
Aleatorio  :   120.80 ms (sum=8796090925056)

Secuencial :    18.76 ms (sum=8796090925056)
Aleatorio  :    82.98 ms (sum=8796090925056)
```

| Corrida   | Secuencial | Aleatorio | Razón Aleatorio/Secuencial |
| :-------- | ---------: | --------: | -------------------------: |
| 1         |   9.90 ms  | 113.01 ms |                  ~11.4×    |
| 2         |  20.55 ms  | 120.80 ms |                   ~5.9×    |
| 3         |  18.76 ms  |  82.98 ms |                   ~4.4×    |
| **Promedio** | **16.4 ms** | **105.6 ms** | **~6.4×**          |

La suma es idéntica en los dos accesos (8 796 090 925 056) porque visitan
exactamente los mismos elementos: solo cambia el **orden** y, por tanto, la
localidad espacial.
