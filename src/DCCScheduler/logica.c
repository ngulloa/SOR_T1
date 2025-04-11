
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <stdbool.h>
#include <string.h> // strcmp, strcpy, etc
#include "../file_manager/manager.h"

#include "logica.h"

void actualizar_waiting_cola(struct queue* cola){
    struct process* proceso_actual = cola->cabeza;
    while(proceso_actual != NULL){
        if(strcmp(proceso_actual->estado, "WAITING")==0){
            // Condición para que terminó de esperar
            if(proceso_actual->tiempo_espera_IO == proceso_actual->tiempo_espera_actual){
                // Cambiar estado a READY
                strcpy(proceso_actual->estado, "READY"); // Ver si esto sirve o asignar directamente el valor
                proceso_actual->tiempo_espera_IO_total += proceso_actual->tiempo_espera_actual;
                proceso_actual->tiempo_espera_actual = 0;
            }
        }
    }
}

struct process* obtener_siguiente_proceso(struct queue* colaH, struct queue* colaM, struct queue* colaL){
    struct process* proceso_actual = NULL;
    // Verificar si hay procesos en la cola de alta prioridad
    proceso_actual = pop_cabeza(colaH);
    if(proceso_actual == NULL){
        proceso_actual = pop_cabeza(colaM);
        if(proceso_actual == NULL){
            proceso_actual = pop_cabeza(colaL);
        }
    }
    return proceso_actual;
}

void logica_programa(struct queue* colaH, struct queue* colaM, struct queue* colaL, int procesos_totales){
    int tick_actual = 0;
    int procesos_terminados = 0;
    bool programa_activo = true;
    bool cpu_ocupada = false;
    struct process* proceso_ejecutandose = NULL;

    while(programa_activo){
        actualizar_waiting_cola(colaH);
        actualizar_waiting_cola(colaM);
        actualizar_waiting_cola(colaL);

        // Manejar proceso running
        if(!cpu_ocupada){
            proceso_ejecutandose = obtener_siguiente_proceso(colaH, colaM, colaL);
            if(proceso_ejecutandose == NULL){
                //Ver que hacer en este caso
                // Pues aquí todos estarían en WAITING o no hay procesos
            }
            
            // Veo lógica de RUNNING
            // Ver si le sumo 1 aquí a su tiempo de CPU

            strcpy(proceso_ejecutandose->estado, "READY");
            if(proceso_ejecutandose->cola_asignada == 'L'){
                // Ver que se ejecute el burst
                // Posibilidades: Fin de burst o medio de burst
            }
            else{
                // Vero que se ejecute el burst y el quantum
                // Posibilidades: Fin de burst o fin de quantum o medio de burst y quantum
            }
            
        }
    }
}


void liberar_memoria(struct queue* colaH, struct queue* colaM, struct queue* colaL){
/*     // Liberar memoria de las colas
    struct process* temp;
    while (colaH->cabeza != NULL) {
        temp = colaH->cabeza;
        colaH->cabeza = colaH->cabeza->siguiente;
        free(temp);
    }
    free(colaH);

    while (colaM->cabeza != NULL) {
        temp = colaM->cabeza;
        colaM->cabeza = colaM->cabeza->siguiente;
        free(temp);
    }
    free(colaM);

    while (colaL->cabeza != NULL) {
        temp = colaL->cabeza;
        colaL->cabeza = colaL->cabeza->siguiente;
        free(temp);
    }
    free(colaL); */

    // Adaptar a la implementación
}