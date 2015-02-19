import processing.core.*; 

import controlP5.*; 
import processing.serial.*; 

import java.applet.*; 
import java.awt.Dimension; 
import java.awt.Frame; 
import java.awt.event.MouseEvent; 
import java.awt.event.KeyEvent; 
import java.awt.event.FocusEvent; 
import java.awt.Image; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class OpenWheels extends PApplet {

/*
  OpenWheels 
  by Mirco Segatello 2012
*/

              //import the Serial, and controlP5 libraries.


ControlP5 controlP5;             //Define the variable controlP5 as a ControlP5 type.
DropdownList PortsList;          //Define the variable ports as a Dropdownlist.
Serial serial;                   //Define the variable port as a Serial object.
Slider AccSlider;                //Define slider for Acc
Slider GyroSlider;               //Define slider for gyro
Numberbox conf_KP, conf_KI, conf_KD;

String[] comList ;               //A string to hold the ports in.

int c_yellow = color(200, 200, 20);
int c_green = color(30, 120, 30);
int c_red = color(250, 30, 30);
int c_azure = color(170, 180, 180);
int c_blue = color(35, 70, 130);

// Variables for displayng data
int[] VideoBuffer1;
int[] VideoBuffer2;

float time1;
float time2;
int init_com = 0;

int Acc_RAW;
int Gyro_RAW;
float Angle;
int Acc_Angle;
int Gyro_Rate;
int Drive;
int Steer;
int statusFlag;
float BatLevel;

//public float conf_KP = 0.0;
int DisplayWidth = 800; //screenWidth
int DisplayHeight = 600; //screenHeight

boolean RUN=false;
boolean requestPID=false;
boolean writePID=false;

public void setup() {
  //Set the size, for this example 120 by 120 px is fine.
  size(DisplayWidth, DisplayHeight);  
  frameRate(20);  //Refresh graph rate
  smooth(); // smoothed shapes looks better!
  //I dont fully understand the library and the thing with this, but monkey see monky do ;) (oh yeah and it needs this line).
  controlP5 = new ControlP5(this);
  InitComDropdown();  
  InitSlider();
  InitButton();
  InitNumberBox();
  VideoBuffer1 = new int[width];
  VideoBuffer2 = new int[width];  
}

public void draw() 
{
  background(0);
  // Strings composition
  textFont(createFont("Arial bold",24));
  fill(c_red);
  stroke(255);
  text("OpenWheels GUI V1.0", 20, 30);
  textSize(16);
  //textAlign(CENTER);
  fill(c_azure);
  text("Acc_RAW:  " + Acc_RAW, 150, 65);
  AccSlider.setValue(Acc_RAW); 
  text("Gyro_RAW: " + Gyro_RAW, 150, 90);
  GyroSlider.setValue(Gyro_RAW);  
  text("Acc_Angle:  " + Acc_Angle + "\u00b0", 150, 115);
  text("Gyro_Rate: " + Gyro_Rate + "\u00b0/sec", 150, 140);
  text("Drive: " + Drive, 150, 165);
  text("Steer: " + Steer , 150, 190);   
  text("BatLevel: " + nf(BatLevel,1,1) + "V", 150, 215); 
  text("Status: " + statusFlag, 150, 240); 
  graphGauge();
  graphGrid();
 
   // call the function that plot the angular of acc, with few screen settings
  graphRoll(PApplet.parseFloat(Acc_Angle), VideoBuffer2, c_red); //xPos, YPos, YSpan  
    // call the function that plot the estimate angular, with few screen settings
  graphRoll(Angle, VideoBuffer1, c_yellow); //xPos, YPos, YSpan 
 
  // call arduino for data every timePolling [msec]
  int timePolling = 50;  //50msec=20Hz
  time1=millis();
  if (init_com==1) {
    while (serial.available() > 0)
     processSerialData();
    if ((time1-time2) > timePolling) {
      if (requestPID==true) { serial.write('E'); requestPID=false;}
      else if (writePID==true) {
          int P = PApplet.parseInt(conf_KP.value());        
          int I = PApplet.parseInt(conf_KI.value());
          int D = PApplet.parseInt(conf_KD.value());   
          char data[]={0,0,0,0,0} ;
          data[0]='W';
          data[1]=PApplet.parseChar(P);
          data[2]=PApplet.parseChar(I);
          data[3]=PApplet.parseChar(D);
          data[4]=' ';
          String str = new String(data);
          serial.write(str);        
          println(str);
          // println(" P=" + P + " I=" + I + " D=" + D);
          writePID=false;}
      //else if (RUN==true ) {serial.write('A');}
      else {serial.write('A');}
      time2=time1;
    }
  }   
}
/*
  OpenWheels Graphics
  by Mirco Segatello 2012
*/

public void InitNumberBox()
{
  // Initialize number box
  conf_KP = controlP5.addNumberbox("KP")
    .setPosition(550,50) 
    .setColorBackground(c_red)    
    .setSize(60,18)
    .setRange(0,250)
   //      .setScrollSensitivity(5.1)//?
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right
 
  conf_KI = controlP5.addNumberbox("KI")
    .setPosition(550,90) 
    .setColorBackground(c_red)    
    .setSize(60,18)
    .setRange(0,250)
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right
    
   conf_KD = controlP5.addNumberbox("KD")
    .setPosition(550,130) 
    .setColorBackground(c_red)    
    .setSize(60,18)
    .setRange(0,250)
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right
 
}


public void InitButton()
{
  // Set zero acc e gyro
  controlP5.addButton("SET_ZERO")
    // .setValue(0)
     .setPosition(450,50)
     .setSize(60,20)
     .setColorBackground(c_blue)
     ;  
     
  // Read PID parameter
  controlP5.addButton("READ_PID")
    // .setValue(0)
     .setPosition(450,90)
     .setSize(60,20)
     .setColorBackground(c_blue)     
     ;       

  // Write PID parameter
  controlP5.addButton("WRITE_PID")
    // .setValue(0)
     .setPosition(450,130)
     .setSize(60,20)
     .setColorBackground(c_blue)     
     ;       
     
  // Write PID parameter
  controlP5.addButton("SAVE_PID")
   //  .setValue(0)
     .setPosition(450,170)
     .setSize(60,20)
     .setColorBackground(c_blue)     
     ;       
/*     
      // START
  controlP5.addButton("START")
     .setPosition(650,50)
     .setSize(60,20)
     .setColorBackground(c_blue)     
     ;  
     
  // STOP
  controlP5.addButton("STOP")
     .setPosition(650,90)
     .setSize(60,20)
     .setColorBackground(c_blue)     
     ;      
*/     
}


public void InitSlider()
{
  // Initialize slider plot
  int posX = 330;
  int posY = 50;
  AccSlider = controlP5.addSlider("Acc_RAW")
     .setPosition(posX,posY)
     .setSize(20,100)
     .setRange(0,1023)
     .setValue(0)     
     .lock() 
     .setColorBackground(c_blue)     
     ;  
  GyroSlider = controlP5.addSlider("Gyro_RAW")
     .setPosition(posX+50,posY)
     .setSize(20,100)
     .setRange(0,1023)
     .setValue(0)     
     .lock() 
     .setColorBackground(c_blue)     
     ;     
}  


public void graphGauge()
{
  // Graph Gauge Indicator
  int posX = 650;
  int posY = 400;
  int Diameter = 120;
  // measure of center
  textSize(26);  
  fill(50);
  ellipse(posX, posY, Diameter, Diameter);    // Draw the gauges
  fill(c_azure);
  text("Angle", posX-Diameter/2, posY-Diameter/2-20);
  text(nfp(Angle,1,1) + "\u00b0", posX+20, posY-Diameter/2-20 );
  // Draw the gauges indicator. Will display the Angle
  strokeWeight(3);
  float Angle_radians = radians(Angle)-HALF_PI;
  line( posX, posY, posX+(60*cos(Angle_radians)), posY+(60*sin(Angle_radians)) );  
}  

public void graphGrid()
{
  //Draw Grid for Roll Graph
  int posX = 500;
  int posY = 400;
  int spanY = 200;  //same as graphRoll
  textSize(20);  
  fill(c_red);
  text("  0\u00b0", posX, posY);
  text("+30\u00b0", posX, posY-spanY/2);
  text("-30\u00b0", posX, posY+spanY/2);
  stroke(c_green);
  strokeWeight(2);  
  line( posX, posY-spanY/2, 10, posY-spanY/2);  
  line( posX, posY+spanY/2, 10, posY+spanY/2); 
}

public void graphRoll(float inData, int[] graphBuffer, int c_color)
{
  int posX = 500;
  int posY = 400;
  int spanY = 200;
  // For each frame the buffer that contains the older samples are shifted by one position
  // Plot in back mode
  for (int i=0; i<posX-1; i++)  
    graphBuffer[i] = graphBuffer[i+1];
  // Store the new entry data
  graphBuffer[posX-1] = (int)map(inData, -30, 30, -spanY/2, spanY/2);  
  stroke(c_color);
  strokeWeight(2);
  // plot the graph that goes from right to left
  for (int x=posX; x>1; x--)
    line( x, posY-graphBuffer[x-1], x+1, posY-graphBuffer[x]);
  noStroke();
}


public void InitComDropdown()
{
  // Initialize portCommList 
  int posX = 20;
  int posY = 70;
  PortsList = controlP5.addDropdownList("portComList",posX,posY,100,84);    
  //Set the background color of the list (you wont see this though).
  PortsList.setBackgroundColor(color(200));
  //Set the height of each item when the list is opened.
  PortsList.setItemHeight(20);
  //Set the height of the bar itself.
  PortsList.setBarHeight(15);
  //Set the lable of the bar when nothing is selected.
  PortsList.captionLabel().set("Select COM port");
  //Set the top margin of the lable.
  PortsList.captionLabel().style().marginTop = 3;
  //Set the left margin of the lable.
  PortsList.captionLabel().style().marginLeft = 3;
  //Set the top margin of the value selected.
  PortsList.valueLabel().style().marginTop = 3;
  //Store the Serial ports in the string comList (char array).
  comList = serial.list();
  //We need to know how many ports there are, to know how many items to add to the list, so we will convert it to a String object (part of a class).
  String comlist = join(comList, ",");
  //We also need how many characters there is in a single port name, we\u00b4ll store the chars here for counting later.
  String COMlist = comList[0];
  //Here we count the length of each port name.
  int size2 = COMlist.length();
  //Now we can count how many ports there are, well that is count how many chars there are, so we will divide by the amount of chars per port name.
  int size1 = comlist.length() / size2;
  //Now well add the ports to the list, we use a for loop for that. How many items is determined by the value of size1.
  for(int i=0; i< size1; i++)
  {
    //This is the line doing the actual adding of items, we use the current loop we are in to determin what place in the char array to access and what item number to add it as.
    PortsList.addItem(comList[i],i);
  }
  //Set the color of the background of the items and the bar.
  PortsList.setColorBackground(color(60));
  //Set the color of the item your mouse is hovering over.
  PortsList.setColorActive(color(255,128));
}

public void controlEvent(ControlEvent theEvent) {
  //select com from list 
  if(theEvent.isGroup() && theEvent.name().equals("portComList"))   
  { 
      println("Select portComList"+"   value = "+theEvent.group().value()); // for debugging    
      InitSerial(theEvent.group().value()); // initialize the serial port selected    
  }
}


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

public void processSerialData() {
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
    Angle = PApplet.parseFloat(AngleInt)/10;
    Acc_Angle = (inBuf[7]<<8) + (inBuf[6]&0xFF);
    Gyro_Rate = (inBuf[9]<<8) + (inBuf[8]&0xff);
    Drive = (inBuf[11]<<8) + (inBuf[10]&0xFF);    
    statusFlag = (inBuf[13]<<8) + (inBuf[12]&0xFF);    
    int BatLevelInt = (inBuf[15]<<8) + (inBuf[14]&0xFF);
    Steer = (inBuf[17]<<8) + (inBuf[16]&0xFF);
    BatLevel = PApplet.parseFloat(BatLevelInt)/10;
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

public void InitSerial(float portValue) 
{
  // initialize the serial port selected in the listBox  
  println("initializing serial " + PApplet.parseInt(portValue) + " in serial.list()"); // for debugging
  // grab the name of the serial port
  String portPos = Serial.list()[PApplet.parseInt(portValue)]; 
  // initialize the port
  serial = new Serial(this, portPos, 115200); 
  // read bytes into a buffer until you get a linefeed (ASCII 10):
  serial.bufferUntil('\n');
  println("done init serial");
  // initialized com port flag
  init_com=1;
}
  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#F0F0F0", "OpenWheels" });
  }
}
