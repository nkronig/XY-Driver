/*
 * UART.h
 *
 * Created: 13.11.2018 12:56:46
 *  Author: nilsk
 */ 


#ifndef UART_H_
#define UART_H_

#define USART0_BAUD_RATE(BAUD_RATE) ((float)20000000 * 64 / (16 * (float)BAUD_RATE))

int8_t USART_init(void);
void USART_write(const uint8_t data);
void printString(const char myString[]);


#endif /* UART_H_ */