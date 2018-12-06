/*
 * inits_tiva.h
 * @brief This header file will contain the initialization functions
 * @author Jared Rickard, Taylor Burton
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

///initialize the ping sensor
void ping_init(void);

///initialize the IR sensor
void adc_init(void);

///initialize the PWM signal for the turret servo
void PWM_init(void);

///Load in a song to the bit
void loadMary();

#endif /* INITS_TIVA_H_ */
