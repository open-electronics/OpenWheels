/*
  OpenWheels IMU use GY-512 (MPU6050)
 */
 
 //quando si mette sotto sopra invertire la lettura di acc e gyro!!!
  
  int i;

void initIMU()
{
    MPU6050_init();  
}


void SetZero()
{
  // calculate offset acc and gyro during RUN
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
  
void EstimateAngle()
{  
  Acc_RAW = readAcc();
  Acc_Value = float(Acc_RAW-Acc_Zero) / 16384.0; // scale Acc_RAW to ±1 (fs 2g) 16384 LSB/g (MPU6050m pag.30di47 di MPU6050 register v4.0) 
  Acc_Angle= Acc_Value*57.296; // convert Acc to angle  alpha=Acc*360/(2*PI) DEG
  
  Gyro_RAW = readGyro();
  Gyro_Rate = float(Gyro_Zero-Gyro_RAW)/131.0;  // scale Gyro_RAW to DEG/sec (with FS ± 250 °/s sensibility is 131 LSB/°/s)
  
  // Use complementary filter to fusion acc e gyro data
  float Comp_Filter_Constant = 0.01; 
    
  Angle = (1-Comp_Filter_Constant)*(Angle + Gyro_Rate*dt) + (Comp_Filter_Constant * Acc_Angle);
  
  Angle = constrain(Angle, -10, 10);  //limits range of sensor value 
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
      Steer_Zero += analogRead(Steer_ADC_pin);
      delay(20);
   }
   Steer_Zero /= i;    
}  

void Steering()
{
  Steer_RAW=0;  
  for (i=0; i<10; i++)
  {
    Steer_RAW += analogRead(Steer_ADC_pin);
    //delay(1); 
  }
  Steer_RAW /= i;
  

  SteerSpeedAdj = (250.0-abs(drive))/250.0; 
  // (Speed=0 SteerSpeedAdj=1.0) ((Speed=250 SteerSpeedAdj=0.0)  
  Steer = float(Steer_RAW-Steer_Zero) *  SteerSpeedAdj * SteerScale * 0.01; 
  
  Steer = constrain(Steer, -100, 100); //limits range of steering   
}



void MPU6050_init()
{  
  byte error;
  byte data;  
  
  Wire.begin();
  // Disable internal Arduino R-pullup
  digitalWrite(SDA, 0);
  digitalWrite(SCL, 0);    
  
  // default at power-up:
  //    Gyro at 250 degrees second
  //    Acceleration at 2g
  //    Clock source at internal 8MHz
  //    The device is in sleep mode.
  
    error = MPU6050_read_byte (MPU6050_WHO_AM_I, &data); 
  
    if(data == 0x68)
    {
         Serial.print("I2C Read Test Passed, MPU6050 Address: 0x");
         Serial.println(data,HEX);
    }
    else
    {
	Serial.println("I2C Read Test Failed! NO MPU6050");
	//while(1){}	// STOP
    }  
  
    MPU6050_write(MPU6050_SMPLRT_DIV, 7);   // SMPRT_DIV (This register specifies the divider from the gyroscope output rate used to generate the Sample Rate) Giroscope sample rate = 1KHz
    MPU6050_write(MPU6050_CONFIG, 0x04);    // DLPF_CFG set cut off frequency of filter EXT_SYNC_SET=0 (input disabled)  DLPF_CFG=5 (10Hz bandwidth)  DLPF_CFG=4 (20Hz bandwidth)
    MPU6050_write(MPU6050_GYRO_CONFIG, 0);  // GYRO_CONFIG Gyro Full Scale Range ± 250 °/s (131 LSB/°/s)
    MPU6050_write(MPU6050_ACCEL_CONFIG, 0); // ACCEL_CONFIG Acc  Full Scale Range ± 2g (16384 LSB/g)
    //MPU6050_write(MPU6050_PWR_MGMT_1, 0);   // PWR_MGMT_1 (This register allows the user to configure the power mode and clock source) wakes up the MPU-6050, Internal 8MHz oscillator  
        MPU6050_write(MPU6050_PWR_MGMT_1, 1); //Clock Source: PLL with X axis gyroscope reference   SLEEP_OFF
}

// --------------------------------------------------------
// MPU6050_read_byte
// This is a common function to read ONE bytes from an I2C device.

byte MPU6050_read_byte(byte regAddr, byte *data)
{
  byte error;
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  Wire.write(regAddr);
  error = Wire.endTransmission(false);    // hold the I2C-bus
  if (error != 0)
    return (error);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, 1, true);
  if (Wire.available() != 1)
    return(5);  //data is not modified
    
  data[0]=Wire.read();
  return (0);  // return : no error
}

// --------------------------------------------------------
// MPU6050_read_int
// This is a common function to read TWO bytes from an I2C device.

byte MPU6050_read_int(byte regAddr, int *data)
{
  byte error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  Wire.write(regAddr);
  error = Wire.endTransmission(false);    // hold the I2C-bus

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, 2, true);
  if (Wire.available() != 2)
    return(5);  //data is not modified
  *data=Wire.read()<<8|Wire.read();
   
  return (0);  // return : no error
}


// --------------------------------------------------------
// MPU6050_write
// This is a common function to write ONE bytes to an I2C device.

byte MPU6050_write(byte regAddr, byte data)
{
  byte error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  Wire.write(regAddr);        // write the register address
  Wire.write(data);  // write one bytes
  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  return (0);         // return : no error
}


// --------------------------------------------------------
// MPU6050_read_Accelerometer
// This is a common function to read ACC.
int readAcc()
{
  byte error;
  int data;
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  //Wire.write(MPU6050_ACCEL_YOUT_H);  //horizontal board
  Wire.write(MPU6050_ACCEL_ZOUT_H);  //vertical board  
  error = Wire.endTransmission(false);    // hold the I2C-bus
  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, 2, true);
  data=Wire.read()<<8|Wire.read();   
  return (data);
}

// --------------------------------------------------------
// MPU6050_read_Gyroscope
// This is a common function to read GYRO.
int readGyro()
{
  byte error;
  int data;
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  Wire.write(MPU6050_GYRO_XOUT_H);   //attenzione YOUT è la rotazione attorno l'asse Y
  error = Wire.endTransmission(false);    // hold the I2C-bus
  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, 2, true);
  data=Wire.read()<<8|Wire.read();   
  return (data);
}
