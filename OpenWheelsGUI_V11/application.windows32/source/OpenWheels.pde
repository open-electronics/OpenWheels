/*
  OpenWheels 
  by Mirco Segatello 2012
*/

import controlP5.*;              //import the Serial, and controlP5 libraries.
import processing.serial.*;

ControlP5 controlP5;             //Define the variable controlP5 as a ControlP5 type.
DropdownList PortsList;          //Define the variable ports as a Dropdownlist.
Serial serial;                   //Define the variable port as a Serial object.
Slider AccSlider;                //Define slider for Acc
Slider GyroSlider;               //Define slider for gyro
Numberbox conf_KP, conf_KI, conf_KD;

String[] comList ;               //A string to hold the ports in.

color c_yellow = color(200, 200, 20);
color c_green = color(30, 120, 30);
color c_red = color(250, 30, 30);
color c_azure = color(170, 180, 180);
color c_blue = color(35, 70, 130);

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

void setup() {
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

void draw() 
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
  text("Acc_Angle:  " + Acc_Angle + "°", 150, 115);
  text("Gyro_Rate: " + Gyro_Rate + "°/sec", 150, 140);
  text("Drive: " + Drive, 150, 165);
  text("Steer: " + Steer , 150, 190);   
  text("BatLevel: " + nf(BatLevel,1,1) + "V", 150, 215); 
  text("Status: " + statusFlag, 150, 240); 
  graphGauge();
  graphGrid();
 
   // call the function that plot the angular of acc, with few screen settings
  graphRoll(float(Acc_Angle), VideoBuffer2, c_red); //xPos, YPos, YSpan  
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
          int P = int(conf_KP.value());        
          int I = int(conf_KI.value());
          int D = int(conf_KD.value());   
          char data[]={0,0,0,0,0} ;
          data[0]='W';
          data[1]=char(P);
          data[2]=char(I);
          data[3]=char(D);
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
