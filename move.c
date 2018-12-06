/**
 * @file move.c
 * @brief this file will contain all the movement functions
 * @author Ismael Duran & Giovanni Mejia 
 * @date 12/05/2018
 */

#include <move.h>

///calibrated left wheel speed
int leftWheelSpeed = 150;
///calibrated right wheel speed
int rightWheelSpeed = 150;
///string to be sent over UART to the GUI
char str[50];

/**
 * move the robot forward by a specified distance
 * @param sensor 		connection to the OI sensors and motors
 * @param centimeters 	distance to move forward measured in centimeters
 * @author Ismael Duran & Giovanni Mejia 
 * @date 12/05/2018
 */
void move_forward(oi_t *sensor, int centimeters){
	///the distance traversed so far
    int sum = 0;
    ///update the OI data
    oi_update(sensor);
    ///set wheels to move forward
    oi_setWheels(rightWheelSpeed,leftWheelSpeed);
    ///boolean to determine if an object was hit or not
    int hitObject = 0;
    ///while loop while it reaches past given centimeters
    while(sum < centimeters*10){
    	///update the OI data
        oi_update(sensor);
        ///update the distance traversed
        sum += sensor->distance;
        ///if the left bumper is pressed
        if (sensor->bumpLeft) {
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that an object was found in front of the left bump sensor
            sprintf(str,"1,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the right bumper is pressed
        else if(sensor->bumpRight){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that an object was found in front of the right bump sensor
            sprintf(str,"2,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the robot detects a cliff
        else if(sensor->cliffFrontRight || sensor->cliffFrontLeft){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that a cliff was found in front of the robot
            sprintf(str,"3,%d,e", sum);
            uart_sendStr(str);
            //set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the robot detects white tape
        else if((sensor->cliffFrontRightSignal > 2700) || (sensor->cliffFrontLeftSignal > 2700)){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that the white tape was found
            sprintf(str,"6,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the left side of the robot detects a cliff
        else if(sensor->cliffLeft){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that a cliff was found to the left of the robot
            sprintf(str,"5,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the left side of the robot detects white tape
        else if(sensor->cliffLeftSignal > 2700){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that white tape was found to the left of the robot
            sprintf(str,"8,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
        ///if the right side of the robot detects a cliff
        else if(sensor->cliffRight){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that a cliff was found to the right of the robot
            sprintf(str,"4,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            //break the while loop early
            break;
        }
        ///if the right side of the robot detects white tape
        else if(sensor->cliffRightSignal > 2700){
        	///back up the robot
            bumper_backward(sensor, sum);
            ///inform the GUI that white tape was found to the right of the robot
            sprintf(str,"7,%d,e", sum);
            uart_sendStr(str);
            ///set the hit object boolean to true
            hitObject=1;
            ///break the while loop early
            break;
        }
    }
    ///if the hit object boolean was never set to true
    if(hitObject==0){
    	///inform the GUI that the movement finished successfully
        sprintf(str, "-1,%d,e", sum);
        uart_sendStr(str);
    }
    ///stop the robot
     oi_setWheels(0,0);
}

/**
 * turn the robot by a specified amount
 * @param sensor 	connection to OI motors and sensors
 * @param degrees	amount of degrees to turn
 * @author Ismael Duran & Giovanni Mejia
 * @date 12/05/2018
 */
void turn(oi_t *sensor, int degrees){
	///set angle to zero
    oi_update(sensor);
    ///turns clockwise if angle is less than zero
    if(degrees<0){
        ///right wheel moves backward and left wheel moves forward
        oi_setWheels(-rightWheelSpeed,leftWheelSpeed);
        ///reset the angle turned
        int sum = 0;
        ///while sum is a higher value than the degree given
        while(sum>degrees){
        	///update the sensor
            oi_update(sensor);
            ///update the degrees turned
            sum += sensor->angle;
        }
    }
    ///turns counterclockwise if  degree is positive
    else{
        ///right wheel moves forward and left wheel moves backward
        oi_setWheels(rightWheelSpeed,-leftWheelSpeed);
        ///reset the angle turned
        int sum = 0;
        // keeps updating time until the sum is greater than the degree given
        while(sum<degrees){
        	///update the sensor
            oi_update(sensor);
            ///update the degrees turned
            sum += sensor->angle;
        }
    }
    ///stop the robot from turning
    oi_setWheels(0,0);
}

/**
 * This method moves the robot back a specified distance
 * @param sensor		connection to OI sensors and motors
 * @param centimeters 	distance to travel in centimeters
 * @author Ismael Duran & Giovanni Mejia 
 * @date 12/05/2018
 */
void move_backward(oi_t *sensor, int centimeters){
	///distance traveled
    int sum = 0;
    ///update the sensors
    oi_update(sensor);
    ///set wheels moving back
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
    ///move back while distance traveled is less than given centimeters
    while(sum < centimeters*10){
    	///update sensors
        oi_update(sensor);
        ///update the distance traversed
        sum += abs( sensor->distance);
    }
    ///stop the robot
    oi_setWheels(0,0);
}

/**
 * This method moves the robot back a specified distance
 * @param sensor connection to the OI sensors and motors
 * @param millimeters distance to travel in millimeters
 * @author Ismael Duran
 * @date 12/05/2018
 */
void bumper_backward(oi_t *sensor, int millimeters){
	///distance traveled
    int sum = 0;
    ///update the sensors
    oi_update(sensor);
    ///set the wheels moving back
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
    ///move back while distance traveled is less than given millimeters
    while(sum < millimeters){
    	//update the sensors
        oi_update(sensor);
        ///update the distance traversed
        sum += abs( sensor->distance);
    }
    ///stop the robot
    oi_setWheels(0,0);
}

