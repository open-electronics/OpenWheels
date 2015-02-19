/*
  OpenWheels Motor
 */


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
  maxPWM = float(ICR1)*0.95/2; 
  stallMotor=ICR1/2;   //25
  OCR1A = stallMotor;  //0=0%  ICR1/2=50%  ICR1=100%
  OCR1B = stallMotor;   
}  

void SoftStart()
{
 if (statusFlag>1) {
   goStart = 0.0;
 }  
 else if (digitalRead(PStart_pin)==1) {
     goStart -= 0.01;
 }
 else {
     goStart += 0.01;     
     bitSet(PORTD, ShutDown_pin);  // Driver ON
     bitSet(statusFlag, 0);
  }
     
 goStart = constrain(goStart, 0, 1);   
 
 if (goStart==0.0) {      
     bitClear(PORTD, ShutDown_pin);  // Driver OFF
     bitClear(statusFlag, 0);
 }
} 
  
void SetMotor()
{
    drivePWM = int(drive * goStart);   // 0 in fase di standby
    drivePWM = constrain(drivePWM, -maxPWM, maxPWM);
    dutyCycleLeft = stallMotor + drivePWM + Steer; 
    dutyCycleRight = stallMotor + drivePWM - Steer;
    dutyCycleLeft = constrain(dutyCycleLeft, stallMotor-maxPWM, stallMotor+maxPWM);
    dutyCycleRight = constrain(dutyCycleRight, stallMotor-maxPWM, stallMotor+maxPWM);    
    OCR1B = dutyCycleLeft;   //set PWM SX 250+-250
    OCR1A = dutyCycleRight;  //set PWM DX 250+-250
}  

 
