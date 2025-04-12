
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
            proceso_actual->tiempo_esperaIO_actual++;
            if(proceso_actual->tiempo_espera_IO == proceso_actual->tiempo_esperaIO_actual){
                // Cambiar estado a READY
                strcpy(proceso_actual->estado, "READY"); // Ver si esto sirve o asignar directamente el valor
                proceso_actual->tiempo_espera_IO_total += proceso_actual->tiempo_esperaIO_actual;
                proceso_actual->tiempo_esperaIO_actual = 0;
            }
        }
    }
}
void actualizar_espera_ready_cola(struct queue* cola){
    struct process* proceso_actual = cola->cabeza;
    while(proceso_actual != NULL){
        if(strcmp(proceso_actual->estado, "READY")==0){
            proceso_actual->tiempo_espera_ready_total++;
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


void logica_programa(struct queue* colaH, struct queue* colaM, struct queue* colaL, int procesos_totales, int n_ticks){
    int tick_actual = 0;
    int n_procesos_terminados = 0;
    struct process** procesos_terminados = calloc(procesos_totales, sizeof(struct process*));
    bool programa_activo = true;
    bool cpu_ocupada = false;
    struct process* proceso_ejecutandose = NULL;
    char cola_asignada = "";
    struct queue* cola_actual = NULL;

    while(programa_activo){
        tick_actual++;
        actualizar_waiting_cola(colaH);
        actualizar_waiting_cola(colaM);
        actualizar_waiting_cola(colaL);
        
        // Coloco esto aquí porque creo importante guardar el tiempo de espera en la cola
        actualizar_espera_ready_cola(colaH);
        actualizar_espera_ready_cola(colaM);
        actualizar_espera_ready_cola(colaL);
        // Manejo del proceso RUNNING
        if(cpu_ocupada){
            proceso_ejecutandose->tiempo_burst_actual++;
            if(cola_asignada != "L"){
                // Caso de ejecución en quantum (READY, WAITING o FINISHED)
                if(proceso_ejecutandose->tiempo_burst_actual == proceso_ejecutandose->tiempo_burst){
                    proceso_ejecutandose->rafagas_realizadas++;
                    if(proceso_ejecutandose->rafagas_realizadas == proceso_ejecutandose->n_bursts){
                        // Caso FINISHED
                        strcpy(proceso_ejecutandose->estado, "FINISHED");
                        // Añadir a la cola de procesos terminados
                        procesos_terminados[n_procesos_terminados] = proceso_ejecutandose;
                        n_procesos_terminados++;
                    }
                    else{
                        // Caso WAITING
                        strcpy(proceso_ejecutandose->estado, "WAITING");
                        // TODO: Reinsertar en la cola de origen
                        insertar_cola(cola_asignada, proceso_ejecutandose);
                    }
                    cpu_ocupada = false;
                    proceso_ejecutandose->tiempo_burst_total += proceso_ejecutandose->tiempo_burst_actual;
                    proceso_ejecutandose->tiempo_burst_actual = 0;
                    if(cola_actual->quantum_actual == cola_actual->quantum_queue){
                        // Caso se acabó el quantum
                        // Pero como se considera que terminó su ráfaga y por ello cede, no se reinicia quantum
                        // TODO: Ver qué pasa acá, si debo reiniciar el quantum o no
                        cola_actual->quantum_actual = 0;
                        // De momento asumiré que se reinicia, pues tiraría un proceso sin quantum
                    }
                    else{
                        // Caso no se acabó el quantum
                        // Aquí el quantum no se reinicia
                    }
                }
                else if(cola_actual->quantum_actual == cola_actual->quantum_queue){
                    // Caso acaba el quantum sin terminar el burst
                    // Cambiar estado a READY
                    strcpy(proceso_ejecutandose->estado, "READY");
                    // Debo bajar a cola de menor prioridad
                    struct queue* cola_baja = NULL;
                    if(cola_asignada == "H"){
                        cola_baja = colaM;
                    }
                    else if(cola_asignada == "M"){
                        cola_baja = colaL;
                    }
                    insertar_cola(cola_baja, proceso_ejecutandose);
                    // REiniciar quantum
                    cola_actual->quantum_actual = 0;
                }
                else{
                    // Caso donde quantum y burst todavía no terminan
                    // Se sigue en ejecución, no se hace nada
                }
            }
            else{
                // Caso de ejecución en cola baja (LOW)
                if(proceso_ejecutandose->tiempo_burst_actual == proceso_ejecutandose->tiempo_burst){
                    if(proceso_ejecutandose->rafagas_realizadas == proceso_ejecutandose->n_bursts){
                        // Caso FINISHED
                        strcpy(proceso_ejecutandose->estado, "FINISHED");
                        // Añadir a la cola de procesos terminados
                        procesos_terminados[n_procesos_terminados] = proceso_ejecutandose;
                        n_procesos_terminados++;
                    }
                    else{
                        // Caso WAITING
                        strcpy(proceso_ejecutandose->estado, "WAITING");
                        // TODO: Reinsertar en la cola de origen
                        insertar_cola(cola_asignada, proceso_ejecutandose);
                    }
                    cpu_ocupada = false;
                    proceso_ejecutandose->tiempo_burst_total += proceso_ejecutandose->tiempo_burst_actual;
                    proceso_ejecutandose->tiempo_burst_actual = 0;
                }
                else{
                    // Caso donde burst todavía no termina
                    // Se sigue en ejecución, no se hace nada
                }
            }
        }
        // Ahora verifico los "n" ticks y subo de cola
        if(tick_actual%n_ticks == 0){
            // Subo los de colaM
            struct process* proceso_subido = pop_cabeza(colaM);
            while(proceso_subido != NULL){
                proceso_subido->cola_asignada = 'H';
                insertar_cola(colaH, proceso_subido);
                proceso_subido = pop_cabeza(colaM); // Subo los de colaM
            }
            // Subo los de colaL
            struct process* proceso_subido = pop_cabeza(colaL);
            while(proceso_subido != NULL){
                proceso_subido->cola_asignada = 'M';
                insertar_cola(colaM, proceso_subido);
                proceso_subido = pop_cabeza(colaL); // Subo los de colaL
            }
        }
        // Ahora asigno proceso a CPU en caso de no haber
        if(!cpu_ocupada){
            proceso_ejecutandose = obtener_siguiente_proceso(colaH, colaM, colaL);
            if(proceso_ejecutandose == NULL){
                //Ver que hacer en este caso
                // Pues aquí todos estarían en WAITING o no hay procesos
                if(procesos_terminados == procesos_totales){
                    programa_activo = false;
                    break;
                }
                else{
                    // No hago nada, dejo pasar el tick para que puedan esperar su IO
                    // Caso donde todos los procesos de las colas están en WAITING
                }
            }
            else{
                // ME da un resultado válido
                char cola_asignada = proceso_ejecutandose->cola_asignada;
                struct queue* cola_actual = NULL;
                if(cola_asignada == 'H'){
                    cola_actual = colaH;
                    //cola_actual->quantum_actual++; // Activar si cuenta como que se empieza a ejecutar en ese tick
                }
                else if(cola_asignada == 'M'){
                    cola_actual = colaM;
                    //cola_actual->quantum_actual++; // Activar si cuenta como que se empieza a ejecutar en ese tick
                }
                else{
                    cola_actual = colaL;
                }
                //proceso_ejecutandose->tiempo_espera_ready_total-=1; // Activar en caso de que empiece a ejecutar en este mismo tick
                // TODO: Aregar lógica en caso que se empiece a ejecutar en este mismo tick
                // ...
            }
        }

    }

    // MAnejar Output
    FILE* archivo = fopen("output.txt", "w");
    // Ver qué hacer aquí
    fclose(archivo);
    liberar_memoria(colaH, colaM, colaL, procesos_terminados, procesos_totales);
}


void liberar_memoria(struct queue* colaH, struct queue* colaM, struct queue* colaL, struct process** procesos_terminados, int procesos_totales){
    // Liberar memoria de los procesos terminados
    for(int i = 0; i < procesos_totales; i++){
        free(procesos_terminados[i]);
    }
    free(procesos_terminados);
    free(colaH);
    free(colaM);
    free(colaL);
}