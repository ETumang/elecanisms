#include "ui.h"
#include "common.h"
#include "pin.h"
#include "oc.h"
#include "timer.h"
#include "uart.h"
#include <stdio.h>

#define TIME_READING_WINDOW 30
#define SET_POINT 59060
#define KP 30
//KI will be what the integral control is devided by, 
// for the KI you want, compute KI_desired^(-1)
#define KI 4
//KD will be what deritive control is devided by,
//cor the KD you want, compute KD_desired^(-1)
#define KD 1

#define  FULL_DUTY_CYCLE 2^16
#define  MAX_VOLTAGE 2^12

#define  DATA_PERIOD .001
#define  SEND_SCALAR  100
#define DutyCycle 65534 

int get_pos = 1;
int send_data = 0;
int flipped = 0;

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

int computeMotorControl(){
    //allocate array for sensor readings:
    int readings[TIME_READING_WINDOW]; 
    int sensorReading = 5; //pseudo sensor reading, TODO: REPLACE WITH REAL VALUE
    
    //Oust the deamons: (Initialize array)
    int i=0;
    for(i=0; i<TIME_READING_WINDOW; ++i){
        readings[i]=0;
    }
    
    //Loop through the array and discard the oldest position reading:
    i=1;
    for(i=1; i<TIME_READING_WINDOW; ++i){
        if(readings[i]>0.1000){
            readings[i-1] = readings[i]; 
        }
        else{
            readings[i-1] = sensorReading;
        }        
    }
    
    //Write the current sensor reading to end of array:
    readings[TIME_READING_WINDOW-1] = sensorReading;
    
    /* Print array values, Debug only. */
    i=0;
    for(i=0; i<=TIME_READING_WINDOW-1; i++){
        printf("Array value at %2d is: %2d \n", i, readings[i]);
    }
    
    int sum_sensor_readings = sum_array(readings, 30);
    int sum_error_readings = sum_error(readings, 30);
    int sum_difference_readings = sum_difference(readings,30);
    
    //Average sensor readings in time window to get approximate current position:
    int current_position = sum_sensor_readings/TIME_READING_WINDOW; 
    
    //Compute motor control signal with control:
    int motor_command_portional = KP*(SET_POINT - current_position);
    int motor_command_integral = sum_error_readings/KI/TIME_READING_WINDOW;
    int motor_command_derivative = sum_difference_readings/KD/TIME_READING_WINDOW;
    int totalMotorControl=motor_command_portional+motor_command_integral+motor_command_derivative;
    
    //Print computed values for debugging purposes. Comment out on PIC. 
    /*printf("The sum of the sensor readings is: %3d \n", sum_sensor_readings);
    printf("Current position is: %3d\n",current_position);
    printf("Command for porportional motor control is: %3d\n",motor_command_portional);
    printf("Command for integral motor control is: %3d\n",motor_command_integral);
    printf("Command for deritive motor control is: %3d\n",motor_command_derivative);*/

    return totalMotorControl;
}


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
            
            //Compute the motor control information:
            int motorCommand = computeMotorControl();
            motorCommand <<1;
            pin_write(&D[6],(unsigned int) motorCommand);
            if (motorCommand>0){
                led_write(&led2, 1);
            }
            else{
                led_write(&led2,0);
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

