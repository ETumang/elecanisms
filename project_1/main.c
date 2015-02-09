#include "ui.h"
#include "common.h"
#include "pin.h"
#include "oc.h"
#include "timer.h"
#include "uart.h"
#include <stdio.h>

#define  FULL_DUTY_CYCLE 2^16
#define  MAX_VOLTAGE 2^12

#define  DATA_PERIOD .001
#define  SEND_SCALAR  100
#define DutyCycle 65534 

int get_pos = 1;
int send_data = 0;
int flipped = 0;


void data_timing(_TIMER *timer){
	send_data ++;
	get_pos = 1;
}

void main(void){

	init_pin();
	init_oc();
	init_ui();
	init_timer();
	init_uart();

	uint16_t rawPos;
	long int updatedPos = 0;
	long int rawDiff;
	long int lastRawPos=0;
	long int lastLastRawPos;
	long int lastRawDiff;
	long int rawOffset;
	long int lastRawOffset;
	int flips;
	long int offset;
	int i;

	timer_setPeriod(&timer1, .001);
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
	pin_analogIn(&A[0]);

	//timer_setPeriod(&timer1, .01);
	timer_enableInterrupt(&timer1);
	timer_every(&timer1, DATA_PERIOD, data_timing);

	led_on(&led1);

	rawPos = pin_read(&A[0])>>6;
	lastRawPos = pin_read(&A[0])>>6;
	lastLastRawPos = pin_read(&A[0])>>6;

	oc_pwm(&oc1, &D[6], NULL, 500, DutyCycle);

	while(1){
		if(get_pos){
			rawPos = pin_read(&A[0])>>6;
		
  // Calculate differences between subsequent MR sensor readings
  rawDiff = rawPos - lastRawPos;          //difference btwn current raw position and last raw position
  lastRawDiff = rawPos - lastLastRawPos;  //difference btwn current raw position and last last raw position
  rawOffset = abs(rawDiff);
  lastRawOffset = abs(lastRawDiff);
  
  // Update position record-keeping vairables
  lastLastRawPos = lastRawPos;
  lastRawPos = rawPos;
  
  // Keep track of flips over 180 degrees
  if((lastRawOffset > 700) && (!flipped)) { // enter this anytime the last offset is greater than the flip threshold AND it has not just flipped
    if(lastRawDiff > 0) {        // check to see which direction the drive wheel was turning
      flips--;              // cw rotation 
    } else {                     // if(rawDiff < 0)
      flips++;              // ccw rotation
    }
  // check to see if the data was good and the most current offset is above the threshold
    updatedPos = rawPos + flips*800; // update the pos value to account for flips over 180deg using the most current offset 
    offset = rawOffset;
    flipped = 1;        // set boolean so that the next time through the loop won't trigger a flip
  } else {                        // anytime no flip has occurred
    updatedPos = rawPos + flips*800; // need to update pos based on what most recent offset is 
    flipped = 0;
  }
}
		
		if(send_data == SEND_SCALAR){
			
			printf("%lli\n",(long long int)((rawDiff)));
			send_data = 0;
			led_toggle(&led3);
		}
		/*Don't forget to check your serial port and baud rate*/
	}
}

