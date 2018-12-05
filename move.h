/**
 * @file move.h
 * @brief this header file will contain the movement functions
 * @author
 * @date 12/05/2018
 */

#ifndef MOVE_H_
#define MOVE_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inc/tm4c123gh6pm.h>
#include <open_interface.h>
#include <lcd.h>
#include <uart.h>

///Function to make the cybot move a given centimeters
void move_forward(oi_t *sensor, int centimeters);

///Fuction to make the cybot turn given degrees (degrees<0=right degrees>0=left)
void turn(oi_t *sensor, int degrees);

///Function to make the cybot move back a given amount of centimeters
void move_backward(oi_t *sensor, int centimeters);

///Function used when the move cybot backward when it bumps, edge, or cliff
///in millimeter to be more accurate when moving back
void bumper_backward(oi_t *sensor, int millimeters);

#endif /* MOVE_H_ */
