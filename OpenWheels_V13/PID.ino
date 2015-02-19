/*
  OpenWheels PID
 */

float Pval, Ival, Dval;
//float Accumulator=0;


void PID()
{
  //Implement PID
  
  // solo in marcia accumulatore attivo
  if(digitalRead(PStart_pin)==0)
      Accumulator = Accumulator + Angle*0.05;
  else
      Accumulator = 0;
      
  Accumulator = constrain(Accumulator, -20, 20);
  
  Pval = float(KP) * Angle;  
  Ival = float(KI) * Accumulator;  
  Dval = float(KD) * Gyro_Rate;
  
  drive = (Pval + Ival + Dval)/10;  
  drive = drive * 24.0/VBatLevel;  // to compensate Battery level
} 
