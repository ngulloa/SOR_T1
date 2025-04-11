#pragma once
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <sys/types.h>
#include "../file_manager/manager.h"

typedef enum { HIGH, MEDIUM, LOW } QueueType;
typedef enum { READY, WAITING, RUNNING, FINISHED} State;

typedef struct proceso {
    char nombre[256];
    pid_t pid;
    State estado;
    time_t tiempo_rafagas;
    int rafagas_ejecutando;
    time_t tiempo_espera_io;
    int prioridad;
  } Proceso;

typedef struct queue {
    Proceso* primero;   // Primer proceso en la cola
    Proceso* ultimo;    // Último proceso en la cola
    QueueType tipo;
    int quantum;
} Queue;
