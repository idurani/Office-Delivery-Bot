#include "open_interface.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <inits_tiva.h>
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "inits_tiva.h"



#define tpd 153
#define zeroPos 7500
#define power -1.086
#define coe 65430


void move_forward(oi_t *sensor, int centimeters);
void turn(oi_t *sensor, int degrees);
void move_backward(oi_t *sensor, int centimeter);
void turn_left(oi_t *sensor);
void turn_right(oi_t *sensor);
void spin(oi_t *sensor);
void bumper_backward(oi_t *sensor, int millimeters);
void read_turret();
void sweep_servo();
void sweep_servo();
void read_turret();
void sendPulse();
float read_ping();
void read_ir();
int get_ir_raw();
float get_dis_ir();
void move_servo(float deg);
void sendSweepData();
int checkClearPath(float yMax);
int turretAvoid(oi_t *sensor, int cm);
void clearObjects();

int leftWheelSpeed = 250;
int rightWheelSpeed = 250;

/**
 * width is the width of the object
 * center is the degree messurement
 * distance is the distance form the roomba
 */
struct block
{
    float width;
    int center;
    float distance;

};

volatile struct block finds[6];
volatile int objectCount = 0;
volatile float sweepData[2][91];

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

state s = forward;

int x, y;




int main(void)
{
   oi_t *sensor = oi_alloc();
   oi_init(sensor);

   ping_init();
   adc_init();
   PWM_init();
   uart_init();
   lcd_init();

   int avoided;
   while(1){


       //scan
       // case for object in front
       // case for goal object
       if(s == forward){
           avoided = turretAvoid(sensor, 30);
           lcd_printf("%d", avoided);
       }
       else if(s == frontEdge){
           s = forward;
           turn_right(sensor);
       }
       else if(s == rightEdge){
           s = forward;
           turn(sensor, 20);
       }
       else if(s == leftEdge){
           s = forward;
           turn(sensor, -20);
       }
       else if(s == rightBump){
           s = forward;
           turn_right(sensor);
           avoided = turretAvoid(sensor, 40);
           if(avoided == 0){
               turn_left(sensor);
           }
       }
       else if(s == leftBump){
           s = forward;
           turn_right(sensor);
           avoided = turretAvoid(sensor, 15);
           if(avoided == 0){
               turn_left(sensor);
           }
       }
       else{
           lcd_printf("we are stuck");
       }


   }

}



// function to have the cybot move forward
void move_forward(oi_t *sensor, int centimeters){
    int sum = 0;

    oi_update(sensor);
    //set wheels to move forward
    oi_setWheels(rightWheelSpeed,leftWheelSpeed);

    //while loop while it reaches past given centimeters
    while(sum < centimeters*10){

        oi_update(sensor);
        sum += sensor->distance;

        if (sensor->bumpLeft) { // Responsive if the left bumper is being pressed
            bumper_backward(sensor, sum);
            s = leftBump;
            break;

        }
        else if(sensor->bumpRight){ // Responsive if the right bumper is being pressed
            bumper_backward(sensor, sum);
            s = rightBump;
            break;
        }
        else if(sensor->cliffFrontRight || sensor->cliffFrontLeft || (sensor->cliffFrontRightSignal > 2700) || (sensor->cliffFrontLeftSignal > 2700)){
            bumper_backward(sensor, sum);
            s = frontEdge;
            break;
        }
        else if(sensor->cliffLeft || (sensor->cliffLeftSignal > 2700)){
            bumper_backward(sensor, sum);
            s = leftEdge;
            break;
        }
        else if(sensor->cliffRight || (sensor->cliffRightSignal > 2700)){
            bumper_backward(sensor, sum);
            s = rightEdge;
            break;
        }


 }

    // stop Cybot
    oi_setWheels(0,0);


    switch(dir){

    case Forward:
        y += centimeters;
        break;
    case Backward:
        y -= centimeters;
        break;
    case Left:
        x -= centimeters;
        break;
    case Right:
        x += centimeters;
        break;
    default:
        break;
    }


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


//Function to have the Cybot move backward
void move_backward(oi_t *sensor, int centimeters){
    int sum = 0;
    oi_update(sensor);
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
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

void turn_left(oi_t *sensor){
    turn(sensor, 90);
}

void turn_right(oi_t *sensor){
    turn(sensor, -90);
}

void spin(oi_t *sensor){

    int angle=0;

    oi_update(sensor);
    oi_setWheels(rightWheelSpeed, -leftWheelSpeed);
    while(angle<360){

        oi_update(sensor);
        angle+= sensor->angle;

    }
    oi_setWheels(0,0);
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







void sweep_servo(){
    //char str[50];
    //sprintf(str, "Deg\tIR Distance(cm)\tSonarDistance(cm)\n\r");
    int i;
    //sweep the servo across 180 degrees, 2 degrees at a time
    for(i = 0; i < 91; i += 1){
        //move by two degrees
        move_servo(i*2);
        //give the servo time to move
        timer_waitMillis(20);
        //read ir data
        read_ir();
        //save data into the next section of the array
        sweepData[0][i] = get_dis_ir();
        sweepData[1][i] = read_ping();
        //sprintf(str, "%d\t%.1f\t\t%.1f\n\r", i*2, sweepData[0][i], sweepData[1][i] );
        //uart_sendStr(str);
    }
}



void read_turret(){
    //record data
        clearObjects();
        sweep_servo();
    //variables for iterating through data to find blocks and info about blocks
        objectCount = 0;
        int i;
        int looking = 0;
        int currentWidth = 0;
        float minDist = 100;

        for(i = 0; i < 6; i++){
            //if the center is -1 then the object is NULL
            finds[i].center = -1;
        }
        //start iterating through data gathered
        for(i = 0; i < 91; i++)
        {
            //if it finds an object, set the boolean variable indicating an object found to 1
            if(sweepData[0][i] < 100 && sweepData[1][i] < 50 && looking == 0)
            {
                looking = 1;
            }
            //if the end of an object is found
            if(((sweepData[0][i] >= 100 || sweepData[1][i] >= 50) && looking == 1) || (looking == 1 && i == 90))
            {
                //set boolean variable indicating an object found to 0
                looking = 0;
                //record distance
                finds[objectCount].distance = minDist;
                //compute center of object
                finds[objectCount].center = i*2-currentWidth;
                //compute width of object using arc length formula
                finds[objectCount].width = currentWidth * (6.28 / 180) * minDist;
                //iterate to the next block in the block array
                objectCount++;
                //reset helper variables
                currentWidth = 0;
                minDist = 100;
            }
            //if currently looking at a block
            if(looking == 1)
            {
                //increase the width helper variable
                currentWidth++;
                //make sure the minimum distance of the object is still the minimum distance
                if(sweepData[1][i] < minDist)
                {
                    minDist = sweepData[1][i];
                }
            }
            //if the block array is full
            if(objectCount == 6)
            {
                break;
            }
        }
}

/**
 * send a sound pulse that will then be read by the read_ping() function
 */
void sendPulse()
{
    //disable alternate function
    GPIO_PORTB_AFSEL_R &= ~0b00001000;
    //set as output
    GPIO_PORTB_DIR_R |= 0b00001000;
    //set pin high
    GPIO_PORTB_DATA_R |= 0b00001000;
    //wait 5 us
    timer_waitMicros(5);
    //set pin low
    GPIO_PORTB_DATA_R &= 0b11110111;
    //set as input
    GPIO_PORTB_DIR_R &= 0b11110111;
    //re-enable alternate function
    GPIO_PORTB_AFSEL_R |= 0b00001000;
}

/**
 * read for a sound pulse sent by the sendPulse() function
 */
float read_ping(){
    //send a pulse out and set GPIO in
    unsigned int startTime = 0;
    unsigned int endTime = 0;
    signed int delta = 0;
    sendPulse();
    timer_waitMicros(115);
    TIMER3_ICR_R = 0xFFFFFFFF;
    //wait for an event from the timer
    while ((TIMER3_RIS_R & TIMER_RIS_CBERIS) == 0)
    {}
    //record the start time of delta
    startTime = TIMER3_TBPS_R & 0x000000FF;
    startTime = startTime << 16;
    startTime += (TIMER3_TBR_R & 0x0000FFFF);
    //clear the interrupt indicating start of delta
    TIMER3_ICR_R |= 0x00000400;
    //wait for an event from the timer
    while ((TIMER3_RIS_R & TIMER_RIS_CBERIS) == 0)
    {}
    //record the end time of delta
    endTime = TIMER3_TBPS_R & 0x000000FF;
    endTime = endTime << 16;
    endTime += (TIMER3_TBR_R & 0x0000FFFF);
    //clear the interrupt indicating end of delta and overflow of the counter
    TIMER3_ICR_R |= 0x00000400;
    //change the delta calculation depending on if there is overflow
    delta = startTime - endTime;
    if(delta < 0)
    {
        delta = startTime + (0x0000FFFF - endTime);
    }
    return 0.0010625 * delta;
}

//value of adc for ir sensor
int adc = 1;
//distance from ir sensor
float cm = 0;

/**
 * update the adc register with new value of ir sensor
 */
void read_ir(){
    ADC0_PSSI_R=ADC_PSSI_SS0;                   // initialize sampling
    while((ADC0_RIS_R & ADC_RIS_INR0) == 0){    //wait for a sample to finish
        }
    adc = ADC0_SSFIFO0_R & 0x00000FFF;          //take sample data into main function
    ADC0_ISC_R = 0x0000001;                     //clear the interrupt
    timer_waitMillis(50);                       //wait 0.02s
    cm = coe * pow((float) adc, power);     //i hate powers 60188, 1.081
}

/**
 * return the raw adc value of the ir sensor
 */
int get_ir_raw(){
    return adc;
}

/**
 * return the distance from the ir sensor
 */
float get_dis_ir(){
    return cm;
}

/**
 * move the turret servo by deg (degrees)
 */
void move_servo(float deg)
{
    //calculate the position
    float posChangeFloat = 320000 - (zeroPos + (tpd * deg));
    //cast to int so it can be written to registers
    int posChange = (int) posChangeFloat;
    //write to match registers
    TIMER1_TBMATCHR_R = posChange;
    TIMER1_TBPMR_R = posChange >> 16;
}


void sendSweepData(){
    char str[50];
    int i;

    sprintf(str, "Deg\tIR Distance(cm)\tSonarDistance(cm)\n\r");
    uart_sendStr(str);

    for(i = 0; i < 91; i += 1){
        move_servo(i*2);
        timer_waitMillis(20);
        read_ir();
        sweepData[0][i] = get_dis_ir();
        sweepData[1][i] = read_ping();
        sprintf(str, "%d\t%.1f\t\t%.1f\n\r", i*2, sweepData[0][i], sweepData[1][i] );
        uart_sendStr(str);
    }
}


int checkClearPath(float yMax){
    float X = 0;
    float Y = 0;
    int i;
    for(i = 0; i < objectCount; i++){
        X = finds[i].distance * cos(finds[i].center);
        Y = finds[i].distance * sin(finds[i].center);
        //for debugging
        lcd_printf("%.2f, %.2f", X, Y);
        timer_waitMillis(1000);
        if(finds[i].center > 0){
            if(abs(X) < 15){
                if(Y < yMax && Y > 0){

                    return 1;
                }
            }
        }
    }
    return 0;
}

int turretAvoid(oi_t *sensor, int cm){
    int result= 0;
    int notClear;
    read_turret();
    notClear = checkClearPath(cm + 5);
    while(notClear){
        result = 1;
        s = objectCenter;
        turn_right(sensor);
        read_turret();
        notClear = checkClearPath(cm + 5);
        lcd_printf("not clear");

    }
    lcd_printf("clear");
    move_forward(sensor, cm);
    if(s == objectCenter){
        turn_left(sensor);
    }
    s = forward;
    return result;
}

void clearObjects(){
    int i;
    for(i = 0; i < 6; i++){
        finds[i].distance = 100;
        finds[i].center = 45;
        finds[i].width = 100;

    }
}
