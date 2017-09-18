/*
  OpenWheels V2 Test MOTOR Program
  motor PWM is specified with pushbutton or serial command
  by Mirco Segatello 
  
  Hardware OpenWheels 2.0 Febrary 2015 
  board: compatibility to ARDUINO MEGA 2560
  compiler: Arduino 1.0.5  
  Copyright: Creative Commons  BY-NC-SA
 */

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

//Battery variables
float VBatLevel;
float VBatScale=0.036;
float IBatLevel;
float IBatScale=0.0538;

//motor variables
int dutyCycleLeft;
int dutyCycleRight;
int stallMotor;            // value of PWM for motor stall
float drive=0;             // power of motor +-250
int drivePWM;
int maxPWM;
boolean status;


void setup() {
  Serial.begin(115000);              // for DEBUG
  Serial1.begin(9600);               // for LCD
  analogReference(EXTERNAL);         // 3V3 reference for POT and analog IMU (VPOT=3V3)

  pinMode(P1_pin, INPUT_PULLUP);
  pinMode(P2_pin, INPUT_PULLUP);
  pinMode(P3_pin, INPUT_PULLUP);
  pinMode(PStart_pin, INPUT_PULLUP);
  
  Serial.println("OpenWheelsV2 Test Motor\n");
  Serial.println("Init motor...");  
  initMotor();
  delay(500); 
  Serial.println("Init LCD...\n");   
  serLCDInit();
  delay(500);   
}

void loop() 
{
  if (Serial.available()>0)
    switch (Serial.read())
    {
       case '0':  
         drive=0.0;
         break;      
       case 'a':  
         drive=25.0;
         break;
       case 's':
         drive=50.0;
         break;
       case 'd':  
         drive=100.0;
         break;   
       case 'z':  
         drive=-25.0;
         break;
       case 'x':
         drive=-50.0;
         break;
       case 'c':  
         drive=-100.0;
         break;           
    }
    
    
  status=digitalRead(PStart_pin);
  digitalWrite(ShutDown_pin, !status);
  
  if (digitalRead(P1_pin)==0) 
      if (drive>-stallMotor) drive-=1.0;

  if (digitalRead(P2_pin)==0) 
      if (drive<stallMotor) drive+=1.0;
 
  if (digitalRead(P3_pin)==0) 
      drive=0.0;

  Serial.print("drive=");  // ECO on SerialMonitor
  Serial.println(int(drive));
  clearLCD();  
  LCDPosition(0,0);      
  Serial1.print("drive=");
  Serial1.print(int(drive));
  LCDPosition(1,0);  
  if (status==0)
  {
    Serial.print("RUN   ");  // ECO on SerialMonitor 
    Serial1.print("RUN");  
  }  
  else
  {
    Serial.print("STOP   ");  // ECO on SerialMonitor    
    Serial1.print("STOP");  
  }
  
  SetMotor();
  delay(100);  
}






