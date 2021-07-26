//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "ficheros.h"
#define NOFFSETS 5

int main (int argc, char **argv){
    //PROBAR CON TEXTOS QUE OCUPEN MÁS DE UN BLOQUE
    struct  STAT stat;
    unsigned int offset[NOFFSETS]= {0, 5120, 256000, 30720000, 71680000};
    int Long, ninodo, diferentes_inodos=0;

    //Para pasar texto, lo hacemos como argumento haciendo cat de cualquier fichero
    if(argc != 4){
        //printf("Error de sintaxis: escribir <nombre_dispositivo> <$(cat fichero)> <diferentes_inodos> \n");
        printf("Error de sintaxis: escribir <nombre_dispositivo> <texto> <diferentes_inodos> \n"); //para pasar texto como argumento
        //printf("Error de sintaxis: escribir <nombre_dispositivo> <diferentes_inodos> \n"); // para asignar directamente desde el código
        //printf("Error de sintaxis: escribir <nombre_dispositivo> <nombrefichero.txt> <diferentes_inodos> \n"); //para ficheros externos
        exit(EXIT_FAILURE);
    }

    if(bmount(argv[1]) == -1) {
        printf("Error de montaje\n");
        exit(EXIT_FAILURE);
    }

    //Para pasar texto como argumento directamente
    Long=strlen(argv[2]);
    printf("Longitud texto: %i\n", Long);
    char buffer[Long];
    strcpy(buffer, argv[2]);
    diferentes_inodos= atoi(argv[3]);// si vale = se reserva un solo inodo para todos los offsets

    ninodo=reservar_inodo('f', 6);
    for(int i = 0; i<NOFFSETS; i++){
        if(i>0 && diferentes_inodos !=0){
            ninodo=reservar_inodo('f', 6);
        }
        fprintf(stderr, "se ha reservado el inodo nº: %i\n", ninodo);
        fprintf(stderr, "offset: %i\n", offset[i]);

        if(mi_write_f(ninodo, &buffer, offset[i], Long)==-1){
            printf("Error escribir.c escribiendo texto\n");
            if(bumount()== -1) {
                printf("Cerrado incorrecto\n");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }

        if(mi_read_f(ninodo,&buffer,offset[i],Long)==-1){
            printf("Error escribir.c leyendo texto\n");
            if(bumount()== -1) {
                printf("Cerrado incorrecto\n");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }

        mi_stat_f(ninodo, &stat);
        fprintf(stderr, "Tamaño en bytes lógico del inodo %i\n", stat.tamEnBytesLog);
        fprintf(stderr, "Nº de bloques ocupado: %i\n", stat.numBloquesOcupados);
    }

    /*//cat de fichero ??*/

    /*//Asignamos en texto directamente desde el codigo 
    int tamanyo= 25;
    unsigned char buffer[tamanyo];
    strcpy(buffer, "blablablablabla");*/
    
    /*//El texto es un fichero pasado como argumento
    FILE *fp;
	fp = fopen ( argv[3], "rw" );        
	if (fp==NULL) {fputs ("File error",stderr); exit (1);}
	fclose ( fp );*/

    if(bumount()== -1) {
        printf("Cerrado incorrecto\n");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}