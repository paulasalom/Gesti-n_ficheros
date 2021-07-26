//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "ficheros.h"
#define TAMANYO 1500

int main (int argc, char **argv){

    struct STAT stat;
    
    int ninodo, offset, nbytesLeidos, leidos;
    offset=0; leidos=0;
    unsigned char buffer[TAMANYO];
    memset(buffer, 0, TAMANYO);
    char string[128];

     if(argc != 3){
        printf("Error de sintaxis: leer <nombre_dispositivo> <numero de inodo> \n");
        exit(EXIT_FAILURE);
    }
    
    if(bmount(argv[1]) == -1) {
        printf("Error de montaje\n");
        exit(EXIT_FAILURE);
    }

    
    ninodo=atoi(argv[2]);
    while ((nbytesLeidos=mi_read_f(ninodo, buffer, offset, TAMANYO))>0){
        write(1, buffer, nbytesLeidos);
        memset(buffer, 0, TAMANYO);
        leidos= leidos+nbytesLeidos;
        offset=offset+TAMANYO;
    }
    
    sprintf(string, "bytes leídos %d\n", leidos);
    write(2, string, strlen(string));
    
    mi_stat_f(ninodo, &stat);
    fprintf(stderr, "tamEnBytesLog %i\n", stat.tamEnBytesLog);

    if(bumount()== -1) {
        printf("Cerrado incorrecto\n");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}