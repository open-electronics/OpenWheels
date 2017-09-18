/*
  OpenWheels PID
 */

float Pval, Ival, Dval, Ppos, Pvel;

void PID()
{
  //Implement PID
  
  // solo in marcia accumulatore attivo
  if(digitalRead(PStart_pin)==0)
  {  
      Integral += Angle*dt;     
      Integral = constrain(Integral, -50, 50);       
  }    
  else
  {
      Integral = 0;
      encoderCounterLeft = 0;
      encoderCounterRight = 0;
  }    
      
  
  Pval = float(KP) * Angle *2.0;   
  
  Ival = float(KI) * Integral;  //used only openwheels

  Dval = float(KD) * Gyro_Rate;

  Ppos = float(KO) * wheelsPos/10;
  
  Pvel = float(KV) * wheelsVel/10 - Speed;
  
  drive = (Pval + Ival + Dval + Ppos + Pvel)/5; 
  
  drive = drive * VBatAlim/VBatLevel;  // for compensate Battery level

  drive = constrain(drive, -250.0, 250.0);  //limit drive values
  
} 
