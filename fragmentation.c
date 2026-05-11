/*
    fragmentation.c — Sección 6.2 (Fragmentación con malloc/glibc)
    Reserva 10 bloques de tamaños variados, libera los de índice par
    para crear huecos en el heap, e intenta luego una asignación grande
    (1500 bytes) que normalmente tiene éxito porque glibc apela al
    coalescing de bloques contiguos y al sbrk si hace falta. El
    objetivo es observar el patrón de direcciones devuelto por malloc
    y discutir la fragmentación a nivel del allocator de usuario.
*/

#include <stdio.h>
#include <stdlib.h>

#define N 10

// Prototipos.
int fragmentation(void);

/*
    main — delega en fragmentation().
*/
int main(void) {
    return fragmentation();
}

/*
    fragmentation — Asigna N bloques de tamaños variados, libera los de
    índice par, intenta una asignación grande y libera el resto. Tras
    cada free se asigna NULL al puntero (regla del curso) para que
    cualquier acceso posterior por error sea fácil de detectar.
*/
int fragmentation(void) {
    void *ptrs[N];
    int sizes[] = {16, 32, 64, 128, 256, 512, 1024, 512, 256, 128};

    // 1) Asignar N bloques de tamaños variados.
    for (int i = 0; i < N; i++) {
        ptrs[i] = malloc(sizes[i]);
        if (ptrs[i] == NULL) {
            // Si malloc falla se libera lo asignado hasta el momento.
            perror("malloc");
            for (int k = 0; k < i; k++) {
                free(ptrs[k]);
                ptrs[k] = NULL;
            }
            return 1;
        }
        printf("malloc(%4d) -> %p\n", sizes[i], ptrs[i]);
    }

    // 2) Liberar los índices pares para crear huecos en el heap.
    printf("\nLiberando bloques en indices pares...\n");
    for (int i = 0; i < N; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }

    // 3) Intentar reservar un bloque grande sobre el heap fragmentado.
    void *big = malloc(1500);
    printf("\nmalloc(1500) -> %p [%s]\n",
           big, big ? "exito" : "FALLO");
    if (big != NULL) {
        free(big);
        big = NULL;
    }

    // 4) Liberar el resto de bloques (índices impares).
    for (int i = 1; i < N; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    return 0;
}
