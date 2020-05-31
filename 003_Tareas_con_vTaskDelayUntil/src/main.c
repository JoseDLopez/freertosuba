#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * En los ejercicios anteriores creamos dos tareas que se ejecutaban cada 500ms, pero eso no es del todo cierto
 * ya que se ejecuta cada 500 ms desde la ultima vez que termino la ejecucución, es decir que si una tarea
 * tiene una duración de ejecución de 100ms, la tarea se va a ejecutar cada 600ms.
 * Para resolver este problema vamos a utilizar ahora la función xLastWakeTime, esta función se asegura
 * de que la tarea sea ejecutada exactamente cierta cantidad de tiempo, ya que toma en cuenta la ùltima vez que
 * la tarea despertó.
 *
 * Entonces la finalidad de este ejercicio es crear dos tareas que se ejecuten exactamente cada 500 ms.
 */



/* Declaración de las funciones que se ejecutan cuando una tarea apunta a ellas */
void vTaskPrintText( void *pvParameters );


/* Declaración de texto a imprimir para las tareas creadas */
const char *pcTextoTarea1 = "Soy la tarea 1 corriendo cada 500ms.\r\n";
const char *pcTextoTarea2 = "Soy la tarea 2 corriendo cada 500ms.\r\n";


/*-----------------------------------------------------------*/
/* MAIN Principal, siempre debe existir. */

int main (){

	/**
	 * El primer paso siempre es crear las tareas y luego de crearlas se incia el programador (scheduler)
	 */

	/* Creación de la tarea 1 */
	xTaskCreate( vTaskPrintText, (const char *)"Tarea 1",configMINIMAL_STACK_SIZE, (void *)pcTextoTarea1, tskIDLE_PRIORITY+1,NULL );

	/* Creación de la tarea 2 */
	xTaskCreate( vTaskPrintText, (const char *) "Tarea 2", configMINIMAL_STACK_SIZE, (void *)pcTextoTarea2, tskIDLE_PRIORITY+2, NULL);

	/* Inicio del programador de tareas (Scheduler) */
	vTaskStartScheduler();


	for (;;); /* Cumple la misma funciona que un while (true) */
	return 0; /* El programa NUNCA debería llegar acá */
}
/*-----------------------------------------------------------*/



/**
 * Creación de funciones declaradas anteriormente que se encarga de ejecutar el programa cuando sea el turno de cada tarea
 */

void vTaskPrintText (void *pvParameters ){
	/* Acá recibimos el campo de texto enviado como parámetro, ya no es constante porque varía según lo recibido */
	char *pcTextoAMostrar = ( char * ) pvParameters;

	/* Inicializamos la variable tipo ticket xLastWakeTime */
	TickType_t xLastWakeTime;

	/* Inicializamos la variable de delay basado en los ms deseados */
	const TickType_t xDelay = pdMS_TO_TICKS( 500 );

   /**
    * La variable xLastWakeTime va a tomar el valor actual del contador de tickets. Y luego pasará a ser controlada por vTaskDelayUntil()
    */
   xLastWakeTime = xTaskGetTickCount();

	for(;;){
		vPrintString( pcTextoAMostrar ); /* Imprimimos el texto que deseamos mostrar */

		/**
		 * Antes usabamos al función vTaskDelay, pero ahora vamos a usar la función vTaskDelayUntil, la cual espera dos parametros,
		 * un parametro es el contador de la ùltima vez que se despertó la tarea y el segundo valor es la cantidad de tickets que esperamos
		 * que dure bloqueada la tarea. Entonces procedemos con:
		 */
		vTaskDelayUntil (&xLastWakeTime, xDelay);
	}

}
