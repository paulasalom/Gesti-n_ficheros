//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "ficheros_basico.h"
#include "bloques.h"

int main (int argc, char **argv){
    struct superbloque SB;
    struct inodo dRaiz;
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    
     if(argc != 2){
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if(bmount(argv[1]) == -1) {
        printf("Error de montaje\n");
        exit(EXIT_FAILURE);
    }
    
    if(bread(POSSB, &SB) == -1){
        printf("Error de lectura del superbloque\n");
        bumount();
        exit(EXIT_FAILURE);
    }
    
    printf("DATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %i\n", SB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %i\n", SB.posUltimoBloqueMB);
    printf("posPrimerBloqueAI = %i\n", SB.posPrimerBloqueAI);
    printf("posUltimoBloqueAI = %i\n", SB.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %i\n", SB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %i\n", SB.posUltimoBloqueDatos);
    printf("posInodoRaiz = %i\n", SB.posInodoRaiz);
    printf("posPrimerInodoLibre = %i\n", SB.posPrimerInodoLibre);
    printf("cantBloquesLibres = %i\n", SB.cantBloquesLibres);
    printf("cantInodosLibres = %i\n", SB.cantInodosLibres);
    printf("totBloques = %i\n", SB.totBloques);
    printf("totInodos = %i\n", SB.totInodos);
    //printf("sizeof struct Superbloque: %li\n", sizeof(struct superbloque));
    //printf("sizeof struct inodo: %li\n", sizeof(struct inodo));

    if(bread(POSSB, &SB) == -1){
        printf("Error de lectura del superbloque\n");
        bumount();
        exit(EXIT_FAILURE);
    }
    printf("\n");
    printf("RESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
    int n = reservar_bloque();
    if(bread(POSSB, &SB) == -1){
        printf("Error de lectura del superbloque\n");
        bumount();
        exit(EXIT_FAILURE);
    }
    printf("se ha reservado el bloque físico número %i\n", n);    
    printf("cantBloquesLibres = %i\n", SB.cantBloquesLibres); 
    liberar_bloque(n);
    if(bread(POSSB, &SB) == -1){
        printf("Error de lectura del superbloque\n");
        bumount();
        exit(EXIT_FAILURE);
    }
    printf("cantBloquesLibres = %i\n", SB.cantBloquesLibres);   
    printf("cantInodosLibres = %i\n", SB.cantInodosLibres);   
    printf("\n");
    printf("MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
    leer_bit(SB.posInodoRaiz);
    leer_bit(SB.posPrimerInodoLibre);
    leer_bit(SB.posUltimoBloqueMB);
    leer_bit(SB.posPrimerBloqueAI);
    leer_bit(SB.posUltimoBloqueAI);
    leer_bit(SB.posPrimerBloqueDatos);
    leer_bit(SB.posUltimoBloqueDatos);
    printf("\n");
    printf("DATOS DEL DIRECTORIO RAIZ\n");
    
    if(leer_inodo(0,&dRaiz)==-1){
        printf("Lectura incorrecta del inodo raiz\n");
        exit(EXIT_FAILURE);
    }
    
    printf("tipo: %d\n", dRaiz.tipo);
    printf("permisos: %i\n", dRaiz.permisos);
    ts= localtime(&dRaiz.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts= localtime(&dRaiz.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts= localtime(&dRaiz.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);
    printf("nlinks: %i\n", dRaiz.nlinks);
    printf("tamEnBytesLog: %i\n", dRaiz.tamEnBytesLog);
    printf("numBloquesOcupados: %i\n", dRaiz.numBloquesOcupados);

    if(bumount()== -1) {
        printf("Cerrado incorrecto\n");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);

}
