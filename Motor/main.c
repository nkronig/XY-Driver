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

volatile uint8_t transmit[] = {0,1,2,3,4,5};
volatile uint8_t update = 0;
volatile uint8_t transmitProgress = 0;
volatile uint8_t transmitProgressMax = 5;
volatile uint8_t pos = 0;
volatile uint8_t poslast = 1;
volatile char rec[BUFFER];
volatile char tempRec[BUFFER];
volatile uint8_t endframe = 0;
volatile uint8_t done = 0;

volatile uint8_t test;

volatile int counterUsed = 0;

volatile uint32_t timeA1 =15278;
volatile uint32_t timeA2 =15278;

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
void timerBInit(void)
{
	TCB0.CCMP = 0xFFFF;

	TCB0.INTCTRL = 1 << TCB_CAPT_bp;

	TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | 1 << TCB_ENABLE_bp;
}
void clkInit(void){
	CPU_CCP = 0xD8;
	CLKCTRL.MCLKCTRLB = 0x01;
	CPU_CCP = 0xD8;
	CLKCTRL.OSC20MCTRLA = 0x02;
}
void setMotor(int m, long speed){
	if(m == 0){
		if((~PORTA.IN & 0x20) && (speed > 10)){
			speed = 300;
		}
		else if((~PORTA.IN & 0x10) && (speed < -10)){
			speed = 300;
		}
		
		if(speed <= 10 && speed >= -10){
			TCA0.SPLIT.LCMP0 = 0;
			TCA0.SPLIT.LCMP1 = 0;
		}
		else if (speed == 300)
		{
			TCA0.SPLIT.LCMP0 = 255;
			TCA0.SPLIT.LCMP1 = 255;
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
			speed = 300;
		}
		else if((~PORTA.IN & 0x80) && (speed < -10)){
			speed = 300;
		}
		
		if(speed <= 10 && speed >= -10){
			TCA0.SPLIT.LCMP2 = 0;
			TCA0.SPLIT.HCMP0 = 0;
		}
		else if (speed == 300)
		{
			TCA0.SPLIT.LCMP2 = 255;
			TCA0.SPLIT.HCMP0 = 255;
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
ISR(TCB0_INT_vect)
{
	
	TCB0_INTFLAGS |= TCB_CAPT_bm;
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
			}
		}
		else
		{
			tempRec[pos] = temp;
			pos ++;
		}
		USART0_STATUS |= USART_RXCIF_bm;
	}

}
int main(void)
{
	PORTA.DIR &= 0x00;
	
	PORTB.DIR |= 0x38;
	PORTC.DIR |= 0x08;
	clkInit();
	timerBInit();
	timerAInit();
	
	USART_init();
	printString("start");
	
	sei();
	while (1)
	{
		if (done >= 1)
		{
			if (rec[0] == '!')
			{
				char *command = strtok(rec, "?");
				if ((strcmp(command, "!X") == 0))
				{
					char *command2 = strtok(0, "?");
					int tmp = command2[0] << 8 | command2[1];
					m1Speed = Clamp(map(tmp, 10052, 20504, -255, 255),-255,255);
					setMotor(0,m1Speed);
				}
				if ((strcmp(command, "!Y") == 0))
				{
					char *command2 = strtok(0, "?");
					int tmp = command2[0] << 8 | command2[1];
					m2Speed = Clamp(map(tmp, 10052, 20504, -255, 255),-255,255);
					setMotor(1,m2Speed);
				}
			}
			done = 0;
		}
	}
}

