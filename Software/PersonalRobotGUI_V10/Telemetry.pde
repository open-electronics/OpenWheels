/*
 OpenWheels-GUI V2.0
 Telemetry
*/

public void OFFSET(int theValue)
{
  //set zero acc e gyro
  if (init_com == 0) return;  
//  serial.write('Z');
//  println("Set Zero");  
   setOFFSET=true;
}


public void READ(int theValue)
{
  //read PID parameter from Arduino  
  if(init_com == 0) return;
  requestPID=true;
}


public void WRITE(int theValue)
{
  // Write PID parameter to Arduino   
  if(init_com == 0) return;
  writePID=true;
}

public void SAVE(int theValue)
{
  //save PID parameter on Arduino EEPROM
  if (init_com == 0) return;
  savePID=true;  
}

public void RESET(int theValue)
{
  //reset alarm error
  if (init_com == 0) return;
  resetERROR=true;
}  

/*
esempio lettura sino a ...
while (myPort.available() > 0) {
    int lf = 10;
    // Expand array size to the number of bytes you expect:
    byte[] inBuffer = new byte[7];
    myPort.readBytesUntil(lf, inBuffer);
    if (inBuffer != null) {
      String myString = new String(inBuffer);
      println(myString);
    }
  }
  
//val = serial.readStringUntil('\n');   
*/


void processSerialData()
{     
  int temp, numBytes;  
  byte[] inBuf = new byte[20];
  switch (serial.read()) 
  {
  case 'A':
      // wait for all data arrived           
      //      while (serial.available() < 18) {} //wait for data arrived. se si perde anche solo un byte si blocca
      long time=millis()+2;  
      while( millis() < time ){ }

      numBytes = serial.readBytes(inBuf); // read all data

      if (numBytes==18)
      {
          temp = (inBuf[1]<<8) + (inBuf[0]&0xFF);
          Acc_Value = float(temp)/100;
          temp = (inBuf[3]<<8) + (inBuf[2]&0xFF);
          Gyro_Rate = float(temp)/100;    
          temp = (inBuf[5]<<8) + (inBuf[4]&0xFF);   
          Angle = float(temp)/10;
          int IBatLevelInt = (inBuf[7]<<8) + (inBuf[6]&0xFF);       
          Drive = (inBuf[9]<<8) + (inBuf[8]&0xFF);  
          Drive = int (0.4 * Drive);  //scale to %  
          statusFlag = (inBuf[11]<<8) + (inBuf[10]&0xFF);    
          int VBatLevelInt = (inBuf[13]<<8) + (inBuf[12]&0xFF);
          Steer = (inBuf[15]<<8) + (inBuf[14]&0xFF);  
          Position = (inBuf[17]<<8) + (inBuf[16]&0xFF);
          IBatLevel = float(IBatLevelInt)/10;    
          VBatLevel = float(VBatLevelInt)/10;
          println(" Acc=" + Acc_Value + "  Gyro=" + Gyro_Rate +
          "  Angle=" + Angle + "  Drive=" + Drive + "  Status=" + statusFlag);
      }
      else
      {
          print("Error receive data! ");
          println(numBytes);
      }  
      serial.clear();
      break;    

  case 'D':
      // wait for all data arrived           
      //      while (serial.available() < 18) {} //wait for data arrived. se si perde anche solo un byte si blocca
      time=millis()+2;  
      while( millis() < time ){ }

      numBytes = serial.readBytes(inBuf); // read all data

      if (numBytes==12)
      {
//          temp = (inBuf[1]<<8) + (inBuf[0]&0xFF);
//          Acc_Value = float(temp)/100;
//          temp = (inBuf[3]<<8) + (inBuf[2]&0xFF);
//          Gyro_Rate = float(temp)/100;    
//          temp = (inBuf[5]<<8) + (inBuf[4]&0xFF);   
//          Angle = float(temp)/10;
          int IBatLevelInt = (inBuf[1]<<8) + (inBuf[0]&0xFF);       
          Drive = (inBuf[3]<<8) + (inBuf[2]&0xFF);  
          Drive = int (0.4 * Drive);  //scale to %  
          statusFlag = (inBuf[5]<<8) + (inBuf[4]&0xFF);    
          int VBatLevelInt = (inBuf[7]<<8) + (inBuf[6]&0xFF);
          Steer = (inBuf[9]<<8) + (inBuf[8]&0xFF);  
          Position = (inBuf[11]<<8) + (inBuf[10]&0xFF);
          IBatLevel = float(IBatLevelInt)/10;    
          VBatLevel = float(VBatLevelInt)/10;
          println(" Acc=" + Acc_Value + "  Gyro=" + Gyro_Rate +
          "  Angle=" + Angle + "  Drive=" + Drive + "  Status=" + statusFlag);
      }
      else
      {
          print("Error receive data! ");
          println(numBytes);
      }  
      serial.clear();
      break;      
    
  case 'E':
      // wait for all data arrived
//      while (serial.available() < 8) {}
      // read all data
      numBytes =  serial.readBytes(inBuf);
      if (numBytes==12)
      {
          int P = (inBuf[1]<<8) + (inBuf[0]&0xFF);
          int I = (inBuf[3]<<8) + (inBuf[2]&0xFF);   
          int D = (inBuf[5]<<8) + (inBuf[4]&0xFF);  
          int SS = (inBuf[7]<<8) + (inBuf[6]&0xFF);  
          int O = (inBuf[9]<<8) + (inBuf[8]&0xFF);
          int V = (inBuf[11]<<8) + (inBuf[10]&0xFF);  
          conf_KP.setValue(P);
          conf_KI.setValue(I);   
          conf_KD.setValue(D);    
          conf_STEER.setValue(SS);
          conf_KO.setValue(O);
          conf_KV.setValue(V);
          println("P=" + P + " I=" + I + " D=" + D + " SC=" + SS + " O=" + O + " V=" + V);
      }
      else
      {
          println("Error receive data!");
      }    
      serial.clear();     
      break;
  }
    
   
}

void InitSerial(float portValue) 
{
  // initialize the serial port selected in the listBox  
  println("initializing serial " + int(portValue) + " in serial list"); // for debugging
  // grab the name of the serial port
  String portPos = Serial.list()[int(portValue)]; 
  // initialize the port
  serial = new Serial(this, portPos, 115200); 
  // read bytes into a buffer until you get a linefeed (ASCII 10):
  serial.bufferUntil('\n');
  println("Done init " + portPos);
  // initialized com port flag
  init_com=1;
  delay(1000); //attendo accensione arduino prima di inviare richieste  
}
