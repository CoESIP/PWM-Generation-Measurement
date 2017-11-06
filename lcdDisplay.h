#ifndef LCD_DEFINES
#define LCD_DEFINES


#endif //LCD_DEFINES

void GPIOInit() ;
void LCDInit() ;
void lcdSendCmd(char var) ;
void lcdSendData(char var) ;
void lcdSendString(char *var) ;
void lcdCursorGoto(char address);
int lcdClearScreen() ;
