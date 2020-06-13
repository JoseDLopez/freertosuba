#include "stdio.h"
/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"

/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * PERIODOS FIJOS
 * Problema: Implementar una tarea que genere una onda cuadrada (y que encienda un LED) con periodo de 1 seg y ciclos
 * de actividad incrementándose T1 = 100 ms, T2 = 200 ms, T3 = 300 ms, .. T9 = 900ms
 */

/* Creación de tarea para encender el led */
void vTareaEncenderLed (void *pvParameters );


/*------------------ MAIN PRINCIPAL --------------------------*/
int main (){

	/* Instanciamos una tarea encender led */
	BaseType_t res = xTaskCreate(vTareaEncenderLed, (const char *)"LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);

	/* Vemos si la tarea fue instanciada correctamente, si es asi iniciamos el scheduler */
	if(res == pdFAIL)
	{
		vPrintString("No fue posible instanciar la tarea");
	}else{
		vTaskStartScheduler();
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

	uint16_t duty = 0; // Creamos la variable Duty, que nos indicará cuanto tiempo va a estar encendido el led

	for(;;){
		duty += 100;
		TickType_t xDelayDutyms = pdMS_TO_TICKS( duty ); // Retardo del tamaño de duty.
		gpioWrite( LEDB ,1 );
		vPrintString("Encendemos el led.\r\n");
		vTaskDelay( xDelayDutyms );
		gpioWrite( LEDB ,0 );
		vPrintString("Apagamos el led.\r\n");
		if (duty > 900){
			duty = 0;
		}

		vTaskDelayUntil (&xLastWakeTime, xDelay1000ms);
	}
}
