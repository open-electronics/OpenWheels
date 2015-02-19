/*
  OpenWheels serLCD
 */


const int LCDdelay=2;  // conservative, 2 actually works
  
// wbp: goto with row & column
void lcdPosition(int row, int col) {
  LCD.write(0xFE);   //command flag
  LCD.write((col + row*64 + 128));    //position 
  delay(LCDdelay);
}

void clearLCD(){
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  delay(LCDdelay);
}

void serCommand(){   //a general function to call the command flag for issuing all other commands   
  LCD.write(0xFE);
}

void backlightOn() { //turns on the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(157);    //light level.
  delay(LCDdelay);
}

void backlightOff(){ //turns off the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);    //light level for off.
   delay(LCDdelay);
}
/*
void splashSet(){   //set the splash display text to memory
  LCD.write(0x7C);   //command flag
  LCD.write(10);     //this is the <control> j char or line feed
}
*/
void serLCDInit() {  
  LCD.begin(9600);
  backlightOff();
  clearLCD();
  lcdPosition(0,0);
  LCD.print("OpenWheels v1.3");
  delay(1000);  
}  
