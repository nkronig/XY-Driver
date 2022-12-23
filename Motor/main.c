#define F_CPU 20000000

#define BUFFER 20

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

volatile int m1Speed = 0;
volatile int m2Speed = 0;

volatile uint8_t pos = 0;
volatile char rec[BUFFER];
volatile char tempRec[BUFFER];
volatile uint8_t done = 0;

volatile uint8_t test;

volatile int counterUsed = 0;
char transmitString[20];
uint8_t tmp;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int Clamp( int value, int min, int max )
{
	return (value < min) ? min : (value > max) ? max : value;
}
void timerAInit(void)
{
	PORTMUX.CTRLC |= 0x0F;
	
	TCA0.SPLIT.LPER = 255;
	TCA0.SPLIT.HPER = 255;

	TCA0.SPLIT.CTRLD |= (TCA_SPLIT_SPLITM_bm);
	TCA0.SPLIT.CTRLB |= 0x17;

	TCA0.SPLIT.CTRLA |= (TCA_SPLIT_CLKSEL_DIV8_gc) | (TCA_SPLIT_ENABLE_bm);
}

void clkInit(void){
	CPU_CCP = 0xD8;
	CLKCTRL.MCLKCTRLB = 0x00;
	CPU_CCP = 0xD8;
	CLKCTRL.OSC20MCTRLA = 0x02;
}
void setMotor(int m, long speed){
	if(m == 0){
		if((~PORTA.IN & 0x10) && (speed > 10)){
			speed = 0;
		}
		else if((~PORTA.IN & 0x20) && (speed < -10)){
			speed = 0;
		}
		
		if(speed <= 10 && speed >= -10){
			TCA0.SPLIT.LCMP0 = 0;
			TCA0.SPLIT.LCMP1 = 0;
		}
		else if(speed > 0){
			TCA0.SPLIT.LCMP0 = speed;
			TCA0.SPLIT.LCMP1 = 0;
		}
		else{
			TCA0.SPLIT.LCMP0 = 0;
			TCA0.SPLIT.LCMP1 = 255-speed;
		}
	}
	if(m == 1){

		if((~PORTA.IN & 0x40) && (speed > 10)){
			speed = 0;
		}
		else if((~PORTA.IN & 0x80) && (speed < -10)){
			speed = 0;
		}
		
		if(speed <= 10 && speed >= -10){
			TCA0.SPLIT.LCMP2 = 0;
			TCA0.SPLIT.HCMP0 = 0;
		}
		else if(speed > 0){
			TCA0.SPLIT.LCMP2 = speed;
			TCA0.SPLIT.HCMP0 = 0;
		}
		else{
			TCA0.SPLIT.LCMP2 = 0;
			TCA0.SPLIT.HCMP0 = 255-speed;
		}
	}
}
ISR(USART0_RXC_vect){
	if (USART0_STATUS & USART_RXCIF_bm)
	{
		uint8_t temp;
		temp = USART0_RXDATAL;
		if(temp == 0x0A || pos >= 19 || temp == 0x00){
			tempRec[pos] = 0x00;
			pos = 0;
			done ++;
			for (int i = 0; i < BUFFER; i++)
			{
				rec[i] = tempRec[i];
				tempRec[i]=0;
			}
		}
		else
		{
			tempRec[pos] = temp;
			pos ++;
		}
		
	}
	USART0_STATUS |= USART_RXCIF_bm;
}
int main(void)
{
	PORTA.DIR &= 0x00;
	
	PORTB.DIR |= 0x38;
	PORTC.DIR |= 0x08;
	
	clkInit();

	timerAInit();
	
	USART_init();
	
	sei();
	while (1)
	{
		setMotor(1,m2Speed);
		setMotor(0,m1Speed);
		
		if (done >= 1)
		{
			if (rec[0] == '!')
			{
				char *command = strtok(rec, "?");
				if ((strcmp(command, "!x") == 0))
				{
					char *command2 = strtok(0, "?");
					sscanf(command2, "%2hhx", &tmp);
					m1Speed = map(tmp, 0, 255, -255, 255);
				}
				else if ((strcmp(command, "!y") == 0))
				{
					char *command2 = strtok(0, "?");
					sscanf(command2, "%2hhx", &tmp);
					m2Speed = map(tmp, 0, 255, -255, 255);
				}
				else if ((strcmp(command, "!s") == 0))
				{
					sprintf(transmitString, "!s?%x\n", (~PORTA.IN & 0xF0)>>4);
					printString(transmitString);
				}
			}
			done = 0;
		}
		
	}
}

