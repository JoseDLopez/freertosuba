#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * Nuestro primer ejercicio con colas. Vamos a crear una cola que se utilizará para recibir información y leerla.
 * Entonces crearemos 2 tareas que se encargaran de enviar información con datos distintos a la cola.
 * Y una tercera tarea que se encargará de leer la cola.
 */



/* Declaración de las funciones que se ejecutan cuando una tarea apunta a ellas */
void vTaskSend( void *pvParameters );
void vTaskReceive( void *pvParameters );

QueueHandle_t xQueue;


/*-----------------------------------------------------------*/
/* MAIN Principal, siempre debe existir. */

int main (){

	/**
	 * Anteriormente creabamos primero las tareas. Pero ahora se debe crear primero la cola y luego las tareas
	 */

	/* Esto crea una cola de longitud 5, y de tamaño por elemento de un int32_t	 */
	xQueue = xQueueCreate( 5, sizeof( int32_t ) );

	if (xQueue != NULL){ /* Si la cola fue creada efectivamente, si procedemos a crear las tareas */

		/* Tareas de envío de información */
		xTaskCreate(vTaskSend, (const char *)"Tarea 100", configMINIMAL_STACK_SIZE, (void *) 100, tskIDLE_PRIORITY+1,NULL);
		xTaskCreate(vTaskSend, (const char *)"Tarea 200", configMINIMAL_STACK_SIZE, (void *) 200, tskIDLE_PRIORITY+1,NULL);

		/* Tareas de recepción de información */
		xTaskCreate(vTaskReceive, (const char *)"Tarea Leer", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2,NULL);

		/* Inicio del programador de tareas (Scheduler) */
		vTaskStartScheduler();

	}

	for (;;); /* Cumple la misma funciona que un while (true) */
	return 0; /* El programa NUNCA debería llegar acá */
}
/*-----------------------------------------------------------*/



/**
 * Creación de funciones declaradas anteriormente que se encarga de ejecutar el programa cuando sea el turno de cada tarea
 */

void vTaskSend (void *pvParameters ){
	/* Acá recibimos el campo int_32 enviado como parámetro, ya no es constante porque varía según lo recibido */
	int32_t lValueToSend = ( int32_t ) pvParameters;

	/* Creamos una variable tipo status para conocer el estado de la cola al quere enviar */
	BaseType_t xStatus;



	for(;;){
		/* Ahora vamos a hacer el procedo de envío y veremos que sucede con la cola.
		 * El primer parametro indica la cola a la cual queremos enviar información.
		 * El segundo parametro indica el valor a enviar. (La dirección de la data que queremos enviar)
		 * Y el tercer parametro indica cuanto tiempo tiene que pasar la tarea bloqueada hasta recibir
		 * una respuesta de la cola. Si el parametro es 0, siempre va a estar bloqueada.
		 *
		 */
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0);

		if( xStatus != pdPASS){
			/* Si el estado de envío falla mostraremos el siguiente mensaje: */
			vPrintString("No fue posible agregar información a la cola. \r\n;");
		}

	}
}

void vTaskReceive (void *pvParameters ){
	/* Declaramos las variables que necesitamos utilizar.
	 * lReceiveValue es el valor que recibimos de la cola
	 * xStatus es el estado de la cola
	 * xTicksToWait nos indica cuantos tickets debe esperar la cola para recibir información que leer.
	 */

	int32_t lReceivedValue;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 ); /* Deseamos esperar 100 ms */


	for(;;){
		/* Lo siguiente nos indicará si la cola esta vacía cosa que no debería pasar pero si es así,
		 * nos mostrará este mensaje cada vez.
		 */
		if( uxQueueMessagesWaiting( xQueue ) != 0 ) {
			vPrintString( "La cola deberia estar vacía!\r\n" );
		}

		/* Ahora vamos a trabajar en recibir la información de la cola.
		 * El primer parametro es la cola a la que hacemos referencia.
		 * El segundo parametro es la dirección donde queremos guardar el valor recibido.
		 * El tercer valor indica cuantos tiempo va a durar la tarea bloqueada esperando data en la cola
		 */
		xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );

		/* Si fue posible leer la cola, recibiremos un pdPASS y mostraremos lo que recibimos.
		 * De lo contrario mostraremos un mensaje de error
		 */
		if( xStatus == pdPASS ) {
			 vPrintStringAndNumber( "Received = ", lReceivedValue );
		} else {
			 vPrintString( "No es posible leer de la cola.\r\n" );
		}
	}
}
