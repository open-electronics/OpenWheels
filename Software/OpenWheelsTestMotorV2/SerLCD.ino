/*
  OpenWheels serLCD
  use SparkFun Serial LCD on hardware Serial1
 */


const int LCDdelay=2;  // conservative, 2 actually works
  
// wbp: goto with row & column
void LCDPosition(int row, int col) {
  Serial1.write(0xFE);   //command flag
  Serial1.write((col + row*64 + 128));    //position 
  delay(LCDdelay);
}

void clearLCD(){
  Serial1.write(0xFE);   //command flag
  Serial1.write(0x01);   //clear command.
  delay(LCDdelay);
}

void backlightOn() { //turns on the backlight
  Serial1.write(0x7C);   //command flag for backlight stuff
  Serial1.write(157);    //light level.
  delay(LCDdelay);
}

void backlightOff(){ //turns off the backlight
  Serial1.write(0x7C);   //command flag for backlight stuff
  Serial1.write(128);    //light level for off.
  delay(LCDdelay);
}

void serCommand(){   //a general function to call the command flag for issuing all other commands   
  Serial1.write(0xFE);
}

void splashSet(){    //set the splash display text to memory
  Serial1.write(0x7C);   //command flag
  Serial1.write(10);     //this is the <control> j char or line feed
}

void serLCDInit() {  

  backlightOff();
  clearLCD();
  LCDPosition(0,0);
  Serial1.write("OpenWheels V2.0");
  delay(2000);  
  //  splashSet();
}  
