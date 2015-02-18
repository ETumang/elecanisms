#include "ui.h"
#include "common.h"
#include "pin.h"
#include "oc.h"
#include "timer.h"
#include "uart.h"
#include <stdio.h>
#include "positionCount.h"

#define TIME_READING_WINDOW 30
#define SET_POINT 59060
#define KP 40
//KI will be what the integral control is devided by, 
// for the KI you want, compute KI_desired^(-1)
#define KI 10
//KD will be what deritive control is devided by,
//cor the KD you want, compute KD_desired^(-1)
#define KD 4

#define  FULL_DUTY_CYCLE 2^16
#define  MAX_VOLTAGE 2^12

#define  DATA_PERIOD .001
#define  SEND_SCALAR  100
#define DutyCycle 65534 

int get_pos = 1;
int send_data = 0;
int motorDirection=1;

int get_amount(char *line) {
    // return the number in a string such as "r1200" as an int
    int amount;
    amount = (int) strtol(line+1, NULL, 10);
    return amount;
}

int sum_array(int a[], int num_elements){
    /*Written by:
    Peter H. Anderson, MSU, Feb 21, '97
    http://www.phanderson.com/C/arraysum.html
    
    Sum specified elements in an array, a. */
    int i;
    int sum=0;
    for (i=0; i<num_elements; i++)
    {
     sum = sum + a[i];
    }
    return(sum);
}

int sum_error(int a[], int num_elements){
    /*Computes the sum of the error terms as computed by the difference
    in the set point and recorded point. Does this for specified 
    elements in an array and returns the sum of these error terms.*/
    int sum_errors = 0;
    int error = 0;
    int i=0;
    for(i=0; i<num_elements;++i){
        error = SET_POINT - a[i];
        sum_errors = sum_errors + error;
    }
    return sum_errors;
}

int sum_difference(int a[], int num_elements){
    /*Computes the sum of the difference between terms. This is an
    approximation of the derivative of the position. Returns the sum
    of these diferences for the aray and length of said array 
    specified. */
    int sum_difference = 0;
    int difference = 0;
    int i=0;    
    for(i=1; i<num_elements; i++){
        difference = a[i]-a[i-1];
        //printf("Diff is: %2d\n",difference);
        sum_difference = sum_difference + difference;
    }
    //printf("Sum diff is: %2d\n",sum_difference);
    return sum_difference;
}

void data_timing(_TIMER *timer){
	send_data ++;
	get_pos = 1;
}

void setup(void){

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
}

/*void writeMotor(int command){
    if (command>=20){
        //Write to motor direction 1 (right?)
        if (motorDirection==1){
            pin_write(&D[6], command);
        }
        if (motorDirection==0){
            oc_pwm(&D[6], &oc1, NULL, 500, DutyCycle);
        }
    }
    if (command<20){
        if (motorDirection==1){
        
        }
        if (motorDirection==0){
        
        }
    }
}*/


void writeLEDs(int led1State, int led2State, int led3State){
    /* Write the three LEDs according to the values passed in. If value for LED 
    is 0, it will be off. Otherwise, it will be on. Will also turn off leds which are on. */
    led_write(&led1, led1State);
    led_write(&led2,led2State);
    led_write(&led3, led3State);
}

void wallMotion(int position){
    /*Creates the texture motion for the wall state, given the position of the 
    handle controler. Does this by writing the motor command to high if the 
    handle's position is within two angle readings. */
    unsigned int writeVal = 300;
    int writeState = 0;
    if ((position<60) && (position>35)){
        pin_write(&D[6], writeVal);
        writeState=1;
    }
    if ((position<15) && (position>0) ){
        pin_write(&D[6], writeVal);
        writeState=1;
    }
    if (writeState==0){
    /*If it's not in the range of the walls, write the motor command to low. */
        pin_write(&D[6], 0);    
    }
}

int spring(int k, int readings[]){
    int position = sum_array(readings, TIME_READING_WINDOW);
    int command = k*abs(position);
    pin_write(&D[6], command);
}


int damper(int k, int readings[]){
    int derivative = sum_difference(readings,30)/TIME_READING_WINDOW;
    int command = k*derivative;
    pin_write(&D[6], command);
}


int texture(){
    int text[11]={1,1,0,0,0,0,1,0,0,1};
    int i=0;
    for (i=0; i<=(sizeof(text)/sizeof(text[0])); i++){
        if (text[i]==1){
            pin_write(&D[6],(unsigned int) 30);
        }
        if (text[i]==0){
            pin_write(&D[6],(unsigned int) 2);
        }
    }  
}

int wall(){}

int main(){
    setup();
    //Controler receives the state over USB
    /*0 is spring
    1 is damper
    2 is texture
    3 is wall*/
    int KDd = 40; //constant fof the damper derivative control!
    int KSs = 10; //Constant for spring setting
    
    /*Make an array to hole our position readings and initialize its elements to 0. */
    int readings[TIME_READING_WINDOW];
    int i = 0; 
    for(i; i<TIME_READING_WINDOW; i++){
        readings[i]=0;
    }
    
    while(1){
        int position = (update_pos(pin_read(&A[0])<<6)-59215)/-17;
        
        //Shift every element in the array one space to the left
        for(i=1; i<TIME_READING_WINDOW;i++){
            readings[i-1]=readings[i];
        }
        readings[TIME_READING_WINDOW]=position;
        int state = 1; //TODO: Hardcode state for testing!
        switch (state){
            case 0:
                //The spring mode!  
                writeLEDs(1,0,0);
                //spring(KSs, readings);
                pin_write(&D[6], 300);
                break;
            case 1:
                //The damper mode!!!
                writeLEDs(0,1,0);
                damper(KDd, readings);
                break;
            case 2:
                //Texture mode!
                writeLEDs(1,1,0);
                texture();
                break;
            case 3: 
                //Wall mode!
                writeLEDs(0,0,1);
                wall();
                break;   
        }
    }
}

