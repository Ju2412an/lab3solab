/*
    tlb_locality.c — Sección 7 (TLB y localidad)
    Compara el costo de recorrer un arreglo de 16 MB de forma
    secuencial (alta localidad espacial) frente a un recorrido en orden
    aleatorio (Fisher-Yates). El acceso aleatorio salta entre páginas y
    fuerza muchos TLB miss, lo que hace que la CPU vaya a la tabla de
    páginas (en memoria) con frecuencia y el tiempo se dispare. El
    recorrido secuencial reusa la misma traducción mientras avanza por
    los 1024 enteros de cada página de 4 KB → muchos TLB hit.
*/

#define _POSIX_C_SOURCE 200809L  // habilita CLOCK_MONOTONIC y clock_gettime

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N (1 << 22)   // 4M enteros = 16 MB

// Prototipos.
double ms(struct timespec a, struct timespec b);
int    tlb_locality(void);

/*
    main — delega en tlb_locality().
*/
int main(void) {
    return tlb_locality();
}

/*
    ms — Calcula la diferencia entre dos timespec en milisegundos.
    Se factorizó para usarla en los dos benchmarks.
*/
double ms(struct timespec a, struct timespec b) {
    return (b.tv_sec  - a.tv_sec ) * 1000.0
         + (b.tv_nsec - a.tv_nsec) / 1e6;
}

/*
    tlb_locality — Reserva un arreglo de N enteros, lo recorre primero
    secuencialmente y luego en orden aleatorio, midiendo en cada caso
    el tiempo de la suma de todos los elementos. Sigue el patrón
    reservar → verificar → usar → liberar y anula los punteros tras
    free.
*/
int tlb_locality(void) {
    int *arr = (int *) malloc((size_t) N * sizeof(int));
    if (arr == NULL) {
        perror("malloc arr");
        return 1;
    }
    for (int i = 0; i < N; i++) {
        arr[i] = i;
    }

    struct timespec t0, t1;
    long sum = 0;

    // -------- Acceso SECUENCIAL (alta localidad espacial) --------
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N; i++) {
        sum += arr[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("Secuencial : %8.2f ms (sum=%ld)\n", ms(t0, t1), sum);

    // -------- Acceso ALEATORIO (baja localidad) --------
    int *idx = (int *) malloc((size_t) N * sizeof(int));
    if (idx == NULL) {
        perror("malloc idx");
        free(arr);
        arr = NULL;
        return 1;
    }
    for (int i = 0; i < N; i++) {
        idx[i] = i;
    }
    // Fisher-Yates con semilla fija para reproducibilidad.
    srand(42);
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t  = idx[i];
        idx[i] = idx[j];
        idx[j] = t;
    }

    sum = 0;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N; i++) {
        sum += arr[idx[i]];
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("Aleatorio  : %8.2f ms (sum=%ld)\n", ms(t0, t1), sum);

    free(arr);
    arr = NULL;
    free(idx);
    idx = NULL;
    return 0;
}
