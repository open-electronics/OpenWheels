/*
  OpenWheels Telemetry
  by Mirco Segatello 2012
*/
int temp;

public void START(int theValue)
{
   RUN = true;
 
}  

public void STOP(int theValue)
{
   RUN = false; 
}  

public void SET_ZERO(int theValue)
{
  //set zero acc e gyro
  if (init_com == 0) return;  
  serial.write('Z');
  println("Zet Zero");  
}
/*
public void READ_PID(int theValue)
{
  //read PID parameter from Arduino  
  if(init_com == 0) return;
  serial.write('R');
  String myString = null;
  float[] datas = new float[3];
  while (serial.read() != 'R') {}
  while (myString == null) myString = serial.readStringUntil(';');
  datas = float(split(myString, ','));
  conf_KP.setValue(datas[0]);
  conf_KI.setValue(datas[1]);
  conf_KD.setValue(datas[2]);
}
*/
/*
public void READ_PID(int theValue)
{
  //read PID parameter from Arduino  
  if(init_com == 0) return;
  serial.write('E');
  byte[] inBuf = new byte[6];
  while (serial.read() != 'E') {}
   
  while (serial.available() < 6) {}
    // read all data
    serial.readBytes(inBuf);
    int temp;
    temp = (inBuf[1]<<8) + (inBuf[0]&0xFF);
    conf_KP.setValue(float(temp)/10);
    temp = (inBuf[3]<<8) + (inBuf[2]&0xFF);
    conf_KI.setValue(float(temp)/10);   
    temp = (inBuf[5]<<8) + (inBuf[4]&0xFF);
    conf_KD.setValue(float(temp)/10);   
}
*/
/*
public void READ_PID(int theValue)
{
  //read PID parameter from Arduino  
  if(init_com == 0) return;
  serial.write('E');
  //char[] inBuf = new char[6];
  while (serial.read() != 'E') {}
   
  while (serial.available() < 3) {}
  int inByte;
  // read  data
  inByte = serial.read();
  conf_KP.setValue(inByte);
  print("P=" + inByte);
   
  inByte = serial.read();  
  conf_KI.setValue(inByte);
  print(" I=" + inByte);
  
  inByte = serial.read();  
  conf_KD.setValue(inByte);
  println(" D=" + inByte);
  
  serial.clear();
}
*/
/*
public void READ_PID(int theValue)
{
  //read PID parameter from Arduino  
  if(init_com == 0) return;
  serial.write('E');
  byte[] inBuf = new byte[20];   
  while (serial.available() < 6) {}

  if (serial.read() == 'E') {
    // wait for all data arrived
    while (serial.available() < 4) {}
    // read all data
    serial.readBytes(inBuf);
    int P = (inBuf[1]<<8) + (inBuf[0]&0xFF);
    int I = (inBuf[3]<<8) + (inBuf[2]&0xFF);   
    int D = (inBuf[5]<<8) + (inBuf[4]&0xFF);   
    conf_KP.setValue(P);
    conf_KI.setValue(I);   
    conf_KD.setValue(D);    
    println(" P=" + P + " I=" + I + " D=" + D);
   }
}  
*/

public void READ_PID(int theValue)
{
  //read PID parameter from Arduino  
  if(init_com == 0) return;
  requestPID=true;
  //serial.write('E');
}

public void WRITE_PID(int theValue)
{
  // Write PID parameter to Arduino 
/*  if(init_com == 0) return;  
  String s = new String("W" 
  +round(conf_KP.value()*1000)/1000.0+","
  +round(conf_KI.value()*1000)/1000.0+","
  +round(conf_KD.value()*1000)/1000.0+",");
  serial.write(s);
  println(s);*/
  
  if(init_com == 0) return;
  writePID=true;
/*  
  serial.write('W');
  serial.write(int(conf_KP.value()));
  serial.write(int(conf_KI.value()));
  serial.write(int(conf_KD.value()));   
  */
}

public void SAVE_PID(int theValue)
{
  //save PID parameter on Arduino EEPROM
  if (init_com == 0) return;  
  serial.write('S');
  //println("Save PID");  
}

void processSerialData() {
  byte[] inBuf = new byte[20];
  switch (serial.read()) 
  {
  case 'A':
    // wait for all data arrived
   while (serial.available() < 16) {}
    // read all data
    serial.readBytes(inBuf);
    Acc_RAW = (inBuf[1]<<8) + (inBuf[0]&0xFF);
    Gyro_RAW = (inBuf[3]<<8) + (inBuf[2]&0xFF);   
    //int intbit = 0;
    //intbit = (inBuf[7] << 24) | ((inBuf[6] & 0xFF) << 16) | ((inBuf[5] & 0xFF) << 8) | (inBuf[4] & 0xFF);
    //Angle = Float.intBitsToFloat(intbit);
    int AngleInt = (inBuf[5]<<8) + (inBuf[4]&0xFF);   
    Angle = float(AngleInt)/10;
    Acc_Angle = (inBuf[7]<<8) + (inBuf[6]&0xFF);
    Gyro_Rate = (inBuf[9]<<8) + (inBuf[8]&0xff);
    Drive = (inBuf[11]<<8) + (inBuf[10]&0xFF);    
    statusFlag = (inBuf[13]<<8) + (inBuf[12]&0xFF);    
    int BatLevelInt = (inBuf[15]<<8) + (inBuf[14]&0xFF);
    Steer = (inBuf[17]<<8) + (inBuf[16]&0xFF);
    BatLevel = float(BatLevelInt)/10;
    println("Acc=" + Acc_RAW + "  Gyro=" + Gyro_RAW +
    "  Angle=" + Angle + "  Acc_Angle=" + Acc_Angle +
    "  Gyro_Rate=" + Gyro_Rate + "  Drive=" + Drive + "  Status=" + statusFlag);
    break;    
    
    
  case 'E':
    // wait for all data arrived
   while (serial.available() < 6) {}
    // read all data
    serial.readBytes(inBuf);
    int P = (inBuf[1]<<8) + (inBuf[0]&0xFF);
    int I = (inBuf[3]<<8) + (inBuf[2]&0xFF);   
    int D = (inBuf[5]<<8) + (inBuf[4]&0xFF);   
    conf_KP.setValue(P);
    conf_KI.setValue(I);   
    conf_KD.setValue(D);    
    println("P=" + P + " I=" + I + " D=" + D);    
    break;
    }
    serial.clear();
}

void InitSerial(float portValue) 
{
  // initialize the serial port selected in the listBox  
  println("initializing serial " + int(portValue) + " in serial.list()"); // for debugging
  // grab the name of the serial port
  String portPos = Serial.list()[int(portValue)]; 
  // initialize the port
  serial = new Serial(this, portPos, 115200); 
  // read bytes into a buffer until you get a linefeed (ASCII 10):
  serial.bufferUntil('\n');
  println("done init serial");
  // initialized com port flag
  init_com=1;
}
