#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * En el siguiente ejercicio vamos a comprobar como una tarea puede ser modificada
 * luego de haber sido creada. Se va crear dos tareas donde ambas modifican la prioridad
 * de una, ya sea subiendole o bajandole prioridad.
 */



/* Declaración de las funciones que se ejecutan cuando una tarea apunta a ellas */
void vTareaPeriodica( void *pvParameters );
void vTareaContinua( void *pvParameters );

/* Declaramos el handle de la tarea que queremos manipular */
TaskHandle_t xTareaContinuaHandle;


/* Declaración de texto a imprimir para las tareas creadas */
const char *pcTextoTarea1 = "Soy la tarea 1 corriendo cada 500ms.\r\n";
const char *pcTextoTarea2 = "Soy la tarea 2 corriendo cada 500ms.\r\n";


/*-----------------------------------------------------------*/
/* MAIN Principal, siempre debe existir. */

int main (){

	/**
	 * El primer paso siempre es crear las tareas y luego de crearlas se incia el programador (scheduler)
	 */

	/* Creación de la tarea 1 con prioridad 2 */
	xTaskCreate( vTareaPeriodica, (const char *)"Tarea 1",configMINIMAL_STACK_SIZE, (void *)pcTextoTarea1, tskIDLE_PRIORITY+2,NULL );

	/* Creación de la tarea 2, que es la tarea continua con prioridad 1, que es menor. Pero queremos controlar la tarea con un handle que va al final*/
	xTaskCreate( vTareaContinua, (const char *) "Tarea 2", configMINIMAL_STACK_SIZE, (void *)pcTextoTarea2, tskIDLE_PRIORITY+1, xTareaContinuaHandle);

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
	/* Declaramos una variable tipo UBASE que tenga la prioridad */
	UBaseType_t uxPriority;

	/* Obtenemos la prioridad de la propia tarea con la siguiente función enviando NULL */
	uxPriority = uxTaskPriorityGet( NULL );


	/* Acá recibimos el campo de texto enviado como parámetro, ya no es constante porque varía según lo recibido */
	char *pcTextoAMostrar = ( char * ) pvParameters;

	/* Inicializamos la variable tipo ticket xLastWakeTime */
	TickType_t xLastWakeTime;

	/* Inicializamos la variable de delay basado en los ms deseados */
	const TickType_t xDelay = pdMS_TO_TICKS( 100 );

   /**
    * La variable xLastWakeTime va a tomar el valor actual del contador de tickets. Y luego pasará a ser controlada por vTaskDelayUntil()
    */
   xLastWakeTime = xTaskGetTickCount();

	for(;;){
		vPrintString( pcTextoAMostrar ); /* Imprimimos el texto que deseamos mostrar */

		vPrintString( "Subimos la prioridad de la tarea continua\r\n" );
		vTaskPrioritySet( xTareaContinuaHandle, ( uxPriority + 1 ) );

		/**
		 * Antes usabamos al función vTaskDelay, pero ahora vamos a usar la función vTaskDelayUntil, la cual espera dos parametros,
		 * un parametro es el contador de la ùltima vez que se despertó la tarea y el segundo valor es la cantidad de tickets que esperamos
		 * que dure bloqueada la tarea. Entonces procedemos con:
		 */
		vTaskDelayUntil (&xLastWakeTime, xDelay);
	}
}

void vTareaContinua (void *pvParameters ){
	/* Declaramos una variable tipo UBASE que tenga la prioridad */
	UBaseType_t uxPriority;

	/* Obtenemos la prioridad de la propia tarea con la siguiente función enviando NULL */
	uxPriority = uxTaskPriorityGet( NULL );

	/* Acá recibimos el campo de texto enviado como parámetro, ya no es constante porque varía según lo recibido */
	char *pcTextoAMostrar = ( char * ) pvParameters;

	for(;;){
		vPrintString( pcTextoAMostrar ); /* Imprimimos el texto que deseamos mostrar */
		/* Bajamos la priodidad de la tarea con -2 */
		vPrintString( "Bajamos la prioridad de la tarea\r\n" );
		vTaskPrioritySet( NULL, ( uxPriority - 1 ) );
	}
}
