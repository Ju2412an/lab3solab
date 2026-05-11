/*
    mem_map.c — Sección 1 (Espacio de direcciones)
    Programa base que imprime las direcciones virtuales de variables
    ubicadas en distintas regiones del espacio de direcciones (código,
    datos globales, stack y heap). Mientras el proceso espera el ENTER
    se puede inspeccionar /proc/[pid]/maps desde otra terminal.
*/

#include <stdio.h>      // printf, getchar
#include <stdlib.h>     // malloc, free, exit
#include <unistd.h>     // getpid

// Prototipos.
int mem_map(void);

// Variable global → segmento de datos (.data) por estar inicializada.
int global_var = 42;

/*
    main — Punto de entrada. Delega en mem_map() siguiendo la
    convención del laboratorio (main corto que solo invoca la función
    principal del programa).
*/
int main(void) {
    return mem_map();
}

/*
    mem_map — Reserva memoria dinámica para una variable en heap, declara
    una variable local en stack, e imprime las direcciones de cada una
    junto con la dirección de main (segmento de código). Se sigue el
    patrón reservar → verificar → usar → liberar y se asigna NULL al
    puntero después de free para evitar punteros colgantes.
*/
int mem_map(void) {
    // Variable local en el stack (su dirección debe estar en el rango
    // alto del espacio de direcciones, donde el stack crece hacia abajo).
    int local_var = 10;

    // Variable en heap. Se reservan 100 enteros (no solo uno) para que la
    // región se note claramente en /proc/maps al ser un bloque grande.
    int *heap_var = malloc(sizeof(int) * 100);
    // Verificación obligatoria del retorno de malloc (puede ser NULL si
    // el sistema no tiene memoria disponible).
    if (heap_var == NULL) {
        perror("malloc");
        return 1;
    }
    *heap_var = 99;

    // Impresión de las direcciones para comparar con /proc/[pid]/maps.
    printf("PID del proceso     : %d\n", getpid());
    printf("Dir. codigo (main)  : %p\n", (void *)main);
    printf("Dir. global_var     : %p\n", (void *)&global_var);
    printf("Dir. local_var      : %p\n", (void *)&local_var);
    printf("Dir. heap_var       : %p\n", (void *)heap_var);

    printf("\nPresione ENTER para continuar...\n");
    // Pausa que da tiempo a inspeccionar /proc/[pid]/maps en otra terminal.
    getchar();

    // Liberación + NULL para evitar puntero colgante (regla del curso).
    free(heap_var);
    heap_var = NULL;
    return 0;
}
