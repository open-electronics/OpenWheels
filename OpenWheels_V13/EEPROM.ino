/*
  OpenWheels EEPROM
 */

  
void readEepromParams()
{
  Acc_Zero = eepromReadInt(AccZero_ADR);
  Gyro_Zero = eepromReadInt(GyroZero_ADR);
  KP = EEPROM.read(KP_ADR);
  KI = EEPROM.read(KI_ADR);
  KD = EEPROM.read(KD_ADR);  
  SteerScale = EEPROM.read(Steer_ADR);
}

int eepromReadInt(int address){
   int value = 0x0000;
   value = value | (EEPROM.read(address) << 8);
   value = value | EEPROM.read(address+1);
   return value;
}

void eepromWriteInt(int value, int address){
   EEPROM.write(address, (value >> 8) & 0xFF );
   EEPROM.write(address+1, value & 0xFF);
}

/*
void writeEepromParams()
{
  writeEepromInt(Acc_Zero, AccZero_ADR);
  writeEepromInt(Gyro_Zero, GyroZero_ADR);
  writeEepromFloat(KP, KP_ADR);
  writeEepromFloat(KI, KI_ADR);
  writeEepromFloat(KD, KD_ADR);
}
 

float eepromReadFloat(int address){
   union u_tag {
     byte b[4];
     float fval;
   } u;   
   u.b[0] = EEPROM.read(address);
   u.b[1] = EEPROM.read(address+1);
   u.b[2] = EEPROM.read(address+2);
   u.b[3] = EEPROM.read(address+3);
   return u.fval;
}



void eepromWriteFloat(float value, int address){
   union u_tag {
     byte b[4];
     float fval;
   } u;
   u.fval=value;
 
   EEPROM.write(address  , u.b[0]);
   EEPROM.write(address+1, u.b[1]);
   EEPROM.write(address+2, u.b[2]);
   EEPROM.write(address+3, u.b[3]);
}


float readEepromFloat(int add)
{
  static float val;
  for(int i=0; i<4; i++)
    ((int*)&val)[i]=EEPROM.read(add+i);
  return val;
}


float readEepromInt(int add)
{
  static int val;
  for(int i=0; i<2; i++)
    ((int*)&val)[i]=EEPROM.read(add+i);
  return val;
}


void writeEepromFloat(float val, int add)
{
  for(int i=0; i<4; i++)
    EEPROM.write(add+i,((int*)&val)[i]);
}


void writeEepromInt(int val, int add)
{
  for(int i=0; i<2; i++)
    EEPROM.write(add+i,((int*)&val)[i]);
}
*/

