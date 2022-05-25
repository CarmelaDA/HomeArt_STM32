/*
 * UART_HAL.h
 *
 * Creado por Carmen Díez Almoguera, 2022
 */

#ifndef UART_HAL_H_
#define UART_HAL_H_


#include "stm32f4xx_hal.h"


/* Cambiar el tamaño del buffer según su requerimiento */
#define UART_BUFFER_SIZE 512


/* Estructura del BUFFER */
typedef struct
{
  unsigned char buffer[UART_BUFFER_SIZE]; // SIZE
  volatile unsigned int head; // HEAD
  volatile unsigned int tail; // TAIL
} Ring_Buffer;


//************************************************************************************************************************************//


/* Lee RX_BUFFER e incrementa la cuenta de TAIL del RX_BUFFER del UART indicado */
int UART_read(UART_HandleTypeDef *uart);


/* Escribe en RX_BUFFER e incrementa la cuenta de HEAD en TX_BUFFER */
void UART_write(int c, UART_HandleTypeDef *uart);


/* Envía un STRING */
void UART_send(const char *s, UART_HandleTypeDef *uart);


/* Imprime un número en cualquier base (decimal, octal, binaria, hexadecimal, etc.) */
void UART_changeBase(long n, uint8_t base, UART_HandleTypeDef *uart);


/* Inicializa el RingBuffer */
void ringInit(void);  // MIRAR SI SE PUEDE CAMBIAR A UART_INIT();


/* NO SÉ QUÉ HACE */
void UART_flush(UART_HandleTypeDef *uart);


/* Comprueba si el DATA está disponible para ser leída y registrada en el RX_BUFFER del UART seleccionado */
int UART_available(UART_HandleTypeDef *uart);


/* Busca una STRING en el BUFFER indicado */
// (devuelve 1 en caso de éxito, -1 en caso contrario) */
// if (look_for("some string", buffer)) do something
int lookFor(char *str, char *buffertolookinto);


/* Copia y pega una STRING de un BUFFER a otro */
void getData(char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto);


/* Busca en RX_BUFFER sin incrementar la cuenta de la COLA */
// (devuelve el caracter de la COLA) */
// if (Uart_peek () == 'M') do something
int UART_peek(UART_HandleTypeDef *uart);


/* Copia la DATA del RX_BUFFER en el BUFFER. Se hace en BLOCKING MODE */
// (devuelve 1 en caso de éxito, -1 en caso contrario) */
// while (!(Copy_Upto ("some string", buffer, uart)));
int UART_copyUpto(char *string, char *buffertocopyinto, UART_HandleTypeDef *uart);


/* Copia y pega el NÚMERO de caracteres indicado (BLOCKING MODE) del RX_BUFFER a un BUFFER tras detectar el STRING indicado */
// (devuelve 1 en caso de éxito, -1 en caso contrario)
// while (!(Get_after ("some string", 6, buffer, uart)));
int UART_getAfter(char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart);


/* Espera hasta que una STRING sea detectada en RX_BUFFER (devuelve 1 en caso de éxito, -1 en caso contrario) */
// while (!(Wait_for("some string", uart)));
int UART_waitFor(char *string, UART_HandleTypeDef *uart);


/* ISR (interrupción) para el UART. Ponerlo en IRQ HANDLER */
void UART_isr(UART_HandleTypeDef *huart);


//************************************************************************************************************************************//


/* Obtiene la POSICIÓN del STRING en la UART indicada */
// (devuelve la posición en la cual el STRING se termina)
int16_t UART_getPosition(char *string, UART_HandleTypeDef *uart);


#endif /* UART_HAL_H_ */
