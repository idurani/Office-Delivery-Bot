

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



void move_forward(oi_t *sensor, int centimeters){
    int sum = 0;
    oi_setWheels(rightWheelSpeed,leftWheelSpeed);
    while(sum < centimeters*10){


        oi_update(sensor);
        sum += sensor->distance;
    }
    oi_setWheels(0,0);
}

void turn(oi_t *sensor, int degrees){

    if(degrees<0){
        oi_setWheels(-rightWheelSpeed,leftWheelSpeed); // turning clockwise
        oi_update(sensor);
        int sum = 0;
        while(sum>degrees){
            sum += sensor->angle;
            oi_update(sensor);
        }
    }
    else{
        oi_setWheels(rightWheelSpeed,-leftWheelSpeed); // turning counterclockwise
        oi_update(sensor);
        int sum = 0;
        while(sum<degrees){
            sum += sensor->angle;
            oi_update(sensor);
        }
    }

    oi_setWheels(0,0);
}



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



