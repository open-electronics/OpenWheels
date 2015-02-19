/*
  OpenWheels Test Motor Program
  by Mirco Segatello 
  
  Hardware OpenWheels 1.1 Ottobre 2012  
  board: compatibility to ARDUINO Duemilanove
  compiler: Arduino 1.0.2  
  Copyright: Creative Commons  BY-NC-SA
 */

#include <SoftwareSerial.h>

//ADC Inputs
#define Acc_ADC_pin    0
#define Gyro_ADC_pin   1
#define Steer_ADC_pin  2
#define VBat_ADC_pin   3
#define IBat_ADC_pin   4

//Driver pin
#define ShutDown_pin   3   // ShutDown for motor driver (LOW=SD)
#define PWMLeft_pin    9   // PWM for motor Left
#define PWMRight_pin  10   // PWM for motor Right


//Digital pin
#define SerLCD_TX      4   // software serial TX for Serial LCD
#define P1_pin         5   // P1 pushbutton + function
#define P2_pin         6   // P2 pushbutton - function
#define P3_pin         7   // P3 pushbutton Enter function
#define PStart_pin     8   // Start Contact

//Battery variables
float VBatLevel;
float VBatScale=0.036;
float IBatLevel;
float IBatScale=0.0538;

//motor variables
int dutyCycleLeft;
int dutyCycleRight;
int stallMotor;            // value of PWM for motor stall
float drive=0;
int drivePWM;
int maxPWM;
boolean status;

// Sensor
int Acc_RAW;
int Gyro_RAW;
int Steer_RAW;

// LCD TX pin (RX not necessary)
SoftwareSerial LCD = SoftwareSerial(0, SerLCD_TX);

void setup() {
  Serial.begin(115200);
  analogReference(EXTERNAL);

  pinMode(P1_pin, INPUT_PULLUP);
  pinMode(P2_pin, INPUT_PULLUP);
  pinMode(P3_pin, INPUT_PULLUP);
  pinMode(PStart_pin, INPUT_PULLUP);
  
  Serial.println("OpenWheels Test Motor\n");
  Serial.println("Init motor...");  
  initMotor();
  delay(500); 
  Serial.println("Init LCD...\n");   
  serLCDInit();
  delay(500);   
}

void loop() {

  status=digitalRead(PStart_pin);
  digitalWrite(ShutDown_pin, !status);
  
  if (digitalRead(P1_pin)==0) {
      if (drive>-stallMotor) drive-=1.0;
      SetMotor();
  }
  if (digitalRead(P2_pin)==0) {
      if (drive<stallMotor) drive+=1.0;
      SetMotor();
  }    
  
  if (digitalRead(P3_pin)==0) {
      drive=0.0;
      SetMotor();
  }   
  Serial.print("drive=");  // ECO on SerialMonitor
  Serial.println(int(drive));
  clearLCD();  
  lcdPosition(0,0);      
  LCD.print("PWM=");
  LCD.print(int(drive));
  lcdPosition(1,0);  
  if (status==0)
    LCD.print("RUN");  
  else
    LCD.print("STOP");  
  
  delay(100);  
}






