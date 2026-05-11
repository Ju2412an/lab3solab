/*
    heap_demo.c — Sección 2 (API de Memoria)
    Demuestra el uso correcto de malloc/realloc/free. Reserva un arreglo
    de 10 enteros, lo amplía a 20 con realloc y libera al final. Se usa
    como objetivo de Valgrind para verificar que no hay errores ni
    fugas de memoria.
*/

#include <stdio.h>      // printf
#include <stdlib.h>     // malloc, realloc, free

// Prototipos.
int heap_demo(void);

/*
    main — delega en heap_demo() siguiendo la convención del laboratorio.
*/
int main(void) {
    return heap_demo();
}

/*
    heap_demo — Reserva un arreglo dinámico de n enteros, lo llena, lo
    redimensiona con realloc y lo libera. Sigue estrictamente el patrón
    reservar → verificar → usar → liberar y verifica el retorno de
    malloc y realloc antes de cualquier acceso. Tras free se anula el
    puntero para evitar punteros colgantes.
*/
int heap_demo(void) {
    int n = 10;

    // Reservar y verificar.
    int *arr = (int *) malloc(n * sizeof(int));
    if (arr == NULL) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = i * i;
    }
    printf("Arreglo original: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    /*
        Redimensionar a 20 enteros. Importante: si realloc falla devuelve
        NULL pero el bloque original sigue reservado, por lo que se usa
        un puntero auxiliar para no perder la referencia y poder liberar
        antes de salir.
    */
    int *tmp = (int *) realloc(arr, 20 * sizeof(int));
    if (tmp == NULL) {
        perror("realloc");
        free(arr);
        arr = NULL;
        return 1;
    }
    arr = tmp;

    // Inicializar las nuevas posiciones (n .. 19).
    for (int i = n; i < 20; i++) {
        arr[i] = i * i;
    }

    printf("Arreglo ampliado: ");
    for (int i = 0; i < 20; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    arr = NULL;
    return 0;
}
