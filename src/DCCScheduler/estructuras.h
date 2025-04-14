#pragma once
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <sys/types.h>
#include "../file_manager/manager.h"

typedef enum { HIGH, MEDIUM, LOW } QueueType;
typedef enum { READY, WAITING, RUNNING, FINISHED} State;

typedef struct proceso {
    char* nombre;
    pid_t pid;
    State estado;
    int tiempo_rafaga;
    int n_rafagas;
    int tiempo_espera_io;
    int prioridad;
    int tiempo_inicio
  } Proceso;

typedef struct queue {
    Proceso* primero;   // Primer proceso en la cola
    Proceso* ultimo;    // Último proceso en la cola
    QueueType tipo;
    int quantum;
} Queue;
