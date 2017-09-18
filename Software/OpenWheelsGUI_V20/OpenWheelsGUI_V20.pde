/*
 Openwheels-GUI V2.0
 
 gestione di tutte le funzioni da GUI
 nella versione V1.1 non sono state implementate tutte le funzioni. 
 aggiunto ritardo prima di inviare richiesta dati
 visualizzazione accelerometro con scala +-1g
 visualizzazione giroscopio con scala in DEG/SEC
 avviare GUI dopo Arduino
 implementato controllo errori in ricezione
 
 
 by Mirco Segatello 
 Processing Ver2.2.1 
 date: june 2015
  
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License Creative Commons  BY-NC-SA
 as published by the Free Software Foundation, see <http://creativecommons.org/licenses/by-nc-sa/3.0/>    
*/

import controlP5.*;              //import the Serial, and controlP5 libraries.
import processing.serial.*;
import javax.swing.ImageIcon;    // for ICON BAR image
 
ControlP5 controlP5;             //Define the variable controlP5 as a ControlP5 type.
DropdownList PortsList;          //Define the variable ports as a Dropdownlist.
Serial serial;                   //Define the variable port as a Serial object.
Slider AccSlider;                //Define slider for Acc
Slider GyroSlider;               //Define slider for gyro
Numberbox conf_KP, conf_KI, conf_KD, conf_STEER, conf_KO, conf_KV;

String[] comList;                //A string to hold the ports in.

color c_yellow = color(200, 200, 20);
color c_green = color(30, 120, 30);
color c_red = color(250, 30, 30);
color c_azure = color(170, 180, 180);
color c_blue = color(35, 70, 130);

// Variables for displayng data
int[] VideoBuffer1;
int[] VideoBuffer2;
int[] VideoBuffer3;

// call arduino for data every timePolling [msec]
int timePolling = 100;  //100msec=10Hz
float time1;
float time2;
int init_com = 0;

float Acc_Value;
float Angle;
float Gyro_Rate;
int Drive;
int Steer;
int Position;
int statusFlag=0x0400;
String error;                    //The string with bit error
float VBatLevel;
float IBatLevel;

int DisplayWidth = 800; //screenWidth
int DisplayHeight = 600; //screenHeight

boolean RUN=false;
boolean requestPID=false;
boolean writePID=false;
boolean savePID=false;
boolean resetERROR=false;
boolean setOFFSET=false;

void setup() {
  size(800, 600); 
  ImageIcon myicon = new ImageIcon(loadBytes("gear.png"));
  frame.setIconImage(myicon.getImage()); 
  
  frameRate(20);  //Refresh graph rate
  smooth(); // smoothed shapes looks better!

  controlP5 = new ControlP5(this);
  ControlFont fonT = new ControlFont(createFont("Arial bold",15),15); 
  controlP5.setControlFont(fonT); 
  InitComDropdown();  
  InitSlider();
  InitButton();
  InitNumberBox();
  VideoBuffer1 = new int[width];
  VideoBuffer2 = new int[width];
  VideoBuffer3 = new int[width];  
}

void draw() 
{
  background(0);
  // Strings composition
  textFont(createFont("Arial bold",24));
  
  fill(c_red);
  stroke(255);
  text("OpenWheels GUI V2.0", 20, 30);
  textSize(16);
  //textAlign(CENTER);
  fill(c_azure);
  text("Acc:   " + Acc_Value, 180, 65);
  AccSlider.setValue(Acc_Value); 
  text("Gyro: " + Gyro_Rate, 180, 90);
  GyroSlider.setValue(Gyro_Rate);  
  text("Drive: " + int(Drive) + "%", 180, 115);
  text("Steer: " + Steer, 180, 140);   
  text("Pos:   " + Position, 180, 165);
  text("VBatLevel: " + nf(VBatLevel,1,1) + "V", 180, 190);
  text("IBatLevel: " + nf(IBatLevel,1,1) + "A", 180, 215);
  error = binary(statusFlag,12);
  text("Status: " + error, 180, 240);
  viewStatus();
  graphGauge();
  graphGrid();
 
   // call the function that plot the angular of acc, with few screen settings
  graphRoll(Angle, VideoBuffer1, c_yellow);
  graphRoll(Drive, VideoBuffer2, c_red);   
  graphRoll(IBatLevel, VideoBuffer3, c_green); 

  

 
  time1=millis();
  if (init_com==1) 
  {
    if (serial.available() > 0)  
    { 
      processSerialData();
    }    
    else if ((time1-time2) > timePolling) 
    {     
      if (requestPID==true)
      {
        println("request PID");
        requestPID=false;        
        serial.write('E');
      }  
      else if (writePID==true)
      {
        println("write PID");
        writePID=false;    
        byte outByte[] = new byte[7];
        outByte[0] = 'W';
        outByte[1] = byte(conf_KP.value());
        outByte[2] = byte(conf_KI.value());
        outByte[3] = byte(conf_KD.value());
        outByte[4] = byte(conf_STEER.value());     
        outByte[5] = byte(conf_KO.value());
        outByte[6] = byte(conf_KV.value());  
        serial.write(outByte);
      }
      else if (savePID==true)
      {
        println("save PID");
        savePID=false;        
        serial.write('S');     
      }
      else if (resetERROR==true)
      {
        println("reset ERROR");
        resetERROR=false;
        serial.write('C');
      }  
      else if (setOFFSET==true)
      {
        println("set OFFSET");
        setOFFSET=false;
        serial.write('Z');        
      }  
      else 
        serial.write('A');        
      time2=time1;
    }
  }   
}
