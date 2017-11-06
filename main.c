/*
**********************************************************************************************
 @file    main.c
 	@title: PWM Generation & Duty cycle measurement
 	@version: 1.0
  * @author  COESIP Team
  * 			1) Dr. Prashant Bartakke
  * 			2) Gaurav Chaudhari
  * 			3) Shailesh Kharche
  * 			4) Girish Desai
  * 			5) Amar Nerune
  * 			6) Kaustubh Deshpande
  * @contact: desaigg16.extc@coep.ac.in
  * @date    24-October-2017
  * @brief: This file contains source code to generate PWM signal and measure its Dutycycle.
  * 		Two buttons are configured to increment and decrement the duty cycle at runtime.
  * 		The  measured duty cycle is displayed on the LCD.
  * 		The system clock frequency is configured to 25 MHz.
  * 		By default the generation frequency & Duty Cycle is 1 KHz and 50% respectively
  * @Note:	All the calculation are valid for 25 MHz system clock
  * 		This code is designed for STM32F4xx Micro-controller
**********************************************************************************************
*/

//includes
#include"PWM.h"
#include"lcdDisplay.h"

//function prototypes
void convertFloatToString(float, char*);
void DutycycleVaryingButton_Init(void);
void display(char*);

//main starts here
int main(void)
{
	float dutycycle;
	char displayString[5];
	int changeDutyCycle=50;

	//Initilize LCD
	LCDInit();

	//PWM generation initialization, default at 50% duty cycle
	PWM_GenerationInit(PWM_frequency_1KHz,PWM_DutyCycle_50);

	/*Initialize input capture channel to capture PWM wave*/
	PWM_CaptureInit();

	//Initialize button to change dutycycle
	changeDutycycleButton_Init();

	while(1){
		/*Measure dutycycle*/
		dutycycle=PWM_GetDutyCycle();

		/*Decimal to String*/
		convertFloatToString(dutycycle,displayString);

		//Display result on LCD
		display(displayString);

		//Check button to increase DutyCycle
		if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)){
			changeDutyCycle++;
			PWM_GenerationInit(PWM_frequency_1KHz,changeDutyCycle%99);
		}
		//Check button to decrease DutyCycle
		else if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)){
			changeDutyCycle--;
			//To avoid Dutycycle to fall below 0%
			if(changeDutyCycle==0)
				changeDutyCycle==1;
			PWM_GenerationInit(PWM_frequency_1KHz,changeDutyCycle%99);
		}

	}

	return 0;
}


/*@brief: Initializes GPIO Pin to sense input from button
 * 		  PC2 - DutyCycle Increment button
 * 		  PC4 - DUtyCycle Decrement button
 *retval: None
 */
void changeDutycycleButton_Init(void)
{
	/*Clock for GPIOD */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/*Configure GPIO pins in AF mode */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*brief: converts float to string
 *parameters1: dutycycle: Dutycycle to be displayed
 *parameters2: displayString: pointer to character array where the string to be
 *			   				  displayed will be stored
 *retval: None
 */
void convertFloatToString(float dutycycle, char* displayString)
{
	int dutycycleIntPart, dutycycleFracPart;

	//Copying integer part in to variable
	dutycycleIntPart=(u32)dutycycle;
	//Extracting fractional part
	dutycycleFracPart=(u32)((dutycycle-dutycycleIntPart)*100.0);

	/*Convert decimal to String*/
	sprintf(displayString,"%02d.%02d",dutycycleIntPart,dutycycleFracPart);
}

/*brief: Displays string on LCD
 *parameter: displayString: pointer to the string to be displayed
 *retval: None
 */
void display(char* displayStringPtr)
{
	lcdClearScreen();
	lcdSendString("Duty Cycle:");
	lcdSendString(displayStringPtr);
	delay(100000);
}
