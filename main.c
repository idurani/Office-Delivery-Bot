/**
 * 		@file main.c
 * 		@brief This file contains the movement functions and the main function for the robot
 * 		@author Jared Rickard, Taylor BurtonIsmael Duran, Giovanni Mejia
 * 		@date 12/05/2018
 */
#include "open_interface.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <inits_tiva.h>
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "inits_tiva.h"

///Function to make the cybot move a given centimeters
void move_forward(oi_t *sensor, int centimeters);
///Fuction to make the cybot turn given degrees (degrees<0=right degrees>0=left)
void turn(oi_t *sensor, int degrees);
///Function to make the cybot move back a given amount of centimeters
void move_backward(oi_t *sensor, int centimeters);
///Function used when the move cybot backward when it bumps, edge, or cliff
///in millimeter to be more accurate when moving back
void bumper_backward(oi_t *sensor, int millimeters);
///Function to have bot scan around it to identify small or large objects to notify the GUI
void scan();
///Load in a song to the bit
void loadMary();

///calibrated left wheel speed
int leftWheelSpeed = 150;
///calibrated right wheel speed
int rightWheelSpeed = 150;
///cartesian coordinates, legacy
int x, y;

///the enumeration of relative directions
typedef enum{
    Forward,
    Backward,
    Left,
    Right
} direction;

///current direction of the robot
 direction dir = Forward;

 ///enumeration of the possible states that the robot can be in
typedef enum{
    leftEdge,
    rightEdge,
    frontEdge,
    leftBump,
    rightBump,
    forward,
    objectCenter,
    objectRight,
}state;

///string to be sent over UART to the GUI
char str[50];

///data array from sweep
volatile float sweepData[2][91];
///array of objects found during sweep
volatile struct block finds[6];
///number of objects found during sweep
volatile int objectCount = 0;


 void main(void){
	///initialize the UART
    uart_init();
    ///initialize the LCD
    lcd_init();
    ///initialize the ping sensor
    ping_init();
    ///initialize the IR sensor
    adc_init();
    ///initialize the PWM signal for the sensor turret servo
    PWM_init();
    ///set the sensor turret to 0 degrees for a quicker response
    move_servo(0);
    ///wait for the sensor turret to get into position
    timer_waitMillis(300);
    ///allocate space for the OI data
    oi_t *sensor = oi_alloc();
    ///initialize the OI
    oi_init(sensor);
    ///play
    loadMary();

    char command;
    move_forward(sensor,10);
    while(1){
       lcd_printf("waiting");
       timer_waitMillis(10);
       command = uart_receive();
       lcd_printf("%c", command);
       if(command == 'w'){
           //adjust for bot 7, move 30cm
           move_forward(sensor, 28);
       }
       if( command == 'd'){
           //turn 10 adjusted for bot 7
           turn(sensor, -1);
       }
       if( command == 's'){
           //adjusted for bot 7, move 10cm
           move_backward(sensor, 8);
       }
       if(command == 'a'){
           //turn 10 adjusted for bot 7
           turn(sensor, 1);
       }
       if(command == 'f'){
           scan();
       }
       if(command == 'q'){
           //turn 45 adjusted for bot 7
           turn(sensor, 36);
       }
       if(command == 'e'){
           //turn 45, adjusted for bot 7
           turn(sensor, -37);
       }
       if(command == 'r'){
           //turn 180, adjusted for bot 7
           turn(sensor, 173);
       }
       if(command == 'm'){
           //play mary had a little lamb
           oi_play_song(1);
       }



    }
}






// data example: 1,2,3,-15e
/*
 * dataType:
 * bumpLeft 1
 * bumpRight 2
 * cliffFront 3
 * cliffRight 4
 * cliffLeft 5
 * edgeFront 6
 * edgeRight 7
 * edgeLeft 8
 * scannedData 9
 * (datatype),data,data,data(followed by the char e)
 */

/**
 * move the robot forward by a specified distance
 * @param sensor 		connection to the OI sensors and motors
 * @param centimeters 	distance to move forward measured in centimeters
 * @author
 * @date 12/05/2018
 */
void move_forward(oi_t *sensor, int centimeters){
	///the distance traversed so far
    int sum = 0;
    ///update the OI data
    oi_update(sensor);
    ///set wheels to move forward
    oi_setWheels(rightWheelSpeed,leftWheelSpeed);
    ///boolean to determine if an object was hit or not
    int hitObject = 0;
    ///while loop while it reaches past given centimeters
    while(sum < centimeters*10){
    	///update the OI data
        oi_update(sensor);
        ///update the distance traversed
        sum += sensor->distance;
        ///if the left bumper is pressed
        if (sensor->bumpLeft) {
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that an object was found in front of the left bump sensor
            sprintf(str,"1,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the right bumper is pressed
        else if(sensor->bumpRight){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that an object was found in front of the right bump sensor
            sprintf(str,"2,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the robot detects a cliff
        else if(sensor->cliffFrontRight || sensor->cliffFrontLeft){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that a cliff was found in front of the robot
            sprintf(str,"3,%d,e", sum);
            uart_sendStr(str);
            //set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the robot detects white tape
        else if((sensor->cliffFrontRightSignal > 2700) || (sensor->cliffFrontLeftSignal > 2700)){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that the white tape was found
            sprintf(str,"6,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the left side of the robot detects a cliff
        else if(sensor->cliffLeft){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that a cliff was found to the left of the robot
            sprintf(str,"5,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the left side of the robot detects white tape
        else if(sensor->cliffLeftSignal > 2700){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that white tape was found to the left of the robot
            sprintf(str,"8,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the right side of the robot detects a cliff
        else if(sensor->cliffRight){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that a cliff was found to the right of the robot
            sprintf(str,"4,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            //break the while loop early
            break;
        }
        ///if the right side of the robot detects white tape
        else if(sensor->cliffRightSignal > 2700){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that white tape was found to the right of the robot
            sprintf(str,"7,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
    }
    ///if the hit object boolean was never set to true
    if(hitObject==0){
    	///inform the GUI that the movement finished successfully
        sprintf(str, "-1,%d,e", sum);
        uart_sendStr(str);
    }
    ///stop the robot
     oi_setWheels(0,0);
}

/**
 * turn the robot by a specified amount
 * @param sensor 	connection to OI motors and sensors
 * @param degrees	amount of degrees to turn
 * @author
 * @date 12/05/2018
 */
void turn(oi_t *sensor, int degrees){
	///set angle to zero
    oi_update(sensor);
    ///turns clockwise if angle is less than zero
    if(degrees<0){
        ///right wheel moves backward and left wheel moves forward
        oi_setWheels(-rightWheelSpeed,leftWheelSpeed);
        ///reset the angle turned
        int sum = 0;
        ///while sum is a higher value than the degree given
        while(sum>degrees){
        	///update the sensor
            oi_update(sensor);
            ///update the degrees turned
            sum += sensor->angle;
        }
    }
    ///turns counterclockwise if  degree is positive
    else{
        ///right wheel moves forward and left wheel moves backward
        oi_setWheels(rightWheelSpeed,-leftWheelSpeed);
        ///reset the angle turned
        int sum = 0;
        // keeps updating time until the sum is greater than the degree given
        while(sum<degrees){
        	///update the sensor
            oi_update(sensor);
            ///update the degrees turned
            sum += sensor->angle;
        }
    }
    ///stop the robot from turning
    oi_setWheels(0,0);
    ///switch the current direction
    switch(dir){

    case Forward:
        if(degrees==90){
            dir=Left;
        }
        else if(degrees == -90){
            dir = Right;
        }
        break;
    case Backward:
        if(degrees==90){
            dir=Right;
        }
        else if(degrees == -90){
            dir = Left;
        }
        break;
    case Left:
        if(degrees==90){
            dir=Backward;
        }
        else if(degrees == -90){
            dir = Forward;
        }
        break;
    case Right:
        if(degrees==90){
            dir=Forward;
        }
        else if(degrees == -90){
            dir = Backward;
        }
        break;
    default:
        break;
    }
}
void move_backward(oi_t *sensor, int centimeters){
    int sum = 0;
    oi_update(sensor);
    //set wheels moving back
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
    //move back while distance traveled is less than given centimeters 
    while(sum < centimeters*10){

        oi_update(sensor);
        sum += abs( sensor->distance);

    }
    oi_setWheels(0,0);

    switch(dir){

    case Forward:
        y -= centimeters;
        break;
    case Backward:
        y += centimeters;
        break;
    case Left:
        x += centimeters;
        break;
    case Right:
        x -= centimeters;
        break;
    default:
        break;
    }
}

void bumper_backward(oi_t *sensor, int millimeters){
    int sum = 0;
    oi_update(sensor);
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
    while(sum < millimeters){

        oi_update(sensor);
        sum += abs( sensor->distance);

    }
    oi_setWheels(0,0);

    switch(dir){

    case Forward:
        y -= sum;
        break;
    case Backward:
        y += sum;
        break;
    case Left:
        x += sum;
        break;
    case Right:
        x -= sum;
        break;
    default:
        break;
    }
}

/**
 * This method scans the area in front of the robot for objects
 * and sends this information to the GUI
 * @author Jared Rickard
 * @date 12/05/2018
 */
void scan()
{
    ///start off with zero objects
    objectCount = 0;
    ///read the data and finds object width, angle, and distance from the cybot
    read_turret(&sweepData, &finds, &objectCount);
    ///Strings to send to the cybot
    char str[100];
    char temp[20];
    sprintf(str, "9,%d", objectCount);
    ///determine if the object is large or small
    int LorS;
    //iterate through all the objects found during the scan
    int i;
    for(i = 0; i < objectCount; i++)
    {
    	///if the object's width was smaller than 7 cm
        if(finds[i].width < 7)
        {
        	///the object was a small object
            LorS = 0;
        }
        ///else the object's width was larger than 7 cm
        else
        {
        	///the object was a large object
            LorS = 1;
        }
        ///add the relevant to the string being sent to the GUI
        sprintf(temp, ",%d,%d,%d", LorS, (int) finds[i].x, (int) finds[i].y);
        strcat(str, temp);
    }
    ///add the final delimiter
    strcat(str, ",e");
    ///send object data to the GUI
    uart_sendStr(str);
    ///reset the position of the sensor turret
    move_servo(0);
}

//Load in a song 
void loadMary(){
    unsigned char notes[] = {64, 62, 60, 62, 64, 64, 64};
    unsigned char duration[] = {32, 32, 32, 32, 32, 32, 32};
    oi_loadSong(1, 7, notes, duration);
}

