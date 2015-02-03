#include "ui.h"
#include "common.h"
#include "pin.h"
#include "oc.h"
#include "timer.h"
#include "uart.h"

#define  FULL_DUTY_CYCLE 2^16
#define  MAX_VOLTAGE 2^10 << 5

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

	for(i = 1; i < 9; i++){
		pin_digitalOut(&D[i]);
	}

	pin_digitalIn(&D[0]);

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


	while(1){
		voltage = pin_read(&A[5]);
		diff = prev_voltage - voltage;

		if (diff < (MAX_VOLTAGE){
			angle += diff;
			printf(angle);
			printf('Hello');

		}

		

	}

	led_on(&led1);
}
