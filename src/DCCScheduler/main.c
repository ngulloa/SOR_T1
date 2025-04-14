#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../file_manager/manager.h"

#include "estructuras.h"

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	char *output_file_name = (char *)argv[2];
	int quantum = atoi(argv[3]);
	int n_ticks = atoi(argv[4]);
	struct nodo* todos_procesos = NULL;
	
	printf("output_file %s, quantum %d, n_ticks %d\n", output_file_name, quantum, n_ticks);
	InputFile *input_file = read_file(file_name);

	/*Mostramos el archivo de input en consola*/
	int n_procesos = input_file->len;
	printf("Cantidad de procesos: %d\n", input_file->len);
	printf("Procesos:\n");
	for (int i = 0; i < input_file->len; ++i)
	{
		struct proceso* nuevo_proceso = calloc(1, sizeof(struct proceso));
		char* nombre_proceso = input_file->lines[i][0];
		nuevo_proceso->nombre = nombre_proceso;
		nuevo_proceso->pid = atoi(input_file->lines[i][1]);
		nuevo_proceso->tiempo_inicio = atoi(input_file->lines[i][2]);
		nuevo_proceso->tiempo_rafaga = atoi(input_file->lines[i][3]);
		nuevo_proceso->n_rafagas = atoi(input_file->lines[i][4]);
		nuevo_proceso->tiempo_espera_io = atoi(input_file->lines[i][5]);
		nuevo_proceso->prioridad = atoi(input_file->lines[i][6]);
		nuevo_proceso->estado = READY;
		
/* 		printf("  NOMBRE_PROCESO: %s\n", input_file->lines[i][0]);
		printf("  PID: %s\n", input_file->lines[i][1]);
		printf("  T_INICIO: %s\n", input_file->lines[i][2]);
		printf("  T_CPU_BURST: %s\n", input_file->lines[i][3]);
		printf("  N_BURSTS: %s\n", input_file->lines[i][4]);
		printf("  IO_WAIT: %s\n", input_file->lines[i][5]);
		printf("  PRIORITY: %s\n", input_file->lines[i][6]); */

		printf("  NOMBRE_PROCESO: %s\n", nuevo_proceso->nombre);
		printf("  PID: %d\n", nuevo_proceso->pid);
		printf("  T_INICIO: %d\n", nuevo_proceso->tiempo_inicio);
		printf("  T_CPU_BURST: %d\n", nuevo_proceso->tiempo_rafaga);
		printf("  N_BURSTS: %d\n", nuevo_proceso->n_rafagas);
		printf("  IO_WAIT: %d\n", nuevo_proceso->tiempo_espera_io);
		printf("  PRIORITY: %d\n", nuevo_proceso->prioridad);
		printf("\n");

		// Agregar a la lista de procesos
		/* Debo incluir en la LL al final pero ordenado con los t_inicio
		Mover while(nuevo->t_inicio <=??? actual->t_inicio) 
		Ver condición de lo de ser NULL y que esto esté bien*/


		free(nuevo_proceso);
	}

	input_file_destroy(input_file);
}