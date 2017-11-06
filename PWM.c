/*
**********************************************************************************************
 @file    PWM.c
  * @author  COESIP Team
  * 			1) Dr. Prashant Bartakke
  * 			2) Gaurav Chaudhari
  * 			3) Shailesh Kharche
  * 			4) Girish Desai
  * 			5) Amar Nerune
  * 			6) Kaustubh Deshpande
  * @contact: desaigg16.extc@coep.ac.in
  * @date    24-October-2017
  * @Note:	All the calculation are valid for 25 MHz system clock
**********************************************************************************************
*/

//includes
#include"PWM.h"

/*prototypes for timer initialization*/
void PWM_captureTimerInit(void);
void PWM_generationTimerInit(PWM_frequency);

/*prototypes for alternate function configuration*/
void PWM_generationPinConfig(void);
void PWM_CapturePinConfig(void);


/* @brief: Configures timer4 with 1 MHz clock for counter
 * @Note: Timer 4 is used to capture PWM wave with time period upto 65ms(approx 16 Hz equivalent)
   @retval: None
*/
void PWM_captureTimerInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_TimeBaseInitTypeDef timer4;

	timer4.TIM_Prescaler= 25;					//Scaling down system clock from 25 MHz to 1 MHz
	timer4.TIM_ClockDivision=TIM_CKD_DIV1;
	timer4.TIM_CounterMode=TIM_CounterMode_Up;
	timer4.TIM_Period=0xFFFF;					//for 65ms period max
	timer4.TIM_RepetitionCounter=0;

	TIM_TimeBaseInit(TIM4,&timer4);
	TIM_InternalClockConfig(TIM4);
}


/* @brief: Configures timer2
   @Note: Timer 2 is used for generating PWM wave
   @parameters: period: PWM_frequency_xKHz where x can be 1,2,4,8
   @retval: None
 */
void PWM_generationTimerInit(PWM_frequency period)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBaseInitTypeDef timer2;

	timer2.TIM_Prescaler=24;					//Scaling down system clock from 25 MHz to 1 MHz
	timer2.TIM_ClockDivision=TIM_CKD_DIV1;
	timer2.TIM_CounterMode=TIM_CounterMode_Up;
	timer2.TIM_Period=period;
	timer2.TIM_RepetitionCounter=0;

	TIM_TimeBaseInit(TIM2,&timer2);
	TIM_InternalClockConfig(TIM2);
}


/* @brief: Configure AF pins for timer2 channel1
 * @Note: Pin 0 of Port A is used for channel1 of timer2
 * @retval: None
 */
void PWM_generationPinConfig(void)
{
	/* Clock for GPIOD */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Set parameters for configuration */
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Alternating functions for pins */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
}


/* @brief: Configure AF pins for timer4 channel1
 * @Note: Pin 6 of Port B is used for channel1 of timer4
 * @retval: None
 */
void PWM_CapturePinConfig(void)
{
	/*Clock for GPIOD */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/*Configure GPIO pins in AF mode */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Choose alternate function for the configured pin*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
}


/*
 * @brief: Configure timer2 to generate
 * @Parameter: period : PWM_frequency_xKHz where x can be 1,2,4,8
 * 			   dutycucle: Dutycycle_x where x can be 0,25,50,75,100
 * @retval: None
 * */
void PWM_GenerationInit(PWM_frequency period, PWM_DutyCycle dutycycle )
{
	//Initialize timer2
	PWM_generationTimerInit(period);

	//Configuring AF pins for Channel1 of timer 1
	PWM_generationPinConfig();

	//Configuring Output channel1 in PWM mode
	TIM_OCInitTypeDef GeneratePWM;
	GeneratePWM.TIM_OCMode=TIM_OCMode_PWM1;			//output = 1 if TIM_CNT < TIM_CCRx
	GeneratePWM.TIM_OutputState=TIM_OutputState_Enable;
	GeneratePWM.TIM_OCPolarity=TIM_OCPolarity_High; //Active high output w.r.t OCRef
	GeneratePWM.TIM_Pulse=(u16)(period*dutycycle)/100.0;
	TIM_OC1Init(TIM2,&GeneratePWM);
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);

	//enable timer
	TIM_Cmd(TIM2,ENABLE);
}


/*
 * @brief: Configure timer4 to capture PWM wave
 * @Note: Captures PWM in the range of 16 Hz to 99KHz
 * @retval: None
 * */
void PWM_CaptureInit(void)
{
	//Initialize timer4 with period of
	PWM_captureTimerInit();

	//Configuring AF pins for channel1 of timer4
	PWM_CapturePinConfig();

	//Configure Input channel in PWM input mode
	TIM_ICInitTypeDef PWMInputConf;
	PWMInputConf.TIM_Channel=TIM_Channel_1;
	PWMInputConf.TIM_ICFilter=0x3;
	PWMInputConf.TIM_ICPolarity=TIM_ICPolarity_Rising;			//CH1 configured to capture PWM input
	PWMInputConf.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	PWMInputConf.TIM_ICSelection=TIM_ICSelection_DirectTI;

	//Configure channel1 and channel2 for PWM input mode
	TIM_PWMIConfig(TIM4,&PWMInputConf);

	//Configure trigger as filtered output of TI1
	TIM_SelectInputTrigger(TIM4,TIM_TS_TI1FP1);

	//Configure Slave controller in RESET mode
	TIM_SelectSlaveMode(TIM4,TIM_SlaveMode_Reset);

	//Enable channel 1 & 2
	TIM_CCxCmd(TIM2, TIM_Channel_1,TIM_CCx_Enable);
	TIM_CCxCmd(TIM2, TIM_Channel_2,TIM_CCx_Enable);
}


/* @brief: Measures the duty cycle of PWM wave
 * @note: dutycycle = (On_time/Total_period)*100
 * 		: *dutycycle measured will be always less than 100%
 * @retval: returns the dutycycle(float)*/
float PWM_GetDutyCycle()
{
	int total_period, On_period;
	float dutyCycle;

	//clear CC1 flag if it is set initially
	if(TIM_GetFlagStatus(TIM4,TIM_FLAG_CC1)){
		TIM_ClearFlag(TIM4,TIM_FLAG_CC1);
	}

	if(TIM_GetFlagStatus(TIM4,TIM_FLAG_Update)){
		TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	}

	//enable timer
	TIM_Cmd(TIM4,ENABLE);
	while(1){
		//wait for first rising edge so that counter is set to zero
		if(TIM_GetFlagStatus(TIM4,TIM_FLAG_CC1)){
			TIM_ClearFlag(TIM4,TIM_FLAG_CC1);

		//measurement starts here
			do{
				//if next rising edge occurs then extract values of CCR1 and CCR2
				if(TIM_GetFlagStatus(TIM4,TIM_FLAG_CC1)){
					On_period=TIM_GetCapture2(TIM4);
					total_period=TIM_GetCapture1(TIM4);
					break;
				}
			}while(1);
		break;
		}
	}

	//computing duty cycle
	dutyCycle=(On_period*100.0)/total_period;

	//disable timer4
	TIM_Cmd(TIM4,DISABLE);
	return dutyCycle;
}

