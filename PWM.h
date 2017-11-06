/*
**********************************************************************************************
 @file    PWM.h
  * @author  COESIP Team
  * 			1) Dr. Prashant Bartakke
  * 			2) Gaurav Chaudhari
  * 			3) Shailesh Kharche
  * 			4) Girish Desai
  * 			5) Amar Nerune
  * 			6) Kaustubh Deshpande
  * @contact: desaigg16.extc@coep.ac.in
  * @date    24-October-2017
  * @brief: This file contains function prototypes required to generate PWM, capture PWM and measure dutycycle
  * @Note:	All the calculation are valid for 25 MHz system clock
**********************************************************************************************
*/

/*includes*/
#include<stm32f4xx.h>
#include<stm32f4xx_gpio.h>
#include<stm32f4xx_rcc.h>
#include<stm32f4xx_tim.h>
#include<stm32f4xx_exti.h>

/* PWM Duty cycle enumeration*/
typedef enum
{
	PWM_DutyCycle_100 = 100,
	PWM_DutyCycle_75 = 75,
	PWM_DutyCycle_50=50,
	PWM_DutyCycle_25=25,
	PWM_DutyCycle_5=5,
	PWM_DutyCycle_0=0
}PWM_DutyCycle;

/* PWM frequency Enumeration*/
typedef enum
{
	PWM_frequency_1KHz=500,
	PWM_frequency_2KHz=250,
	PWM_frequency_4KHz=125,
	PWM_frequency_8KHz=63
}PWM_frequency;


/*prototypes for PWM configuration*/
void PWM_GenerationInit(PWM_frequency frequency, PWM_DutyCycle Dutycycle);
void PWM_CaptureInit(void);

/* Prototype for measuring duty cycle*/
float PWM_GetDutyCycle(void);

