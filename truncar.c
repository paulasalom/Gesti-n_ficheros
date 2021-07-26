//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "ficheros.h"

int main (int argc, char **argv){
    struct STAT stat;
    struct superbloque SB;
    unsigned int ninodo, nbytes=0;
    int numeroBloques;
    printf("Paso 1\n");
    if(argc != 4){
        printf("Error de sintaxis: truncar <nombre_dispositivo> <ninodo> <nbytes> \n");
        exit(EXIT_FAILURE);
    }
    printf("Paso 2\n");
    if(bmount(argv[1]) == -1) {
        printf("Error de montaje\n");
        exit(EXIT_FAILURE);
    }
    printf("Paso 3\n");
    ninodo=atoi(argv[2]);
    nbytes=atoi(argv[3]);    
    if((numeroBloques=mi_truncar_f(ninodo, nbytes))==-1){  // >> liberar_bloques_inodo: fichero vacío al truncar
        printf("Truncado incorrecto\n");
        exit(EXIT_FAILURE);
    }
    //El problema debe estar en liberar_bloques_inodo, pq detecta que el inodo->tamEnBytesLog==0 (trunc)
    printf("Paso 4\n");
    printf("Bloques liberados: %i\n", numeroBloques);
    if(bumount()== -1) {
        printf("Cerrado incorrecto\n");
        exit(EXIT_FAILURE);
    }
    printf("Paso 5\n"); //Hasta aquí funciona!

    mi_stat_f(ninodo, &stat);
    //El numBloquesOcupados del inodo sólo lo debería incrementar la función traducir_bloque_inodo() con reservar=1, y sólo lo debería decrementar la función mi_truncar_f() o liberar_inodo()
    //fprintf(stderr, "tamEnBytesLog: %i (debería ser igual a nbytes (%i))\n", stat.tamEnBytesLog, nbytes);
    fprintf(stderr, "Info: Número de bloques ocupados  %i\n", stat.numBloquesOcupados);
    fflush(stderr);
    
    printf("Paso 6\n");

    if(bread(POSSB, &SB) == -1){
        printf("Error de lectura del superbloque\n");
        bumount();
        exit(EXIT_FAILURE);
    }
    //La cantBloquesLibres del SB sólo la debería decrementar la función reservar_bloque(), y solo lo debería incrementar la función liberar_bloques()
    fprintf(stderr, "cantBloquesLibres %i\n", SB.cantBloquesLibres);

    exit(EXIT_SUCCESS);
}