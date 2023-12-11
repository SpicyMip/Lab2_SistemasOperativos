#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "funciones.h"

#define FILAS 85
#define COLUMNAS 85

int main(){
    int shmid;
    int *turnos;
    key_t key = ftok("mi_archivo", 'R');  // Crear una clave única basada en un archivo y un carácter
    int shmid2 = shmget(key, 4, IPC_CREAT | 0666);  // Crear una región de memoria compartida para 4 caracteres

    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(1);
    }

    char *cartas = (char *)shmat(shmid, NULL, 0);
    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(1);
    }
    turnos= (int *)shmat(shmid, NULL, 0);
    *turnos=15;
    struct jugador j1;
    struct jugador j2;
    struct jugador j3;
    struct jugador j4;
    int cartaAleatoria;
    char mazo[4] = {'E', 'E', 'b', 'b'};
    printf("Bienvenido a Magic Maze\nAcontinuacion se repartiran las cartas\n");
    for(int i=0; i<4; i++){
        do{
            cartaAleatoria=rand()%4;
        } while (mazo[cartaAleatoria]==' ');
        if (i==0){
            j1.habilidad=mazo[cartaAleatoria];
            cartas[i]=mazo[cartaAleatoria];
            if (j1.habilidad=='E'){
                printf("La carta del Jugador 1 es Escalera\n");
            }else{
                printf("La carta del Jugador 1 es Busqueda\n");
            }
        }else if(i==1){
            j2.habilidad=mazo[cartaAleatoria];
            cartas[i]=mazo[cartaAleatoria];
            if (j2.habilidad=='E'){
                printf("La carta del Jugador 2 es Escalera\n");
            }else{
                printf("La carta del Jugador 2 es Busqueda\n");
            }
        }else if(i==2){
            j3.habilidad=mazo[cartaAleatoria];
            cartas[i]=mazo[cartaAleatoria];
            if (j3.habilidad=='E'){
                printf("La carta del Jugador 3 es Escalera\n");
            }else{
                printf("La carta del Jugador 3 es Busqueda\n");
            }
        }else if(i==3){
            j4.habilidad=mazo[cartaAleatoria];
            cartas[i]=mazo[cartaAleatoria];
            if (j4.habilidad=='E'){
                printf("La carta del Jugador 4 es Escalera\n");
            }else{
                printf("La carta del Jugador 4 es Busqueda\n");
            }
        }
        mazo[cartaAleatoria]=' ';
    }



    int opcion=0;
    int messages[4];
    int pipeP[2][2];
    int pipesDesdePadre[3][2];
    int pipesHaciaPadre[3][2];
    for (int i=0; i<2; i++){
        if(pipe(pipeP[i])==-1){
            perror("Error al crear Pipe");
            exit(EXIT_FAILURE);
        }
    }
    for(int i=0; i<3; i++){
        if (pipe(pipesDesdePadre[i])==-1){
            perror("Error al crear el pipe");
            exit(EXIT_FAILURE);
        }
        if (pipe(pipesHaciaPadre[i])==-1){
            perror("Error al creal el pipe");
            exit(EXIT_FAILURE);
        }
    }
    pid_t hijoPrincipal;
    int *decisiones;

    hijoPrincipal=fork();

    if (hijoPrincipal == -1) {
        perror("Error al crear el primer hijo");
        exit(EXIT_FAILURE);
    }else if(hijoPrincipal==0){
        close(pipeP[0][0]); //cierra lectura,con esta pipe escribe
        close(pipeP[1][1]); //cierra escritura, con esta pipe lee
        for(int i=0;i<3; i++){
            close(pipesHaciaPadre[i][1]);//leo
            close(pipesDesdePadre[i][0]);//escribo
        }

        for(int i=0; i<(*turnos); i++){
            read(pipeP[1][0], &opcion, sizeof(opcion));
            decisiones=(int*)malloc(4*sizeof(int));
            if (opcion==0){
                decisiones[0]=turno(1, cartas[0]);

                for(int j=0;j<3;j++){
                    write(pipesDesdePadre[j][1], &opcion, sizeof(opcion));
                    read(pipesHaciaPadre[j][0], &decisiones[j+1], sizeof(decisiones[j+1]));
                }
                for (int j=0; j<4; j++){
                    write(pipeP[0][1], &decisiones[j], sizeof(decisiones[j]));
                }
                free(decisiones);
                
                
            }else if(opcion==3){
                free(decisiones);
                for(int j=0;j<3;j++){
                    write(pipesDesdePadre[j][1], &opcion, sizeof(opcion));
                    }
                for(int i=0;i<3; i++){
                    close(pipesHaciaPadre[i][0]);
                    close(pipesDesdePadre[i][1]);
                    }
                close(pipeP[0][1]);
                close(pipeP[1][0]);
                exit(EXIT_SUCCESS);
            }
        }

        for(int i=0;i<3; i++){
            close(pipesHaciaPadre[i][0]);
            close(pipesDesdePadre[i][1]);
        }
        close(pipeP[0][1]);
        close(pipeP[1][0]);
        exit(EXIT_SUCCESS);
    }

    
    pid_t child_pids[3];
    for (int i=0; i<3; i++){

        child_pids[i]=fork();

        if (child_pids[i] == -1) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        }else if (child_pids[i] == 0) {
            close(pipesHaciaPadre[i][0]);
            close(pipesDesdePadre[i][1]);
            for(int j=0; j<(*turnos); j++){
                read(pipesDesdePadre[i][0], &opcion, sizeof(opcion));

                if (opcion==0){
                    //inicio nuevo turno
                    messages[i+1]=turno(i+2, cartas[i+2]);
                    write(pipesHaciaPadre[i][1], &messages[i+1], sizeof(messages[i+1]));
                }else if (opcion==3){
                    for (int x=0; x<3;x++){
                        close(pipesHaciaPadre[i][1]);
                        close(pipesDesdePadre[i][0]);
                    }
                    exit(EXIT_SUCCESS);
                }
            }
            exit(EXIT_SUCCESS);
        }
    }
    close(pipeP[0][1]);// cerrada escritura
    close(pipeP[1][0]);//cerrada la lecura
    srand(time(NULL));
    int arreglo[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int tamano = sizeof(arreglo) / sizeof(arreglo[0]);
    int camaras = 0;
    int cantidad_tesoros = 4;
    char vacio;
    int eliminar;
    int mapas_generados = 0;
    char mapas[8][26];
    char matriz[FILAS][COLUMNAS];
    int tp[8][2];
    

    cargar_mapas(mapas);
    inicializarMatriz(matriz);

    char *copia = reemplazarEnArchivo("Inicio.txt");

    cambiosMapa(matriz,copia, 40, 40, &j1,&j2,&j3,&j4);
    
    
    char direccion;
    int cantidad;
    opcion=0;
    int opcion2;
    for(int i=0;i<(*turnos);i++){
        if(j1.tesoro && j2.tesoro && j3.tesoro && j4.tesoro){
                if ( (j1.x > 39) && (j1.x<45) && (j1.y>39) && (j1.y<45) && j2.x>39 && j2.x<45 && j2.y>39 && j2.y<45 && j3.x>39 && j3.x<45 && j3.y>39 && j3.y<45 && j4.x>39 && j4.x<45 && j4.y>39 && j4.y<45){
                    opcion=3;
                    write(pipeP[1][1], &opcion, sizeof(opcion));
                    printf("Los Jugadores han ganado el juego\nFELICIDADES!!!\n");
                    break;
                }              
            }
        printf("Inicio del turno %d:\n(Pulse enter para continuar)", i+1);
        scanf("%c", &vacio);
        printf("Mapa Actual:\n");
        imprimirMatriz(matriz);
        write(pipeP[1][1], &opcion, sizeof(opcion));
        for(int j=0; j<4; j++){
            read(pipeP[0][0],&opcion2, sizeof(opcion2));
            if (opcion2==1){
                printf("Ingrese la direccion a moverse del Jugador %d:\n(d)Derecha\n(i)Izquierda\n(a)Arriba\n(b)Abajo\n", j+1);
                scanf("%c", &direccion);
                printf("Ingrese cuantas casillas quiere moverse el Jugador %d:\n", j+1);
                scanf("%d", &cantidad);
                if(j==0){
                    mover(matriz,direccion,&j1, 'a', cantidad,turnos,&camaras, tp);
                    printf("(Pulse enter para continuar)\n");
                    scanf("%c", &vacio);
                }else if(j==1){
                    mover(matriz,direccion,&j2, 'b', cantidad,turnos,&camaras, tp);
                }else if (j==2){
                    mover(matriz,direccion,&j3, 'c', cantidad,turnos,&camaras, tp);
                }else if(j==3){
                    mover(matriz,direccion,&j4, 'd', cantidad,turnos,&camaras, tp);
                }

            }else if (opcion2==2){
                if(j==0){
                    if(j1.habilidad=='E'){
                        escalera(matriz, &j1);
                    }else{
                    eliminar = buscar(matriz,&j1,mapas,&camaras, &cantidad_tesoros,j1.y,j1.x,tamano,&mapas_generados,tp);
                    eliminarElemento(arreglo,&tamano,eliminar);
                    }
                }else if (j==1){
                    if (j2.habilidad=='E'){
                        escalera(matriz, &j2);
                    }else{
                        eliminar = buscar(matriz,&j2,mapas,&camaras, &cantidad_tesoros,j2.y,j2.x,tamano,&mapas_generados,tp);
                        eliminarElemento(arreglo,&tamano,eliminar);
                    }
                }else if (j==2){
                    if (j3.habilidad=='E'){
                        escalera(matriz, &j3);
                    }else{
                        eliminar = buscar(matriz,&j3,mapas,&camaras, &cantidad_tesoros,j3.y,j3.x,tamano,&mapas_generados,tp);
                        eliminarElemento(arreglo,&tamano,eliminar);
                    }
                }else if (j==3){
                    if(j4.habilidad=='E'){
                        escalera(matriz, &j4);
                    }else{
                        eliminar = buscar(matriz,&j4,mapas,&camaras, &cantidad_tesoros,j4.y,j4.x,tamano,&mapas_generados,tp);
                        eliminarElemento(arreglo,&tamano,eliminar);
                    }
                }
            }
        }
        printf("EL Mapa despues del turno %d\n", i+1);
        imprimirMatriz(matriz);
    }
    shmdt(cartas);
    shmctl(shmid2, IPC_RMID, NULL);
    shmdt(turnos);
    shmctl(shmid, IPC_RMID, NULL);
    free(copia);
    return 0;
}