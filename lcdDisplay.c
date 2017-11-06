//#include<stm32l1xx_lcd.h>
#include "PWM.h"
#include "lcdDisplay.h"
//#include "businessLogic.h"

void delay (uint32_t microsec) ;
void  LcdBusWrite ( char  value ) ;
const uint32_t   RSIDX = 8;
const uint32_t  RWIDX  = 9;
const uint32_t ENIDX= 10;

typedef   struct lcdGpioMap {
	GPIO_TypeDef  *gpioZone ;
	  uint32_t   gpioPin ;
}lcdGpioMap_t ;
volatile lcdGpioMap_t  lcdGpioPins[11] =  {


				{GPIOE ,GPIO_Pin_9},//D0
				{GPIOE, GPIO_Pin_11},
				{GPIOE, GPIO_Pin_13},
				{GPIOE, GPIO_Pin_15},
				{GPIOD, GPIO_Pin_1},
				{GPIOD, GPIO_Pin_3},
				{GPIOD, GPIO_Pin_5},
				{GPIOD, GPIO_Pin_7}, // D7




#if 0
				{GPIOC ,GPIO_Pin_10},//D0
				{GPIOC, GPIO_Pin_12},
				{GPIOD, GPIO_Pin_1},
				{GPIOD, GPIO_Pin_3},
				{GPIOD, GPIO_Pin_5},
				{GPIOD, GPIO_Pin_7},
				{GPIOB, GPIO_Pin_4},
				{GPIOB, GPIO_Pin_6}, // D7
#endif


		{GPIOC, GPIO_Pin_6},  // Control Pins, RS, RW, EN
		{GPIOC, GPIO_Pin_8},
		{GPIOA, GPIO_Pin_8},

};



/*=============================================================================
  *                         GPIO Init
  *=============================================================================
  * Initializing GPIO pins for LCD data lines and LCD command lines
  * Enable specified peripheral clock .
  *
  */

void LCDInit() {
    GPIO_InitTypeDef  GPIO_InitStruct;
    SystemInit() ;
    SystemCoreClockUpdate();

    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA ,ENABLE);
    RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOB ,ENABLE);
    RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOC ,ENABLE);
    RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOD ,ENABLE);
    RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOE ,ENABLE);
    uint32_t idx = 0 ;
    for (idx=0 ;  idx < 11 ; idx++) {
    	GPIO_InitStruct.GPIO_Pin  = lcdGpioPins[idx].gpioPin;
    	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;;
    	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz  ;
    	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP  ;
    	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP  ;
    	GPIO_Init(lcdGpioPins[idx].gpioZone, &GPIO_InitStruct);
    }

	delay(15000);
	GPIO_WriteBit(lcdGpioPins[RWIDX].gpioZone,lcdGpioPins[RWIDX].gpioPin,Bit_RESET) ;
	delay(5000);

	lcdSendCmd(0x38) ; // function set.
	lcdSendCmd(0x00) ; // display on/off and cursor on/off.
	lcdSendCmd(0x0F) ; // display on/off and cursor on/off.
	lcdSendCmd(0x06) ; // entry mode set.
	lcdSendCmd(0x1F) ; // entry mode set.
	lcdSendCmd(0x1F) ; // entry mode set.
	lcdSendCmd(0x1F) ; // entry mode set.


	lcdSendCmd(0x01) ; // clear display .

  }



void  LcdBusWrite ( char  value ){
	uint16_t idx ;
	for (idx = 0 ; idx < sizeof(char)*8 ; idx++ ) {
		uint32_t pin ;
		int16_t busValue ;
		GPIO_TypeDef* gpioZone= lcdGpioPins[idx].gpioZone ;
		uint16_t gpioPin = lcdGpioPins[idx].gpioPin ;
		BitAction bitValue  = value & 0x1;
		GPIO_WriteBit(gpioZone,gpioPin, bitValue) ;
		value = value >> 1 ;
	}
}
/*=============================================================================
  *                         Lcd send command
  *=============================================================================
  * In this function we have assigning var value to tempVar.
  * left shifting tempVar value to 2 times because we want GPIO port from PB2 to PB9
  * then we are making RS reset and EN set.
  * (RS - register selector, reset - command register, set - data register)
  * Finally we are making RS set and EN reset.
  *
  */
/**
  * @brief  Sending command to LCD for intializing LCD and to make some perticular task
  *
  * @param  var is any char value
  * @retval None
  */

void lcdSendCmd(char var) {
	short tempVar=0;
	delay(150);
	/*
	tempVar=(short)var;
	tempVar=tempVar<<2;
	*/
	GPIO_WriteBit(lcdGpioPins[RSIDX].gpioZone,lcdGpioPins[RSIDX].gpioPin,Bit_RESET) ;
   //GPIO_WriteBit(GPIOA,RS,Bit_RESET) ;
	delay(150);
	LcdBusWrite (var) ;
    delay(150);

   // GPIO_WriteBit(GPIOA,EN,Bit_SET) ;
    GPIO_WriteBit(lcdGpioPins[ENIDX].gpioZone,lcdGpioPins[ENIDX].gpioPin,Bit_SET) ;
    delay(300);
    //checkBusyFlagState();
    GPIO_WriteBit(lcdGpioPins[ENIDX].gpioZone,lcdGpioPins[ENIDX].gpioPin,Bit_RESET) ;
    //GPIO_WriteBit(GPIOA,EN,Bit_RESET) ;
    delay(300);
     GPIO_WriteBit(lcdGpioPins[RSIDX].gpioZone,lcdGpioPins[RSIDX].gpioPin,Bit_SET) ;
   // GPIO_WriteBit(GPIOA,RS,Bit_SET) ;
    delay(300);
}


/*=============================================================================
  *                         Lcd send data
  *=============================================================================
  * In this function we have assigning var value to tempVar.
  * left shifting tempVar value to 2 times because we want GPIO port from PB2 to PB9
  * then we are making RS and EN to set Finally we are setting EN.
  *
  */
/**
  * @brief  Sending data to LCD for displaying on LCD
  *
  * @param  var is any char value
  * @retval None
  */

void lcdSendData(char var) {
	short tempVar=0;
	delay(150);
	GPIO_WriteBit(lcdGpioPins[RSIDX].gpioZone,lcdGpioPins[RSIDX].gpioPin,Bit_SET) ;
	//GPIO_WriteBit(GPIOA,RS,Bit_SET) ;

//    delay(150);
	LcdBusWrite (var) ;
	//GPIO_Write(LCDData,tempVar) ;
     delay(150);

	GPIO_WriteBit(lcdGpioPins[ENIDX].gpioZone,lcdGpioPins[ENIDX].gpioPin,Bit_SET) ;
   // GPIO_WriteBit(GPIOA,EN,Bit_SET) ;
    delay(300);
	GPIO_WriteBit(lcdGpioPins[ENIDX].gpioZone,lcdGpioPins[ENIDX].gpioPin,Bit_RESET) ;
    //GPIO_WriteBit(GPIOA,EN,Bit_RESET) ;
    delay(300);

}

/*=============================================================================
  *                        lcd Send String
  *=============================================================================
  * In this function we are sending a string to the lcdSendData function character by character.
  *
  */
/**
  * @brief  Sending String to LCD for displaying on LCD
  *
  * @param  var is any string
  * @retval None
  */


void lcdSendString(char *var) {
	char sendchar=0;uint8_t idx = 0;
    while(var[idx] != '\0') {
    	sendchar=var[idx];
    	lcdSendData(sendchar) ;
        idx++;
    }
}

/*=============================================================================
  *                        Cursor goto
  *=============================================================================
  * In this function we are moving cursor by sending the command to the lcdSendCmd function.
  */
/**
  * @brief  Sending LCD cursor to specific address
  *
  * @param  address is any hexa decimal value
  * @retval None
  */
void lcdCursorGoto(char address) {
	lcdSendCmd(address | 0x80) ;
}

int lcdClearScreen() {
	lcdSendCmd(0x01);
	delay(15000);
//	lcdSendCmd(0x02);
	return 0;
}

#if 0
Settings of CLK
#define PLL_M      8
#define PLL_N      100

/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      4

25 Mhz system and timer clock will be ennabled.
1 instruction       4 * 10-8  seconds
 25000                10-3 seconds ( 1 miil second)

#endif

void delay (uint32_t microsec)
{
  volatile  uint32_t  tim ;
   for (tim= 0 ; tim < microsec; tim++) {
	  volatile  uint32_t iter=0;
	   for (iter=0 ; iter <=25 ; iter++) ;
   }

}





