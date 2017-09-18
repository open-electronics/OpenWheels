/*
  OpenWheels Telemetry
 */

int index;
byte inByte[10];
char data;
        
void Telemetry()
{
  
 if (GUI.available()) 
 {
  switch (GUI.read()) 
  {
    case 'A':   
      // Arduino to GUI Telemetry data 
      GUI.write('A');
      //sendSerialInt(Acc_RAW);
      sendSerialInt(int(Acc_Value*100));  // Use two digits after the decimal point 
      sendSerialInt(int(Gyro_Rate*100));      
      sendSerialInt(int(Angle*10));     
      //sendSerialInt(int(Acc_Angle));     
      //sendSerialInt(int(Gyro_Rate));
      sendSerialInt(int(IBatLevelSmooth*10));    
      sendSerialInt(int(drive));
      sendSerialInt(statusFlag);      
      sendSerialInt(int(VBatLevel*10));
      sendSerialInt(Steer);      
      sendSerialInt(encoderPos);
      //send 1+8*2=18+1 byte to GUI
      // clear RX buffer
      while (GUI.available() > 0) data=GUI.read();
      break;
      
    case 'E':
      // Arduino to GUI PID parameters
      GUI.write('E');     
      sendSerialInt(KP);
      sendSerialInt(KI);
      sendSerialInt(KD);  
      sendSerialInt(SteerScale);
      sendSerialInt(KO);
      sendSerialInt(KV);
      break;  
      
      case 'R':
      // Arduino to SerialMonitor PID parameters      
      GUI.print("KP=");      
      GUI.print(KP);
      GUI.print(" KI=");
      GUI.print(KI);
      GUI.print(" KD=");
      GUI.print(KD);
      GUI.print(" SteerScale=");
      GUI.print(SteerScale);
      GUI.println();
      
      break;         
      
    case 'W':
      // GUI to Arduino PID parameters      
      index=0;
      while (GUI.available()>0)
      {
         inByte[index] = GUI.read();
         index++;      
      }        
      KP=inByte[0];
      KI=inByte[1];
      KD=inByte[2]; 
      SteerScale=inByte[3]; 
      KO=inByte[4];
      KV=inByte[5];
      break;  
      
    case 'S':
      // Save PID paremeter on Arduino EEPROM    
      EEPROM.write(KP_ADR, KP);
      EEPROM.write(KI_ADR, KI);     
      EEPROM.write(KD_ADR, KD);   
      EEPROM.write(Steer_ADR, SteerScale);     
      EEPROM.write(KO_ADR, KO);
      EEPROM.write(KV_ADR, KV);      
      break;
   
    case 'Z':
      // Set Zero Acc & Gyro
      setZeroCount = 50;
      AccZeroSum = 0;
      GyroZeroSum = 0;
      break;  
      
      
    case 'D':
      // GUI movement
      index=0;
      while (GUI.available()>0)
      {
         inByte[index] = GUI.read();
         index++;      
      }        
      dataX=inByte[0];
      dataY=inByte[1];

      // Arduino respond to GUI with Telemetry data 
      GUI.write('D');
      sendSerialInt(int(IBatLevelSmooth*10));    
      sendSerialInt(int(drive));
      sendSerialInt(statusFlag);      
      sendSerialInt(int(VBatLevel*10));
      sendSerialInt(int(Steer));      
      sendSerialInt(encoderPos);
      //send 1+6*2=13 byte to GUI
      // clear RX buffer
      while (GUI.available() > 0) data=GUI.read();
      break;       

    case 'C':
      // reset error
      statusFlag=0;          
      errorIMU=0;
      Integral=0;
      encoderCounterLeft=0;
      encoderCounterRight=0;
      break;      
    } 
  }  
}  


void sendSerialInt(int num)
{
  GUI.write(num);
  GUI.write(num>>8);
} 
