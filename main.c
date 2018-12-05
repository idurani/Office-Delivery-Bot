/**
 * 		@file main.c
 * 		@brief This file contains the movement functions and the main function for the robot
 * 		@author Jared Rickard, Taylor Burton, Ismael Duran, Giovanni Mejia
 * 		@date 12/05/2018
 */
#include "open_interface.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <inits_tiva.h>
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "inits_tiva.h"
#include "sensors.h"
#include "move.h"

void main(void){
	///initialize the UART
	uart_init();
	///initialize the LCD
	lcd_init();
	///initialize the ping sensor
	ping_init();
	///initialize the IR sensor
	adc_init();
	///initialize the PWM signal for the sensor turret servo
	PWM_init();
	///set the sensor turret to 0 degrees for a quicker response
	move_servo(0);
	///wait for the sensor turret to get into position
	timer_waitMillis(300);
	///allocate space for the OI data
	oi_t *sensor = oi_alloc();
	///initialize the OI
	oi_init(sensor);
	///send song information to the robot
	loadMary();
	///command key from the GUI
	char command;
	///move forward 10 centimeters automatically
	move_forward(sensor,10);
	///infinite loop to wait for commands
	while(1){
		///print on the screen that the robot is waiting for a command
       lcd_printf("waiting");
       ///wait 10 ms
	   timer_waitMillis(10);
	   ///set the command to the key received through the UART
	   command = uart_receive();
	   ///print out the command on the screen
	   lcd_printf("%c", command);
	   ///if the command was 'w', move forward 30 cm
	   if(command == 'w'){
		   //adjust for bot 7, move 30cm
		   move_forward(sensor, 28);
	   }
	   ///if the key pressed was 'd', turn 10 degrees to the right
	   if( command == 'd'){
		   //turn 10 adjusted for bot 7
		   turn(sensor, -1);
	   }
	   ///if the key pressed was 's', move backward 10 cm
	   if( command == 's'){
		   //adjusted for bot 7, move 10cm
		   move_backward(sensor, 8);
	   }
	   ///if the key pressed was 'a', turn 10 degrees to the left
	   if(command == 'a'){
		   //turn 10 adjusted for bot 7
		   turn(sensor, 1);
	   }
	   ///if the key pressed was 'f', scan the area in front of the bot
	   if(command == 'f'){
		   scan();
	   }
	   ///if the key pressed was 'q', turn 45 degrees to the left
	   if(command == 'q'){
		   //turn 45 adjusted for bot 7
		   turn(sensor, 36);
	   }
	   ///if the key pressed was 'e', turn 45 degrees to the right
	   if(command == 'e'){
		   //turn 45, adjusted for bot 7
		   turn(sensor, -37);
	   }
	   ///if the key pressed was 'r', turn around
	   if(command == 'r'){
		   //turn 180, adjusted for bot 7
		   turn(sensor, 173);
	   }
	   ///if the key pressed was 'm', play music
	   if(command == 'm'){
		   //play mary had a little lamb
		   oi_play_song(1);
	   }
    }
}






// data example: 1,2,3,-15e
/*
 * dataType:
 * bumpLeft 1
 * bumpRight 2
 * cliffFront 3
 * cliffRight 4
 * cliffLeft 5
 * edgeFront 6
 * edgeRight 7
 * edgeLeft 8
 * scannedData 9
 * (datatype),data,data,data(followed by the char e)
 */
