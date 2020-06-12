#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * Ahora vamos a enviar a la cola una estructura en vez de un simple dato.
 * Esto nos permite tener más información, como por ejemplo de quien viene la data.
 * Entonces vamos a crear 2 tareas que envian datos a la cola con su respectivo identificador
 * y una tercera tarea que lee la información y dependiendo del identificador toma una acción.
 */



/* Declaración de las funciones que se ejecutan cuando una tarea apunta a ellas */
void vTaskSend( void *pvParameters );
void vTaskReceive( void *pvParameters );

/* Declaración de la variable QueueHandle_t, es usada para guardar la cola
 * y puede ser accsible por las tres tareas */
QueueHandle_t xQueue;

/* Creamos la estructura de tipo de fuente de datos */
typedef enum {
   Motor,
   Giroscopio
} DataSource_t;

/* Ahora vamos a definir la estructura de lo que vamos a pasar en la cola */
typedef struct {
	uint8_t uValue;
	DataSource_t eDataSource;
} Data_t;

/* Declaramos dos variables de tipo de dato que seran enviado a la cola de ejemplo */
static const Data_t xStructsToSend[ 2 ] = {
		{ 100, Motor },  /* Lo que va a enviar la tarea 1 */
		{ 200, Giroscopio } /* Lo que va a enviar la tarea 2 */
};

/*-----------------------------------------------------------*/
/* MAIN Principal, siempre debe existir. */

int main (){

	/* Creamos la cola con su longitud, PERO del tamaño de la estructura */
	xQueue = xQueueCreate( 5, sizeof( Data_t ) );

	if (xQueue != NULL){ /* Si la cola fue creada efectivamente, si procedemos a crear las tareas */

		/* Tareas de envío de información */
		xTaskCreate(vTaskSend, (const char *)"Tarea 100", configMINIMAL_STACK_SIZE, (void *) 100, tskIDLE_PRIORITY+1,NULL);
		xTaskCreate(vTaskSend, (const char *)"Tarea 200", configMINIMAL_STACK_SIZE, (void *) 200, tskIDLE_PRIORITY+1,NULL);

		/* Creamos dos instancias de tarea que envia información,
		 * pero cada una de ellas enviará algo distinto que viene de la estructura de data a enviar
		 */
		xTaskCreate(vTaskSend, (const char *)"EnviaMotor", configMINIMAL_STACK_SIZE, (void *) &( xStructsToSend[0]), tskIDLE_PRIORITY+2, NULL);
		xTaskCreate(vTaskSend, (const char *)"EnviaGiroscopio", configMINIMAL_STACK_SIZE, (void *) &( xStructsToSend[1]), tskIDLE_PRIORITY+2, NULL);


		/* Ahora creamos la tarea que recibe información */
		xTaskCreate(vTaskReceive, (const char *)"Tarea Leer", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1,NULL);

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

	/* Creamos una variable tipo status para conocer el estado de la cola al quere enviar */
	BaseType_t xStatus;

	/* Creamos la variable de espera para enviar */
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );



	for(;;){
		/* Ahora vamos a hacer el procedo de envío y veremos que sucede con la cola.
		 * El primer parametro indica la cola a la cual queremos enviar información.
		 * El segundo argumento indica la estrcutura de data que enviamos que en este caso viene del pvParameters
		 * Y el tercer parametro indica cuanto tiempo tiene que pasar la tarea bloqueada hasta recibir
		 * una respuesta de la cola. Si el parametro es 0, siempre va a estar bloqueada.
		 *
		 */
		xStatus = xQueueSendToBack( xQueue, pvParameters, xTicksToWait );

		if( xStatus != pdPASS){
			/* Si el estado de envío falla mostraremos el siguiente mensaje: */
			vPrintString("No fue posible agregar información a la cola. \r\n;");
		}

	}
}

void vTaskReceive (void *pvParameters ){
	Data_t xReceivedStruct;
	BaseType_t xStatus;


	for(;;){
		/* Lo siguiente nos indicará si la cola esta vacía cosa que no debería pasar pero si es así,
		 * nos mostrará este mensaje cada vez.
		 */
		if( uxQueueMessagesWaiting( xQueue ) != 5 ) {
			vPrintString( "La cola deberia estar vacía!\r\n" );
		}

		/* Ahora vamos a trabajar en recibir la información de la cola.
		 * El primer parametro es la cola a la que hacemos referencia.
		 * El segundo parametro es la dirección donde queremos guardar el valor recibido.
		 * El tercer valor indica cuantos tiempo va a durar la tarea bloqueada esperando data en la cola
		 */
		xStatus = xQueueReceive( xQueue, &xReceivedStruct, 0 );

		/* Si fue posible leer la cola, recibiremos un pdPASS y mostraremos lo que recibimos.
		 * De lo contrario mostraremos un mensaje de error
		 */
		if( xStatus == pdPASS ) {
			if( xReceivedStruct.eDataSource == Motor ) {
				vPrintStringAndNumber( "Leido del motor = ", xReceivedStruct.uValue );
			} else {
				vPrintStringAndNumber( "Leido del Giroscopio = ", xReceivedStruct.uValue );
			}
		} else {
			 vPrintString( "No es posible leer de la cola.\r\n" );
		}
	}
}
