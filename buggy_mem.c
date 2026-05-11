/*
    buggy_mem.c — Sección 2.3 (errores intencionales de memoria)
    NO ejecutar sin Valgrind. Contiene tres errores clásicos:
        1) buffer overflow (escribe en p[5] cuando solo hay 5 enteros).
        2) memory leak (nunca se hace free(q)).
        3) use-after-free (lee p[0] tras free(p)).
    El programa se conserva igual al del enunciado para que Valgrind
    los detecte y reporte. La versión corregida está en buggy_mem_fixed.c.
*/

#include <stdio.h>      // printf
#include <stdlib.h>     // malloc, free
#include <string.h>     // strcpy

// Prototipos.
int buggy_mem(void);

/*
    main — delega en buggy_mem() siguiendo la convención del laboratorio.
*/
int main(void) {
    return buggy_mem();
}

/*
    buggy_mem — Reproduce los tres errores clásicos de memoria. Se deja
    intencionalmente sin correcciones para mostrar la salida de Valgrind.
*/
int buggy_mem(void) {
    /* ERROR 1: buffer overflow */
    int *p = malloc(5 * sizeof(int));
    if (p == NULL) {
        perror("malloc");
        return 1;
    }
    for (int i = 0; i <= 5; i++) {  // <= en vez de <  → escribe p[5]
        p[i] = i;
    }

    /* ERROR 2: memory leak (nunca se llama free(q)) */
    char *q = malloc(100);
    if (q == NULL) {
        perror("malloc");
        free(p);
        return 1;
    }
    strcpy(q, "hola mundo");
    printf("%s\n", q);

    /* ERROR 3: use-after-free */
    free(p);
    printf("p[0] = %d\n", p[0]);  // acceso ilegal: p ya fue liberado.

    return 0;
}
