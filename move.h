#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inc/tm4c123gh6pm.h>
#include <math.h>
#include <open_interface.h>

typedef enum{
    Forward,
    Backward,
    Left,
    Right
} direction;

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

void move_forward(oi_t *sensor, int centimeters, int *x, int *y, state *s, direction *dir);
void turn(oi_t *sensor, int degrees, direction *dir);
void move_backward(oi_t *sensor, int centimeters, int *y, int *x, direction *dir);
void turn_left(oi_t *sensor, direction *dir);
void turn_right(oi_t *sensor, direction *dir);
void spin(oi_t *sensor);
void bumper_backward(oi_t *sensor, int millimeters, int *x, int *y, direction *dir);
