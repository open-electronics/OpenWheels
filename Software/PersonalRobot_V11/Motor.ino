/*
  OpenWheels Motor
 */

int drivePWM_L, drivePWM_R;
int maxPWM;
// for control differenzial motor speed at sloww speed
// for maintenance direction in stable mode
// must be zero for remote control (usualy is 0.5 )
float Kdm=0.5
;
// Sensibility of Steer for remote control
float steerSensibilty = 0.5;
// Sensibility of Speed for remote control
float speedSensibility = 4.0;
// for compensate dead zone motor
int deadZoneMotor = 10;

void initMotor()
{
  //Initialize motor
  pinMode(ShutDown_pin, OUTPUT);   
  digitalWrite(ShutDown_pin, LOW);   //ShutDown driver at startUP  
  
  pinMode(PWMLeft_pin, OUTPUT);  
  pinMode(PWMRight_pin, OUTPUT); 
  
  // Phase and Frequency correct PWM on TIMER1
  //PWM_frequency = (16000000)/(2*Prescale_factor*ICR1) 
  //ICR1=(16000000)/(2*Prescale_factor*PWM_frequency)
  // ICR1=400  fPWM=20000
  // ICR1=500  fPWM=16000
  // ICR1=666  fPWM=12012
  // ICR1=1024 fPWM=7812
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) ; // phase and frequency correct mode. NON-inverted mode
  // TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1A0) | _BV(COM1B0) ;   //phase/frequency correct mode. SELECT THIS FOR INVERTED OUTPUTS.
  TCCR1B = _BV(WGM13) | _BV(CS10); // Select mode 8 and select divide by 1 on prescaler
  ICR1=500;
 
  // *************************************************************************
  // limit for driver compatibiliti  (5% di ICR1A) < drivePWM < (95% di ICR1A)
  // stop motor driver=stallMotor=ICR1/2;
  // *************************************************************************  
  maxPWM = float(ICR1)*0.80/2; //0.95
  stallMotor=ICR1/2;   //25
  OCR1A = stallMotor;  //0=0%  ICR1/2=50%  ICR1=100%
  OCR1B = stallMotor;   
}  

void SoftStart()
{
 if (statusFlag>1) 
 {
   goStart = 0.0;
 }  
 else if (digitalRead(PStart_pin)==1) 
 {
     //goStart -= 0.01; su openwheels
     goStart = 0.0;
 }
 else 
 {
     goStart += 0.01;     
     digitalWrite(ShutDown_pin, HIGH);
     bitSet(statusFlag, 0);
  }
     
 goStart = constrain(goStart, 0, 1);   
 
 if (goStart==0.0) 
 {      
     //bitClear(PORTB, 4);  // Driver OFF
     digitalWrite(ShutDown_pin, LOW);
     bitClear(statusFlag, 0);
 }
} 
  
void SetMotor()
{
/*  
    drivePWM = int(drive * goStart);   // 0 in standby, drive is limited to +-250
    // compensate motor dead band 
    if (drivePWM>0) drivePWM+=10;
    if (drivePWM<0) drivePWM-=10;    
    drivePWM = constrain(drivePWM, -maxPWM, maxPWM);
*/    
    Steer = steerSensibilty * float(int(dataX)-127);
    Speed = speedSensibility * float(int(dataY)-127);
    
    drivePWM_L = int(  (drive*goStart) - Kdm*float(encoderDif) + Steer );  //+Steer + float(int(dataX)-127)/10
    drivePWM_R = int(  (drive*goStart) + Kdm*float(encoderDif) - Steer );  //+Steer - float(int(dataX)-127)/10
    
    // compensate motor dead band 
    if (drivePWM_L>0) drivePWM_L += deadZoneMotor;
    if (drivePWM_L<0) drivePWM_L -= deadZoneMotor;
    if (drivePWM_R>0) drivePWM_R += deadZoneMotor;
    if (drivePWM_R<0) drivePWM_R -= deadZoneMotor;    
//    drivePWM = constrain(drivePWM, -maxPWM, maxPWM);
    
    
    dutyCycleLeft = stallMotor + drivePWM_L;// + Steer; 
    dutyCycleRight = stallMotor + drivePWM_R;// - Steer;
       
    
    dutyCycleLeft = constrain(dutyCycleLeft, stallMotor-maxPWM, stallMotor+maxPWM);
    dutyCycleRight = constrain(dutyCycleRight, stallMotor-maxPWM, stallMotor+maxPWM);    
    OCR1B = dutyCycleLeft;   //set PWM SX 250+-250
    OCR1A = dutyCycleRight;  //set PWM DX 250+-250
}  

 
