//Paula Ferrer y Crist Alcázar  -- Grupo de práctica "PK" -- Grupo tarde

#include "ficheros_basico.h"

int tamMB(unsigned int nbloques){
    int tamMB = 0;
    tamMB= (nbloques / 8) / BLOCKSIZE; // nº bloques MB
    if((nbloques / 8) % BLOCKSIZE > 0) tamMB++; //redondeo por arriba
    return tamMB;
}

int tamAI(unsigned int ninodos){
    int tamAI = 0;
    tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;
    if((ninodos * INODOSIZE) % BLOCKSIZE > 0) tamAI++; //redondeo por arriba
    return tamAI;
}

int initSB(unsigned int nbloques, unsigned int ninodos){
    struct superbloque SB;

    SB.posPrimerBloqueMB = POSSB + TAMSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques)-1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos)-1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    
    SB.posPrimerInodoLibre = 0; 
    //Posteriormente se irá actualizando para apuntar a la cabeza de la lista de inodos libres (mediante las llamadas a las funciones reservar_inodo() y liberar_inodo())
    SB.cantBloquesLibres = nbloques; // Inicialmente, posteriormente restaremos esos bloques de la cantidad de bloques libres (Nivel 3)
    //Al reservar un bloque ⇒ SB.cantBloquesLibres--
    //Al liberar un bloque ⇒ SB.cantBloquesLibres++
    SB.cantInodosLibres = ninodos; // Inicialmente
    //Al reservar un inodo ⇒ SB.cantInodosLibres--
    //Al liberar un inodo ⇒ SB.cantInodosLibres++

    //Cantidad total de bloques
    //Se pasará como argumento en la línea de comandos al inicializar el sistema ($ ./mi_mkfs <nombre_fichero> <nbloques>) y lo recibimos como parámetro
    SB.totBloques = nbloques;
    //Cantidad total de inodos
    SB.totInodos = ninodos;

    if(bwrite(POSSB, &SB) == -1) {
        printf("Error initSB en escritura de suberbloque \n");
        return -1;
    }
    return 0;
}

//Escribir los bits de un byte de una: numDecimal=2^(8-posbit-1) << para el nivel 3

int initMB(){
    struct superbloque SB;
    unsigned char buff[BLOCKSIZE];
    memset( buff, 0, BLOCKSIZE);
    if (bread(POSSB, &SB) == -1) {
        printf("Error initMB en lectura de superbloque \n");
        return -1;
    }
    for (int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB; i++){
        if(bwrite(i, buff) == -1){
            printf("Error initMB en limpiado bloque \n");
            return -1;
        }
    }
    
    for(int i = POSSB; i <= SB.posUltimoBloqueAI; i++){
        if(escribir_bit(i,1)==-1){
            printf("Error initMB en escritura bit \n");
            return -1;
        }
        SB.cantBloquesLibres--;
    }
    if(bwrite(POSSB, &SB) == -1) {
        printf("Error initMB en escritura de superbloque \n");
        return -1;
    }
    return 0;
}

int initAI(){
    struct superbloque SB;

    if (bread(POSSB, &SB) == -1) {
        printf("Error initAI en lectura de superbloque \n");
        return -1;
    }
    unsigned int firstPosAI = SB.posPrimerBloqueAI;
    unsigned int lastPosAI = SB.posUltimoBloqueAI;

    //si hemos inicializado SB.posPrimerInodoLibre = 0
    //if(SB.posPrimerInodoLibre==0) return -1; //nose si es necesario
    
    int inodosCount = SB.posPrimerInodoLibre+1;
    struct inodo inodos [BLOCKSIZE/INODOSIZE];

    for(int i=firstPosAI; i<=lastPosAI; i++){
        for(int j=0; j<BLOCKSIZE/INODOSIZE; j++){
            inodos[j].tipo = 'l';
            if(inodosCount < SB.totInodos){
                inodos[j].punterosDirectos[0]=inodosCount;
                inodosCount++;
            }else{
                inodos[j].punterosDirectos[0]=UINT_MAX;
                j = BLOCKSIZE/INODOSIZE;//para salir del bucle
            }
        }
        if (bwrite(i,&inodos) == -1){
            printf("Error initAI en escritura bloque \n");
            return -1;
        }
    }
    return 0;
}



int escribir_bit(unsigned int nbloque, unsigned int bit){
    int posmb=0;
    int posbyte=0;
    int posbit=0;
    int nbloqueMB=0;
    int nbloqueabs = 0;
    struct superbloque SB;
    unsigned char buffMB[BLOCKSIZE];
    unsigned char mascara = 128; // 10000000

    if (bread(POSSB, &SB) == -1) {
        printf("Error escribir_bit en lectura superbloque");
        return -1;
    }
    //obtener localización del MB
    posmb = SB.posPrimerBloqueMB;
    //pos del byte en MB y pos bit en Byte
    posbyte = nbloque / 8;
    posbit = nbloque % 8;
    //en que bloque se encuentra el bit
    nbloqueMB = posbyte/BLOCKSIZE;
    //pos absoluta del bloque
    nbloqueabs = nbloqueMB + posmb;
    //leer bloque
    if (bread(nbloqueabs, buffMB) == -1){
        printf("Error escribir_bit en lectura mapa de bits \n");
        return -1;
    }
    posbyte = posbyte % BLOCKSIZE;
    mascara >>= posbit;
    if(bit ==1 ) buffMB[posbyte] |= mascara;
    if(bit ==0 ) buffMB[posbyte] &= ~mascara;
    if (bwrite(nbloqueabs, buffMB) == -1){
        printf("Error escribir_bit en escritura mapa de bits \n");
        return -1;
    }
    return 0;
}

unsigned char leer_bit(unsigned int nbloque){
    int posmb=0;
    int posbyte=0;
    int posbit=0;
    int nbloqueMB=0;
    int nbloqueabs = 0;
    struct superbloque SB;
    unsigned char buffMB[BLOCKSIZE];
    unsigned char mascara = 128; // 10000000

    if (bread(POSSB, &SB) == -1) {
        printf("Error leer_bit en lectura superbloque");
        return -1;
    }

    //obtener localización del MB
    posmb = SB.posPrimerBloqueMB;
    //pos del byte en MB y pos bit en Byte
    posbyte = nbloque / 8;
    posbit = nbloque % 8;
    //en que bloque se encuentra el bit
    nbloqueMB = posbyte/BLOCKSIZE;
    //pos absoluta del bloque
    nbloqueabs = nbloqueMB + posmb;
    //leer bloque
    if (bread(nbloqueabs, buffMB) == -1){
        printf("Error leer_bit en lectura mapa de bits");
        return -1;
    }
    posbyte = posbyte % BLOCKSIZE;
    mascara >>= posbit;
    mascara &= buffMB[posbyte];     // operador AND para bits
    mascara >>= (7-posbit);         // desplazamiento de bits a la derecha

    printf("[leer_bit(%i)-> ", nbloque);
    printf("posbyte: %i,", posbyte);
    printf(" posbit: %i,", posbit);
    printf(" nbloqueMB: %i,", nbloqueMB);
    printf(" nbloqueAbs: %i]\n", nbloqueabs);
    printf("valor del bit correspondiente a posSb (o sea al BF nº%i): %i\n", nbloque,mascara);
    printf("\n");

    return mascara;
}

int reservar_bloque(){
    struct superbloque SB;

    int posBloqueMB, nbloque;
    int posbyte=0;
    int posbit=0;

    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferAux[BLOCKSIZE];
    unsigned char mascara = 128; // 10000000
    memset (bufferAux, 255, BLOCKSIZE);

    if (bread(POSSB, &SB) == -1) {
        printf("Error reservar_bloque en lesctura superbloque \n");
        return -1;
    }
    posBloqueMB=SB.posPrimerBloqueMB;

    if(SB.cantBloquesLibres < 1) {
        printf("Error reservar_bloque, no quedan bloques \n");
        return -1;
    }

    if(bread(posBloqueMB, bufferMB) == -1){
            printf("Error reservar_bloque en lectura mapa de bits \n");
            return -1;
    }
    while(memcmp(bufferAux, bufferMB, BLOCKSIZE)==0){
        posBloqueMB++;
        if(bread(posBloqueMB, bufferMB) == -1){
            printf("Error reservar_bloque en lectura mapa de bits \n");
            return -1;
        }
    }
    while (posbyte < BLOCKSIZE){
        if(bufferMB[posbyte]< 255){
            while (bufferMB[posbyte] & mascara) {  
                posbit++;
                bufferMB[posbyte] <<= 1; // desplaz. de bits a la izqda
            }break;
        } else posbyte++;
    }
    
    nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
    if(escribir_bit(nbloque,1) == -1){
        printf("Error reservar_bloque en escritura de bit \n");
        return -1;
    }

    //grabamos un buffer de 0's en la posición del nbloque del dispositivo
    memset (bufferAux, 0, BLOCKSIZE);
    if(bwrite(nbloque, bufferAux) == -1){
        printf("Error reservar_bloque en limpiado de bloque");
        return -1;
    }
    SB.cantBloquesLibres--;
    if(bwrite(POSSB, &SB) == -1) {
        printf("Error reservar_bloque en escritura de superbloque \n");
        return -1;
    } 
    return nbloque;
}

int liberar_bloque(unsigned int nbloque){
    struct superbloque SB;

    if (bread(POSSB, &SB) == -1) {
        printf("Error liberar_bloque en lectura superbloque\n");
        return -1;
    }
    //Ponemos a 0 el bit del MB correspondiente al bloque nbloque (lo recibimos como parámetro)
    if(escribir_bit(nbloque, 0)== -1){
        printf("Error de escritura de bit \n");
        return -1;
    }
    //Incrementamos la cantidad de bloques libres en el superbloque, pero no limpiamos el bloque en la zona de datos; se queda basura pero se interpreta como espacio libre. Salvamos el superbloque.
    SB.cantBloquesLibres++;

    if(bwrite(POSSB, &SB) == -1) {
        printf("Error liberar_bloque en escritura superbloque\n");
        return -1;
    }
    //Devolvemos el nº de bloque liberado, nbloque.
    return nbloque;
}



int escribir_inodo(unsigned int ninodo, struct inodo inodo){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];

    if (bread(POSSB, &SB) == -1) {
        printf("Error escribir_inodo en lectura superbloque\n");
        return -1;
    }
    //Leemos el superbloque para obtener la localización del array de inodos
    unsigned int posAI=SB.posPrimerBloqueAI;
    //Obtenemos el nº de bloque del array de inodos que tiene el inodo solicitado
    unsigned int numBloque = posAI + ninodo / (BLOCKSIZE / INODOSIZE);

    if(bread(numBloque, inodos)==-1){
        printf("Error escribir_inodo en lectura inodo");
        return -1;
    } 
    //Empleamos un array de inodos, del tamaño de la cantidad de inodos que caben en un bloque: struct inodo inodos[BLOCKSIZE/INODOSIZE], como buffer de lectura del bloque que hemos de leer
    //Una vez que tenemos el bloque en memoria escribimos el inodo en el lugar correspondiente del array: ninodo%(BLOCKSIZE/INODOSIZE)
    inodos[ninodo%(BLOCKSIZE/INODOSIZE)] = inodo;
    //El bloque modificado lo escribimos en el dispositivo virtual utilizando la función bwrite()
    if(bwrite(numBloque, inodos)==-1){
        printf("Error escribir_inodo en escritura inodo");
        return -1;
    }
    return 0;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    struct superbloque SB;
    
    if(bread(POSSB, &SB)==-1){
        printf("Error leer_inodo en lectura superbloque \n");
        return -1;
    }
    
    //Leemos el superbloque para obtener la localización del array de inodos
    unsigned int posAI=SB.posPrimerBloqueAI;
    //Obtenemos el nº de bloque del array de inodos que tiene el inodo solicitado 
    unsigned int numBloque = posAI + ninodo / (BLOCKSIZE / INODOSIZE); 
    //Empleamos un array de inodos, del tamaño de la cantidad de inodos que caben en un bloque: struct inodo inodos[BLOCKSIZE/INODOSIZE], como buffer de lectura del bloque que hemos de leer
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; 

    if(bread(numBloque, inodos) == -1){
        printf("Error leer_inodo en lectura inodo\n");
        return -1;
    }
    //El inodo solicitado está en la posición ninodo%(BLOCKSIZE/INODOSIZE) del buffer 
    *inodo=inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
    //Si ha ido todo bien devolvemos 0
    return 0;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
    struct superbloque SB;
    struct inodo inodo;
    struct inodo inodoAux;
    
    if(bread(POSSB, &SB)==-1){
        printf("Error reservar_inodo en lectura superbloque");
        return -1;
    }
    //Comprobar si hay inodos libres y si no hay inodos libres indicar error y salir
    if (SB.cantInodosLibres == 0){
        printf("Error reservar_inodo, no hay inodos libres");
        return -1;
    }
    
    unsigned int primerInodoLibre=SB.posPrimerInodoLibre;
    unsigned int posInodoReservado=primerInodoLibre;
    leer_inodo(posInodoReservado, &inodoAux);

    //A continuación inicializamos todos los campos del inodo al que apuntaba inicialmente el superbloque:
    /*tipo (pasado como argumento)
    permisos (pasados como argumento)
    cantidad de enlaces de entradas en directorio: 1
    tamaño en bytes lógicos: 0
    timestamp de creación para todos los campos de fecha y hora: time(NULL)
    cantidad de bloques ocupados en la zona de datos: 0
    punteros a bloques directos: 0 (el valor 0 indica que no apuntan a nada)
    punteros a bloques indirectos. 0 (el valor 0 indica que no apuntan a nada) */

    inodo.tipo=tipo;
    inodo.permisos=permisos;
    inodo.nlinks=1;
    inodo.tamEnBytesLog=0;
    inodo.atime=time(NULL);
    inodo.ctime=time(NULL);
    inodo.mtime=time(NULL);
    inodo.numBloquesOcupados=0;
    //sizeof(inodo.punterosDirectos)/sizeof(unsigned int)
    for(int i=0; i< DIRECTOS; i++){
        inodo.punterosDirectos[i]=0;
    }
    //sizeof(inodo.punterosIndirectos)/sizeof(unsigned int)
    for(int i=0; i< 3; i++){
        inodo.punterosIndirectos[i]=0;
    }
    //Utilizar la función escribir_inodo() para escribir el inodo inicializado en la posición del que era el primer inodo libre, posInodoReservado.
    if(escribir_inodo(posInodoReservado, inodo)==-1){
        printf("Error reservar_inodo en escritura inodo");
        return -1;
    }
    //Actualizar la cantidad de inodos libres, y reescribir el superbloque.
    SB.posPrimerInodoLibre = inodoAux.punterosDirectos[0];
    SB.cantInodosLibres--;
    
    if(bwrite(POSSB, &SB)==-1){
        printf("Error reservar_inodo en escritura superbloque");
        return -1;
    }
    //Devolver posInodoReservado.
    return posInodoReservado;
}

int obtener_nrangoBL(struct inodo inodo, unsigned int nblogico, unsigned int *ptr){
    if(nblogico<DIRECTOS){
        *ptr=inodo.punterosDirectos[nblogico];
        return 0;
    }else if(nblogico<INDIRECTOS0){
        *ptr=inodo.punterosIndirectos[0];
        return 1;
    }else if(nblogico<INDIRECTOS1){
        *ptr=inodo.punterosIndirectos[1];
        return 2;
    }else if(nblogico<INDIRECTOS2){
        *ptr=inodo.punterosIndirectos[2];
        return 3;
    }else{
        *ptr=0;
        printf("Error obtener_rangoBL, bloque lógico fuera de rango \n");
        return -1;
    }
}

int obtener_indice(int nblogico, int nivel_punteros){
    if(nblogico<DIRECTOS){
        return nblogico;
    }else if(nblogico<INDIRECTOS0){
        return nblogico-DIRECTOS;
    }else if(nblogico<INDIRECTOS1){
        if(nivel_punteros==2){
            return (nblogico-INDIRECTOS0)/NPUNTEROS;
        }else if(nivel_punteros==1){
            return (nblogico-INDIRECTOS0)%NPUNTEROS;
        }
    }else if(nblogico<INDIRECTOS2){
        if(nivel_punteros==3){
            return (nblogico-INDIRECTOS1)/(NPUNTEROS*NPUNTEROS);
        }else if(nivel_punteros==2){
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))/NPUNTEROS;
        }else if(nivel_punteros==1){
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))%NPUNTEROS;
        }
    }
        printf("Error obtener_indice \n");
        return -1;
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar){
    struct inodo inodo;
    int salvar_inodo, nRangoBL, nivel_punteros;
    int indice=0;
    unsigned int ptr, ptr_ant;
    int buffer[NPUNTEROS];

    if(leer_inodo(ninodo, &inodo)==-1){
        printf("Error traducir_bloque_inodo en lectura inodo \n");
        return -1;
    }
    ptr=0; 
    ptr_ant = 0;
    salvar_inodo = 0;
    if((nRangoBL = obtener_nrangoBL(inodo, nblogico, &ptr))==-1){
        printf("Error traducir_bloque_inodo en obtención de rango \n");
        return -1;
    }
    nivel_punteros = nRangoBL;

    while (nivel_punteros>0){
        if(ptr == 0){
            if(reservar == 0) {
            printf("Error traducir_bloque_inodo, error de lectura, bloque inexistente");
            return -1;
        } 
            else{
                salvar_inodo =1;
                if((ptr = reservar_bloque())==-1)return-1;
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL);
                if(nivel_punteros == nRangoBL){
                    inodo.punterosIndirectos[nRangoBL-1] = ptr; 
                    printf("inodo.punterosInDirectos[%i]= %i (reservado BF %i, para puteros_nivel%i)\n", nRangoBL-1,ptr, ptr,nivel_punteros);
                }else{
                    buffer[indice] = ptr; 
                    printf("punteros_nivel%i[%i]= %i (reservado BF %i, para puteros_nivel%i)\n", nivel_punteros+1,indice,ptr, ptr,nivel_punteros);
                    if(bwrite(ptr_ant, buffer)==-1) return -1;
                }
            }
            
        }
        if(bread(ptr, buffer)==-1) return -1;
        if((indice = obtener_indice(nblogico, nivel_punteros)) ==-1) return -1;
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }
    
    if(ptr == 0){
        if(reservar == 0){
            printf("Error traducir_bloque_inodo, error de lectura, bloque inexistente");
            return -1;
        }else{
            salvar_inodo = 1;
            if ((ptr = reservar_bloque())==-1) return -1;
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            if(nRangoBL == 0){
                inodo.punterosDirectos[nblogico] = ptr;
                printf("inodo.punterosDirectos[%i]= %i (reservado BF %i, para BL %i)\n", nblogico,ptr, ptr,nblogico);
            }else{
                buffer[indice] = ptr;
                printf("punteros_nivel%i[%i]=  %i (reservado BF %i, para BL %i)\n", nivel_punteros+1,indice,ptr, ptr,nblogico);
                if (bwrite(ptr_ant, buffer)==-1) return -1;
            }
        } 
    }
    if(salvar_inodo == 1){
        if(escribir_inodo(ninodo, inodo)==-1) return -1; 
    }
    return ptr;
}

int liberar_inodo(unsigned int ninodo){
    struct superbloque SB;
    struct inodo inodo;
    int ninodosLiberados=0;

    ninodosLiberados= liberar_bloques_inodo(ninodo, &inodo);
    leer_inodo(ninodo, &inodo);

    inodo.numBloquesOcupados = inodo.numBloquesOcupados - ninodosLiberados;
    inodo.tipo= 'l';

    if(bread(POSSB, &SB) == -1){
        printf("Error liberar_inodo en lectura de superbloque \n");
        return-1;
    }
    inodo.punterosDirectos[0]= SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre= ninodo;
    SB.cantInodosLibres++;

    if(escribir_inodo(ninodo, inodo)==-1){
        printf("Error liberar_inodo en escritura inodo \n");
        return-1;
    }

    if(bwrite(POSSB, &SB)==-1){
        printf("Error liberar_inodo en escritura de superbloque \n");
        return-1;
    }

    return ninodo;
}

int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo){
    unsigned int nRangoBL, nivel_punteros, indice, ptr, nblog, ultimoBL;
    int liberados, salvar_inodo;
    unsigned char buf_punteros[BLOCKSIZE]; //1024 bytes
    unsigned int bloque_punteros [3][NPUNTEROS]; //1024 bytes
    int ptr_nivel[3];
    int indices[3];

    memset (buf_punteros, 0, BLOCKSIZE);

    liberados=0; salvar_inodo=0;

    leer_inodo(primerBL, inodo);
    
    if(inodo->tamEnBytesLog==0){
        printf("Info: liberar_bloques_inodo: fichero vacío \n");
        return 0;
    }
    
    if((inodo->tamEnBytesLog%BLOCKSIZE)==0){
        ultimoBL= inodo->tamEnBytesLog/BLOCKSIZE-1; 
    }else{
        ultimoBL= inodo->tamEnBytesLog/BLOCKSIZE;
    }

    printf("primerBL: %i\n",primerBL);
    printf("ultimoBL: %i\n",ultimoBL);

    ptr=0;

    for(nblog=primerBL; nblog <=ultimoBL; nblog++){
        nRangoBL=obtener_nrangoBL(*inodo, nblog,&ptr);
        /*
        printf("ptr: %i \n",ptr);
        printf("nblog: %i \n",nblog);
        printf("Rango: %i\n",nRangoBL); //debug*/
        if(nRangoBL<0){
            printf("ERROR\n");
            return -1;
        }
        nivel_punteros = nRangoBL;

        while( ptr > 0 && nivel_punteros>0){
            bread(ptr, bloque_punteros[nivel_punteros-1]);
            indice=obtener_indice(nblog, nivel_punteros);
            ptr_nivel[nivel_punteros-1]=ptr;
            indices[nivel_punteros-1]=indice;
            ptr= bloque_punteros[nivel_punteros-1][indice];
            nivel_punteros--;
        }
        if (ptr > 0){
            liberar_bloque(ptr);
            liberados++;
            if(nRangoBL == 0){
                inodo->punterosDirectos[nblog]=0;
                salvar_inodo=1;
            }else{
                while(nivel_punteros< nRangoBL){
                    indice= indices[nivel_punteros];
                    bloque_punteros[nivel_punteros][indice]=0;
                    ptr=ptr_nivel[nivel_punteros];
                    if(memcmp (bloque_punteros[nivel_punteros], buf_punteros, BLOCKSIZE)==0){
                        liberar_bloque(ptr);
                        liberados++;
                        nivel_punteros++;
                        if(nivel_punteros==nRangoBL){
                            inodo->punterosIndirectos[nRangoBL-1]=0;
                            salvar_inodo=1;
                        }
                    }else{
                        bwrite(ptr, bloque_punteros[nivel_punteros]);
                        nivel_punteros= nRangoBL;
                    }
                }
            }
        }
    }
    if(salvar_inodo==1){
        escribir_inodo(primerBL, *inodo);
    }
    return liberados;
}