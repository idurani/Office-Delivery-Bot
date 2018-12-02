#include <move.h>

int leftWheelSpeed = 250;
int rightWheelSpeed = 250;

// function to have the cybot move forward
void move_forward(oi_t *sensor, int centimeters, int *x, int *y, state *s, direction *dir){
    int sum = 0;
     oi_update(sensor);
    //set wheels to move forward
    oi_setWheels(rightWheelSpeed,leftWheelSpeed);
     //while loop while it reaches past given centimeters
    while(sum < centimeters*10){
         oi_update(sensor);
        sum += sensor->distance;
         if (sensor->bumpLeft) { // Responsive if the left bumper is being pressed
            bumper_backward(sensor, sum, *x, *y, *dir);
            *s = leftBump;
            break;
         }
        else if(sensor->bumpRight){ // Responsive if the right bumper is being pressed
            bumper_backward(sensor, sum, *x, *y, *dir);
            *s = rightBump;
            break;
        }
        else if(sensor->cliffFrontRight || sensor->cliffFrontLeft || (sensor->cliffFrontRightSignal > 2700) || (sensor->cliffFrontLeftSignal > 2700)){
            bumper_backward(sensor, sum, *x, *y, *dir);
            *s = frontEdge;
            break;
        }
        else if(sensor->cliffLeft || (sensor->cliffLeftSignal > 2700)){
            bumper_backward(sensor, sum, *x, *y, *dir);
            *s = leftEdge;
            break;
        }
        else if(sensor->cliffRight || (sensor->cliffRightSignal > 2700)){
            bumper_backward(sensor, sum, *x, *y, *dir);
            *s = rightEdge;
            break;
        }
  }
     // stop Cybot
    oi_setWheels(0,0);
     switch(*dir){
     case Forward:
        *y += centimeters;
        break;
    case Backward:
        *y -= centimeters;
        break;
    case Left:
        *x -= centimeters;
        break;
    case Right:
        *x += centimeters;
        break;
    default:
        break;
    }
 }

void turn(oi_t *sensor, int degrees, direction *dir){
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
     switch(*dir){
     case Forward:
        if(degrees==90){
            *dir=Left;
        }
        else if(degrees == -90){
            *dir = Right;
        }
        break;
    case Backward:
        if(degrees==90){
            *dir=Right;
        }
        else if(degrees == -90){
            *dir = Left;
        }
        break;
    case Left:
        if(degrees==90){
            *dir=Backward;
        }
        else if(degrees == -90){
            *dir = Forward;
        }
        break;
    case Right:
        if(degrees==90){
            *dir=Forward;
        }
        else if(degrees == -90){
            *dir = Backward;
        }
        break;
    default:
        break;
    }
}

//Function to have the Cybot move backward
void move_backward(oi_t *sensor, int centimeters, int *y, int *x, direction *dir){
    int sum = 0;
    oi_update(sensor);
    oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
    while(sum < centimeters*10){
         oi_update(sensor);
        sum += abs( sensor->distance);
     }
    oi_setWheels(0,0);
     switch(*dir){
     case Forward:
        *y -= centimeters;
        break;
    case Backward:
        *y += centimeters;
        break;
    case Left:
        *x += centimeters;
        break;
    case Right:
        *x -= centimeters;
        break;
    default:
        break;
    }
}
void turn_left(oi_t *sensor, direction *dir){
    turn(sensor, 90, *dir);
}
void turn_right(oi_t *sensor, direction *dir){
    turn(sensor, -90, *dir);
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

void bumper_backward(oi_t *sensor, int millimeters, int *x, int *y, direction *dir){
     int sum = 0;
     oi_update(sensor);
     oi_setWheels(-rightWheelSpeed,-leftWheelSpeed);
     while(sum < millimeters){
          oi_update(sensor);
         sum += abs( sensor->distance);
      }
     oi_setWheels(0,0);
      switch(*dir){
      case Forward:
         *y -= sum;
         break;
     case Backward:
         *y += sum;
         break;
     case Left:
         *x += sum;
         break;
     case Right:
         *x -= sum;
         break;
     default:
         break;
     }
 }
