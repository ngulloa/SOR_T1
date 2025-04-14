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
    struct proceso* anterior;
    int cambios_cola;
    QueueType cola_asignada;

    // Ticks
    int tick_inicio; // Tick en el que el proceso entra por primera vez a la cola, útil para el calculo de response time
    int tick_entrada; // Tick en el que entró a la CPU por primera vez. Útil para calcular response_time.
    // Los tiempos se medirán en cantidad de tics y luego multiplicar por el periodo
    int tick_finalizado; // Tick en el que el proceso finalizó, util para calcular turnaround time.
    int ticks_waiting; // Ticks totales en el que el proceso está en estado WAITING y READY
    int tiempo_response; // tiempo inicio - tiempo entrada

    // Tiempos netos
    double tiempo_rafaga_total;
    double tiempo_espera_io_total;
  } Proceso;

typedef struct queue {
    Proceso* primero;   // Primer proceso en la cola
    Proceso* ultimo;    // Último proceso en la cola
    QueueType tipo;
    int quantum_base;    /* Si es FIFO no será necesario */
    int quantum_actual;
} Queue;
