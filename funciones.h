#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define FILAS 85
#define COLUMNAS 85


// Definición de la estructura "jugador" para representar a los jugadores.
struct jugador
{
    int x;          // Posicion en el eje x
    int y;          // Posicion en el eje y
    int tesoro;     // 0 si no tiene tesoro, 1 si tiene tesoro
    char habilidad; // Habilidad o carta obtenida
};

//Descipcion de la funcion: Se encarga de mostrar por consola la matriz completa
//      Parametros:
//          matriz[FILAS][COLUMNAS](char):  Matriz a mostrar por consola
//      Retorno:
//          no tiene reotorno
void imprimirMatriz(char matriz[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            printf("%c", matriz[i][j]);
        }
        printf("\n");
    }
}

//Descipcion de la funcion: Se encarga de inizializar la matriz con '/'
//      Parametros:
//          matriz[FILAS][COLUMNAS](char):  Matriz a inicializar
//      Retorno:
//          no tiene retorno
void inicializarMatriz(char matriz[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            matriz[i][j] = '/';
        }
    }
}

//Descipcion de la funcion: Se encarga de cambiar un elemento de una matriz en una cordenada especifica
//      Parametros:
//          matriz[FILAS][COLUMNAS](char):  Matriz a donde se realizan los cambios
//          fila(int):                      Fila de la matriz donde se genera el cambio
//          columna(int):                   Columna de la matriz donde se genera el cambio
//          nuevoValor(char):               Caracter que remplaza en la matriz
//      Retorno:
//          no tiene retorno
void cambiarValor(char matriz[FILAS][COLUMNAS], int fila, int columna, char nuevoValor) {
    if (fila >= 0 && fila < FILAS && columna >= 0 && columna < COLUMNAS) {
        matriz[fila][columna] = nuevoValor;
    } else {
        printf("Coordenadas fuera de rango.\n");
    }
}

//Descipcion de la funcion: Se encarga de remplazar a los Jx por letras para que hagan de representacion de los jugadores desde un .txt
//      Parametros:
//          nombreArchivo(char*):  Nombre del archivo a abrir
//      Retorno:
//          Retorna un string obtenido del .txt
char *reemplazarEnArchivo(const char *nombreArchivo) {
    FILE *archivo;
    char caracter;

    // Abrir el archivo en modo lectura
    archivo = fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        return NULL;
    }

    char copia[30];
    int i = 0;

    // Leer el archivo caracter por caracter
    while ((caracter = fgetc(archivo)) != EOF) {
        if (!(caracter == 'J' || caracter == ' ' || caracter == '\n' || caracter == '\r')){
            if (caracter == '1')
            {
                copia[i] = 'a';
            }
            else if (caracter == '2')
            {
                copia[i] = 'b';
            }
            else if (caracter == '3')
            {
                copia[i] = 'c';
            }
            else if (caracter == '4')
            {
                copia[i] = 'd';
            }
            else
            {
                copia[i] = caracter;
            }

            i++;
        }
    }

    fclose(archivo);

    // Agregar un terminador nulo al final de la cadena
    copia[i] = '\0';

    // Almacenar la cadena en memoria dinámica y devolverla
    char *resultado = strdup(copia);
    return resultado;
}

//Descipcion de la funcion: Carga la cadena del mapa inicio en la matriz nueva y guarda las posiciones de los jugadores
//      Parametros:
//          matriz[FILAS][COLUMNAS](char):  Matriz a donde se realizan los cambios
//          cadena(char *):                 string que representa al inicio.txt
//          fila(int):                      Fila donde inicia el centro de la matriz
//          columna(int):                   Columna donde inicia el centro de la matriz
//          j1(struct jugador *):           Struct que representa al jugador 1
//          j2(struct jugador *):           Struct que representa al jugador 2
//          j3(struct jugador *):           Struct que representa al jugador 3
//          j4(struct jugador *):           Struct que representa al jugador 4
//      Retorno:
//          no tiene retorno
void cambiosMapa(char matriz[FILAS][COLUMNAS], char *cadena, int fila, int columna, struct jugador *j1,struct jugador *j2,struct jugador *j3,struct jugador *j4){
    
    int z = 0;
    int inicio = columna;
    for (int i = 0; i < 5; i++)
    {
        columna = inicio;
        for (int j = 0; j < 5; j++)
        {
            cambiarValor(matriz,fila,columna,cadena[z]);

            if (cadena[z] == 'a')
            {
                j1->x= columna;
                j1->y= fila;
            }
            
            if (cadena[z]== 'b')
            {
                j2->x= columna;
                j2->y= fila;
            }

            if (cadena[z]== 'c')
            {
                j3->x= columna;
                j3->y= fila;
            }
            if (cadena[z]== 'd')
            {
                j4->x= columna;
                j4->y= fila;
            }
            columna++;
            z++;
        }
        fila++;
    }
    

}

//Descipcion de la funcion: Se encarga de expandir el mapa si es que el jugador esta en la ubicacion y tiene la habilidad de buscar
//      Parametros:
//          matriz[FILAS][COLUMNAS](char):  Matriz a donde se realizan los cambios
//          jugador(struct jugador *):      Jugador que aplica buscar
//          mapas[8][26](char):             Arreglo con todos los mapas cargados
//          camaras(int *):                 Camaras activas
//          tesoros(int *):                 Tesoros activos
//          columna(int):                   Posicion en x del jugador
//          fila(int):                      Posicion en y del jugador
//          tamano(int):                    Tamaño del arreglo con los mapas
//          mapas_generados(int *):         Cantidad de mapas generados
//          tp[8][2](int):                  Arreglo con las posiciones de los TP
//      Retorno:
//          retorna el numero del mapa a eliminar
int buscar(char matriz[FILAS][COLUMNAS],struct jugador *jugador,char mapas[8][26], int *camaras, int *tesoros, int columna, int fila, int tamano, int *mapas_generados, int tp[8][2]){

    int eliminar = 0;
    char posicion;
    int opscion = 0;
    int teso;

    if (jugador->habilidad != 'b')
    {
        return eliminar;
    }
    

    if ((*tesoros) != 0)
    {
        teso = rand() % 2;
    }
    
    else
    {
        teso = 0;
    }
    
    int azar;
    int si;
    int creado = 0;
    int x;
    int y;

    int numb = (*mapas_generados);

    if (numb == 9)
    {
        return eliminar;
    }
    

    if (matriz[fila - 1][columna]== '/')
    {
        int j = 0;
        int numero = 2;
        while (columna >= numero)
        {
            numero = 2 + 5*j;
            if (columna == numero){
                opscion = 1;
                posicion = 'a';
            }
            j++;
        }
        
    }

    if (matriz[fila + 1][columna]== '/')
    {
        int j = 0;
        int numero = 2;
        while (columna >= numero)
        {
            numero = 2 + 5*j;
            if (columna == numero){
                opscion = 1;
                posicion = 'b';
            }
            j++;
        }
        
    }

    if (matriz[fila ][columna - 1]== '/')
    {
        int j = 0;
        int numero = 2;
        while (fila >= numero)
        {
            numero = 2 + 5*j;
            if (fila == numero){
                opscion = 1;
                posicion = 'i';
            }
            j++;
        }
        
    }

    if (matriz[fila ][columna + 1]== '/')
    {
        int j = 0;
        int numero = 2;
        while (fila >= numero)
        {
            numero = 2 + 5*j;
            if (fila == numero){
                opscion = 1;
                posicion = 'd';
            }
            j++;
        }
        
    }
    
    if (opscion == 0)
    {
        printf( "\nAca no puedes buscar\n");
    }

    if (opscion == 1){

        if (posicion == 'i')
        {
            int indiceAleatorio = rand() % tamano;
            eliminar = indiceAleatorio + 1;
            while (mapas[indiceAleatorio][14] != 'B')
            {
                indiceAleatorio = rand() % tamano;
                eliminar = indiceAleatorio + 1 ;
            }

            x = columna - 5;
            y = fila - 2;

            
        }

        if (posicion == 'd')
        {
            int indiceAleatorio = rand() % tamano;
            eliminar = indiceAleatorio + 1;
            while (mapas[indiceAleatorio][10] != 'B')
            {
                indiceAleatorio = rand() % tamano;
                eliminar = indiceAleatorio + 1 ;
            }

            x = columna + 1;
            y = fila - 2;

            
        }

        if (posicion == 'a')
        {
            int indiceAleatorio = rand() % tamano;
            eliminar = indiceAleatorio + 1;
            while (mapas[indiceAleatorio][22] != 'B')
            {
                indiceAleatorio = rand() % tamano;
                eliminar = indiceAleatorio + 1 ;
            }

            x = columna - 2;
            y = fila - 5;

            
        }

        if (posicion == 'b')
        {
            int indiceAleatorio = rand() % tamano;
            eliminar = indiceAleatorio + 1;
            while (mapas[indiceAleatorio][2] != 'B')
            {
                indiceAleatorio = rand() % tamano;
                eliminar = indiceAleatorio + 1 ;
            }

            x = columna - 2;
            y = fila + 1;

            
        }

        int z = 0;
        int inicio = x;
        for (int i = 0; i < 5; i++)
        {
            x = inicio;
            for (int j = 0; j < 5; j++)
            {
                if (mapas[eliminar - 1][z] == '0' && (*mapas_generados) != 8)
                {
                    si = rand() % 2;
                    if (si == 1 && creado == 0){
                        if (teso == 0 )
                        {
                            azar = rand() % 4;
                            if (azar == 0)
                            {
                                cambiarValor(matriz,y,x,'C');
                                camaras++;
                                creado = 1;
                            }
                            if (azar == 1)
                            {
                                cambiarValor(matriz,y,x,'N');
                                creado = 1;
                            }
                            if (azar == 2)
                            {
                                cambiarValor(matriz,y,x,'Q');
                                creado = 1;
                            }
                            if (azar == 3)
                            {
                                cambiarValor(matriz,y,x,'T');
                                int J = 0;
                                int cond = 1;
                                creado = 1;
                                while (cond)
                                {
                                    if (((tp[j][0]) + (tp[j][1])) == 0)
                                    {
                                        tp[j][0] = x;
                                        tp[j][1] = y;
                                        cond = 0;
                                    }
                                    
                                }
                                
                            }
                            
                            
                        }

                        else
                        {
                            char rar;
                            if ((*tesoros) == 4)
                            {
                                cambiarValor(matriz,y,x,'4');
                            }
                            if ((*tesoros) == 3)
                            {
                                cambiarValor(matriz,y,x,'3');
                            }
                            if ((*tesoros) == 2)
                            {
                                cambiarValor(matriz,y,x,'2');
                            }
                            if ((*tesoros) == 1)
                            {
                                cambiarValor(matriz,y,x,'1');
                            }
                            
                            (*tesoros) --;
                            creado = 1;
                        }
                        
                        

                    }
                    else{
                        cambiarValor(matriz,y,x,mapas[eliminar - 1][z]);
                    }
                    
                    
                }
                
                else
                {
                    if ((mapas[eliminar - 1][z] == '0') && ((*tesoros) != 0) && ((*mapas_generados) == 9)){
                        char rar;
                            if ((*tesoros) == 4)
                            {
                                cambiarValor(matriz,y,x,'4');
                            }
                            if ((*tesoros) == 3)
                            {
                                cambiarValor(matriz,y,x,'3');
                            }
                            if ((*tesoros) == 2)
                            {
                                cambiarValor(matriz,y,x,'2');
                            }
                            if ((*tesoros) == 1)
                            {
                                cambiarValor(matriz,y,x,'1');
                            }
                        (*tesoros) --;
                    }
                    else
                    {
                        cambiarValor(matriz,y,x,mapas[eliminar - 1][z]);
                    }
                    
                }
                
                x++;
                z++;
            }
            y++;
        }

    }
    
    
    return eliminar;

}

//Descipcion de la funcion: Elimina del arreglo con los mapas no usados un mapa ya utilizado
//      Parametros:
//          arreglo[]:                      Arreglo con los mapas no generados
//          tamano(int):                    Tamaño del arreglo con los mapas
//          elemento(int):                  Numero del mapa a eliminar
//      Retorno:
//          no tiene retorno
void eliminarElemento(int arreglo[], int *tamano, int elemento) {
    int encontrado = 0;

    for (int i = 0; i < *tamano; i++) {
        if (arreglo[i] == elemento) {
            encontrado = 1;

            for (int j = i; j < *tamano - 1; j++) {
                arreglo[j] = arreglo[j + 1];
            }

            (*tamano)--; 
            break; 
        }
    }

    if (!encontrado) {
        printf("El jugador no puede buscar.\n");
    }
}

// Descripción de la función: Carga mapas desde archivos de texto en una matriz bidimensional.
//      Parámetros:
//          mapas[8][26]: Un arreglo bidimensional que almacenará los mapas cargados.
//      Retorno:
//          no tiene retorno
void cargar_mapas(char mapas[8][26]){
    
    char mapa[30];
    char caracter;


    for (int i = 0; i < 8; i++)
    {
        snprintf(mapa, sizeof(mapa), "tablero%d.txt", i +1);
        FILE *archivo;
        archivo = fopen(mapa, "r");
        int j = 0;

        if (archivo == NULL) {
            perror("No se pudo abrir el archivo");
        }

        while ((caracter = fgetc(archivo)) != EOF){
            if (!(caracter == ' ' || caracter == '\n' || caracter == '\r')){
                
                mapas[i][j]= caracter;
                j++;
            }
            
        }
        
        mapas[i][j] = '\0';
        fclose(archivo);
    }
    

}

//Descipcion de la funcion: Se encarga de mover al jugador
//      Parametros:
//          matriz[FILAS][COLUMNAS](char):  Matriz a donde se realizan los cambios
//          direccion(char):                Direccion donde se mueve el jugador
//          jugador(struct jugador *):      Jugador que se mueve
//          player(char):                   Letra que represneta al jugador
//          pasos(int):                     Pasos que da el jugador
//          turnos(int *):                  Turnos del jugador
//          camaras(int *):                 Camaras activas
//          tp[8][2](int):                  Arreglo con las posiciones de los TP
//      Retorno:
//          no tiene retorno
void mover(char matriz[FILAS][COLUMNAS], char direccion, struct jugador *jugador, char player, int pasos, int *turnos, int *camaras, int tp[8][2]){
    //cargar tps
    for (int i = 0; i < 9; i++)
    {
        if (tp[i][0] + tp[i][1])
        {
            if (matriz[tp[i][1]][tp[i][0]] != 'a' && matriz[tp[i][1]][tp[i][0]] != 'b' && matriz[tp[i][1]][tp[i][0]] != 'c' && matriz[tp[i][1]][tp[i][0]] != 'd')
            {
                cambiarValor(matriz, tp[i][1],tp[i][0],'T');
            }
            
        }
        else
        {
            break;
        }
    }

    //direccion

    int x;
    int y;

    if (direccion == 'd')
    {
        x = 1;
        y = 0;
    }

    if (direccion == 'b')
    {
        x = 0;
        y = 1;
    }

    if (direccion == 'i')
    {
        x = -1;
        y = 0;
    }

    if (direccion == 'a')
    {
        x = 0;
        y = -1;
    }

    //movimiento

    for (int i = 0; i < pasos; i++)
    {
        if (matriz[(jugador->y) + y][(jugador-> x) + x] != '/' && matriz[(jugador->y) + y][(jugador-> x) + x] != 'E' && matriz[(jugador->y) + y][(jugador-> x) + x] != 'a' && matriz[(jugador->y) + y][(jugador-> x) + x] != 'b' && matriz[(jugador->y) + y][(jugador-> x) + x] != 'c' && matriz[(jugador->y) + y][(jugador-> x) + x] != 'd')
        {
            if (matriz[(jugador->y) + y][(jugador-> x) + x] == '1' || matriz[(jugador->y) + y][(jugador-> x) + x] == '2' || matriz[(jugador->y) + y][(jugador-> x) + x] == '3' || matriz[(jugador->y) + y][(jugador-> x) + x] == '4')
            {
                if (player == 'a' && matriz[(jugador->y) + y][(jugador-> x) + x] == '1')
                {
                    cambiarValor(matriz, jugador->y,jugador->x,'0');
                    jugador->tesoro = 1;
                    printf( "Tesoro Conseguido\n");
                    cambiarValor(matriz, (jugador->y) + y,(jugador->x) + x,'a');
                    jugador->x = (jugador-> x) + x;
                    jugador->y = (jugador-> y) + y;
                }
                if (player == 'b' && matriz[(jugador->y) + y][(jugador-> x) + x] == '2')
                {
                    cambiarValor(matriz, jugador->y,jugador->x,'0');
                    jugador->tesoro = 1;
                    printf( "Tesoro Conseguido\n");
                    cambiarValor(matriz, (jugador->y) + y,(jugador->x) + x,'b');
                    jugador->x = (jugador-> x) + x;
                    jugador->y = (jugador-> y) + y;
                }
                if (player == 'c' && matriz[(jugador->y) + y][(jugador-> x) + x] == '3')
                {
                    cambiarValor(matriz, jugador->y,jugador->x,'0');
                    jugador->tesoro = 1;
                    printf( "Tesoro Conseguido\n");
                    cambiarValor(matriz, (jugador->y) + y,(jugador->x) + x,'c');
                    jugador->x = (jugador-> x) + x;
                    jugador->y = (jugador-> y) + y;
                }
                if (player == 'd' && matriz[(jugador->y) + y][(jugador-> x) + x] == '4')
                {
                    cambiarValor(matriz, jugador->y,jugador->x,'0');
                    jugador->tesoro = 1;
                    printf( "Tesoro Conseguido\n");
                    cambiarValor(matriz, (jugador->y) + y,(jugador->x) + x,'d');
                    jugador->x = (jugador-> x) + x;
                    jugador->y = (jugador-> y) + y;
                }
                else
                {
                    printf("Este no es tu tesoro\n");
                    break;
                }
            }

            if (matriz[(jugador->y) + y][(jugador-> x) + x] == 'C')
            {
                cambiarValor(matriz, jugador->y,jugador->x,'0');
                cambiarValor(matriz, (jugador->y) + y,(jugador->x) + x,player);
                printf("Camara destruida\n");
                camaras--;
                jugador->x = (jugador-> x) + x;
                jugador->y = (jugador-> y) + y;
            }

            if (matriz[(jugador->y) + y][(jugador-> x) + x] == 'N')
            {
                cambiarValor(matriz, jugador->y,jugador->x,'0');
                cambiarValor(matriz, (jugador->y) + y,(jugador->x) + x,player);
                jugador->x = (jugador-> x) + x;
                jugador->y = (jugador-> y) + y;

                if ((*camaras) >= 2)
                {
                    printf("Perdiste estos turnos\n");
                }

                else
                {
                    printf("Ganaste 5 turnos\n");
                    turnos = turnos + 5;
                }
            }

            if (matriz[(jugador->y) + y][(jugador-> x) + x] == 'Q')
            {
                cambiarValor(matriz, jugador->y,jugador->x,'0');
                cambiarValor(matriz, (jugador->y) + y,(jugador->x) + x,player);
                printf("perdiste 3 turnos\n");
                turnos = turnos - 3;
                jugador->x = (jugador-> x) + x;
                jugador->y = (jugador-> y) + y;
            }

            else
            {
                cambiarValor(matriz, jugador->y,jugador->x,'0');
                cambiarValor(matriz, (jugador->y) + y,(jugador->x) + x,player);
                jugador->x = (jugador-> x) + x;
                jugador->y = (jugador-> y) + y;
            }
            
        }
        
        else
        {
            printf("No puedes seguir por aca\n");
            break;
        }
        
    }
    
    

}

//Descipcion de la funcion: Se encarga de mover un jugador a otro tp
//      Parametros:
//          matriz[FILAS][COLUMNAS](char):  Matriz a donde se realizan los cambios
//          jugador(struct jugador *):      Jugador que se teletransportar
//          player(char):                   Letra que represneta al jugador
//          tp[8][2](int):                  Arreglo con las posiciones de los TP
//      Retorno:
//          no tiene retorno
void teletransportar(char matriz[FILAS][COLUMNAS],struct jugador *jugador, char player, int tp[8][2]){

    int bool = 0;
    int eleccion;

    for (int i = 0; i < 8; i++)
    {
        if (!(tp[i][0] + tp[i][1]))
        {
            break;
        }

        if (tp[i][0] == jugador->x && tp[i][1] == jugador->y)
        {
            bool = 1;
        }
    }
    


    if (bool)
    {
        for (int i = 0; i < 8; i++)
        {
            if (!(tp[i][0] + tp[i][1]))
            {
                if (i == 1)
                {
                    printf("No hay tps disponibles\n");
                }
                
                break;

            }

            printf("%d) x = %d, y = %d\n", i+1, tp[i][0], tp[i][1]);
        }

        printf("Por favor, seleccione el numero del tp: ");
        scanf("%d", &eleccion);
    }


    
    else
    {
        printf("Aca no hay tps\n");
    }
    
    if ((tp[eleccion - 1][0] + tp[eleccion - 1][1]) == 0)
    {
        printf("Eleccion erronea, perdio esta jugada\n");
    }
    else
    {
        cambiarValor(matriz, tp[eleccion - 1][1],tp[eleccion - 1][0],player);
        jugador->x = tp[eleccion - 1][0];
        jugador->y = tp[eleccion - 1][1];

        printf("Tp completado\n");
    }
    
    

    //cargar tps
    for (int i = 0; i < 9; i++)
    {
        if (tp[i][0] + tp[i][1])
        {
            if (matriz[tp[i][1]][tp[i][0]] != 'a' && matriz[tp[i][1]][tp[i][0]] != 'b' && matriz[tp[i][1]][tp[i][0]] != 'c' && matriz[tp[i][1]][tp[i][0]] != 'd')
            {
                cambiarValor(matriz, tp[i][1],tp[i][0],'T');
            }
            
        }
        else
        {
            break;
        }
    }

}

//Descipcion de la funcion: Se encarga de abrir las escaleras al jugador
//      Parametros:
//          matriz[FILAS][COLUMNAS](char):  Matriz a donde se realizan los cambios
//          jugador(struct jugador *):      Jugador que aplica escaleras
//      Retorno:
//          no tiene retorno
void escalera(char matriz[FILAS][COLUMNAS],struct jugador *jugador){

    if (jugador->habilidad != 'E')
    {
        return;
    }
    
    
    if (matriz[(jugador->y) + 1][jugador->x] == 'E' || matriz[(jugador->y) - 1][jugador->x] == 'E' || matriz[jugador->y][(jugador->x) + 1] == 'E' || matriz[jugador->y][(jugador->x) - 1] == 'E')
    {
        
        if (matriz[(jugador->y) + 1][jugador->x] == 'E')
        {
            cambiarValor(matriz, (jugador->y) + 1,jugador->x,'0');
        }
        if (matriz[(jugador->y) - 1][jugador->x] == 'E')
        {
            cambiarValor(matriz, (jugador->y) - 1,jugador->x,'0');
        }
        if (matriz[(jugador->y)][(jugador->x) + 1] == 'E')
        {
            cambiarValor(matriz, jugador->y,(jugador->x) + 1,'0');
        }
        if (matriz[(jugador->y)][(jugador->x) - 1] == 'E')
        {
            cambiarValor(matriz, jugador->y,(jugador->x) - 1,'0');
        }
        
        printf("Escaleras abiertas\n");
    }
    else
    {
        printf("No hay escaleras cerca\n");
    }
}

//Descipcion de la funcion: Se encarga de desarrollar el turno del jugador
//      Parametros:
//          jugador(int):       Numero del jugador
//          carta(char):        Habilidad del jugador
//      Retorno:
//          retorna un entero que representa la accion del jugador
int turno(int jugador, char carta){
    int opcion;
    printf("Turno de jugador %d\n", jugador);
    if (carta=='E'){
        printf("La Carta del Jugador es Escalera\n");
    }else{
        printf("La Carta del Jugador es Busqueda\n");
    }
    printf("Elije una opcion:\n1) Mover Jugador\n2) Usar Carta\n");
    scanf("%d", &opcion);
    return opcion;
}