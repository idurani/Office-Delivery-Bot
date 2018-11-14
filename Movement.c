

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
void turn_left(oi *sensor);
void turn_right(oi *sensor);

int leftWheelSpeed = 500;
int rightWheelSpeed = 495;

int main(void)
{
  /*  oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);



    move_forward(sensor_data, 50);
	return 0;

	*/

}


// function to have the cybot move forward
void move_forward(oi_t *sensor, int centimeters){
    int sum = 0;

    oi_update(sensor);
    //set wheels to move forward
    oi_setWheels(rightWheelSpeed,leftWheelSpeed);

    //while loop while it reaches past given centimeters
    while(sum < centimeters*10){

        sum += sensor->distance;
        oi_update(sensor);
    }

    // stop Cybot
    oi_setWheels(0,0);
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
            sum += sensor->angle;
            oi_update(sensor); // set angle to zero
        }
    }
    // turns counterclockwise if  degree is positive
    else{
        //right wheel moves forward and left wheel moves backward make it rotate couter clockwise
        oi_setWheels(rightWheelSpeed,-leftWheelSpeed); // turning counterclockwise
        int sum = 0;
        // keeps updating time until the sum is greater than the degree given
        while(sum<degrees){
            sum += sensor->angle;
            oi_update(sensor); // set angle to zero
        }
    }

    oi_setWheels(0,0);//stops the cybot
}


//Function to have the Cybot move backward
void move_backward(oi_t *sensor, int centimeter){
    int sum = 0;
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
    while(sum < centimeters*10){


        oi_update(sensor);
        sum += abs( sensor->distance);
    }
    oi_setWheels(0,0);
}

void turn_left(oi *sensor){
    turn(sensor, 90);
}

void turn_right(oi *sensor){
    turn(senosr, -90);
}

void spin(oi *sensor){

    int angle=0;
    oi_update(sensor);
    oi_setWheels(rightWheelSpeed, -leftWheelSpeed);
    while(angle<180){
        sum+= sensor->angle;
        oi_update(sensor);
    }
}



