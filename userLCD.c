/*
 * userLCD.c
 *
 *  Created on: Nov 14, 2018
 *      Author: tayburt
 */



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
void setLocations(){
    sprintf(locations[0].name, "Pam's Desk");
    locations[0].xCord = 1;
    locations[0].yCord = 1;

    sprintf(locations[1].name, "Dwight's Lair");
    locations[1].xCord = 2;
    locations[1].yCord = 2;

    sprintf(locations[2].name, "Michael's Office");
    locations[2].xCord = 3;
    locations[2].yCord = 3;
}

void userSelect(){
    destination = -1;
    task = -1;
    int selected = 0;
    int button;
    while(destination == -1){
        button = button_getButton();
        if(button == 6){
            selected = (selected + 1) % numOfLocations; //increase the count
        }
        else if(button == 5){
            selected -= 1;  //decrease the count
            if(selected < 0){
                selected = numOfLocations -1;
            }
        }

        if(button == 1){
            destination = selected;
        }

        lcd_printf(locations[selected].name);

        timer_waitMillis(200);


    }
    selected = 1;

    //Wait for user to stop pressing button
    while(button == 1){
        button = button_getButton();
    }
    while(task == -1){
        button = button_getButton();
        if(button == 6){
                selected += 1; //increase the count
                if(selected > numOfTasks){
                    selected = 1;
                }
            }
            else if(button == 5){
                selected -= 1;  //decrease the count
                if(selected < 1){
                    selected = numOfTasks;
                }
            }

            if(button == 1){
                task = selected;
            }

            lcd_printf("Task: %d",selected);

            timer_waitMillis(200);
    }
}
