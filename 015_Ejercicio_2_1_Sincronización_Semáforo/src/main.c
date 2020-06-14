/* Librerias de FreeRTOS. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "sapi.h"
#include "FreeRTOSConfig.h"


/* FreeRTOS incluye unas funciones soportadas básicas */
#include "supporting_functions.h"


/**
 * Sincronizar dos tareas mediante un semáforo binario
 * Escribir un programa con dos tareas:
 * Tarea 1: Medirá el tiempo de pulsación de un botón, aplicando anti-rebote. Liberará un semáforo al obtener la medición.
 * Tarea 2: Esperará por el semáforo y destellará un LED al recibirlo.
 */

/* Creación de tarea para encender el led */
void vTareaEncenderLed (void *pvParameters );
void vTareaMedirPulsacion (void *pvParameters );
void vTareaBoton (void *pvParameters );

TickType_t get_diff();
void clear_diff();
TickType_t duty = 0;
SemaphoreHandle_t xBinarySemaphore;


/*------------------ MAIN PRINCIPAL --------------------------*/
int main (){
	xBinarySemaphore = xSemaphoreCreateBinary(); // Creamos el semáforo binario

	/* Confirmamos que el semaforo haya sido creado correctamente */
	if(xBinarySemaphore != NULL){
		/* Instanciamos una tarea encender led */
		BaseType_t res = xTaskCreate(vTareaEncenderLed, (const char *)"LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);

		/* Vemos si la tarea fue instanciada correctamente, si es asi iniciamos el scheduler */
		if(res == pdFAIL)
		{
			vPrintString("No fue posible instanciar la tarea de encender led.\r\n");
		}else{
			res = xTaskCreate(vTareaBoton, (const char *)"BOTON", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
			if(res == pdFAIL){
				vPrintString("No fue posible instanciar la tarea de monitorizar el boton.\r\n");
			}else{
				res = xTaskCreate(vTareaMedirPulsacion, (const char *)"Medir", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
				if (res == pdFAIL){
					vPrintString("No fue posible instanciar la tarea de medir el boton.\r\n");
				}else {
					vPrintString("Todo fue posible asi que iniciamos todo.\r\n");
					vTaskStartScheduler();
				}

			}
		}
	}else {
		vPrintString("No fue posible crear el semaforo binario\r\n");
	}


	for (;;);
	return 0;
}
/*------------------ FIN MAIN PRINCIPAL --------- -----------*/


/* ------------------ FUNCIONES ---------------------------- */
void vTareaEncenderLed (void *pvParameters){
	for(;;){
		if( xSemaphoreTake( xBinarySemaphore, portMAX_DELAY ) == pdTRUE ){
			gpioWrite( LEDB ,1 );
			vPrintString("Encendemos el led.\r\n");
			vTaskDelay( duty );
			gpioWrite( LEDB ,0 );
			vPrintString("Apagamos el led.\r\n");
		}
	}
}

void vTareaMedirPulsacion (void *pvParameters){
	for(;;){
		TickType_t diff = get_diff();
		if (diff == 0){
			vTaskDelay(pdMS_TO_TICKS( 40 ));
		}else{
			duty=diff;
			clear_diff();
			xSemaphoreGive(xBinarySemaphore);
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
