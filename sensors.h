/**
 * sensors.h
 * @brief This header file will contain the functions pertaining to the sensors
 * @author Jared Rickard, Taylor Burton
 * @date 12/05/2018
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inc/tm4c123gh6pm.h>
#include <math.h>
#include <open_interface.h>
#include <uart.h>

struct block
{
    float width;
    int center;
    float distance;
    float x;
    float y;
};

///send a sound pulse from the ping sensor
void sendPulse(void);

///compute the distance for a sound pulse from the ping sensor
float read_ping(void);

///compute the distance from the IR sensor and store it in a local variable
void read_ir(void);

///get the raw ADC value from the last time the IR sensor was used
int get_ir_raw(void);

///get the distance measured by the IR sensor the last time it was used
float get_dis_ir(void);

///move the turret servo by a specified amount
void move_servo(float deg);

///sweep the sensor turret 180 degrees, recording the data from the ping and IR sensors every 2 degrees
void sweep_servo();

///sweep the sensor and parse through the data, finding the first six objects that are larger than 2 degrees
void read_turret();

///Function to have bot scan around it to identify small or large objects to notify the GUI
void scan();

#endif /* SENSORS_H_ */
