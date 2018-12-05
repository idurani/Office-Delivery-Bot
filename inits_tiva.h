/*
 * inits_tiva.h
 * @brief This header file will contain the initialization and read functions for the sensors
 * @author Jared Rickard
 * @date 12/05/2018
 */

#ifndef INITS_TIVA_H_
#define INITS_TIVA_H_


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inc/tm4c123gh6pm.h>
#include <math.h>
#include <open_interface.h>

struct block
{
    float width;
    int center;
    float distance;
    float x;
    float y;
};

///initialize the ping sensor
void ping_init(void);

///initialize the IR sensor
void adc_init(void);

///initialize the PWM signal for the turret servo
void PWM_init(void);

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
void sweep_servo(volatile float (*sweepDataPtr)[2][91]);

///sweep the sensor and parse through the data, finding the first six objects that are larger than 2 degrees
void read_turret(volatile float (*sweepDataPtr)[2][91],volatile  struct block (*finds)[6],volatile  int *objectCount);


#endif /* INITS_TIVA_H_ */
