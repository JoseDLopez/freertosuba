#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"

/**
 * El objetivo de este ejercicio es crear dos tareas con la misma prioridad que impriman un texto cada 500 ms
 */



/* Declaración de las funciones que se ejecutan cuando una tarea apunta a ellas */
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );


/*-----------------------------------------------------------*/
/* MAIN Principal, siempre debe existir. */

int main (){

	/**
	 * El primer paso siempre es crear las tareas y luego de crearlas se incia el programador (scheduler)
	 */

	/* Creación de la tarea 1 */
	xTaskCreate( vTask1,              /* Puntero que indica que función se va a ejecutar según las instrucciones de esta tarea. */
			(const char *)"Tarea 1",   /* Nombre de la tarea, la finalidad es facilitar el debugging. */
			configMINIMAL_STACK_SIZE, /* Profundidad de la pila. Con el valor mínimo de la pila debería andar. */
			NULL,                     /* Si se desea enviar un parámetro a la tarea se usa este campo, de lo contrario se envía NULL */
			tskIDLE_PRIORITY+1,       /* Prioridad de la tarea, se inicializa con IDLE priority + 1, es decir esta tarea tiene prioridad 1. */
			NULL );                   /* En caso de que se quiera manipular la tarea mientras esta se ejecuta se envía en este campo el handle, de lo contrario se envía NULL */

	/* Creación de la tarea 2 */
	xTaskCreate( vTask2, (const char *) "Tarea 2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);

	/* Inicio del programador de tareas (Scheduler) */
	vTaskStartScheduler();


	for (;;); /* Cumple la misma funciona que un while (true) */
	return 0; /* El programa NUNCA debería llegar acá */
}
/*-----------------------------------------------------------*/



/**
 * Creación de funciones declaradas anteriormente que se encarga de ejecutar el programa cuando sea el turno de cada tarea
 */

void vTask1 (void *pvParameters ){
	const char *pcTextoAMostrar = "Soy la tarea 1 corriendo\r\n"; /* Declaración del texto que deseamos mostrar cuando ocurra la tarea */

	/**
	 * Como deseamos que la tarea se ejecute cierta cantidad de tiempo y se bloquee mientras no suceda ese tiempo, utilizamos primero la función pdMS_TO_TICKS
	 * para que convierta el valor en ms a un correspondiente en tickets, cuyo resultado usaremos despues para generar el delay por desbordamiento de tickets
	 * que necesitamos para poder bloquear la tarea por el tiempo deseado.
	 */
	const TickType_t xDelay = pdMS_TO_TICKS( 500 );

	for(;;){
		vPrintString( pcTextoAMostrar ); /* Imprimimos el texto que deseamos mostrar */
		vTaskDelay( xDelay ); /* Bloqueamos la tarea la cantidad de tickets que resultan de la conversión de ms to tickts */
	}

}


void vTask2 (void *pvParameters ){
	const char *pcTextoAMostrar = "Soy la tarea 2 corriendo\r\n";
	const TickType_t xDelay = pdMS_TO_TICKS( 500 );

	for(;;){
			vPrintString( pcTextoAMostrar );
			vTaskDelay( xDelay );
		}

}
