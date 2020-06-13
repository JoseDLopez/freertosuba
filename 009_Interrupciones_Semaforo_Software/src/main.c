#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * Vamos a hacer nuestro primer ejercicio con interrupciones.
 * Para ello vamos a usar un semáforo que va a ser tomado o soltado según ocurra la
 * interrupción. Esta interrupción la vamos a simular por software.
 *
 * Crearemos dos tareas una periodica (la que generará la interrupción) y la segunda
 * como handler que se encargará de realizar la acción cuando ocurra la interrupción.
 */

/* El número de interrupción para este ejemplo. Del 0 al 2 son usadas por
 * el puerto de FreeRTOS. Entonces podemos usar el primero disponible para
 * este caso.
 */
#define mainINTERRUPT_NUMBER 3


/* Creación de tareas */
void vTareaPeriodica (void *pvParameters );
void vTareaHandler (void *pvParameters );


/* Este es la rutina de interrupción simulada.
 * Es la interrupción a la cual la tarea se sincronizará.
 */
static uint32_t ulExampleInterruptHandler( void );


/* Declaramos una variable de tipo semaforo. Usada para
 * que cuando ocurra la interrupción, indique a la tarea
 * que la maneja que se debe ejecutar.
 */
SemaphoreHandle_t xBinarySemaphore;


/*-----------------------------------------------------------*/
/* MAIN Principal, siempre debe existir. */

int main (){
	/* Antes de poder usar un semaforó debemos crearlo explicitamente */
	xBinarySemaphore = xSemaphoreCreateBinary();

	/* Confirmamos que el semaforo haya sido creado correctamente */
	if(xBinarySemaphore != NULL){
		/* Instanciamos una tarea de Handler */
		xTaskCreate(vTareaHandler, (const char *)"Tarea Handler", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, NULL);

		/* Instanciamos la tarea periodica que genera una interrupción de software */
		xTaskCreate(vTareaPeriodica, (const char *)"Tarea Periodica", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);

		/* Instalamos el handler de la interrupción de software. Solo se usa para los casos simulados */
		vPortSetInterruptHandler( mainINTERRUPT_NUMBER, ulExampleInterruptHandler);

		/* Iniciamos el scheduler. */
		vTaskStartScheduler();
	}

	for (;;); /* Cumple la misma funciona que un while (true) */
	return 0; /* El programa NUNCA debería llegar acá */
}
/*-----------------------------------------------------------*/



/**
 * Creación de funciones declaradas anteriormente que se encarga de ejecutar el programa cuando sea el turno de cada tarea
 */
void vTareaHandler (void *pvParameters){
	for(;;){
		/* Si entramos acá tenemos que tratar de tomar el semaforo y si nos lo permite se desbloquea
		 * Va a estar bloqueada infinitamente hasta que sea capaz de tomar el semaforo.
		 */
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		vPrintString("Fui capaz de tomar el semáforo, es decir que estoy gestionando la tarea.\r\n");
	}
}

void vTareaPeriodica (void *pvParameters){
	/* Declaramos cada cuanto tiempo se va a ejecutar esta tarea periodica que simula la interrupción */
	const TickType_t xDelay500ms = pdMS_TO_TICKS( 500 );
	for(;;){
		/* Declaramos el vTaskDelay que simula la interrupción */
		vTaskDelay( xDelay500ms );
		vPrintString("Tarea periodica a punto de generar una interrupción.\r\n");
		vPortGenerateSimulatedInterrupt( mainINTERRUPT_NUMBER );
		vPrintString("Tarea periodica ejecuto la interrupción.\r\n\r\n\r\n");
	}
}

static uint32_t ulExampleInterruptHandler( void ){
	/* Esta es la función que maneja la interrupción generada */

	/* Declaramos la variable que ayuda a cambiar de contexto y la inicializamos en pdFALSE */
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	/* Ahora entregamos el semaforo con el sufijo de fromISR que aclara que es una función SAFE */
	xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );

	/* Si efectivamente el semaforo convierte la variable xHigherPriorityTaskWoken en pdTRUE
	 * esto le hace saber al YIELD que se debe cambiar de contexto inmediatamente
	 */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
