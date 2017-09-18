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
  KO = EEPROM.read(KO_ADR);  
  KV = EEPROM.read(KV_ADR);
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
