#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t timeA1 =0;
uint32_t timeA2 =0;
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
	TCB0.CCMP = 0x00CB;

	TCB0.INTCTRL = 1 << TCB_CAPT_bp;

	TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | 1 << TCB_ENABLE_bp;
}
void clkInit(void){
	CPU_CCP = 0xD8;
	CLKCTRL.MCLKCTRLB = 0x01;
	CPU_CCP = 0xD8;
	CLKCTRL.OSC20MCTRLA = 0x02;
}
void setMotor(int m, int speed){
	if(m == 0){
		if((~PORTA.IN & 0x20) && (speed <= 10 && speed >= -10)){
			speed = 300;
		}
		else if((~PORTA.IN & 0x10) && (speed <= 10 && speed >= -10)){
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
		if((~PORTA.IN & 0x80) && (speed <= 10 && speed >= -10)){
			speed = 300;
		}
		else if((~PORTA.IN & 0x40) && (speed <= 10 && speed >= -10)){
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
	timeA2 ++;
	timeA1 ++;
	PORTB.OUTTGL |= 0x08;
	TCB0_INTFLAGS |= TCB_CAPT_bm;
}

ISR(PORTA_PORT_vect)
{
	/* Insert your PORTA interrupt handling code here */
	if(PORTA.INTFLAGS & 0x04){
		if(PORTA.IN & 0x04){
			timeA2 = 0;
		}
		else{
			//setMotor(0,map(timeA2, 0, 101, -255, 255));
		}
	}
	if(PORTA.INTFLAGS & 0x02){
		if(PORTA.IN & 0x02){
			timeA2 = 0;
		}
		else{
			//setMotor(1,map(timeA2, 0, 101, -255, 255));
		}
	}
	
	VPORTA_INTFLAGS = (1 << 1);
}
int main(void)
{
	PORTA.DIR &= 0x00;
	PORTA.PIN1CTRL |= 0x01;
	PORTA.PIN2CTRL |= 0x01;
	
	PORTB.DIR |= 0x38;
	PORTC.DIR |= 0x08;
	clkInit();
	timerBInit();
	timerAInit();
	sei();
	while (1)
	{
		/*setMotor(0,255);
		
		
		_delay_ms(500);
		setMotor(0,300);
		_delay_ms(100);
		setMotor(0,-255);
		_delay_ms(500);
		setMotor(0,300);
		_delay_ms(100);*/
	}
}

