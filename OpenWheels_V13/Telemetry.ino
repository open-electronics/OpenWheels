/*
  OpenWheels Telemetry
 */

int index;
byte inByte[10];
        
void Telemetry()
{
  
 if (Serial.available()) 
 {
  switch (Serial.read()) 
  {
    case 'A':   
      // Arduino to GUI Telemetry data 
      Serial.write('A');
      sendSerialInt(Acc_RAW);
      sendSerialInt(Gyro_RAW);      
      sendSerialInt(int(Angle*10));     
      //sendSerialInt(int(Acc_Angle));     
      //sendSerialInt(int(Gyro_Rate));
      sendSerialInt(int(IBatLevelSmooth*10));    
      sendSerialInt(int(drive));
      sendSerialInt(statusFlag);      
      sendSerialInt(int(VBatLevel*10));
      sendSerialInt(Steer);      
      //send 1+8*2=17 byte to GUI
      break;
      
    case 'E':
      Serial.write('E');     
      sendSerialInt(KP);
      sendSerialInt(KI);
      sendSerialInt(KD);   
      break;  
      
      case 'R':
      // Arduino to PC PID parameters      
      Serial.print("KP=");      
      Serial.print(KP);
      Serial.print(" KI=");
      Serial.print(KI);
      Serial.print(" KD=");
      Serial.print(KD);
      Serial.println();
      break;         
      
    case 'W':
      // GUI to Arduino PID parameters      
      index=0;
      while (Serial.available()>0)
      {
         inByte[index] = Serial.read();
         index++;      
      }        
      KP=inByte[0];
      KI=inByte[1];
      KD=inByte[2];
//      EEPROM.write(KP_ADR, P & 0xFF);
//      EEPROM.write(KI_ADR, I & 0xFF);     
//      EEPROM.write(KD_ADR, D & 0xFF);     
      break;  
      
    case 'S':
      // Save PID paremeter on Arduino EEPROM    
      EEPROM.write(KP_ADR, KP);
      EEPROM.write(KI_ADR, KI);     
      EEPROM.write(KD_ADR, KD);           
      break;
   
    case 'Z':
      // Set Zero Acc & Gyro
      setZeroCount = 50;
      AccZeroSum = 0;
      GyroZeroSum = 0;
      break;  
      
    } 
  }  
}  
/*
int readSerialInt()
{
 
}

float readSerialFloat()
{
  uint8_t index = 0;
  char data[32] = "";
  do {
    while (Serial.available() == 0) {}
    data[index++] = Serial.read();
  }  while ((data[index-1] != ',') && (index < 32));
  return atof(data);
}

void sendSerialFloat(float num)
{
  byte * b = (byte *) &num;
  Serial.write(b[0]);
  Serial.write(b[1]);
  Serial.write(b[2]);
  Serial.write(b[3]);
} 
*/
void sendSerialInt(int num)
{
  Serial.write(num);
  Serial.write(num>>8);
} 
