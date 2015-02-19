/*
  OpenWheels Test Sensor Program
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

// Sensor
int Acc_RAW;
int Gyro_RAW;
int Steer_RAW;
int IBat_Zero;

// LCD TX pin (RX not necessary)
SoftwareSerial LCD = SoftwareSerial(0, SerLCD_TX);

void setup() {
  Serial.begin(115200);
  analogReference(EXTERNAL);
  pinMode(ShutDown_pin, OUTPUT);   
  digitalWrite(ShutDown_pin, LOW);   //ShutDown driver at startUP 
  pinMode(P1_pin, INPUT_PULLUP);
  pinMode(P2_pin, INPUT_PULLUP);
  pinMode(P3_pin, INPUT_PULLUP);
  pinMode(PStart_pin, INPUT_PULLUP);
  
  Serial.println("OpenWheels Test Sensor\n");
  delay(500);
  Serial.println("Init IBat offset...\n");   
  initIBat();
  delay(500);
  Serial.println("Init LCD...\n");   
  serLCDInit();
  delay(500);     
}

void loop() {

  lcdPosition(1,0);  
  Steer_RAW = analogRead(Steer_ADC_pin);
  Serial.print("Steer=");  // ECO on SerialMonitor
  Serial.print(Steer_RAW);  
  LCD.print("Str=");
  if (Steer_RAW<10)   LCD.print(" ");
  if (Steer_RAW<100)  LCD.print(" "); 
  if (Steer_RAW<1000) 
    LCD.print(Steer_RAW);
  else
    LCD.print("---");
 

  lcdPosition(1,8);   
  VBatLevel = float(analogRead(VBat_ADC_pin))*VBatScale;  
  Serial.print("\tV Bat=");  // ECO on SerialMonitor
  Serial.print(VBatLevel);  
  LCD.print("Bat=");
  LCD.print(VBatLevel);
  
  IBatLevel = float(analogRead(IBat_ADC_pin)-IBat_Zero)*IBatScale;    
  Serial.print("\tI Bat=");  // ECO on SerialMonitor
  Serial.print(IBatLevel);  // not OffSet calculated
  
  lcdPosition(0,0);  
  Acc_RAW = analogRead(Acc_ADC_pin);
  Serial.print("\tAcc=");  // ECO on SerialMonitor
  Serial.print(Acc_RAW);  
  LCD.print("Acc=");
  if (Acc_RAW<10)   LCD.print(" ");
  if (Acc_RAW<100)  LCD.print(" "); 
  if (Acc_RAW<1000) 
    LCD.print(Acc_RAW);
  else
    LCD.print("---");
  LCD.print(" ");

  
  lcdPosition(0,8);  
  Gyro_RAW = analogRead(Gyro_ADC_pin);
  Serial.print("\tGyro=");  // ECO on SerialMonitor
  Serial.println(Gyro_RAW);  
  LCD.print("Gyr=");
  if (Gyro_RAW<10)   LCD.print(" ");
  if (Gyro_RAW<100)  LCD.print(" "); 
  if (Gyro_RAW<1000) 
    LCD.print(Gyro_RAW);
  else
    LCD.print("---");


  Serial.println();
  
  if (digitalRead(P1_pin)==0) {
      Serial.println("P1 pressed");  // ECO on SerialMonitor
      clearLCD();  
      lcdPosition(0,0);      
      LCD.print("P1 pressed");
  }
  if (digitalRead(P2_pin)==0) {
      Serial.println("P2 pressed");  // ECO on SerialMonitor
      clearLCD();  
      lcdPosition(0,0);      
      LCD.print("P2 pressed");
  }  
  if (digitalRead(P3_pin)==0) {
      Serial.println("P3 pressed");  // ECO on SerialMonitor
      clearLCD();  
      lcdPosition(0,0);      
      LCD.print("P3 pressed");
  }  
  if (digitalRead(PStart_pin)==0) {
      Serial.println("PStart pressed");  // ECO on SerialMonitor
      clearLCD();  
      lcdPosition(0,0);      
      LCD.print("PStart pressed");
  }    
  
  delay(500);  
}


void initIBat() {
    
   int i;
   IBat_Zero=0;
   for (i=0; i<50; i++)
   {                                    
      IBat_Zero += analogRead(IBat_ADC_pin);
      delay(20);
   }
   IBat_Zero /= i; 
}


