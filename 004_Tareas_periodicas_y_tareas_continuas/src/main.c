#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * Ahora vamos a jugar con el concepto de taras periodicas y continuas. Una tarea periodica es aquella que se ejecuta
 * cada cierta cantidad de tiempo (las que hemos hecho anteriormente) y una tarea continua se ejecuta siempre que
 * el programador le asigne tiempo del procesador.
 *
 * Entonces se debe tomar en cuenta que una tarea continua NUNCA puede tener una prioridad alta con respecto a las otras
 * tareas ya que de ser así nunca va a permitir que esta tarea con menor prioridad se ejecute.
 *
 * Por lo tanto una tarea continua siempre debe tener la menor prioridad.
 *
 * Ahora queremos crear dos tareas periodicas y una continua para ver como se comportan.
 *
 * Juega con la prioridad de las tareas para que veas como cambia la salida.
 */



/* Declaración de las funciones que se ejecutan cuando una tarea apunta a ellas */
void vTareaPeriodica( void *pvParameters );
void vTareaContinua( void *pvParameters );


/* Declaración de texto a imprimir para las tareas creadas */
const char *pcTextoTarea1 = "Soy la tarea 1 corriendo cada 500ms.\r\n";
const char *pcTextoTarea2 = "Soy la tarea 2 corriendo cada 500ms.\r\n";
const char *pcTextoTarea3 = "Soy la tarea 3 y siempre que puedo me ejecuto\r\n";


/*-----------------------------------------------------------*/
/* MAIN Principal, siempre debe existir. */

int main (){

	/**
	 * El primer paso siempre es crear las tareas y luego de crearlas se incia el programador (scheduler)
	 */

	/* Creación de la tarea 1 */
	xTaskCreate( vTareaPeriodica, (const char *)"Tarea 1",configMINIMAL_STACK_SIZE, (void *)pcTextoTarea1, tskIDLE_PRIORITY+2,NULL );

	/* Creación de la tarea 2 */
	xTaskCreate( vTareaPeriodica, (const char *) "Tarea 2", configMINIMAL_STACK_SIZE, (void *)pcTextoTarea2, tskIDLE_PRIORITY+3, NULL);

	/* Creación de la tarea 3, que es la tarea continua */
	xTaskCreate( vTareaContinua, (const char *) "Tarea 3", configMINIMAL_STACK_SIZE, (void *)pcTextoTarea3, tskIDLE_PRIORITY+1, NULL);

	/* Inicio del programador de tareas (Scheduler) */
	vTaskStartScheduler();


	for (;;); /* Cumple la misma funciona que un while (true) */
	return 0; /* El programa NUNCA debería llegar acá */
}
/*-----------------------------------------------------------*/



/**
 * Creación de funciones declaradas anteriormente que se encarga de ejecutar el programa cuando sea el turno de cada tarea
 */

void vTareaPeriodica (void *pvParameters ){
	/* Acá recibimos el campo de texto enviado como parámetro, ya no es constante porque varía según lo recibido */
	char *pcTextoAMostrar = ( char * ) pvParameters;

	/* Inicializamos la variable tipo ticket xLastWakeTime */
	TickType_t xLastWakeTime;

	/* Inicializamos la variable de delay basado en los ms deseados */
	const TickType_t xDelay = pdMS_TO_TICKS( 200 );

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

void vTareaContinua (void *pvParameters ){
	/* Acá recibimos el campo de texto enviado como parámetro, ya no es constante porque varía según lo recibido */
	char *pcTextoAMostrar = ( char * ) pvParameters;

	for(;;){
		vPrintString( pcTextoAMostrar ); /* Imprimimos el texto que deseamos mostrar */
	}
}
