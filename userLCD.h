/*
 * userLCD.h
 *
 *  Created on: Nov 14, 2018
 *      Author: tayburt
 */

#ifndef USERLCD_H_
#define USERLCD_H_



#include "lcd.h"
#include "button.h"
#include "timer.h"
#include <stdint.h>

#include <inc/tm4c123gh6pm.h>

#define numOfLocations 3
#define numOfTasks 2

/**
 * Allows the user to select a location
 * using the buttons and the LCD
 */
struct location{
    char name[20];
    int xCord;
    int yCord;
};

struct location locations[numOfLocations];

/**
 * The task that we would like to accomplish
 */
int task;


//index of the location
int destination;

/**
 * Sets the name and coordinates of each location
 */
void setLocations();

void userSelect();

#endif /* USERLCD_H_ */
