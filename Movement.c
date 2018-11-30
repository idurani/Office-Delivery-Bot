

/**
 * main.c
 *
 *
 */

#include "open_interface.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <inits_tiva.h>
#include "lcd.h"
#include "timer.h"
#include "uart.h"



#define tpd 153
#define zeroPos 7500
#define power -1.086
#define coe 65430



volatile float sweepData[2][91];



void move_forward(oi_t *sensor, int centimeters);
void turn(oi_t *sensor, int degrees);
void move_backward(oi_t *sensor, int centimeter);
void turn_left(oi_t *sensor);
void turn_right(oi_t *sensor);
void spin(oi_t *sensor);
void bumper_backward(oi_t *sensor, int millimeters);
void read_turret();
void sweep_servo();
void checkObjects();


int leftWheelSpeed = 250;
int rightWheelSpeed = 250;

/**
 * width is the width of the object
 * center is the degree messurement
 * distance is the distance form the roomba
 */
struct block
{
    float width;
    int center;
    float distance;

};

volatile struct block finds[6];
volatile int objectCount = 0;

typedef enum{
    Forward,
    Backward,
    Left,
    Right
} direction;


 direction dir = Forward;

int x=0, y=0;


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

state s = forward;






int main(void)
{
   oi_t *sensor = oi_alloc();
   oi_init(sensor);

   ping_init();
   adc_init();
   PWM_init();


   while(1){
       if(s == forward){
         read_turret();
         checkObjects();
       }

       //scan
       // case for object in front
       // case for goal object
       if(s == forward){
           move_forward(sensor, 30);
       }
       else if(s == frontEdge){
           turn_right(sensor);
           s = forward;
       }
       else if(s == rightEdge){

           turn(sensor, 20);
           s = forward;
       }
       else if(s == leftEdge){

           turn(sensor, -20);
           s = forward;
       }
       else if(s == rightBump){
           turn_right(sensor);
           move_forward(sensor, 40);
           turn_left(sensor);
           s = forward;
       }
       else if(s == leftBump){
           turn_right(sensor);
           move_forward(sensor, 15);
           turn_left(sensor);
           s = forward;
       }
       else if(s == objectCenter){
           turn(sensor, -45);
           move_forward(sensor, 20);
           turn(sensor, 45);
           s = forward;
       }
       else if(s == objectRight){
           move_backward(sensor, 10);
           turn(sensor, -90);
           move_forward(sensor, 20);
           turn(sensor, 90);
           s = forward;
       }


   }
	return 0;



}

void checkObjects(){
    int i;
    //says if there is an object to the right
    int objectRightFlag = 0;
    //distance we are comparing to
    float D;

    for(i = 0; i < 6; i++){
        D = 20/(cos(finds[i].center-90));
        if(finds[i].distance < 35 && finds[i].center > 0){
            if(finds[i].center < 54){
                objectRightFlag = 1;
            }
            if((finds[i].center > 54) && (finds[i].center < 128)){
                if(s == forward){
                  s = objectCenter;
                }
            }

            //this mmight be the problem
            if(D >= finds[i].distance){
                s = objectCenter;
            }
        }
    }

    if(s == objectCenter && objectRightFlag == 1){
        s = objectRight;
    }
}


// function to have the cybot move forward
void move_forward(oi_t *sensor, int centimeters){
    int sum = 0;

    oi_update(sensor);
    //set wheels to move forward
    oi_setWheels(rightWheelSpeed,leftWheelSpeed);

    //while loop while it reaches past given centimeters
    while(sum < centimeters*10){

        oi_update(sensor);
        sum += sensor->distance;

        if (sensor->bumpLeft) { // Responsive if the left bumper is being pressed
            bumper_backward(sensor, sum);
            s = leftBump;
            break;

        }
        else if(sensor->bumpRight){ // Responsive if the right bumper is being pressed
            bumper_backward(sensor, sum);
            s = rightBump;
            break;
        }
        else if(sensor->cliffFrontRight || sensor->cliffFrontLeft || (sensor->cliffFrontRightSignal > 2700) || (sensor->cliffFrontLeftSignal > 2700)){
            bumper_backward(sensor, sum);
            s = frontEdge;
            break;
        }
        else if(sensor->cliffLeft || (sensor->cliffLeftSignal > 2700)){
            bumper_backward(sensor, sum);
            s = leftEdge;
            break;
        }
        else if(sensor->cliffRight || (sensor->cliffRightSignal > 2700)){
            bumper_backward(sensor, sum);
            s = rightEdge;
            break;
        }

 }

    // stop Cybot
    oi_setWheels(0,0);


    switch(dir){

    case Forward:
        y += centimeters;
        break;
    case Backward:
        y -= centimeters;
        break;
    case Left:
        x -= centimeters;
        break;
    case Right:
        x += centimeters;
        break;
    default:
        break;
    }


}

void turn(oi_t *sensor, int degrees){
    oi_update(sensor); //set angle to zero

    //turns clockwise if angle is less than zero
    if(degrees<0){

        //right wheel goes back and left wheel goes forward
        oi_setWheels(-rightWheelSpeed,leftWheelSpeed); // turning clockwise
        int sum = 0;
        //while sum is a higher value than the degree given
        while(sum>degrees){

            oi_update(sensor);
            sum += sensor->angle;

        }
    }
    // turns counterclockwise if  degree is positive
    else{
        //right wheel moves forward and left wheel moves backward make it rotate couter clockwise
        oi_setWheels(rightWheelSpeed,-leftWheelSpeed); // turning counterclockwise
        int sum = 0;
        // keeps updating time until the sum is greater than the degree given
        while(sum<degrees){

            oi_update(sensor);
            sum += sensor->angle;

        }
    }

    oi_setWheels(0,0);//stops the cybot



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


//Function to have the Cybot move backward
void move_backward(oi_t *sensor, int centimeters){
    int sum = 0;
    oi_update(sensor);
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
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

void turn_left(oi_t *sensor){
    turn(sensor, 90);
}

void turn_right(oi_t *sensor){
    turn(sensor, -90);
}

void spin(oi_t *sensor){

    int angle=0;

    oi_update(sensor);
    oi_setWheels(rightWheelSpeed, -leftWheelSpeed);
    while(angle<360){

        oi_update(sensor);
        angle+= sensor->angle;

    }
    oi_setWheels(0,0);
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







void sweep_servo(){
    //char str[50];
    //sprintf(str, "Deg\tIR Distance(cm)\tSonarDistance(cm)\n\r");
    int i;
    //sweep the servo across 180 degrees, 2 degrees at a time
    for(i = 0; i < 91; i += 1){
        //move by two degrees
        move_servo(i*2);
        //give the servo time to move
        timer_waitMillis(20);
        //read ir data
        read_ir();
        //save data into the next section of the array
        sweepData[0][i] = get_dis_ir();
        sweepData[1][i] = read_ping();
        //sprintf(str, "%d\t%.1f\t\t%.1f\n\r", i*2, sweepData[0][i], sweepData[1][i] );
        //uart_sendStr(str);
    }
}



void read_turret(){
    //record data
        sweep_servo();
    //variables for iterating through data to find blocks and info about blocks
        objectCount = 0;
        int i;
        int looking = 0;
        int currentWidth = 0;
        float minDist = 100;

        for(i = 0; i < 6; i++){
            //if the center is -1 then the object is NULL
            finds[i].center = -1;
        }
        //start iterating through data gathered
        for(i = 0; i < 90; i++)
        {
            //if it finds an object, set the boolean variable indicating an object found to 1
            if(sweepData[0][i] < 100 && sweepData[1][i] < 50 && looking == 0)
            {
                looking = 1;
            }
            //if the end of an object is found
            if((sweepData[0][i] >= 100 || sweepData[1][i] >= 50) && looking == 1)
            {
                //set boolean variable indicating an object found to 0
                looking = 0;
                //record distance
                finds[objectCount].distance = minDist;
                //compute center of object
                finds[objectCount].center = i*2-currentWidth;
                //compute width of object using arc length formula
                finds[objectCount].width = currentWidth * (6.28 / 180) * minDist;
                //iterate to the next block in the block array
                objectCount++;
                //reset helper variables
                currentWidth = 0;
                minDist = 100;
            }
            //if currently looking at a block
            if(looking == 1)
            {
                //increase the width helper variable
                currentWidth++;
                //make sure the minimum distance of the object is still the minimum distance
                if(sweepData[1][i] < minDist)
                {
                    minDist = sweepData[1][i];
                }
            }
            //if the block array is full
            if(objectCount == 6)
            {
                break;
            }
        }
}


