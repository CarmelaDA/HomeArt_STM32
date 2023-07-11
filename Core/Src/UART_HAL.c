/*
 * UART_HAL.h
 *
 * Modificado por Carmen Díez Almoguera, 2023
 *
 */


#include "UART_HAL.h"
#include "string.h"


/* Define PC UART and WiFi UART */

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

#define WiFi_UART &huart2
#define PC_UART &huart6

/* IRQ HANDLER */
// extern void UART_isr(UART_HandleTypeDef *huart);


//************************************************************************************************************************************//


Ring_Buffer rx_buffer1 = { { 0 }, 0, 0};
Ring_Buffer tx_buffer1 = { { 0 }, 0, 0};
Ring_Buffer rx_buffer2 = { { 0 }, 0, 0};
Ring_Buffer tx_buffer2 = { { 0 }, 0, 0};

Ring_Buffer *_rx_buffer1;
Ring_Buffer *_tx_buffer1;
Ring_Buffer *_rx_buffer2;
Ring_Buffer *_tx_buffer2;


void storeChar(unsigned char c, Ring_Buffer *buffer);


void ringInit(void){

	_rx_buffer1 = &rx_buffer1;
    _tx_buffer1 = &tx_buffer1;
    _rx_buffer2 = &rx_buffer2;
    _tx_buffer2 = &tx_buffer2;

    /* Enable the INTERRUPTION by UART ERROR (frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(WiFi_UART, UART_IT_ERR);
    __HAL_UART_ENABLE_IT(PC_UART, UART_IT_ERR);

    /* Enable the INTERRUPTION by EMPTY DATA REGISTER */
    __HAL_UART_ENABLE_IT(WiFi_UART, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(PC_UART, UART_IT_RXNE);
}


void storeChar(unsigned char c, Ring_Buffer *buffer){

    int i = (unsigned int)(buffer->head + 1) % UART_BUFFER_SIZE;

    // If we want to stock something before the TAIL, meaning that HEAD will reach the TAIL position, it will cause a BUFFER overflow

    if(i != buffer->tail)
    {
    	buffer->buffer[buffer->head] = c;
        buffer->head = i;
    }
}


int lookFor (char *str, char *buffertolookinto){

	int stringlength = strlen (str);
	int bufferlength = strlen (buffertolookinto);
	int so_far = 0;
	int indx = 0;

repeat:
	while (str[so_far] != buffertolookinto[indx]) indx++;

	if (str[so_far] == buffertolookinto[indx])
	{
		while (str[so_far] == buffertolookinto[indx])
		{
			so_far++;
			indx++;
		}
	}
	else
	{
		so_far =0;
		if (indx >= bufferlength) return -1;
		goto repeat;
	}

	if (so_far == stringlength) return 1;
	else return -1;
}


void getData(char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto){

	int startStringLength = strlen (startString);
	int endStringLength   = strlen (endString);
	int so_far = 0;
	int indx = 0;
	int startposition = 0;
	int endposition = 0;

repeat1:
	while (startString[so_far] != buffertocopyfrom[indx]) indx++;

	if (startString[so_far] == buffertocopyfrom[indx])
	{
		while (startString[so_far] == buffertocopyfrom[indx])
		{
			so_far++;
			indx++;
		}
	}

	if (so_far == startStringLength) startposition = indx;

	else
	{
		so_far =0;
		goto repeat1;
	}

	so_far = 0;

repeat2:
	while (endString[so_far] != buffertocopyfrom[indx]) indx++;

	if (endString[so_far] == buffertocopyfrom[indx])
	{
		while (endString[so_far] == buffertocopyfrom[indx])
		{
			so_far++;
			indx++;
		}
	}

	if (so_far == endStringLength) endposition = indx-endStringLength;

	else
	{
		so_far =0;
		goto repeat2;
	}

	so_far = 0;
	indx=0;

	for (int i=startposition; i<endposition; i++)
	{
		buffertocopyinto[indx] = buffertocopyfrom[i];
		indx++;
	}
}


void UART_flush(UART_HandleTypeDef *uart){

	if (uart == WiFi_UART)
	{
		memset(_rx_buffer1->buffer,'\0', UART_BUFFER_SIZE);
		_rx_buffer1->head = 0;
	}

	if (uart == PC_UART)
	{
		memset(_rx_buffer2->buffer,'\0', UART_BUFFER_SIZE);
		_rx_buffer2->head = 0;
	}
}


int UART_peek(UART_HandleTypeDef *uart){

	if (uart == WiFi_UART)
	{
		if(_rx_buffer1->head == _rx_buffer1->tail) return -1;

		else return _rx_buffer1->buffer[_rx_buffer1->tail];
	}

	else if (uart == PC_UART)
	{
		if(_rx_buffer2->head == _rx_buffer2->tail) return -1;

		else return _rx_buffer2->buffer[_rx_buffer2->tail];
	}

	return -1;
}


int UART_read(UART_HandleTypeDef *uart){

	if (uart == WiFi_UART)
	{
		// If HEAD is not before TAIL, there is not CHARACTER
		if(_rx_buffer1->head == _rx_buffer1->tail) return -1;

		else
		{
			unsigned char c = _rx_buffer1->buffer[_rx_buffer1->tail];
		    _rx_buffer1->tail = (unsigned int)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
		    return c;
		}
	}

	else if (uart == PC_UART)
	{
		// If HEAD is not before TAIL, there is not CHARACTER
		if(_rx_buffer2->head == _rx_buffer2->tail) return -1;

		else
		{
			unsigned char c = _rx_buffer2->buffer[_rx_buffer2->tail];
		    _rx_buffer2->tail = (unsigned int)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE;
		    return c;
		}
	}

	else return -1;
}


void UART_write(int c, UART_HandleTypeDef *uart){

	if (c>=0)
	{
		if (uart == WiFi_UART)
		{
			int i = (_tx_buffer1->head + 1) % UART_BUFFER_SIZE;

			// If OUTPUT BUFFER is full, INTERRUPTION empties it
		    while (i == _tx_buffer1->tail);

		   _tx_buffer1->buffer[_tx_buffer1->head] = (uint8_t)c;
		   _tx_buffer1->head = i;

		   // Enables INTERRUPTION in the UART TRANSMISSION
		   __HAL_UART_ENABLE_IT(WiFi_UART, UART_IT_TXE);
	    }

		else if (uart == PC_UART)
		{
			int i = (_tx_buffer2->head + 1) % UART_BUFFER_SIZE;

			// If OUTPUT BUFFER is full, INTERRUPTION empties it
			while (i == _tx_buffer2->tail);

			_tx_buffer2->buffer[_tx_buffer2->head] = (uint8_t)c;
			_tx_buffer2->head = i;

			// Enables INTERRUPTION in the UART TRANSMISSION
			__HAL_UART_ENABLE_IT(PC_UART, UART_IT_TXE);
		}
	}
}


int UART_available(UART_HandleTypeDef *uart){

	if (uart == WiFi_UART) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer1->head - _rx_buffer1->tail) % UART_BUFFER_SIZE;

	else if (uart == PC_UART) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer2->head - _rx_buffer2->tail) % UART_BUFFER_SIZE;

	return -1;
}


int UART_getAfter(char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart){

	while (UART_waitFor(string, uart) != 1);

	for (int indx=0; indx<numberofchars; indx++){

		while (!(UART_available(uart)));
		buffertosave[indx] = UART_read(uart);
	}

	return 1;
}


void UART_send (const char *s, UART_HandleTypeDef *uart){

	while(*s!='\0') UART_write(*s++, uart);
}


void UART_changeBase (long n, uint8_t base, UART_HandleTypeDef *uart){

  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *s = &buf[sizeof(buf) - 1];

  *s = '\0';

  // If BASE==1
  if (base < 2) base = 10;

  do{

	  unsigned long m = n;
      n /= base;
      char c = m - base * n;
      *--s = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  while(*s) UART_write(*s++, uart);
}


int UART_copyUpto(char *string, char *buffertocopyinto, UART_HandleTypeDef *uart){

	int so_far =0;
	int len = strlen (string);
	int indx = 0;

again:
	while (!UART_available(uart));

	while (UART_peek(uart) != string[so_far]){

		buffertocopyinto[indx] = _rx_buffer1->buffer[_rx_buffer1->tail];
		_rx_buffer1->tail = (unsigned int)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
		indx++;
		while (!UART_available(uart));

	}

	while (UART_peek(uart) == string [so_far]){

		so_far++;
		buffertocopyinto[indx++] = UART_read(uart);
		if (so_far == len) return 1;
		while (!UART_available(uart));
	}

	if (so_far != len){

		so_far = 0;
		goto again;
	}

	if (so_far == len) return 1;

	else return -1;
}


int UART_waitFor(char *string,UART_HandleTypeDef *uart){

	int so_far =0;
	int len = strlen (string);

again_device:
	while (!UART_available(uart));

	if (UART_peek(uart) != string[so_far]){

		 _rx_buffer1->tail = (unsigned int)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE ;
		goto again_device;

	}

	while (UART_peek(uart) == string [so_far]){

		so_far++;
		UART_read(uart);
		if (so_far == len) return 1;
		while (!UART_available(uart));
	}

	if (so_far != len){

		so_far = 0;
		goto again_device;
	}

	if (so_far == len) return 1;

	else return -1;
}


void UART_isr(UART_HandleTypeDef *huart){

	uint32_t isrflags   = READ_REG(huart->Instance->SR);
	uint32_t cr1its     = READ_REG(huart->Instance->CR1);

	/* If DR (DATA REGISTER) is not empty and RX INT is ENABLE */
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
    	/************************************************************************************************************
    	    	      * @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
    	    	                error) and IDLE (Idle line detected) flags are cleared by software
    	    	                sequence: a read operation to USART_SR register followed by a read
    	    	                operation to USART_DR register.
    	    	      * @note   RXNE flag can be also cleared by a read to the USART_DR register.
    	    	      * @note   TC flag can be also cleared by software sequence: a read operation to
    	    	                USART_SR register followed by a write operation to USART_DR register.
    	    	      * @note   TXE flag is cleared only by a write to the USART_DR register.

    	************************************************************************************************************/

		huart->Instance->SR;                   // Read SR (STATUS REGISTER)
        unsigned char c = huart->Instance->DR; // Read DR (DATA REGISTER)

        if (huart == WiFi_UART){

        	storeChar(c, _rx_buffer1); // Stock DATA in BUFFER
        }

        else if (huart == PC_UART){

        	storeChar(c, _rx_buffer2); // Stock DATA in BUFFER
        }

        return;
    }

    /* If INTERRUPTION is produced by TRANSMIT DATA REGISTER EMPTY */
    if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET)){

    	if (huart == WiFi_UART){

    		if(tx_buffer1.head == tx_buffer1.tail){

    			// Empty BUFFER, disable INTERRUPTION
    	        __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
    	    }

    		else{

    			// There is more DATA in the OUTPUT BUFFER, send next BYTE
    			unsigned char c = tx_buffer1.buffer[tx_buffer1.tail];
    			tx_buffer1.tail = (tx_buffer1.tail + 1) % UART_BUFFER_SIZE;

    			/************************************************************************************************************
        	    	      	  * @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
        	    	                	error) and IDLE (Idle line detected) flags are cleared by software
        	    	                	sequence: a read operation to USART_SR register followed by a read
        	    	                	operation to USART_DR register.
        	    	      	  * @note   RXNE flag can be also cleared by a read to the USART_DR register.
        	    	      	  * @note   TC flag can be also cleared by software sequence: a read operation to
        	    	                	USART_SR register followed by a write operation to USART_DR register.
        	    	      	  * @note   TXE flag is cleared only by a write to the USART_DR register.

    			 ************************************************************************************************************/

    			huart->Instance->SR;
    			huart->Instance->DR = c;
    		}
    	}

    	else if (huart == PC_UART){

    		if(tx_buffer2.head == tx_buffer2.tail){

    			// Empty BUFFER, disable INTERRUPTION
        	    __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
    		}

    		else{

    			// There is more DATA in the OUTPUT BUFFER, send next BYTE
        	    unsigned char c = tx_buffer2.buffer[tx_buffer2.tail];
        	    tx_buffer2.tail = (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;

    			/************************************************************************************************************
        	    	      	  * @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
        	    	                	error) and IDLE (Idle line detected) flags are cleared by software
        	    	                	sequence: a read operation to USART_SR register followed by a read
        	    	                	operation to USART_DR register.
        	    	      	  * @note   RXNE flag can be also cleared by a read to the USART_DR register.
        	    	      	  * @note   TC flag can be also cleared by software sequence: a read operation to
        	    	                	USART_SR register followed by a write operation to USART_DR register.
        	    	      	  * @note   TXE flag is cleared only by a write to the USART_DR register.

    			 ************************************************************************************************************/

        	     huart->Instance->SR;
        	     huart->Instance->DR = c;

        	}
        }

    	return;
    }
}


/*
int16_t UART_getPosition(char *string, UART_HandleTypeDef *uart)
{
	static uint8_t so_far;
    uint16_t counter;
    int len = strlen (string);

    if (uart == device_uart)
    {
    	if (_rx_buffer1->tail>_rx_buffer1->head)
	    {
    		if (Uart_read(device_uart) == string[so_far])
	  		{
    			counter=UART_BUFFER_SIZE-1;
	  		    so_far++;
	  		}
	        else so_far=0;
	    }

        unsigned int start = _rx_buffer1->tail;
	    unsigned int end = _rx_buffer1->head;

	    for (unsigned int i=start; i<end; i++)
	    {
		    if (Uart_read(device_uart) == string[so_far])
		    {
		    	counter=i;
		    	so_far++;
		    }

		    else so_far =0;
	    }
    }

    else if (uart == pc_uart)
    {
    	if (_rx_buffer2->tail>_rx_buffer2->head)
    	{
    		if (Uart_read(pc_uart) == string[so_far])
	  		{
    			counter=UART_BUFFER_SIZE-1;
    			so_far++;
	  		}
    		else so_far=0;
    	}

    	unsigned int start = _rx_buffer2->tail;
    	unsigned int end = _rx_buffer2->head;

    	for (unsigned int i=start; i<end; i++)
    	{
    		if (Uart_read(pc_uart) == string[so_far])
    		{
    			counter=i;
    			so_far++;
    		}
    		else so_far =0;
    	}
    }

	if (so_far == len)
	{
		so_far =0;
		return counter;
	}

	else return -1;
}
*/
