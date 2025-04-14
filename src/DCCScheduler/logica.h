#pragma once
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../file_manager/manager.h"

#include "estructuras.h"

//void logica_programa(struct queue* colaH, struct queue* colaM, struct queue* colaL);

void actualizar_waiting_cola(struct queue* cola);

void actualizar_espera_ready_cola(struct queue* cola);

struct process* obtener_siguiente_proceso(struct queue* colaH, struct queue* colaM, struct queue* colaL);

void logica_programa(struct queue* colaH, struct queue* colaM, struct queue* colaL, int procesos_totales, int n_ticks);

void liberar_memoria(struct queue* colaH, struct queue* colaM, struct queue* colaL, struct process** procesos_terminados, int procesos_totales);