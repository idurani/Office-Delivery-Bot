#include "open_interface.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <inits_tiva.h>
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "inits_tiva.h"

//Function to make the cybot move a given centimeters
void move_forward(oi_t *sensor, int centimeters);
//Fuction to make the cybot turn given degrees (degrees<0=right degrees>0=left)
void turn(oi_t *sensor, int degrees);
//Function to make the cybot move back a given amount of centimeters
void move_backward(oi_t *sensor, int centimeters);
//Function used when the move cybot backward when it bumps, edge, or cliff
//in millimeter to be more accurate when moving back
void bumper_backward(oi_t *sensor, int millimeters);
//Function to have bot scan around it to identify small or large objects to notify the GUI
void scan();
//Load in a song to the bit 
void loadMary();


int leftWheelSpeed = 150;
int rightWheelSpeed = 150;
int x, y;

typedef enum{
    Forward,
    Backward,
    Left,
    Right
} direction;


 direction dir = Forward;

typedef enum{
    leftEdge,
    rightEdge,
    frontEdge,
    leftBump,
    rightBump,
    forward,
    objectCenter,
    objectRight,
}state;

char str[50];

//data array from sweep
volatile float sweepData[2][91];
//array of objects found during sweep
volatile struct block finds[6];
//number of objects found during sweep
volatile int objectCount = 0;


 void main(void){
    uart_init();
    lcd_init();
    ping_init();
    adc_init();
    PWM_init();
    move_servo(0);
    timer_waitMillis(300);
    oi_t *sensor = oi_alloc();
    oi_init(sensor);

    loadMary();

    char command;
//    char angle;
//    char distance;
    move_forward(sensor,10);
    while(1){

    // read turret insert here
       lcd_printf("waiting");
       timer_waitMillis(10);


       command = uart_receive();
       lcd_printf("%c", command);
      // timer_waitMillis(1000);
       if(command == 'w'){
           //adjust for bot 7, move 30cm
           move_forward(sensor, 28);
       }
       if( command == 'd'){
           //turn 10 adjusted for bot 7
           turn(sensor, -1);
       }
       if( command == 's'){
           //adjusted for bot 7, move 10cm
           move_backward(sensor, 8);
       }
       if(command == 'a'){
           //turn 10 adjusted for bot 7
           turn(sensor, 1);
       }
       if(command == 'f'){
           scan();
       }
       if(command == 'q'){
           //turn 45 adjusted for bot 7
           turn(sensor, 36);
       }
       if(command == 'e'){
           //turn 45, adjusted for bot 7
           turn(sensor, -37);
       }
       if(command == 'r'){
           //turn 180, adjusted for bot 7
           turn(sensor, 173);
       }
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

void move_forward(oi_t *sensor, int centimeters){
    int sum = 0;

    oi_update(sensor);
    //set wheels to move forward
    oi_setWheels(rightWheelSpeed,leftWheelSpeed);
    int hitObject = 0;
    //while loop while it reaches past given centimeters
    while(sum < centimeters*10){

        oi_update(sensor);
        sum += sensor->distance;

        if (sensor->bumpLeft) { // Responsive if the left bumper is being pressed
            bumper_backward(sensor, sum);
            sprintf(str,"1,%d,e", sum);
            uart_sendStr(str);
            hitObject=1;
            break;

        }
        else if(sensor->bumpRight){ // Responsive if the right bumper is being pressed
            bumper_backward(sensor, sum);
            sprintf(str,"2,%d,e", sum);
            uart_sendStr(str);
            hitObject=1;
            break;
        }
        else if(sensor->cliffFrontRight || sensor->cliffFrontLeft){ //Responsive if the front cliff sensor are notified there is a cliff
            bumper_backward(sensor, sum);
            sprintf(str,"3,%d,e", sum);
            uart_sendStr(str);
            hitObject=1;
            break;
        }
        else if((sensor->cliffFrontRightSignal > 2700) || (sensor->cliffFrontLeftSignal > 2700)){ //Responsive if the front cliff signals are read as white color 
            bumper_backward(sensor, sum);
            sprintf(str,"6,%d,e", sum);
            uart_sendStr(str);
            hitObject=1;
            break;
        }
        else if(sensor->cliffLeft){ //responsive if left side of the bot sees a cliff
            bumper_backward(sensor, sum);
            sprintf(str,"5,%d,e", sum);
            uart_sendStr(str);
            hitObject=1;
            break;
        }
        else if(sensor->cliffLeftSignal > 2700){ //responsive if left side of the bot sees a white edge 
            bumper_backward(sensor, sum);
            sprintf(str,"8,%d,e", sum);
            uart_sendStr(str);
            hitObject=1;
            break;
        }
        else if(sensor->cliffRight){ //responsive if right side of the bot sees a cliff
            bumper_backward(sensor, sum);
            sprintf(str,"4,%d,e", sum);
            uart_sendStr(str);
            hitObject=1;

            break;
        }
        else if(sensor->cliffRightSignal > 2700){ //responsive if right side if the bot sees a white edge
            bumper_backward(sensor, sum);
            sprintf(str,"7,%d,e", sum);
            uart_sendStr(str);
            hitObject=1;
            break;
        }

    }
    //if hitObject equals zero non of the condition above happen and it sends it to the GUI
    if(hitObject==0){
        sprintf(str, "-1,%d,e", sum);
        uart_sendStr(str);
    }
     oi_setWheels(0,0);
}


void turn(oi_t *sensor, int degrees){
    oi_update(sensor); //set angle to zero

    //turns clockwise if angle is less than zero
    if(degrees<0){

        //right wheel goes back and left wheel goes forward
        oi_setWheels(-rightWheelSpeed,leftWheelSpeed); // turning clockwise
        int sum = 0;
        //while sum is a higher value than the degree given
        while(sum>degrees){

            oi_update(sensor);
            sum += sensor->angle;

        }
    }
    // turns counterclockwise if  degree is positive
    else{
        //right wheel moves forward and left wheel moves backward make it rotate couter clockwise
        oi_setWheels(rightWheelSpeed,-leftWheelSpeed); // turning counterclockwise
        int sum = 0;
        // keeps updating time until the sum is greater than the degree given
        while(sum<degrees){

            oi_update(sensor);
            sum += sensor->angle;

        }
    }

    oi_setWheels(0,0);//stops the cybot



    switch(dir){

    case Forward:
        if(degrees==90){
            dir=Left;
        }
        else if(degrees == -90){
            dir = Right;
        }
        break;
    case Backward:
        if(degrees==90){
            dir=Right;
        }
        else if(degrees == -90){
            dir = Left;
        }
        break;
    case Left:
        if(degrees==90){
            dir=Backward;
        }
        else if(degrees == -90){
            dir = Forward;
        }
        break;
    case Right:
        if(degrees==90){
            dir=Forward;
        }
        else if(degrees == -90){
            dir = Backward;
        }
        break;
    default:
        break;
    }
}
void move_backward(oi_t *sensor, int centimeters){
    int sum = 0;
    oi_update(sensor);
    //set wheels moving back
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
    //move back while distance traveled is less than given centimeters 
    while(sum < centimeters*10){

        oi_update(sensor);
        sum += abs( sensor->distance);

    }
    oi_setWheels(0,0);

    switch(dir){

    case Forward:
        y -= centimeters;
        break;
    case Backward:
        y += centimeters;
        break;
    case Left:
        x += centimeters;
        break;
    case Right:
        x -= centimeters;
        break;
    default:
        break;
    }
}

void bumper_backward(oi_t *sensor, int millimeters){
    int sum = 0;
    oi_update(sensor);
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
    while(sum < millimeters){

        oi_update(sensor);
        sum += abs( sensor->distance);

    }
    oi_setWheels(0,0);

    switch(dir){

    case Forward:
        y -= sum;
        break;
    case Backward:
        y += sum;
        break;
    case Left:
        x += sum;
        break;
    case Right:
        x -= sum;
        break;
    default:
        break;
    }
}

void scan()
{
    //start off with zero objects
    objectCount = 0;
    //read the data and finds object width, angle, and distance from the cybot 
    read_turret(&sweepData, &finds, &objectCount);
    //Strings to sent to the cybot 
    char str[100];
    char temp[20];
    sprintf(str, "9,%d", objectCount);
    int LorS;
    int i;
    /*
      This for goes through all the objects and indentifys if it is
      a small or large object 
    */
    for(i = 0; i < objectCount; i++)
    {
        if(finds[i].width < 7)
        {
            LorS = 0;
        }
        else
        {
            LorS = 1;
        }
        sprintf(temp, ",%d,%d,%d", LorS, (int) finds[i].x, (int) finds[i].y);
        strcat(str, temp);
    }
    strcat(str, ",e");
    //sends object data to the GUI
    uart_sendStr(str);
    move_servo(0);
}

//Load in a song 
void loadMary(){
    unsigned char notes[] = {64, 62, 60, 62, 64, 64, 64};
    unsigned char duration[] = {32, 32, 32, 32, 32, 32, 32};
    oi_loadSong(1, 7, notes, duration);
}

