#include "ui.h"
#include "common.h"
#include "pin.h"
#include "oc.h"
#include "timer.h"
#include "uart.h"
#include <stdio.h>
#include "positionCount.h"

#define TIME_READING_WINDOW 50
#define SET_POINT 59060
#define KP 40
//KI will be what the integral control is devided by, 
// for the KI you want, compute KI_desired^(-1)
#define KI 10
//KD will be what deritive control is devided by,
//cor the KD you want, compute KD_desired^(-1)
#define KD 4

#define K 0

#define  FULL_DUTY_CYCLE 2^16
#define  MAX_VOLTAGE 2^12

#define  DATA_PERIOD .001
#define  SEND_SCALAR  100
#define DutyCycle 65534 

int get_pos = 1;
int send_data = 0;
int motorDirection=1;
//Motor commands above this value make it go right, commands below this value go left:
int motorDirectionThreshold = 200; 

/*void DelayTimer1Init(void) { // Set up timer 1 for microsecond delay function*/
/*    T1CON = 0x0000; // Timer off, 16-bit, 1:1 prescale, gating off*/
/*}*/

/*void UsDelay(unsigned int udelay) {*/
/*    TMR1 = 0;*/
/*    PR1 = udelay * 15; // Number of ticks per microsecond*/
/*    IFS0bits.T1IF = 0; // Reset interrupt flag*/
/*    T1CONbits.TON = 1;*/
/*    while (!IFS0bits.T1IF); // Wait here for timeout*/
/*    T1CONbits.TON = 0;*/
/*}*/

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

int setup(void){
    
    //Initialize timer funzies!!!
    //DelayTimer1Init();
    
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
    
	oc_pwm(&oc1, &D[6], NULL, 500, 0);//go right
    oc_pwm(&oc2, &D[5], NULL, 500, 0);//go left
    
    int j=0; int p0=0;
    for(j=0; i<=90; i++){
        p0=p0 + (update_pos(pin_read(&A[0]))-59215)/-17;
    }
    p0 = p0/90;
    return p0;
}

void delay(int delayTime){
    int j = 0;
    for(j=0; j<=delayTime; j++){
        0.6+0.3/0.2*.009/.007+.006-.90001+0.789/.007*300.9/.90001;
        0.6+0.3/0.2;
        0.6+0.3/0.2;
        0.6+0.3/0.2;
        0.789/.007*300.9/.90001;
        0.789/.007*300.9/.90001;
        0.789/.007*300.9/.90001;
    }
}

void writeMotor(int writeAmount, int motorDirectionDes){
    int Km = 100; //Motor write command constant!
    int controlCommand = 0; 
    
    //Case spin to the right:
    if (motorDirectionDes == 1){
        controlCommand = writeAmount *Km;
        //Write to motor direction 1 (right)
        if (motorDirection==1){
            pin_write(&D[6],controlCommand);
        }
        if (motorDirection==0){
            pin_write(&D[6],0);
            pin_write(&D[5], controlCommand);
            motorDirection=1;
        }
    }
    
    //Case spin to the left
    if (motorDirectionDes == 0){
        //Write to motor direction left (0)
        controlCommand = writeAmount *Km;
        if (motorDirection==1){
            pin_write(&D[6],controlCommand);
        }
        if (motorDirection==0){
            pin_write(&D[5],0);
            pin_write(&D[6], controlCommand);
            motorDirection=1;   
        }
    }
    //printf("Write amount is: %i , Write direction is: %i",writeAmount,motorDirectionDes);
}


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
    if ((position<60) && (position<400)){
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

int spring(int k, int p0, int readings[]){
    int direction= 3; 
    int position = sum_array(readings, TIME_READING_WINDOW);
    int distanceOff = position-p0;
    int command = distanceOff/TIME_READING_WINDOW/k;
    //int derivative = sum_difference(readings,TIME_READING_WINDOW);
    if(command>0){
        direction=1;
    }
    if (command<0){
        direction = 0;
    }
    writeMotor(abs(command), direction);
    printf("Distance off IP is: %i, write direction is: %i for a command of: %i \n",distanceOff,direction,abs(command));
}


int damper(int k, int readings[]){
    int motorDirection = 3; //Should the motor spin right or left?
    int derivative = sum_difference(readings,TIME_READING_WINDOW);
    int writeCommand = abs(derivative/TIME_READING_WINDOW/k);
    if(derivative>0){
        //Actuate to the right = 1:
        motorDirection = 1;
    }
    if(derivative<0){
        //actuate motor left = 0:
        motorDirection = 0;
    }
    
    writeMotor(writeCommand, motorDirection);
    printf("Motor direction of: %i with command of: %i from derivative: %i \n", motorDirection, writeCommand, derivative);
    //printf("Motor derivative position is: %i \nCommand value is: %i \n Write command is: %i\n", derivative, command, writeCommand);
}


int texture(int Kt){
    int text[11]={1,1,0,0,0,0,1,0,0,1};
    int i=0;
    int j=0;
    long delayTime = 2000000000;
    for (i=0; i<=(sizeof(text)/sizeof(text[0])); i++){
        writeMotor(Kt, text[i]);
        printf("Motor direction is: %i                at index %i\n",text[i],i);
        //The most ghetto delay function ever
        delay(delayTime);
    }
}

int wall(){
    
}

int main(){
    printf("Hi, Halie. You rock my socks! Emily, you're a super lazer kitty!'\n");
    int p0 = setup();
    //Controler receives the state over USB
    /*0 is spring
    1 is damper
    2 is texture
    3 is wall*/
    int position = 0;
    int KDd = 1; //constant fof the damper derivative control!
    int KSs = 2; //Constant for spring setting
    int Kt = 120; //Motor control constant for texture mode!
    
    /*Make an array to hole our position readings and initialize its elements to 0. */
    int readings[TIME_READING_WINDOW+1];
    int i = 0; 
    //Initialize array to zero, so we don't get weird errors when there's strange values in memory!
    for(i; i<TIME_READING_WINDOW; i++){
        readings[i]=0;
    }
    writeLEDs(0,0,0);
    
    while(1){
        position = (update_pos(pin_read(&A[0]))-59215)/-17;
        //position=position+20; //TODO:Remove this evil beast!
        //printf("Posion reading new is: ,%d\n",position);
        //Shift every element in the array one space to the left
/*        printf("\n");*/
        for(i=1; i<=TIME_READING_WINDOW;i++){
            readings[i-1]=readings[i];
        }
        readings[TIME_READING_WINDOW]=position;
        
        //Print out contents of position array
/*        for(i=0; i<TIME_READING_WINDOW; i++){*/
/*            printf("[ %d ]",readings[i]);*/
/*        }*/
        
        //printf("\n");
        int state = 0; //TODO: Hardcode state for testing!
        switch (state){
            case 0:
                //The spring mode!  
                writeLEDs(1,0,0);
                spring(KSs, p0, readings);
                break;
            case 1:
                //The damper mode!!!
                writeLEDs(0,1,0);
                damper(KDd, readings);
                break;
            case 2:
                //Texture mode!
                writeLEDs(1,1,0);
                texture(Kt);
                break;
            case 3: 
                //Wall mode!
                writeLEDs(0,0,1);
                wall();
                break;   
        }
    }
}

