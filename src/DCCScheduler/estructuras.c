
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../file_manager/manager.h"

#include "estructuras.h"

void insertar_cola(Queue* cola, Proceso* nuevo){
    if(cola->primero == NULL){
        cola->primero = nuevo;
    }
    else{
        Proceso* actual = cola->primero;
        Proceso* previo = NULL;
        while(actual != NULL && actual->prioridad <= nuevo->prioridad){
            if(actual->pid < nuevo->pid){
                break;
            }
            previo = actual;
            actual = actual->siguiente;
        }
        // Caso donde prioridad es igual
        if(previo == NULL){
            nuevo->siguiente = cola->primero;
            cola->primero = nuevo;
        }
        else{
            previo->siguiente = nuevo;
            nuevo->siguiente = actual;
        }
        return;
    }
}

void insertar_colas(Proceso* nuevo, Queue* colaH, Queue* colaM, Queue* colaL){
    int priori = nuevo->prioridad;
    if(1<=priori && priori<=10){
        insertar_cola(colaH, nuevo);
    }
    else if(11<=priori && priori<=20){
        insertar_cola(colaM, nuevo);
    }
    else if(21<=priori && priori<=30){
        insertar_cola(colaL, nuevo);
    }
}

Proceso* extraer_cola(Queue* cola){
    if(cola->primero != NULL){
        Proceso* extraido = cola->primero;
        cola->primero = cola->primero->siguiente;
        return extraido;
    }
    return NULL;
}

Proceso* insertar_almacenamiento(Proceso* almacenamiento, Proceso* nuevo){
    printf("ALMAC [%p]\n", almacenamiento);
    if(almacenamiento == NULL){
        //printf("Creando el head de almacenamiento\n");
        almacenamiento = nuevo;
        //printf("NEO ALMAC [%p]\n", almacenamiento);
        return almacenamiento;
    }
    else{
        if(nuevo->t_inicio < almacenamiento->t_inicio){
            nuevo->siguiente_almacenamiento = almacenamiento;
            almacenamiento = nuevo;
            //printf("Actualizando el head de almacenamiento\n");
            return almacenamiento;
        }
        Proceso* actual = almacenamiento->siguiente_almacenamiento;
        Proceso* previo = almacenamiento;
        while(actual != NULL){
            if(nuevo->t_inicio < actual->t_inicio){
                nuevo->siguiente_almacenamiento = actual;
                previo->siguiente_almacenamiento = nuevo;
                return almacenamiento;
            }
            previo = actual;
            actual = actual->siguiente_almacenamiento;
        }
        previo->siguiente_almacenamiento = nuevo;
        return almacenamiento;
    }
}

Proceso* extraer_almacencamiento(Proceso* almacenamiento, int n_ticks, Queue* colaH, Queue* colaM, Queue* colaL){
    Proceso* actual = almacenamiento;
    
    if(actual != NULL){
        printf("==== ALMACENAMIENTO ====\n");
        printf("Viendo si extraigo para tick = (%d)\n", n_ticks);
        printf(" = Head actual t_inicio = (%d)\n", actual->t_inicio);
        while(actual != NULL && actual->t_inicio == n_ticks){
            Proceso* proceso_extraido = actual;
            printf("== Proceso extraido: %s\n", proceso_extraido->nombre);
            actual = actual->siguiente_almacenamiento;
            almacenamiento = actual;
            // TODO: Insertar en la colas
            insertar_colas(proceso_extraido, colaH, colaM, colaL);
        }
        printf("==== ------------ ====\n");
    }
    
    return almacenamiento;
}

void printCola(struct queue* cola){
    Proceso* actual = cola->primero;
    if(cola->tipo == HIGH){
        printf("[Cola Alta Prioridad]:\n");
    }
    else if(cola->tipo == MEDIUM){
        printf("[Cola Media Prioridad]:\n");
    }
    else if(cola->tipo == LOW){
        printf("[Cola Baja Prioridad]:\n");
    }
    while(actual != NULL){
        printf("    - Proceso: %s, PID: %d, Estado: %d\n", actual->nombre, actual->pid, actual->estado);
        actual = actual->siguiente;
    }
}