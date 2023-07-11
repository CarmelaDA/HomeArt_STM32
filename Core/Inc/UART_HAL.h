/*
 * UART_HAL.h
 *
 * Modificado por Carmen Díez Almoguera, 2023
 *
 */

#ifndef UART_HAL_H_
#define UART_HAL_H_


#include "stm32f4xx_hal.h"


/* Change buffer size as needed */
#define UART_BUFFER_SIZE 512


/* BUUFER structure */
typedef struct
{
  unsigned char buffer[UART_BUFFER_SIZE]; // SIZE
  volatile unsigned int head; // HEAD
  volatile unsigned int tail; // TAIL
} Ring_Buffer;


//************************************************************************************************************************************//


/* Reads RX_BUFFER and increases the RX_ BUFFER TAIL count at the indicated UART */
int UART_read(UART_HandleTypeDef *uart);


/* Writes in RX_BUFFER and increases the TX_ BUFFER HEAD count */
void UART_write(int c, UART_HandleTypeDef *uart);

/* Sends STRING */
void UART_send(const char *s, UART_HandleTypeDef *uart);


/* Print any number at any base (decimal, octal, binary, hexadecimal, etc.) */
void UART_changeBase(long n, uint8_t base, UART_HandleTypeDef *uart);


/* Initialize RingBuffer */
void ringInit(void);  // MIRAR SI SE PUEDE CAMBIAR A UART_INIT();


/* IDK */
void UART_flush(UART_HandleTypeDef *uart);


/* Test if DATA is available to be readed and registered in RX_BUFFER at indicated UART */
int UART_available(UART_HandleTypeDef *uart);


/* Looks for STRING in indicated BUFFER */
// (returns 1 in case of success, -1 in case of failure) */
// if (look_for("some string", buffer)) do something
int lookFor(char *str, char *buffertolookinto);


/* Copy and paste STRING from ine BUFFER to another */
void getData(char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto);


/* Looks for in RX_BUFFER without increasing TAIL count */
// (returns TAIL value) */
// if (Uart_peek () == 'M') do something
int UART_peek(UART_HandleTypeDef *uart);


/* Copy DATA from RX_BUFFER in the BUFFER. BLOCKING MODE is needed */
// (returns 1 in case of success, -1 in case of failure) */
// while (!(Copy_Upto ("some string", buffer, uart)));
int UART_copyUpto(char *string, char *buffertocopyinto, UART_HandleTypeDef *uart);


/* Copy and paste the indicated number of characters (in BLOCKING MODE) from the RX_BUFFER to a BUFFER after detect the indicated STRING */
// (returns 1 in case of success, -1 in case of failure)
// while (!(Get_after ("some string", 6, buffer, uart)));
int UART_getAfter(char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart);


/* Waits till STRING is detected in the RX_BUFFER */
// (returns 1 in case of success, -1 in case of failure)
// while (!(Wait_for("some string", uart)));
int UART_waitFor(char *string, UART_HandleTypeDef *uart);


/* ISR (interruption) for the UART. IRQ HANDLER is needed */
void UART_isr(UART_HandleTypeDef *huart);


//************************************************************************************************************************************//


/* Gets the POSITION of the STRING from the indicated UART */
// (returns the final position of the STRING)
int16_t UART_getPosition(char *string, UART_HandleTypeDef *uart);


#endif /* UART_HAL_H_ */
