//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "ficheros_basico.h"

int main(int argc, char const **argv){
    unsigned char buf[BLOCKSIZE];
    memset(buf, 0, BLOCKSIZE);

    printf("primero\n");
    
    if(argc != 3){
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        printf("No se han especificado los argumentos correctamente\n");
        exit(EXIT_FAILURE);
    }
    
    //montar fichero
    printf("montar\n");
    if(bmount(argv[1]) == -1) exit(EXIT_FAILURE);
    
    int nbloques=atoi(argv[2]); //nºbloques
    int ninodos = nbloques/4;
    
    printf("initSB\n");
    if(initSB(nbloques,ninodos)== -1) exit(EXIT_FAILURE);
    printf("initMB\n");
    if(initMB()== -1) exit(EXIT_FAILURE);
    printf("initAI\n");
    if(initAI()== -1) {
        printf("ERROR \n"); 
        exit(EXIT_FAILURE);
    }
    
    //inicializar los bloques vacios
    printf("inicializar los bloques vacios\n");
    for(int nb = TAMSB + tamMB(nbloques)+tamAI(ninodos); nb < nbloques; nb++){
        if(bwrite(nb, buf) == -1) exit(EXIT_FAILURE);
    }
    printf("reservar inodo\n");
    reservar_inodo ('d', 7); 

    if(bumount()== -1) exit(EXIT_FAILURE);
    printf("fin\n");
    exit(EXIT_SUCCESS);
}
