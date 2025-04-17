# IIC2333-T1
Tarea 1 - Sistemas Operativos y Redes

## Integrantes:
- Cristóbal Albornoz
- Nicolás Ulloa (Retirado del ramo)

En este documento se detalla las características de la implementación del DCCScheduler, el cual se encuentra implementado a su totalidad Martes 16-04-2025 21:07

## Archivos
- /
    - Makefile
    - README.md
    - /src
        - file_manager (carpeta proveniente del código base de la T1)
        - DCCScheduler
            - estructuras.c
            - estructuras.h
            - logica.c
            - logica.h
            - main.c

## Ejecución
Para poder ejectuar el código se debe abrir la carpeta en el terminal al mismo nivel que el Makefile y ejecutar los siguientes comandos:
- `make`

Para poder compilar el programa
- `./DCCScheduler <input_file> <output_file> <q> <n>`

Para poder ejecutar el programa como tal, donde `input_file` y `output_file` son las rutas a los archivos de input y output respectivamente, y `q` es el "tamaño" del quantum y `n` los ticks que deben pasar para que los procesos suban de nivel en las colas

## Structs
Se implementan 2 structs principales y 2 que sirven de complemento a ellas:
- Proceso
    - nombre
    - pid
    - estado
    - tiempo_rafaga
    - n_rafagas
    - tiempo_espera_io
    - prioridad
    - t_inicio
    - t_finalizado
    - t_turnaround
    - t_response
    - t_entrada
    - t_waiting_total
    - t_ready_total
    - t_burst_total
    - t_burst_actual
    - t_waiting_actual
    - rafagas_hechas
    - cambios_cola
    - quantum_actual
    - entrado
    - cola_asignada
    - siguiente
    - siguiente_almacenamiento

- Queue
    - primero
    - tipo
    - quantum_base

- QueuType (enum)

- State (enum)

## Lógica y flujo del programa
Al momento de partir el programa, se lee el archivo de input, instanciando y almacenando los procesos en una lista ligada que hace uso del atributo `siguiente_almacenamiento` de los procesos, ordenada según tiempo de inicio.

De ahí, se instancias las colas y se da inicio a un while que permite hacer el loop de ejecución basado en el paso de `ticks`. Este solo se termina si la cantidad de procesos terminados es igual a la cantidad de procesos totales con los cuales trabaja el programa.

Dentro del loop ocurren las siguientes fases:
- Actualizar los contadores de los procesos en READY en las cola
- Actualizar los contadores y estado de ser necesario de procesos WAITING
- Manejar si la cpu está ocupada (proceso RUNNING)
    - Si la cola actual es HIGH o MEDIUM se revisa si terminó el burst o terminó el quantum, pudiendo sacar el proceso de la CPU en caso que alguno de estos acabe y mandarlo a la misma cola u la cola de abajo según el caso
    - Si la cola es LOW solo revisa que cumpla sus burst
    - Ambas consideran la opción de que el proceso termine
- Verifico si hay proceso que extraer del almacenamiento según el tick actual v/s el t_inicio y extraigo e insert en cola si corresponde
- Verifico se se cumple lo de los n_ticks para poder subir procesos de cola de ser necesario
- En caso de cpu desocupada extraigo según la lógica de las colas el primer proceso READY que encuentre y lo dejo en la CPU para poder ejecutarlo el próximo tick
