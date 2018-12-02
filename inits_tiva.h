/*
 * dump.h
 *
 *  Created on: Oct 31, 2018
 *      Author: jrickard
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


void ping_init(void);
void adc_init(void);
void PWM_init(void);

void sendPulse(void);
float read_ping(void);
void read_ir(void);
int get_ir_raw(void);
float get_dis_ir(void);
void move_servo(float deg);
void sweep_servo(volatile float (*sweepDataPtr)[2][91]);
void read_turret(volatile float (*sweepDataPtr)[2][91],volatile  struct block (*finds)[6],volatile  int *objectCount);


#endif /* INITS_TIVA_H_ */
