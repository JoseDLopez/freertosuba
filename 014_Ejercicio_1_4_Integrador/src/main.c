/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "FreeRTOSConfig.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * Ejercicio Integrador
 * Escribir un programa con dos tareas:
 * Tarea 1: Medirá el tiempo de pulsación de un botón, aplicando anti-rebote.
 * Tarea 2: Destellará un led con un período fijo de 1 seg, y tomando como tiempo de activación el último tiempo medido.
 * El tiempo medido se puede comunicar entre tareas a través de una variable global, protegiendo sus operaciones dentro de una sección crítica.
 */

/* Creación de tarea para encender el led */
void vTareaEncenderLed (void *pvParameters );
void vTareaBoton (void *pvParameters );

TickType_t get_diff();
void clear_diff();
TickType_t duty = 0;


/*------------------ MAIN PRINCIPAL --------------------------*/
int main (){

	/* Instanciamos una tarea encender led */
	BaseType_t res = xTaskCreate(vTareaEncenderLed, (const char *)"LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);

	/* Vemos si la tarea fue instanciada correctamente, si es asi iniciamos el scheduler */
	if(res == pdFAIL)
	{
		vPrintString("No fue posible instanciar la tarea de encender led");
	}else{
		res = xTaskCreate(vTareaBoton, (const char *)"BOTON", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
		if(res == pdFAIL){
			vPrintString("No fue posible instanciar la tarea de monitorizar el boton");
		}else{
			vTaskStartScheduler();
		}
	}

	for (;;);
	return 0;
}
/*------------------ FIN MAIN PRINCIPAL --------- -----------*/


/* ------------------ FUNCIONES ---------------------------- */
void vTareaEncenderLed (void *pvParameters){
	TickType_t xLastWakeTime; // Inicializamos LastWakeTime que nos permitirá tomar el ticket de la última vez que se inició la tarea
	xLastWakeTime = xTaskGetTickCount(); // Asignamos el valor de last tick count a LastWakeTime para que sea pasado a vTaskDelayUntil
	const TickType_t xDelay1000ms = pdMS_TO_TICKS( 1000 ); // Retardo de 1000ms para saber cada cuanto va a suceder esta tarea.

	for(;;){
		TickType_t diff = get_diff();
		if (diff == 0){
			vTaskDelay(pdMS_TO_TICKS( 40));
		}else{
			if (diff * portTICK_RATE_MS>900){
				diff = 900;
			}
			duty=diff;
		}
		gpioWrite( LEDB ,1 );
		vPrintString("Encendemos el led.\r\n");
		vTaskDelay( duty );
		gpioWrite( LEDB ,0 );
		vPrintString("Apagamos el led.\r\n");
		clear_diff();
		vTaskDelayUntil (&xLastWakeTime, xDelay1000ms);
	}
}

void vTareaBoton (void *pvParameters){
	fsmButtonInit(); // inicializamos la maquina de estado finito para lectura de tecla
	const TickType_t xDelay1ms = pdMS_TO_TICKS( 1 ); // Retardo de 1ms para saber cada cuanto va a suceder esta tarea.
	for(;;){
		fsmButtonUpdate( TEC1 ); // Leemos la tecla 1
		vTaskDelay( xDelay1ms );
	}
}
