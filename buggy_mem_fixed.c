/*
    buggy_mem_fixed.c — Sección 2.4 (versión corregida de buggy_mem.c)
    Aplica las tres correcciones que demanda el enunciado:
        1) Buffer overflow → se cambia "<=" por "<" para no escribir
           fuera del arreglo de 5 enteros.
        2) Memory leak → se libera q al terminar de usarlo.
        3) Use-after-free → se imprime p[0] antes de free(p) y, después
           de free, se asigna NULL al puntero para evitar accesos
           accidentales.
    El programa, ejecutado bajo Valgrind, no debe reportar ningún error
    ni fuga ("All heap blocks were freed -- no leaks are possible").
*/

#include <stdio.h>      // printf
#include <stdlib.h>     // malloc, free
#include <string.h>     // strcpy

// Prototipos.
int buggy_mem_fixed(void);

/*
    main — delega en buggy_mem_fixed().
*/
int main(void) {
    return buggy_mem_fixed();
}

/*
    buggy_mem_fixed — Versión sin errores de buggy_mem. Cumple
    estrictamente el patrón reservar → verificar → usar → liberar y
    asigna NULL después de cada free.
*/
int buggy_mem_fixed(void) {
    /* CORRECCIÓN 1: i < 5 (no <=) para no escribir en p[5]. */
    int *p = malloc(5 * sizeof(int));
    if (p == NULL) {
        perror("malloc");
        return 1;
    }
    for (int i = 0; i < 5; i++) {
        p[i] = i;
    }

    /* CORRECCIÓN 2: se reserva, se usa y se libera q antes de salir. */
    char *q = malloc(100);
    if (q == NULL) {
        perror("malloc");
        free(p);
        p = NULL;
        return 1;
    }
    strcpy(q, "hola mundo");
    printf("%s\n", q);
    free(q);
    q = NULL;

    /*
        CORRECCIÓN 3: se accede a p[0] ANTES de liberar p. Después de
        free se asigna NULL al puntero para que cualquier intento
        posterior de uso falle de forma controlada.
    */
    printf("p[0] = %d\n", p[0]);
    free(p);
    p = NULL;

    return 0;
}
