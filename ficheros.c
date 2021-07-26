//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
     struct inodo inodo;
     int primerBLogico=0;
     int ultimoBLogico=0;
     int desp1=0;
     int desp2=0;
     int nBLfisico=0;
     int bytesEscritos= 0;
     unsigned char buf_bloque[BLOCKSIZE];
     
     leer_inodo(ninodo, &inodo);
     if((inodo.permisos & 2) != 2){
        printf("Error mi_write_f en permisos\n");
        return -1;
     }
     primerBLogico = offset/BLOCKSIZE;
     ultimoBLogico=(offset+nbytes-1)/BLOCKSIZE;
     desp1 = offset % BLOCKSIZE;
     desp2 = (offset+nbytes-1) % BLOCKSIZE;

    if((nBLfisico = traducir_bloque_inodo(ninodo, primerBLogico, 1))==-1){
        printf("Error mi_write_f en traducción a bloque físico\n");
        return -1;
    }

    if(bread(nBLfisico, buf_bloque)==-1){
        printf("Error mi_write_f en lectura bloque lógico\n");
        return -1;
    }
    //caso en que cabe en un solo bloque
    if(primerBLogico==ultimoBLogico){
        memcpy(buf_bloque+desp1, buf_original, nbytes);
        if(bwrite(nBLfisico, buf_bloque)==-1){
            printf("Error mi_write_f en escritura bloque lógico\n");
            return -1;
        }
        bytesEscritos = nbytes;
    }else{//caso en el que modificamos varios bloques
        //PASO 1
        memcpy(buf_bloque+desp1, buf_original, BLOCKSIZE-desp1);
        if(bwrite(nBLfisico, buf_bloque)==-1){
            printf("Error mi_write_f en escritura bloque lógico\n");
            return -1;
        }
        bytesEscritos= BLOCKSIZE-desp1;
        //PASO 2
        for(int i= primerBLogico+1; i<ultimoBLogico; i++){
            if((nBLfisico = traducir_bloque_inodo(ninodo, i, 1))==-1){
            printf("Error mi_write_f en traducción a bloque físico\n");
            return -1;
            }
            if(bwrite(nBLfisico, buf_original + (BLOCKSIZE-desp1) + (i-primerBLogico-1) * BLOCKSIZE)==-1){
                printf("Error mi_write_f en escritura bloque lógico\n");
                return -1;
            }
            bytesEscritos= bytesEscritos+BLOCKSIZE;
        }
        //PASO 3
        if((nBLfisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 1))==-1){
            printf("Error mi_write_f en traducción a bloque físico\n");
            return -1;
        }

        if(bread(nBLfisico, buf_bloque)==-1){
            printf("Error mi_write_f en lectura bloque lógico\n");
            return -1;
        }
        memcpy(buf_bloque, buf_original + (nbytes-desp2-1), desp2+1);
        if(bwrite(nBLfisico, buf_bloque)==-1){
            printf("Error mi_write_f en escritura bloque lógico\n");
            return -1;
        }
        bytesEscritos= bytesEscritos+desp2+1;
    }

    leer_inodo(ninodo, &inodo);
    if(offset+nbytes > inodo.tamEnBytesLog){
        inodo.tamEnBytesLog=offset+nbytes;
        inodo.ctime=time(NULL);
    }
    inodo.mtime=time(NULL);
    if(escribir_inodo(ninodo, inodo)==-1){
        printf("Error mi_write_f en escritura inodo \n");
        return-1;
    }

    return bytesEscritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    int primerBLogico=0;
    int ultimoBLogico=0;
    int desp1=0;
    int desp2=0;
    int nBLfisico=0;
    int bytesLeidos= 0;
    unsigned char buf_bloque[BLOCKSIZE];
    memset(buf_bloque, 0, BLOCKSIZE);

    leer_inodo(ninodo, &inodo);
    if((inodo.permisos & 4) != 4){
        printf("Error mi_write_f en permisos\n");
        return -1;
    }

    if (offset>=inodo.tamEnBytesLog){
        bytesLeidos=0;//no podemos leer nada
        return bytesLeidos;
    }
    if (offset+nbytes >= inodo.tamEnBytesLog){
        nbytes= inodo.tamEnBytesLog-offset;
        //leemos solo los bytes desde el offset hasta EOF
    }
    
    primerBLogico = offset/BLOCKSIZE;
    ultimoBLogico=(offset+nbytes-1)/BLOCKSIZE;
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset+nbytes-1) % BLOCKSIZE;

    //caso en que cabe en un solo bloque
    if(primerBLogico==ultimoBLogico){
        if((nBLfisico= traducir_bloque_inodo(ninodo,primerBLogico,0))!=-1){
            if(bread(nBLfisico, buf_bloque)==-1){
                printf("Error mi_read_f en lectura bloque físico\n");
                return -1;
            }
            memcpy(buf_original, buf_bloque+desp1, desp2-desp1+1);
        }
        bytesLeidos= bytesLeidos+desp2-desp1+1;
    }else{//caso en el que leemos varios bloques
        //PASO 1
        if((nBLfisico= traducir_bloque_inodo(ninodo,primerBLogico,0))!=-1){
            if(bread(nBLfisico, buf_bloque)==-1){
                printf("Error mi_read_f en lectura bloque físico\n");
                return -1;
            }
            memcpy(buf_original, buf_bloque+desp1, BLOCKSIZE-desp1);
        }
        bytesLeidos= bytesLeidos+BLOCKSIZE-desp1;
        //PASO 2
        for(int i= primerBLogico+1; i<ultimoBLogico; i++){
            if((nBLfisico = traducir_bloque_inodo(ninodo, i, 0))!=-1){
                if(bread(nBLfisico, buf_bloque)==-1){
                printf("Error mi_read_f en lectura bloque físico\n");
                return -1;
            }
            memcpy(buf_original + (BLOCKSIZE-desp1) + (i-primerBLogico-1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
            }
            bytesLeidos= bytesLeidos+BLOCKSIZE;
        }
        //PASO 3
        if((nBLfisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 0))!=-1){
            if(bread(nBLfisico, buf_bloque)==-1){
                printf("Error mi_read_f en lectura bloque lógico\n");
                return -1;
             }
            memcpy(buf_original + (nbytes-desp2-1),buf_bloque, desp2+1);
        }
        bytesLeidos= bytesLeidos+desp2+1;
    }

    leer_inodo(ninodo, &inodo);
    inodo.atime=time(NULL);
    if(escribir_inodo(ninodo, inodo)==-1){
        printf("Error mi_write_f en escritura inodo \n");
        return-1;
    }

    return bytesLeidos;
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
    struct inodo inodo;
    
    leer_inodo(ninodo, &inodo);
    
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

    return 0;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
     struct inodo inodo;

    leer_inodo(ninodo, &inodo);
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);

    if(escribir_inodo(ninodo, inodo)==-1){
        printf("Error mi_chmod_f en escritura inodo \n");
        return-1;
    }
    return 0;
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
    struct inodo inodo;
    unsigned int nblogico=0;
    int bloquesLiberados=0;

    leer_inodo(ninodo, &inodo);

    if((inodo.permisos & 2) != 2){
        printf("Error mi_write_f, el fichero no tiene permiso de escritura\n");
        return -1;
    }

    if (nbytes%BLOCKSIZE==0){
        nblogico=nbytes/BLOCKSIZE;
    }else{
        nblogico=nbytes/BLOCKSIZE+1;
    }

    if ((bloquesLiberados = liberar_bloques_inodo(nblogico, &inodo))==-1){
        printf("Error en mi_truncar_f en liberación de inodos\n");
        return -1;
    }
    leer_inodo(ninodo, &inodo);
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.tamEnBytesLog = nbytes;
    inodo.numBloquesOcupados = inodo.numBloquesOcupados -bloquesLiberados;

    if(escribir_inodo(ninodo, inodo)==-1){
        printf("Error en mi_truncar_f en escritura de inodo\n");
        return -1;
    }

    return bloquesLiberados;
}
