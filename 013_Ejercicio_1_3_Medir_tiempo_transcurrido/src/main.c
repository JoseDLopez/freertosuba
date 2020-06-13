/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "FreeRTOSConfig.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * MEDIR TIEMPO TRANSCURRIDO
 * Medir el tiempo de pulsación de un botón utilizando un algoritmos anti-rebote. Luego destellar un led durante el
 * tiempo medido. Ayuda: Se puede consultar el contador de ticks del RTOS para obtener el tiempo del sistema
 * (en ticks) al inicio y al fin del mismo. En este caso hay que prever que esta variable puede desbordar.
 */

/* Creación de tarea para encender el led */
void vTareaEncenderLed (void *pvParameters );
void vTareaBoton (void *pvParameters );

TickType_t get_diff();
void clear_diff();


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
	for(;;){
		TickType_t diff = get_diff();


		if (diff == 0){
			vTaskDelay(pdMS_TO_TICKS( 40));
		}else{
			gpioWrite( LEDB ,1 );
			vTaskDelay( diff );
			gpioWrite( LEDB ,0 );
			clear_diff();
		}
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
