/*
    paging_sim.c — Sección 5.2 (Simulador de paginación)
    Simula la traducción VA -> PA mediante una tabla de páginas. Una VA
    de VA_BITS bits se descompone en VPN (bits altos) y offset (bits
    bajos). El offset ocupa PAGE_BITS bits. Si la entrada de la tabla
    es -1 la página no está presente y se reporta un page fault.
*/

#include <stdio.h>

#define PAGE_BITS  4
#define PAGE_SIZE  (1 << PAGE_BITS)              // 16 bytes por página
#define VA_BITS    8                              // VA de 8 bits
#define NUM_PAGES  (1 << (VA_BITS - PAGE_BITS))   // 16 páginas virtuales

// Prototipos.
void traducir(int va);
int  paging_sim(void);

/*
    Tabla de páginas: índice = VPN, valor = PFN. -1 indica que la
    página no está presente (provoca PAGE FAULT). El contenido se
    eligió para que existan VPNs presentes (3, 7, 2, 1, 5, 4, 6, 0)
    y otras ausentes (-1) que generen page faults en el ejercicio.
*/
int page_table[NUM_PAGES] = {
    3, -1, 7, 2, -1, 1, -1, 5,
   -1, -1, 4, -1, 6, -1, 0, -1
};

/*
    main — delega en paging_sim().
*/
int main(void) {
    return paging_sim();
}

/*
    traducir — Recibe una VA de 8 bits, calcula VPN (bits altos) y
    offset (bits bajos), consulta la tabla de páginas y, si la página
    está presente, construye la PA con (PFN << PAGE_BITS) | offset.
    Si la página no está presente reporta page fault.
*/
void traducir(int va) {
    int vpn    = va >> PAGE_BITS;
    int offset = va & (PAGE_SIZE - 1);
    printf("VA=0x%02X  VPN=%2d  Offset=%2d  ", va, vpn, offset);
    if (page_table[vpn] == -1) {
        printf("-> PAGE FAULT (pagina no presente)\n");
    } else {
        int pfn = page_table[vpn];
        int pa  = (pfn << PAGE_BITS) | offset;
        printf("-> PFN=%2d  PA=0x%02X\n", pfn, pa);
    }
}

/*
    paging_sim — Recorre un conjunto de VAs y traduce cada una. Las
    direcciones se eligieron para mostrar tanto traducciones exitosas
    como page faults.
*/
int paging_sim(void) {
    int vas[] = {0x00, 0x0F, 0x20, 0x35, 0x10, 0xA3, 0xC8, 0xF0};
    int n = sizeof(vas) / sizeof(vas[0]);

    printf("%-22s %-6s %-8s %-6s %s\n",
           "VA", "VPN", "Offset", "PFN", "PA");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        traducir(vas[i]);
    }
    return 0;
}
