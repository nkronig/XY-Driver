#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

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
	CLKCTRL.MCLKCTRLB = 0x01;
	CPU_CCP = 0xD8;
	CLKCTRL.OSC20MCTRLA = 0x02;
}
void setMotor(int m, int speed){
	if(m == 0){
		if(speed == 0){
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
		if(speed == 0){
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

int main(void)
{
	PORTB.DIR |= 0x38;
	PORTC.DIR |= 0x08;
	
	clkInit();
	timerAInit();
	sei(); 
    while (1) 
    {
		setMotor(1,255);
		_delay_ms(500);
		setMotor(1,300);
		_delay_ms(100);
		setMotor(1,-255);
		_delay_ms(500);
		setMotor(1,300);
		_delay_ms(100);
    }
}

