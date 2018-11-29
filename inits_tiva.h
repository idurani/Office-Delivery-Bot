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


void ping_init(void);
void adc_init(void);
void PWM_init(void);

void sendPulse(void);
float read_ping(void);
void read_ir(void);
int get_ir_raw(void);
float get_dis_ir(void);
void move_servo(float deg);



#endif /* INITS_TIVA_H_ */
