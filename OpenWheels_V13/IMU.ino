/*
  OpenWheels IMU
 */
 
 //quando si mette sotto sopra invertire la lettura di acc o gyro!!!


  // vedere appunti per determinare Acc_Scale
  float Acc_Scale = 0.2309; //Pololu MMA7361L 800mV/g
  // vedere appunti per determinare Giro_Scale
  // float Gyro_Scale = 0.0968; //sparkfun LPY503A 33,3mV(DEG/sec) // use 4X output
  float Gyro_Scale = 0.3883; //sparkfun LPY503A 8,3mV(DEG/sec) // use 1X output
  
  int i;
  
void InitAcc()
{
   Acc_Zero=0;
   for (i=0; i<50; i++)
   {                                    
      Acc_Zero += analogRead(Acc_ADC_pin);
      delay(10);
   }
   Acc_Zero /= i; 
}  

void InitGyro()
{
   Gyro_Zero=0;
   for (i=0; i<50; i++)
   {                                    
      Gyro_Zero += analogRead(Gyro_ADC_pin);
      delay(10);
   }
   Gyro_Zero /= i; 
} 

void SetZero()
{
  if (setZeroCount == 1)
  {
     setZeroCount = 0; 
     Acc_Zero = AccZeroSum/49;
     Gyro_Zero = GyroZeroSum/49;
     //salva dati in EEPROM   
     eepromWriteInt(Acc_Zero, AccZero_ADR);
     eepromWriteInt(Gyro_Zero, GyroZero_ADR);    
  }
  
  if (setZeroCount > 1) 
  {
     setZeroCount--;
     AccZeroSum += Acc_RAW;
     GyroZeroSum += Gyro_RAW;    
  }     
}  

void AccAngleMeasure()
{
  Acc_RAW=0;
  for (i=0; i<10; i++)
  {
    Acc_RAW += analogRead(Acc_ADC_pin);
    delayMicroseconds(500); //più alto riduce il rumore
  }
  Acc_RAW /= i;
  //Acc_Angle = float(Acc_RAW-Acc_Zero)*Acc_Scale;  //faccia in su
  Acc_Angle = float(Acc_Zero-Acc_RAW)*Acc_Scale;  //faccia in giù (DEG)
}  

void GyroRateMeasure()
{
  Gyro_RAW=0;  
  for (i=0; i<10; i++)
  {
    Gyro_RAW += analogRead(Gyro_ADC_pin);
    //delay(1); 
  }
  Gyro_RAW /= i;
  //Gyro_Rate = float(Gyro_RAW-Gyro_Zero)*Gyro_Scale;
  Gyro_Rate = float(Gyro_Zero-Gyro_RAW)*Gyro_Scale;  
  //Gyro_Rate = constrain(Gyro_Rate, -100, 100); //limits range of sensor values (DEG/SEC)
}  

void EstimateAngle()
{
  // Use complementary filter to fusion acc e gyro data
  float Comp_Filter_Constant = 0.03;
  float dt=1/Cycle_Freq; // time for cycle
  Angle = (1-Comp_Filter_Constant)*(Angle + Gyro_Rate*dt) + (Comp_Filter_Constant * Acc_Angle);
  Angle = constrain(Angle, -30, 30);  //limits range of sensor values 
}

void initBat()
{    
   VBatLevelold = float(analogRead(VBat_ADC_pin))*VBatScale;
   
   IBatLevel = (float(analogRead(IBat_ADC_pin)-IBat_Zero)*IBatScale);
   IBatLevelSmoothold = abs(IBatLevel);
   
   IBat_Zero=0.0;
   for (i=0; i<50; i++)
   {                                    
      IBat_Zero += analogRead(IBat_ADC_pin);
      delay(20);
   }
   IBat_Zero /= float(i); 
}


void initSteer()
{
   Steer_Zero=0;
   for (i=0; i<50; i++)
   {                                    
      Steer_Zero += analogRead(SPot_ADC_pin);
      delay(20);
   }
   Steer_Zero /= i;    
}  

void Steering()
{
  Steer_RAW=0;  
  for (i=0; i<10; i++)
  {
    Steer_RAW += analogRead(SPot_ADC_pin);
    //delay(1); 
  }
  Steer_RAW /= i;
  
  //Steer = int(float(Steer_RAW-Steer_Zero)*SteerScale*0.01);

  SteerSpeedAdj = (250.0-abs(drive))/250.0; 
  // (Speed=0 SteerSpeedAdj=1.0) ((Speed=250 SteerSpeedAdj=0.0)  
  Steer = float(Steer_RAW-Steer_Zero) *  SteerSpeedAdj * SteerScale * 0.01;// 
  
  Steer = constrain(Steer, -100, 100); //limits range of steering   
}
