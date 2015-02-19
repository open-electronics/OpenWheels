/*
 OpenWheels V1.3
 by Mirco Segatello 
  
 board: OpenWheels V1.1 (gyro 1X)
 compiler: Arduino 1.0.3  
 date: lug 2013
  
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License Creative Commons  BY-NC-SA
 as published by the Free Software Foundation, see <http://creativecommons.org/licenses/by-nc-sa/3.0/>  
 */

#include <EEPROM.h>
#include <SoftwareSerial.h>


//ADC Inputs
#define Acc_ADC_pin    0
#define Gyro_ADC_pin   1
#define SPot_ADC_pin   2
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
#define test_Pin      13   // Use to test interrupt

//EEPROM adress
#define AccZero_ADR   0
#define GyroZero_ADR  2
#define KP_ADR        4
#define KI_ADR        5
#define KD_ADR        6
#define Steer_ADR     7

//Status FLAG
#define  GO_Standby   0
#define  VBat_under   1
#define  VBat_over    2
#define  IBat_over    3
#define  Steer_error  4
#define  Acc_error    5
#define  Gyro_error   6
#define  Angle_error  7
#define  EEPROM_error 8

float Cycle_Freq = 65.0;  // Cycle Frequenzy [Hz]

//Battery voltage variables
float VBatLevel, VBatLevelold; // Battery voltage [V]
float VBatScale = 0.0324;  // Battery scale for Volt converter
int VBatLper;             // Battery level %
float VBatLmax = 26.0;    // Max voltage battery
float VBatLmin = 19.2;    // Min voltage battery

//Battery current variable
float IBatLevel;          // Battery current [A]
float IBatLevelSmooth;    // Battery current smooth
float IBatLevelSmoothold;  
float IBatScale = 0.0538; // Battery scale for Ampere converter
float IBatMAX = 30.0;     // IBat MAX for shutdown MOTOR
float IBat_Zero;          // IBat Zero (for offset sensor)

//IMU Variables
int Acc_Zero;             // Acc Zero value
int Acc_RAW;              // Acc ADC value
float Acc_Angle;          // Angle misured Acc [DEG]
int Gyro_Zero;            // Gyro Zero value
int Gyro_RAW;             // Gyro ADC value
float Gyro_Rate;          // Gyro Angular velocity [DEG/sec]
float Angle=0;            // Angle estimate [DEG]  
int setZeroCount=0;
int AccZeroSum;
int GyroZeroSum;

//motor variables
int dutyCycleLeft;        // value of PWM for motor left
int dutyCycleRight;       // value of PWM for motor right
int dutyCycleLper;        // value of PWM %
int dutyCycleRper;        // value of PWM %
int stallMotor;           // value of PWM for motor stall
float drive=0;
int drivePWM;
int maxPWM;
float goStart=0;          // for softstart function
float SteerSpeedAdj;      // for Steer adaptive of speed


//Steer variable
int Steer_RAW;            // Steer ADC value
int Steer_Zero;           // Steer Zero value
int Steer;                // Steer 
byte SteerScale;     // Steering sensitivity

//Status value
int statusFlag=0;
boolean lastButtonState;
boolean buttonState;
long time1, time2;
float Accumulator=0;

// PID parameters
byte KP;
byte KI;
byte KD;

// LCD does not send data back to the Arduino, only define the txPin
SoftwareSerial LCD = SoftwareSerial(0, SerLCD_TX);

int LCD_Page = 0;         // Page to display

void setup() {
  Serial.begin(115200);
  analogReference(EXTERNAL);
  
  pinMode(P1_pin, INPUT_PULLUP);
  pinMode(P2_pin, INPUT_PULLUP);
  pinMode(P3_pin, INPUT_PULLUP);
  pinMode(PStart_pin, INPUT_PULLUP);     
  
  // For SerialMonitor diagnostic 
  Serial.println("OpenWheels V1.3\n");
  Serial.println("Init SerLCD...");       
  serLCDInit();  
  Serial.println("Init motor...");     
  initMotor();
  delay(100);
  Serial.println("Init Steer...");   
  initSteer();  
  delay(100);
  Serial.println("Init IBat...");      
  initBat();
  delay(100);
  Serial.print("VBat=");
  Serial.println(VBatLevelold);    
  Serial.println("Init EEPROM...");    
  readEepromParams();  

  // verify EEPROM data
  if ( KP==255 ) bitSet(statusFlag, EEPROM_error); 
  if ( KI==255 ) bitSet(statusFlag, EEPROM_error);   
  if ( KD==255 ) bitSet(statusFlag, EEPROM_error); 
  if ( SteerScale==255 ) bitSet(statusFlag, EEPROM_error);   
  if ( (Acc_Zero<480) || (Acc_Zero>520) ) bitSet(statusFlag, Acc_error); 
  if ( (Gyro_Zero<300) || (Gyro_Zero>400) ) bitSet(statusFlag, Gyro_error);   

  Serial.print("EEPROM data:");
  Serial.print("  KP=");      
  Serial.print(KP);
  Serial.print(" KI=");
  Serial.print(KI);
  Serial.print(" KD=");
  Serial.println(KD);
  Serial.print("SteerScale=");
  Serial.println(SteerScale);
  Serial.print("Steer_Zero=");
  Serial.println(Steer_Zero);   
  Serial.print("Acc_Zero=");
  Serial.println(Acc_Zero);
  Serial.print("Gyro_Zero=");
  Serial.println(Gyro_Zero);     
  Serial.print("IBat_Zero=");
  Serial.println(IBat_Zero);   
  delay(200);  
  Init_Timer();       
}

void loop() {

  // Gestione LCD
switch (LCD_Page) {
    case 0:
      lcdPosition(0,0);  
      clearLCD();
      VBatLper = 100 * (VBatLevel-VBatLmin) / (VBatLmax-VBatLmin);
      VBatLper = constrain(VBatLper, 0, 100); 
      LCD.print(VBatLper);      
      LCD.print("%             ");
      lcdPosition(1,0);      

      if (statusFlag>1) {
        LCD.print("ERROR ");
        LCD.print(statusFlag, BIN);      
      }
      else if (goStart>0.5)
        LCD.print("RUN             ");
      else
        LCD.print("STANDBY         ");
 
      if ( (digitalRead(P1_pin)==0) || (digitalRead(P2_pin)==0) ) statusFlag=0;          
        
    break;
      
    case 1:      
      lcdPosition(0,0);    
      LCD.print(VBatLevel);
      LCD.print("V  ");
      
      lcdPosition(0,8);   
      if (IBatLevelSmooth<10.0)   LCD.print(" ");      
      LCD.print(IBatLevelSmooth);  
      LCD.print("A  ");  
      
      lcdPosition(1,0);  
      LCD.print("Acc=");
      if (Acc_RAW<10)   LCD.print(" ");
      if (Acc_RAW<100)  LCD.print(" "); 
      if (Acc_RAW<1000) 
        LCD.print(Acc_RAW);
      else
        LCD.print("---");
      LCD.print(" ");

  
      lcdPosition(1,8);   
      LCD.print("Gyr=");
      if (Gyro_RAW<10)   LCD.print(" ");
      if (Gyro_RAW<100)  LCD.print(" "); 
      if (Gyro_RAW<1000) 
      LCD.print(Gyro_RAW);
      else
        LCD.print("---");

      if ( (digitalRead(P1_pin)==0) || (digitalRead(P2_pin)==0) ) LCD_Page = 4;        
        
    break;
      
    case 2:
      lcdPosition(0,0);  
      if (Angle>=0) LCD.print(" ");
      if (abs(Angle)<10.0)  LCD.print(" ");       
      LCD.print(Angle);     
      LCD.write(0xDF);  
      LCD.print(" ");
      
      lcdPosition(0,8);       
      LCD.print("Str=");
      if (Steer>=0) LCD.print(" ");
      if (abs(Steer)<10)   LCD.print(" ");      
      LCD.print(Steer);
      LCD.print(" ");

      lcdPosition(1,0);       
      LCD.print("dL=");
      dutyCycleLper = 0.4 * float(dutyCycleLeft-stallMotor); //0,4=100*(ICR1/2)
      if (dutyCycleLper>=0)   LCD.print(" ");
      if (abs(dutyCycleLper)<10)   LCD.print(" ");    
      LCD.print(dutyCycleLper);
      LCD.print("%");
      
      lcdPosition(1,8);
      LCD.print("dR=");
      dutyCycleRper = 0.4 * float(dutyCycleRight-stallMotor); //0,4=100*(ICR1/2)
      if (dutyCycleRper>=0)   LCD.print(" ");
      if (abs(dutyCycleRper)<10)   LCD.print(" ");    
      LCD.print(dutyCycleRper);
      LCD.print("%");                

      if ( (digitalRead(P1_pin)==0) || (digitalRead(P2_pin)==0) )   
      {     
        clearLCD();
        lcdPosition(0,0);
        LCD.print("Set Zero wait...");
        delay(2000);          
        setZeroCount = 50;
        AccZeroSum = 0;
        GyroZeroSum = 0;
        //LCD_Page=0;
       }
    break;  

    case 4:    
      lcdPosition(0,0);      
      clearLCD();      
      LCD.print("KP=");
      LCD.print(KP);
      if (digitalRead(P1_pin)==0) KP--; 
      if (digitalRead(P2_pin)==0) KP++;        
    break;   
    
    case 5:
      lcdPosition(0,0);      
      clearLCD();      
      LCD.print("KI=");
      LCD.print(KI);
      if (digitalRead(P1_pin)==0) KI--; 
      if (digitalRead(P2_pin)==0) KI++;      
    break;       

    case 6:
      lcdPosition(0,0);      
      clearLCD();      
      LCD.print("KD=");
      LCD.print(KD);
      if (digitalRead(P1_pin)==0) KD--; 
      if (digitalRead(P2_pin)==0) KD++;          
    break;    
        
    case 7:
      lcdPosition(0,0);      
      clearLCD();      
      LCD.print("Steer=");
      LCD.print(SteerScale);
      if (digitalRead(P1_pin)==0) SteerScale--; 
      if (digitalRead(P2_pin)==0) SteerScale++;          
    break;  
  }

  
  time1=millis();
  time2=0;
  while(digitalRead(P3_pin)==HIGH && time2<200)
  {
    time2=millis()-time1;
  }
  
    // if P3 pressed
    if (digitalRead(P3_pin) == LOW) {
      // P3 pressed
      if ( LCD_Page==4 )  EEPROM.write(KP_ADR, KP);  
      if ( LCD_Page==5 )  EEPROM.write(KI_ADR, KI);      
      if ( LCD_Page==6 )  EEPROM.write(KD_ADR, KD);    
      if ( LCD_Page==7 )  EEPROM.write(Steer_ADR, SteerScale);    
      LCD_Page++;  
      if (LCD_Page==3) LCD_Page=0;      
      if (LCD_Page>7) LCD_Page=0;
    } 
    
  while(digitalRead(P3_pin)==LOW)
  {
    delay(20);
  } 
  
}


ISR(TIMER2_COMPA_vect)
{
  bitSet(PORTB, 5);
  SoftStart();
  AccAngleMeasure();
  GyroRateMeasure();
  EstimateAngle(); 
  Steering();
  Status();  
  PID();
  SetMotor();
  SetZero();   
  Telemetry(); 
  bitClear(PORTB, 5);

  //  digitalWrite(LEDPIN, LOW); // per calcolare la durata dell'interrupt
  // la durata dell'impulso su pin13 permette di calcolare la durata delle operazioni
  // senza delay su acc tciclo=2,3msec
  // con delay(1) su acc tciclo=11msec
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
   
   //if (VBatLevel > VBatLmax) bitSet(statusFlag, VBat_over); 
   if (VBatLevel < VBatLmin) bitSet(statusFlag, VBat_under);   
 
   // IMU overflow
   if ( (Steer_RAW<200) || (Steer_RAW>820) ) bitSet(statusFlag, Steer_error); 
   if ( (Acc_RAW<100) || (Acc_RAW>920) ) bitSet(statusFlag, Acc_error);    
   if ( (Gyro_RAW<100) || (Gyro_RAW>920) ) bitSet(statusFlag, Gyro_error);   
   if ( abs(Angle)>29 ) bitSet(statusFlag, Angle_error);   
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
#define  EEPROM_error 9
*/   
}


void Init_Timer()
{   
  //pinMode(test_Pin, OUTPUT);  //use pin to test interrupt
  
  //freq_INT = 16000000/(Prescaler * OCR2A)
  //OCR2A = 16000000/(Prescaler * freq_INT)
  //OCR2A=255 freq_INT = 61.3Hz  (min freq possible)
  //OCR2A=156 freq_INT = 100.1Hz
  int Count =  15625/Cycle_Freq;  
  //Serial.print("Count=");
  //Serial.println(Count);
  cli();		       // disable global interrupts
  TCCR2A = 0;		       // set TCCR2A control register to 0
  TCCR2B = 0;		       // set TCCR2B control register to 0 
  TCCR2A = _BV(WGM21);  
  TCCR2B = _BV(CS20) | _BV(CS21) | _BV(CS22);  //set prescaler to 1024
  OCR2A = Count;               // Set compared value
  ASSR &= ~(1 << AS2);         // Use system clock for Timer/Counter2 AS2=1
  TIMSK2 = 0;                  // Reset Timer/Counter2 Interrupt Mask Register
  TIMSK2 = _BV(OCIE2A);        // Enable Output Compare Match A Interrupt    
  sei();                       // enable global interrupts
}

  
