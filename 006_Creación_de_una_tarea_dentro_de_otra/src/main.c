#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * Ahora vamos a crear una tarea que dentro de ella crea otra tarea.
 * Esta segunda tarea además, después de ejecutrase, se elmina así misma.
 */



/* Declaración de las funciones que se ejecutan cuando una tarea apunta a ellas */
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );

/* Declaramos el handle de la tarea que queremos manipular */
TaskHandle_t xHandleDeTarea;


/* Declaración de texto a imprimir para las tareas creadas */
const char *pcTextTask1 = "Soy la tarea 1 corriendo cada 1000ms.\r\n";
const char *pcTextTask2 = "Soy la tarea 2 corriendo luego de la tarea 1.\r\n";


/*-----------------------------------------------------------*/
/* MAIN Principal, siempre debe existir. */

int main (){

	/**
	 * El primer paso siempre es crear las tareas y luego de crearlas se incia el programador (scheduler)
	 */

	/* Creación de la tarea 1 con prioridad 2 */
	xTaskCreate( vTask1, (const char *)"Tarea 1",configMINIMAL_STACK_SIZE, (void *)pcTextTask1, tskIDLE_PRIORITY+2,NULL );



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
	/* Acá recibimos el campo de texto enviado como parámetro, ya no es constante porque varía según lo recibido */
	char *pcTextoAMostrar = ( char * ) pvParameters;

	/* Inicializamos la variable tipo ticket xLastWakeTime */
	TickType_t xLastWakeTime;

	/* Inicializamos la variable de delay basado en los ms deseados */
	const TickType_t xDelay = pdMS_TO_TICKS( 1000 );

   /**
    * La variable xLastWakeTime va a tomar el valor actual del contador de tickets. Y luego pasará a ser controlada por vTaskDelayUntil()
    */
   xLastWakeTime = xTaskGetTickCount();

	for(;;){
		vPrintString( pcTextoAMostrar ); /* Imprimimos el texto que deseamos mostrar */

		vPrintString( "Creamos tarea 2 \r\n" ); /* Imprimimos el texto de que creamos la tarea 2 */

		/* Creación de la tarea 2, que es la tarea continua con prioridad 1, que es menor. Pero queremos controlar la tarea con un handle que va al final*/
		xTaskCreate( vTask2, (const char *) "Tarea 2", configMINIMAL_STACK_SIZE, (void *)pcTextTask2, tskIDLE_PRIORITY+3, &xHandleDeTarea);

		/**
		 * Con este delay nos aseguramos que se ejecute exactamente x ciclos despues
		 */
		vTaskDelayUntil (&xLastWakeTime, xDelay);

	}
}

void vTask2 (void *pvParameters ){
	/* Acá recibimos el campo de texto enviado como parámetro, ya no es constante porque varía según lo recibido */
	char *pcTextoAMostrar = ( char * ) pvParameters;

	for(;;){
		vPrintString( pcTextoAMostrar ); /* Imprimimos el texto que deseamos mostrar */

		/* Ahora eliminamos la tarea 2 */
		vPrintString( "Eliminamos la tarea 2\r\n" );
		vTaskDelete( xHandleDeTarea );
	}
}
