#include "ui.h"
#include "common.h"
#include "pin.h"
#include "oc.h"
#include "timer.h"
#include "uart.h"
#include <stdio.h>

#define  FULL_DUTY_CYCLE 2^16
#define  MAX_VOLTAGE 2^10 << 5
#define  FULL_ANGLE MAX_VOLTAGE*3

void main(void){
	init_pin();
	init_oc();
	init_ui();
	init_timer();
	init_uart();

	int voltage;
	int angle = 0;
	int diff;
	int prev_voltage = 0;

	int i;

	timer_setPeriod(&timer1, .1);
	timer_start(&timer1);

	for(i = 1; i < 9; i++){
		pin_digitalOut(&D[i]);
	}

	//pin_digitalIn(&D[0]);

	/*Configure board:*/
	pin_clear(&D[1]);
	pin_set(&D[2]);
	pin_clear(&D[3]);
	pin_set(&D[4]);//enable
	pin_clear(&D[6]);
	pin_clear(&D[5]);
	pin_set(&D[7]);//speed
	pin_clear(&D[8]);//not inverting input

	pin_analogIn(&A[5]);

	led_on(&led1);

	while(1){

		voltage = pin_read(&A[5]);

		diff = voltage - prev_voltage;

		if (abs(diff) < MAX_VOLTAGE){
			angle += diff;
		}
		else{
			if(diff <0){
				angle += diff + MAX_VOLTAGE;
			}

			else{
				angle += diff - MAX_VOLTAGE;
			}
		}
		led_on(&led2);
		printf("Difference : %d\n", diff);
		printf("Current Angle: %d\n",angle);
		led_toggle(&led3);
		/*Don't forget to check your serial port and baud rate*/
	}
}
