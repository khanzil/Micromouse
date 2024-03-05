#ifndef MOTOR_H
#define MOTOR_H


#include "main.h"
void MoveForward(void);
void MoveBackwardSlow(void);
void MoveBackward(void);
void Turn180(void);
void TurnLeft(void);
void TurnRight(void);
void Stop(void);
void CheckForward(uint8_t LSensor,uint8_t RSensor);
#endif
