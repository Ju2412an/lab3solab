/*
    base_bounds.c — Sección 3 (Traducción de direcciones: Base & Bounds)
    Simula el mecanismo más simple de traducción: cada proceso tiene un
    par (base, bounds) y la VA se traduce con PA = VA + base, válida solo
    si 0 <= VA < bounds. Se incluyen tres procesos (A, B y C) para que
    se vea cómo cada uno tiene su propio espacio aislado, y se prueban
    VAs válidas e inválidas para evidenciar las excepciones.
*/

#include <stdio.h>

// Estructura que agrupa los dos registros del hardware (base y bounds).
typedef struct {
    int base;
    int bounds;
} Registro;

// Prototipos.
int  traducir(Registro r, int va);
void probar_proceso(const char *nombre, Registro r, const int *vas, int n);
int  base_bounds(void);

/*
    main — delega en base_bounds().
*/
int main(void) {
    return base_bounds();
}

/*
    traducir — Calcula la dirección física a partir de la virtual usando
    el par (base, bounds) del proceso. Si la VA está fuera de rango se
    imprime una excepción y se retorna -1.

    En un sistema real esta verificación la hace el hardware (MMU) y al
    fallar se invoca el manejador del SO, que típicamente termina el
    proceso con SIGSEGV (segmentation fault).
*/
int traducir(Registro r, int va) {
    if (va < 0 || va >= r.bounds) {
        printf("  [EXCEPCION] VA=%d viola bounds=%d\n", va, r.bounds);
        return -1;
    }
    return r.base + va;
}

/*
    probar_proceso — Recorre un arreglo de VAs e imprime la traducción
    para el proceso indicado. Se factorizó para no repetir el mismo
    bucle por cada proceso.
*/
void probar_proceso(const char *nombre, Registro r, const int *vas, int n) {
    printf("--- Proceso %s (base=%d, bounds=%d) ---\n",
           nombre, r.base, r.bounds);
    for (int i = 0; i < n; i++) {
        int pa = traducir(r, vas[i]);
        if (pa != -1) {
            printf("  VA=%3d -> PA=%3d\n", vas[i], pa);
        }
    }
}

/*
    base_bounds — Define tres procesos con su propio par (base, bounds)
    y prueba un conjunto fijo de VAs en cada uno. El proceso C se agrega
    en cumplimiento del numeral 2 de la actividad 3.2.
*/
int base_bounds(void) {
    Registro procA = {32,  64};   // base=32,  bounds=64
    Registro procB = {128, 80};   // base=128, bounds=80
    Registro procC = {0,   32};   // base=0,   bounds=32  (numeral 2)

    int vas[] = {0, 10, 63, 64, 100};
    int n = sizeof(vas) / sizeof(vas[0]);

    probar_proceso("A", procA, vas, n);
    probar_proceso("B", procB, vas, n);
    probar_proceso("C", procC, vas, n);
    return 0;
}
