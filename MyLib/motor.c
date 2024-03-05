#include "motor.h"
extern int speed_left, speed_right;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
//extern uint8_t LSensor;
//extern uint8_t RSensor;
int pwd_left,pwd_right;

void MoveForward()
 {
	 pwd_left = 311;
	 pwd_right = 301;


	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left); // left ; 715 -> pwd_left
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right);   // right ; 900 -> pwd_right
 }


 void MoveBackwardSlow(){
	 pwd_left = 312;
	 pwd_right = 301;
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left); 
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right);   
}
 
void MoveBackward(){
	 pwd_left = 312;
	 pwd_right = 301;
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left); 
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right);   
}
 
void Turn180()
 {
	 pwd_left = 312;
	 pwd_right = 301;
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left); // 700-> pwd_left
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right); // 700 -> pwd_right
	 HAL_Delay(740);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
 }

void TurnLeft()
 {
	 pwd_left = 312;
	 pwd_right = 301;
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right);
	 HAL_Delay(355);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_SET);		 
 }

void TurnRight()
 {
	 pwd_left = 312;
	 pwd_right = 301;
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	 __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right);
	 HAL_Delay(355);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
 }
 
void Stop()
 {
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	 HAL_GPIO_WritePin (GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
 }
 
 void CheckForward(uint8_t LSensor,uint8_t RSensor)
{
	if(LSensor < 25)
		{
			__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left+20);
			}
		
				if(RSensor < 25)
		{
			__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right+20);
			}
		
		
//	if (LSensor > RSensor )
//	{
//		uint16_t dif;
//		if(LSensor < 8)
//			dif = (LSensor - RSensor) / 2;
//		else 
//			dif = (RSensor - 45) / 2;
//		dif = 5;
//		
//		if (pwd_left >= 999 || pwd_right >= 999)
//		{	
//			__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left-dif);
//		}	
//		else 
//		{
//			__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right+dif);			
//		}			
//	}
//	else if (LSensor < RSensor)
//	{
//		uint16_t dif;
//		if(RSensor < 8)
//			dif = (RSensor - LSensor) / 2;
//		else 
//			dif = (LSensor - 45) / 2;
//		dif = 5;
//		if (pwd_left >= 999 || pwd_right >= 999)
//		{	
//			__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,pwd_right-dif);
//		}
//		else 
//		{
//			__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,pwd_left+dif);			
//		}	
//	}
//	else 
//	{
//		;
//	}
}