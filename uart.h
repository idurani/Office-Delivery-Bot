/*
*
*   uart.h
*
*   Used to set up the RS232 connector and WIFI module
*   uses UART1 at 115200
*
*
*   @author Dane Larson
*   @date 07/18/2016
*/

#ifndef UART_H_
#define UART_H_

#include <inc/tm4c123gh6pm.h>
#include "timer.h"

//initialize UART1
void uart_init(void);

//load byte data into transmit FIFO and send it through TX pin
void uart_sendChar(char data);

//sit and wait until a byte is found in the receive FIFO
char uart_receive(void);

void uart_sendStr(const char *data);


#endif /* UART_H_ */
