/*Code written by Halie Murray-Davis for Elecanisms at Franklin W. 
Olin College of Engineering in Feburary 2015. 

Agknowledgements: 
Integral control based off: 
http://www.facstaff.bucknell.edu/mastascu/eControlHTML/PID/PID2.html
*/

#include <stdio.h>

#define TIME_READING_WINDOW 30
#define SET_POINT 30
#define KP 2
//KI will be what the integral control is devided by, 
// for the KI you want, compute KI_desired^(-1)
#define KI 4
//KD will be what deritive control is devided by,
//cor the KD you want, compute KD_desired^(-1)
#define KD 1

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
    for(int i=0; i<num_elements;++i){
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
    
    for(int i=1; i<num_elements; i++){
        difference = a[i]-a[i-1];
        //printf("Diff is: %2d\n",difference);
        sum_difference = sum_difference + difference;
    }
    //printf("Sum diff is: %2d\n",sum_difference);
    return sum_difference;
}

int main(){
    //allocate array for sensor readings:
    int readings[TIME_READING_WINDOW]; 
    int sensorReading = 5; //pseudo sensor reading, TODO: REPLACE WITH REAL VALUE
    
    //Oust the deamons: (Initialize array)
    for(int i=0; i<TIME_READING_WINDOW; ++i){
        readings[i]=0;
    }
    
    //Loop through the array and discard the oldest position reading:
    for(int i=1; i<TIME_READING_WINDOW; ++i){
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
    for(int i=0; i<=TIME_READING_WINDOW-1; i++){
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
    
    //Print computed values for debugging purposes. Comment out on PIC. 
    printf("The sum of the sensor readings is: %3d \n", sum_sensor_readings);
    printf("Current position is: %3d\n",current_position);
    printf("Command for porportional motor control is: %3d\n",motor_command_portional);
    printf("Command for integral motor control is: %3d\n",motor_command_integral);
    printf("Command for deritive motor control is: %3d\n",motor_command_derivative);

    return 0;
}
