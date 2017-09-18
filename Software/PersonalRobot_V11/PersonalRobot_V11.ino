/*
 Personal Robot V1.2
 
 V1.2 form motor EW360CPR
 V1.1 added remote control (PersonalRobotGUI_V10 or PRobot app)
 V1.0 initial version and test
 manage all data with GUI (no pushbutton need)
 Hardware switch for GO function 
 Telemetry is on UART3 (115Kbps)
 by Mirco Segatello 
  
 basato sul lavoro di openwheels 1.0 ma adattato per MPU-6050
 
 board: OpenWheels V2.0 with digitalIMU MPU-6050
 compiler: Arduino 1.6.4  
 date: June 2015
  
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License Creative Commons  BY-NC-SA
 as published by the Free Software Foundation, see <http://creativecommons.org/licenses/by-nc-sa/3.0/>  
 */

#include <EEPROM.h>
#include <Wire.h>
#include "def.h"

//Battery voltage variables
float VBatLevel, VBatLevelold; // Battery voltage [V]
float VBatScale = 0.0324; // Battery scale for Volt converter
int   VBatLper;           // Battery level %
/*
// For 24V battery power
float VBatLmax = 26.0;    // Max voltage battery
float VBatLmin = 19.2;    // Min voltage battery
float VBatAlim = 24.0;    // power voltage
*/
// For 12V battery power (3 cell LiPo)
float VBatLmax = 12.6;    // Max voltage battery
float VBatLmin = 11.5;    // Min voltage battery
float VBatAlim = 11.8;    // power voltage

//Battery current variable
float IBatLevel;          // Battery current [A]
float IBatLevelSmooth;    // Battery current smooth
float IBatLevelSmoothold;  
float IBatScale = 0.0538; // Battery scale for Ampere converter
float IBatMAX = 3.0;     // IBat MAX for shutdown MOTOR
float IBat_Zero;          // IBat Zero (for offset sensor)

//IMU Variables
int Acc_Zero;             // Acc Zero value
int Acc_RAW;              // Acc ADC value
float Acc_Angle;          // Angle misured Acc [DEG]
float Acc_Value;
int Gyro_Zero;            // Gyro Zero value
int Gyro_RAW;             // Gyro ADC value
float Gyro_Rate;          // Gyro Angular velocity [DEG/sec]
float Angle=0;            // Angle estimate [DEG]  
int setZeroCount=0;
long AccZeroSum;
long GyroZeroSum;
float dt=0;

//motor variables
int dutyCycleLeft;        // value of PWM for motor left
int dutyCycleRight;       // value of PWM for motor right
int dutyCycleLper;        // value of PWM %
int dutyCycleRper;        // value of PWM %
int stallMotor;           // value of PWM for motor stall
float drive=0;
float goStart=0;          // for softstart function
float SteerSpeedAdj;      // for Steer adaptive of speed


//Steer variable
int Steer_RAW;            // Steer ADC value
int Steer_Zero;           // Steer Zero value
float Steer=0.0;                // Steer 
float Speed=0.0;
byte SteerScale;          // Steering sensitivity
int errorIMU=0;

//from joystick
int dataX=127, dataY=127; // data from telecontroller

//Status value
int statusFlag=0;
boolean lastButtonState;
boolean buttonState;
long time1, time2, time3;
float Integral=0.0;
float Derivate;
float Angle_pre=0.0;
boolean test_LED=0;
boolean status_LED=0;

// PID parameters
byte KP;  // proportional angle
byte KI;  // integral angle 
byte KD;  // derivate angle
byte KO;  // proportional position
byte KV;  // derivate position (velocity)

int LCD_Page = 0;         // Page to display

void setup() 
{
  Serial.begin(SERIAL_COM_SPEED);
  GUI.begin(SERIAL_COM_SPEED);
  LCD.begin(LCD_COM_SPEED);
  analogReference(EXTERNAL);
  
  pinMode(test_LED_Pin, OUTPUT);    //use pin to test cycle time
  pinMode(status_LED_pin, OUTPUT);  //use LED to status monitor
  pinMode(P1_pin, INPUT_PULLUP);
  pinMode(P2_pin, INPUT_PULLUP);
  pinMode(P3_pin, INPUT_PULLUP);
  pinMode(PStart_pin, INPUT_PULLUP);   
  digitalWrite(status_LED_pin, LOW);  
  
  // For SerialMonitor diagnostic 
  Serial.println("Personal Robot V1.0\n");
  //Serial.println("Init SerLCD...");       
  //serLCDInit();  
  Serial.println("Init motor...");     
  initMotor();
  Serial.println("Init Steer...");   
  initSteer();  
  Serial.println("Init IBat...");      
  initBat();
  Serial.println("Init IMU...");  
  initIMU();  
  Serial.println("Init encoder...");
  initEncoder();  
  Serial.print("VBat=");
  Serial.println(VBatLevelold);    
  Serial.println("Init EEPROM...");    
  readEepromParams();  

  // verify EEPROM data
  if ( KP==255 ) bitSet(statusFlag, EEPROM_error); 
  if ( KI==255 ) bitSet(statusFlag, EEPROM_error);   
  if ( KD==255 ) bitSet(statusFlag, EEPROM_error); 
  if ( SteerScale==255 ) bitSet(statusFlag, EEPROM_error);   
  if ( KO==255 ) bitSet(statusFlag, EEPROM_error);
  if ( KV==255 ) bitSet(statusFlag, EEPROM_error);  

  Serial.print("EEPROM data:");
  Serial.print("  KP=");      
  Serial.print(KP);
  Serial.print(" KI=");
  Serial.print(KI);
  Serial.print(" KD=");
  Serial.print(KD);
  Serial.print(" KO=");
  Serial.print(KO);  
  Serial.print(" KV=");
  Serial.println(KV);  
  Serial.print(" SteerScale=");
  Serial.println(SteerScale);
  Serial.print("Steer_Zero=");
  Serial.println(Steer_Zero);   
  Serial.print("Acc_Zero=");
  Serial.println(Acc_Zero);
  Serial.print("Gyro_Zero=");
  Serial.println(Gyro_Zero);     
  Serial.print("IBat_Zero=");
  Serial.println(IBat_Zero);   
  time1=millis();  
}

void loop()
{
  
  time2=micros();
  dt = float(time2-time1)/1000000.0;  //cycle time
  time1=time2; 

  SoftStart();
  EstimateAngle();
  EstimatePos();
  //  Steering(); 
  Status();   
  PID();
  SetMotor();
  SetZero();   
  Telemetry(); 
//  test_LED = !test_LED;
//  digitalWrite(test_LED_Pin, test_LED);  
  
// if P-reset is pressed then erase alarm 
  if (digitalRead(P1_pin) == 0)
  {
       // reset error
      statusFlag=0;          
      errorIMU=0;
      Integral=0;
      encoderCounterLeft=0;
      encoderCounterRight=0;
  }  

  delay(18);  //about 50Hz  
}


void Status()
{
   // Set the Status Flag ERROR
   
   if (digitalRead(PStart_pin)==0) 
     bitSet(statusFlag, GO_Standby);
   else
     bitClear(statusFlag, GO_Standby);        
        
   IBatLevel = (float(analogRead(IBat_ADC_pin)-IBat_Zero)*IBatScale);
   if (IBatLevel > IBatMAX) bitSet(statusFlag, IBat_over);
   
   IBatLevelSmooth = abs(IBatLevel);
   
   IBatLevelSmooth = 0.15*IBatLevelSmooth + 0.85*IBatLevelSmoothold;
   IBatLevelSmoothold = IBatLevelSmooth;
   // K = 1/(Cycle_Freq*tau)  tau=0,1sec -> K=0.15
   // K = 0.075 tau=50msec
   /*   // for FIR filter
    	VBat_RAWArray[(ind++)%28] = analogRead(VBat_ADC_pin);
    	for (i=0; i<28; i++) VBat_RAW += VBat_RawArray[i];
    	VBatLevel = VBat_RAW / 79;                  // result VBatt in 0.1V steps (es. 12,47V=124)    
       //constant = (1024/((R1+R2)/R1)*Vref)) * (interation/10) = 1024/(11*3,3) * (28/10)=79
   */
           
   VBatLevel = float(analogRead(VBat_ADC_pin))*VBatScale;  
   VBatLevel = 0.0077*VBatLevel + 0.993*VBatLevelold;  // FILTRO IIR
   VBatLevelold = VBatLevel;   
   // costante di tempo tau[sec] = Tck/K (Tck=1/Cycle_Freq)
   // K = 1/(Cycle_Freq*tau)  tau=2sec -> K=0.0077
   
   if (VBatLevel > VBatLmax) bitSet(statusFlag, VBat_over); 
   if (VBatLevel < VBatLmin) bitSet(statusFlag, VBat_under);   
 
   // IMU overflow
     if (errorIMU>0)
     {
       bitSet(statusFlag, Acc_error); 
       bitSet(statusFlag, Gyro_error);
     }  


//   if ( (Steer_RAW<200) || (Steer_RAW>820) ) bitSet(statusFlag, Steer_error); 
   if ( abs(Angle)>9 ) bitSet(statusFlag, Angle_error);   
//   if ( abs(wheelsVel) > 1400) bitSet(statusFlag, Velocity_error);  // 720 DEG/SEC = 2 turn/sec
 
   
 if (statusFlag>1) // blink status LED
   if (millis() > time3+250)
   {
     status_LED = !status_LED; 
     digitalWrite(status_LED_pin, status_LED);
     time3=millis();
   }
 else
   digitalWrite(status_LED_pin, LOW);
   
   
/*
statusFlag register:
#define  GO_Standby   0
#define  VBat_under   1
#define  VBat_over    2
#define  IBat_over    3
#define  Steer_error  4
#define  Acc_error    5
#define  Gyro_error   6
#define  Angle_error  7
#define  EEPROM_error 8
#define  Speed_error  9
*/  
}

