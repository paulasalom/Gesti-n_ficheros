//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "bloques.h"

static int descriptor = 0;

int bmount(const char *camino){

    if (descriptor > 0) close(descriptor);
    umask(000);  
    descriptor = open( camino, O_RDWR|O_CREAT, 0666);
    
    if (descriptor == -1){
        printf("Error bmount al abrir fichero");
        return -1;
    }
    else return descriptor;
}

int bumount(){
    descriptor=close(descriptor);
    if (descriptor == -1){
        printf("Error bmount al cerrar fichero");
        return -1;
    }
    else return 0;
}

int bread(unsigned int nbloque, void *buf){
    off_t desplazamiento = nbloque * BLOCKSIZE;    
    if(lseek(descriptor, desplazamiento, SEEK_SET)==-1){
        printf("Error bread en desplazamiento: %s\n", strerror(errno));
        return -1;
    }
    size_t size = read(descriptor, buf, BLOCKSIZE);
    if(size==-1){
        printf("Error bread al leer bloque");
        return -1;
    }
    return size;
}

int bwrite(unsigned int nbloque, const void *buf){
    off_t desplazamiento = nbloque * BLOCKSIZE;
    if(lseek(descriptor, desplazamiento, SEEK_SET)==-1){
        printf("Error bwrite en desplazamiento: %s\n", strerror(errno));
        return -1;
    }
    size_t size = write(descriptor, buf, BLOCKSIZE);
        if(size ==-1){
            printf("Error bwrite\n");
            return -1;
        }
    return size;
}