//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "ficheros.h"

int main (int argc, char **argv){

    unsigned int ninodo=0;
    unsigned char permisos;
     if(argc != 4){
        printf("Error de sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos> \n");
        exit(EXIT_FAILURE);
    }
    
    if(bmount(argv[1]) == -1) {
        printf("Error de montaje\n");
        exit(EXIT_FAILURE);
    }
    
    ninodo=atoi(argv[2]);
    permisos=atoi(argv[3]);//no lo tengo claro
    mi_chmod_f(ninodo, permisos);
    
    if(bumount()== -1) {
        printf("Cerrado incorrecto\n");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);

}