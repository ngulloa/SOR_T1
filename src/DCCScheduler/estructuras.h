#pragma once
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <sys/types.h>
#include "../file_manager/manager.h"

typedef enum { HIGH, MEDIUM, LOW } QueueType;
typedef enum { READY, WAITING, RUNNING, FINISHED} State;

typedef struct proceso {
    // Variables de enunciado
    char* nombre;
    pid_t pid;
    State estado;
    int tiempo_rafaga;    // Tiempo de ejecución por ráfaga
    int n_rafagas;        // Número de ejecución de ráfagas de ejecución en CPU
    int tiempo_espera_io; // Tiempo de espera entre ráfagas
    int prioridad;
    
    // Variables propias
    struct proceso* siguiente;
    struct proceso* siguiente_almacenamiento; // Para almacenar en la cola de procesos terminados

    // Ticks
    int t_inicio; // Tick en el que el proceso entra por primera vez a la cola, útil para el calculo de response time
    int t_finalizado; // Tick en el que el proceso finalizó, util para calcular turnaround time.
    int t_turnaround;

    int t_response; // tiempo inicio - tiempo entrada
    int t_entrada; // Tick en el que entró a la CPU por primera vez. Útil para calcular response_time.
    
    // Tiempos netos (alternativa a calcular el turnaround)
    int t_waiting_total; // Ticks totales en el que el proceso está en estado WAITING y READY
    int t_ready_total;
    int t_burst_total;
    // Tiempos actuales (permiten guardar el valor del burst/IO actual con tal de comparar con valor general)
    int t_burst_actual;
    int t_waiting_actual;

    int rafagas_hechas;

    int cambios_cola;
    QueueType cola_asignada;    
  } Proceso;


typedef struct queue {
    Proceso* primero;   // Primer proceso en la cola
    //Proceso* ultimo;    // Último proceso en la cola
    QueueType tipo;
    int quantum_base;    /* Si es FIFO no será necesario */
    int quantum_actual;
} Queue;


void insertar_cola(Queue* cola, Proceso* nuevo);

Proceso*  extraer_cola(Queue* cola);


void insertar_almacenamiento(Proceso* almacenamiento, Proceso* nuevo);

void extraer_almacencamiento(Proceso* almacenamiento, int n_ticks, Queue* colaH, Queue* colaM, Queue* colaL);