
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <stdbool.h>
#include <string.h> // strcmp, strcpy, etc
#include "../file_manager/manager.h"

#include "logica.h"



void actualizar_waiting_cola(struct queue* cola){
    struct proceso* proceso_actual = cola->primero;
    char* nombre_cola = NULL;
    if(cola->tipo == HIGH){
        nombre_cola = "HIGH";
    }
    else if(cola->tipo == MEDIUM){
        nombre_cola = "MEDIUM";
    }
    else if(cola->tipo == LOW){
        nombre_cola = "LOW";
    }
    if(proceso_actual != NULL){
        printf("    [* Actualizando WAITING en cola: %s]\n", nombre_cola);
        //printf(" - Actual: [%p]\n", proceso_actual);
        while(proceso_actual != NULL){
            if(proceso_actual->estado == WAITING){
            //if(strcmp(proceso_actual->estado, "WAITING")==0){
                // Condición para que terminó de esperar
                proceso_actual->t_waiting_actual++;
                if(proceso_actual->tiempo_espera_io == proceso_actual->t_waiting_actual){
                    // Cambiar estado a READY
                    printf("        [* Proceso: %s, PID: %d ha pasado a: READY]\n", proceso_actual->nombre, proceso_actual->pid);
                    proceso_actual->estado = READY;
                    //strcpy(proceso_actual->estado, "READY"); // Ver si esto sirve o asignar directamente el valor
                    proceso_actual->t_waiting_total += proceso_actual->t_waiting_actual;
                    proceso_actual->t_waiting_actual = 0;
                }
            }
            proceso_actual = proceso_actual->siguiente;
        }
    }  
}

void actualizar_espera_ready_cola(struct queue* cola){
    struct proceso* proceso_actual = cola->primero;
    char* nombre_cola = NULL;
    if(cola->tipo == HIGH){
        nombre_cola = "HIGH";
    }
    else if(cola->tipo == MEDIUM){
        nombre_cola = "MEDIUM";
    }
    else if(cola->tipo == LOW){
        nombre_cola = "LOW";
    }
    if(proceso_actual != NULL){
        printf("    [~ Actualizando READY en cola: %s]\n", nombre_cola);
        while(proceso_actual != NULL){
            if(proceso_actual->estado == READY){
            //if(strcmp(proceso_actual->estado, "READY")==0){
                printf("        [~ Proceso: %s, PID: %d con ready++]\n", proceso_actual->nombre, proceso_actual->pid);
                proceso_actual->t_ready_total++;
                if(!proceso_actual->entrado){
                    proceso_actual->t_response++;
                }
            }
            proceso_actual = proceso_actual->siguiente;
        }
    }
    
    
}

struct proceso* obtener_siguiente_proceso(struct queue* colaH, struct queue* colaM, struct queue* colaL){
    struct proceso* proceso_actual = NULL;
    // Verificar si hay procesos en la cola de alta prioridad
    proceso_actual = extraer_cola(colaH);
    if(proceso_actual == NULL){
        proceso_actual = extraer_cola(colaM);
        if(proceso_actual == NULL){
            proceso_actual = extraer_cola(colaL);
        }
    }
    return proceso_actual;
}

// GENERAR OUTPUT FILE
void generar_output_file(Proceso** procesos_terminados, const char* nombre_archivo, int n_procesos){
    FILE* archivo = fopen(nombre_archivo, "w");

    printf("\n\nNombre,PID,TurnaroundTime,ResponseTime,WaitingTime,CambiosCola\n");
    for(int i = 0; i < n_procesos; i++){
        Proceso* actual = procesos_terminados[i];
        actual->t_turnaround += actual->t_burst_total + actual->t_waiting_total + actual->t_ready_total;
        actual->t_waiting_total += actual->t_ready_total;
        printf("%s,%d,%d,%d,%d,%d\n",
            actual->nombre,
            actual->pid,
            actual->t_turnaround,
            actual->t_response,
            actual->t_waiting_total,
            actual->cambios_cola
        );
        fprintf(archivo, "%s,%d,%d,%d,%d,%d\n",
            actual->nombre,
            actual->pid,
            actual->t_turnaround,
            actual->t_response,
            actual->t_waiting_total,
            actual->cambios_cola
        );
    }
    fclose(archivo);

    printf("Información de procesos guardada en: %s\n", nombre_archivo);
}

void liberar_memoria(struct queue* colaH, struct queue* colaM, struct queue* colaL, struct proceso** procesos_terminados, int procesos_totales){
    // Liberar memoria de los procesos terminados
    for(int i = 0; i < procesos_totales; i++){
        free(procesos_terminados[i]);
    }
    free(procesos_terminados);
    free(colaH);
    free(colaM);
    free(colaL);
}
void printLL(struct proceso* head){
    if(head != NULL){
        printf("(>> Lista de procesos esperando su t_inicio:)\n");
        struct proceso* current = head;
        //printf(" > Cabeza: [%p]\n", current);
        while (current != NULL) {
            //printf(" > Actual: [%p]\n", current);
            printf("    (> Proceso: %s con t_inicio = (%d))\n", current->nombre, current->t_inicio);
            current = current->siguiente_almacenamiento;
        }
    }
    
    //printf("-----\n");
}

void logica_programa(struct queue* colaH, struct queue* colaM, struct queue* colaL, int procesos_totales, int n_ticks, Proceso* todos_procesos, const char* output){
    int tick_actual = 0;
    int n_procesos_terminados = 0;
    struct proceso** procesos_terminados = calloc(procesos_totales, sizeof(struct proceso*));
    bool programa_activo = true;
    bool cpu_ocupada = false;
    struct proceso* proceso_ejecutandose = NULL;
    //char cola_asignada = 'A';
    struct queue* cola_actual = NULL;
    // TODO: Quitar el || en adelante, es por tests
    while(programa_activo && tick_actual<30){
        tick_actual++;
        printf("\n[>> Tick]: %d ==========\n", tick_actual);
        printf("    [> CPU]: %s\n", cpu_ocupada ? "Ocupada" : "Libre");
/*         if(cpu_ocupada){
            printf("    [- Proceso: %s, PID: %d, Estado: %d]\n", proceso_ejecutandose->nombre, proceso_ejecutandose->pid, proceso_ejecutandose->estado);
        } */
        printf("    [> n_procesos_terminados]: [%d/%d]\n", n_procesos_terminados, procesos_totales);
        printLL(todos_procesos);
        /* printCola(colaH);
        printCola(colaM);
        printCola(colaL); */
        printf("[>> Actualizando Procesos Esperando en Colas]\n");
        actualizar_espera_ready_cola(colaH);
        actualizar_espera_ready_cola(colaM);
        actualizar_espera_ready_cola(colaL);
        printf("[>> Actualizando Procesos Esperando IO]\n");
        actualizar_waiting_cola(colaH);
        actualizar_waiting_cola(colaM);
        actualizar_waiting_cola(colaL);
        //==============================
        // Coloco esto aquí porque creo importante guardar el tiempo de espera en la cola
        
        //TODO: Ver si debo compensar con un - si lo selecciono
        //==============================
        // Manejo del proceso RUNNING

        if(cpu_ocupada){
            printf("[>> Procesando Ejecución del Proceso]\n");
            proceso_ejecutandose->t_burst_actual++;
            printf("    [- Proceso: %s, t_burst: (%d/%d) y burst realizados (%d//%d)]\n", proceso_ejecutandose->nombre, proceso_ejecutandose->t_burst_actual, proceso_ejecutandose->tiempo_rafaga, proceso_ejecutandose->rafagas_hechas, proceso_ejecutandose->n_rafagas);
            //printf("    [- Cola: %s]\n", proceso_ejecutandose->cola_asignada == HIGH ? "HIGH" : (proceso_ejecutandose->cola_asignada == MEDIUM ? "MEDIUM" : "LOW"));
            printf("    [- Cola actual: %s]\n", cola_actual->tipo == HIGH ? "HIGH" : (cola_actual->tipo == MEDIUM ? "MEDIUM" : "LOW"));
            if(cola_actual->tipo != LOW){
                cola_actual->quantum_actual++;
                // Caso de ejecución en quantum (READY, WAITING o FINISHED)
                printf("    [- Cola t_quantum : (%d/%d)]\n", cola_actual->quantum_actual, cola_actual->quantum_base);
                if(proceso_ejecutandose->t_burst_actual == proceso_ejecutandose->tiempo_rafaga){
                    proceso_ejecutandose->rafagas_hechas++;
                    if(proceso_ejecutandose->rafagas_hechas == proceso_ejecutandose->n_rafagas){
                        // Caso FINISHED
                        printf("        [- Proceso: %s, PID: %d y burst realizados (%d//%d) ha pasado a: FINISHED]\n", proceso_ejecutandose->nombre, proceso_ejecutandose->pid, proceso_ejecutandose->rafagas_hechas, proceso_ejecutandose->n_rafagas);
                        proceso_ejecutandose->estado = FINISHED;
                        //strcpy(proceso_ejecutandose->estado, "FINISHED");
                        // Añadir a la cola de procesos terminados
                        procesos_terminados[n_procesos_terminados] = proceso_ejecutandose;
                        n_procesos_terminados++;
                    }
                    else{
                        // Caso WAITING
                        printf("        [- Proceso: %s, PID: %d ha pasado a: WAITING]\n", proceso_ejecutandose->nombre, proceso_ejecutandose->pid);
                        proceso_ejecutandose->estado = WAITING;
                        //strcpy(proceso_ejecutandose->estado, "WAITING");
                        // TODO: Reinsertar en la cola de origen
                        insertar_cola(cola_actual, proceso_ejecutandose);
                    }
                    cpu_ocupada = false;
                    printf("    [- Liberando CPU]\n");
                    proceso_ejecutandose->t_burst_total += proceso_ejecutandose->t_burst_actual;
                    proceso_ejecutandose->t_burst_actual = 0;
                    if(cola_actual->quantum_actual == cola_actual->quantum_base){
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
                // TODO: Revisar si esto funciona
                else if(cola_actual->quantum_actual == cola_actual->quantum_base){
                    // Caso acaba el quantum sin terminar el burst
                    // Cambiar estado a READY
                    printf("    [- Se ha acabado el quantum, liberando CPU]\n");
                    printf("        [- Proceso: %s, PID: %d ha pasado a: READY]\n", proceso_ejecutandose->nombre, proceso_ejecutandose->pid);
                    
                    cpu_ocupada = false;
                    proceso_ejecutandose->estado = READY;
                    //strcpy(proceso_ejecutandose->estado, "READY");
                    // Debo bajar a cola de menor prioridad
                    struct queue* cola_baja = NULL;
                    if(proceso_ejecutandose->cola_asignada == HIGH){
                        cola_baja = colaM;
                        proceso_ejecutandose->cambios_cola++;
                        printf("        [- Bajando a cola MEDIUM]\n");
                    }
                    else if(proceso_ejecutandose->cola_asignada == MEDIUM){
                        cola_baja = colaL;
                        proceso_ejecutandose->cambios_cola++;
                        printf("        [- Bajando a cola LOW]\n");
                    }
                    else{
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
                if(proceso_ejecutandose->t_burst_actual == proceso_ejecutandose->tiempo_rafaga){
                    if(proceso_ejecutandose->rafagas_hechas == proceso_ejecutandose->n_rafagas){
                        // Caso FINISHED
                        printf("        [- Proceso: %s, PID: %d y burst realizados (%d//%d) ha pasado a: FINISHED]\n", proceso_ejecutandose->nombre, proceso_ejecutandose->pid, proceso_ejecutandose->rafagas_hechas, proceso_ejecutandose->n_rafagas);
                        proceso_ejecutandose->estado = FINISHED;
                        //strcpy(proceso_ejecutandose->estado, "FINISHED");
                        // Añadir a la cola de procesos terminados
                        procesos_terminados[n_procesos_terminados] = proceso_ejecutandose;
                        n_procesos_terminados++;
                    }
                    else{
                        // Caso WAITING
                        printf("        [- Proceso: %s, PID: %d ha pasado a: WAITING]\n", proceso_ejecutandose->nombre, proceso_ejecutandose->pid);
                        proceso_ejecutandose->estado = WAITING;
                        //strcpy(proceso_ejecutandose->estado, "WAITING");
                        // TODO: Reinsertar en la cola de origen
                        insertar_cola(cola_actual, proceso_ejecutandose);
                    }
                    printf("    [- Liberando CPU]\n");
                    cpu_ocupada = false;
                    proceso_ejecutandose->t_burst_total += proceso_ejecutandose->t_burst_actual;
                    proceso_ejecutandose->t_burst_actual = 0;
                }
                else{
                    // Caso donde burst todavía no termina
                    // Se sigue en ejecución, no se hace nada
                }
            }
        }
        //==============================
        // Veo si puedo ingresar procesos según su t_inicio

        todos_procesos = extraer_almacencamiento(todos_procesos, tick_actual, colaH, colaM, colaL);
        printCola(colaH);
        printCola(colaM);
        printCola(colaL);
        //==============================
        // Ahora verifico los "n" ticks y subo de cola
        if(tick_actual%n_ticks == 0){
            printf("[>> Tick (%d/%d), subiendo de Cola]\n", tick_actual, n_ticks);
            // Subo los de colaM
            struct proceso* proceso_subido = extraer_cola(colaM);
            while(proceso_subido != NULL){
                printf("    [+ Proceso: %s, PID: %d ha pasado a: HIGH]\n", proceso_subido->nombre, proceso_subido->pid);
                proceso_subido->cola_asignada = HIGH;
                insertar_cola(colaH, proceso_subido);
                proceso_subido->cambios_cola++;
                proceso_subido = extraer_cola(colaM); // Subo los de colaM
            }
            // Subo los de colaL
            proceso_subido = extraer_cola(colaL);
            while(proceso_subido != NULL){
                printf("    [+ Proceso: %s, PID: %d ha pasado a: MEDIUM]\n", proceso_subido->nombre, proceso_subido->pid);
                proceso_subido->cola_asignada = MEDIUM;
                proceso_subido->cambios_cola++;
                insertar_cola(colaM, proceso_subido);
                proceso_subido = extraer_cola(colaL); // Subo los de colaL
            }
        }
        //==============================
        // Ahora asigno proceso a CPU en caso de no haber
        if(!cpu_ocupada){
            printf("[>> Asignando Proceso a CPU]\n");
            proceso_ejecutandose = obtener_siguiente_proceso(colaH, colaM, colaL);
            
            if(proceso_ejecutandose == NULL){
                //Ver que hacer en este caso
                // Pues aquí todos estarían en WAITING o no hay procesos
                
                if(n_procesos_terminados == procesos_totales){
                    printf("    [!] Se han terminado todos los procesos, terminando programa >>>>>>>\n");
                    programa_activo = false;
                    break;
                }
                else{
                    printf("    [!] No hay procesos para ejecutar\n");
                    // No hago nada, dejo pasar el tick para que puedan esperar su IO
                    // Caso donde todos los procesos de las colas están en WAITING
                }
            }
            else{
                // ME da un resultado válido
                printf("    [-> Proceso: %s, PID: %d ha pasado a: RUNNING]\n", proceso_ejecutandose->nombre, proceso_ejecutandose->pid);
                cpu_ocupada = true;
                proceso_ejecutandose->entrado = true;
                if(proceso_ejecutandose->cola_asignada == HIGH){
                    cola_actual = colaH;
                    //cola_actual->quantum_actual++; // Activar si cuenta como que se empieza a ejecutar en ese tick
                }
                else if(proceso_ejecutandose->cola_asignada == MEDIUM){
                    cola_actual = colaM;
                    //cola_actual->quantum_actual++; // Activar si cuenta como que se empieza a ejecutar en ese tick
                }
                else{
                    cola_actual = colaL;
                }
                //proceso_ejecutandose->tiempo_espera_ready_total-=1; // Activar en caso de que empiece a ejecutar en este mismo tick
                // TODO: Aregar lógica en caso que se empiece a ejecutar en este mismo tick
                // Aquí asumo que no estoy ejecutando en el mismo tick, que debo esperar al siguiente
            }
        }
        
        //==============================
        if(n_procesos_terminados == procesos_totales){
            programa_activo = false;
            printf("    [!] Se han terminado todos los procesos, terminando programa >>>>>>>\n");
            break;
        }

    }

    // MAnejar Output
    generar_output_file(procesos_terminados, output, n_procesos_terminados);
    liberar_memoria(colaH, colaM, colaL, procesos_terminados, procesos_totales);
}


