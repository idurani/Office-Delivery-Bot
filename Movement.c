

/**
 * main.c
 *
 *
 */

#include "open_interface.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>


void move_forward(oi_t *sensor, int centimeters);
void turn(oi_t *sensor, int degrees);
void move_backward(oi_t *sensor, int centimeter);
void turn_left(oi_t *sensor);
void turn_right(oi_t *sensor);
void spin(oi_t *sensor);
void bumper_backward(oi_t *sensor, int millimeters);


int leftWheelSpeed = 250;
int rightWheelSpeed = 250;

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
}state;

state s = forward;

int main(void)
{
   oi_t *sensor = oi_alloc();
   oi_init(sensor);

   while(1){

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

        //    move_forward(sensor_data, 20);
        //    turn_right(sensor_data);
        //    turn_right(sensor_data);
        //    turn_right(sensor_data);
        //    turn_left(sensor_data);
        //    move_forward(sensor_data, 20);
        //    move_backward(sensor_data, 20);
        //    spin(sensor_data);
   }
	return 0;



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


