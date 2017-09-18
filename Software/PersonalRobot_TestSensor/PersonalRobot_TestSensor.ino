/*
  PersonalRobot Test SENSOR Program
  by Mirco Segatello 
  
  test IMU, encoders, Vbat, IBat, pushbutton, LCD
  
  Hardware OpenWheels 2.0 Febrary 2015 
  IMU GY-521 (MPU_6050)
  board: compatibility to ARDUINO MEGA 2560
  compiler: Arduino 1.6.4  
  Copyright: Creative Commons  BY-NC-SA
 */



#include<Wire.h>
// Default I2C address for the MPU-6050 is 0x68. // But only if the AD0 pin is low.
// Some sensor boards have AD0 high, and the I2C address thus becomes 0x69.
#define MPU6050_I2C_ADDRESS 0x68

#define MPU6050_WHO_AM_I           0x75   // R
#define MPU6050_PWR_MGMT_1         0x6B   // R/W
#define MPU6050_ACCEL_YOUT_H       0x3D   // R  
#define MPU6050_GYRO_YOUT_H        0x45   // R  

//ADC Inputs
#define Steer_ADC_pin  0
#define VBat_ADC_pin   1
#define IBat_ADC_pin   2
#define Acc_ADC_pin    4
#define Gyro_ADC_pin   5

//Driver pin
#define ShutDown_pin   10   // ShutDown for motor driver (LOW=SD)
#define PWMLeft_pin    11   // PWM for motor Left
#define PWMRight_pin   12   // PWM for motor Right

//Digital pin
#define P1_pin         6   // P1 pushbutton + function
#define P2_pin         7   // P2 pushbutton - function
#define P3_pin         8   // P3 pushbutton Enter function
#define PStart_pin     9   // Start Contact
#define test_Pin       4   // Use to test interrupt

/* on mega
INT0 pin2  ( 6) PE4
INT1 pin3  ( 7) PE5 
INT2 pin21 (43) SCL
INT3 pin20 (44) SDA
INT4 pin19 (45) RX1
INT5 pin18 (46) TX1
*/

// Encoders
#define encoderLeftPinA 2  // interrupt 4 (ENC1)
#define encoderLeftPinB 49  // PORTL.0 
#define encoderRightPinA 3 // interrupt 5 (ENC2)
#define encoderRightPinB 48 // PORTL.1
volatile long int encoderCounterLeft=0;
volatile long int encoderCounterRight=0;

//Battery variables
float VBatLevel;
float VBatScale=0.036;
float IBatLevel;
float IBatScale=0.0538;

//Motor variables
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
int error;



void setup() {
  Serial.begin(115000);              // for DEBUG
  Serial1.begin(9600);               // for LCD
  analogReference(EXTERNAL);         // 3V3 reference for POT and analog IMU (VPOT=3V3)
  
  // ShutDown Motor Driver
  pinMode(ShutDown_pin, OUTPUT);   
  digitalWrite(ShutDown_pin, LOW);   // ShutDown driver at startUP    
  pinMode(PWMLeft_pin, OUTPUT);  
  pinMode(PWMRight_pin, OUTPUT); 
  digitalWrite(PWMLeft_pin, LOW);
  digitalWrite(PWMRight_pin, LOW);    
  
  // Initialize PushButton
  pinMode(P1_pin, INPUT_PULLUP);
  pinMode(P2_pin, INPUT_PULLUP);
  pinMode(P3_pin, INPUT_PULLUP);
  pinMode(PStart_pin, INPUT_PULLUP);
  
  // Initialize Encoders
  pinMode(encoderLeftPinA, INPUT_PULLUP); // turn on pullup resistor
  pinMode(encoderLeftPinB, INPUT_PULLUP); // turn on pullup resistor  
  pinMode(encoderRightPinA, INPUT_PULLUP); // turn on pullup resistor
  pinMode(encoderRightPinB, INPUT_PULLUP); // turn on pullup resistor   
  attachInterrupt(0, EncoderLeft, FALLING); // encoder pin 2 interrupt 0 (ENC1) 90 pulse/turn
  attachInterrupt(1, EncoderRight, FALLING); // encoder pin 3 interrupt 1 (ENC2)
  
  Serial.println("PersonalRobot Test Sensor\n");
  delay(250);
  Serial.println("Init IBat offset...");   
  initIBat();
  Serial.println(IBat_Zero);
  delay(250);
  Serial.println("Init LCD...");   
  initLCD();
  delay(250); 
  Serial.println("Init IMU...");   
  initIMU();
  delay(100); 
  Serial.println();  
}


void loop() 
{

  LCDPosition(1,0);  
  Steer_RAW = analogRead(Steer_ADC_pin);
  Serial.print("Steer=");  // ECO on SerialMonitor
  Serial.print(Steer_RAW);  
  Serial1.print("Str=");
  if (Steer_RAW<10)   Serial1.print(" ");
  if (Steer_RAW<100)  Serial1.print(" "); 
  if (Steer_RAW<1000) Serial1.print(Steer_RAW);

  LCDPosition(1,8);   
  VBatLevel = float(analogRead(VBat_ADC_pin))*VBatScale;  
  Serial.print("\tVBat=");  // ECO on SerialMonitor
  Serial.print(VBatLevel);  
  Serial1.print("Bat=");
  Serial1.print(VBatLevel);
  
  IBatLevel = float(analogRead(IBat_ADC_pin)-IBat_Zero)*IBatScale;    
  Serial.print("\tIBat=");  // ECO on SerialMonitor
  Serial.print(IBatLevel);  
  
  //readIMU();
  Acc_RAW = readAcc();
  Gyro_RAW = readGyro();
  LCDPosition(0,0);  
  Serial.print("\tAcc=");  // ECO on SerialMonitor 
  Serial.print(Acc_RAW);   // scheda orizzontale
  Serial1.print("A=");
  Serial1.print(Acc_RAW);
  Serial1.print("   ");
  
  LCDPosition(0,8);  
  Serial.print("\tGyro=");  // ECO on SerialMonitor
  Serial.print(Gyro_RAW);   
  Serial1.print("G=");
  Serial1.print(Gyro_RAW); 
  Serial1.print("   ");
  
  Serial.print("\tEncL=");
  Serial.print(encoderCounterLeft);  

  Serial.print("\tEncR=");
  Serial.print(encoderCounterRight);

 
  Serial.println();
  
  if (digitalRead(P1_pin)==0) {
      Serial.println("P1 pressed");  // ECO on SerialMonitor
      clearLCD();  
      LCDPosition(0,0);      
      Serial1.print("P1 pressed");
  }
  if (digitalRead(P2_pin)==0) {
      Serial.println("P2 pressed");  // ECO on SerialMonitor
      clearLCD();  
      LCDPosition(0,0);      
      Serial1.print("P2 pressed");
  }  
  if (digitalRead(P3_pin)==0) {
      Serial.println("P3 pressed");  // ECO on SerialMonitor
      clearLCD();  
      LCDPosition(0,0);      
      Serial1.print("P3 pressed");
  }  
  if (digitalRead(PStart_pin)==0) {
      Serial.println("PStart pressed");  // ECO on SerialMonitor
      clearLCD();  
      LCDPosition(0,0);      
      Serial1.print("PStart pressed");
  }    
  
  delay(500);  
}


void initIBat() 
{    
   int i;
   IBat_Zero=0;
   for (i=0; i<50; i++)
   {                                    
      IBat_Zero += analogRead(IBat_ADC_pin);
      delay(20);
   }
   IBat_Zero /= i; 
}

void EncoderLeft()
{
  // If pinA and pinB are both high or both low, it is spinning
  // forward. If they're different, it's going backward.
     if(PINL & 0b00000001)  //PINL - The Port L Input Pins Register - read only (bit0 PL0)
        encoderCounterLeft++;
     else
        encoderCounterLeft--;
} 

void EncoderRight()
{
  // If pinA and pinB are both high or both low, it is spinning
  // forward. If they're different, it's going backward.
     if(PINL & 0b00000010)  //PINL - The Port L Input Pins Register - read only (bit0 PL0)
        encoderCounterRight++;
     else
        encoderCounterRight--;
}


