/*
 PersonalRobotGUI_V10
 
 visualizzazione dei soli dati essenziali
 gestione movimento robot
 
 
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
Numberbox conf_KP, conf_KI, conf_KD, conf_STEER, conf_KO, conf_KV;

String[] comList;                //A string to hold the ports in.

color c_yellow = color(200, 200, 20);
color c_green = color(30, 120, 30);
color c_red = color(250, 30, 30);
color c_azure = color(170, 180, 180);
color c_blue = color(35, 70, 130);


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

int DisplayWidth = 600; //screenWidth
int DisplayHeight = 600; //screenHeight

boolean RUN=false;
boolean requestPID=false;
boolean writePID=false;
boolean savePID=false;
boolean resetERROR=false;
boolean setOFFSET=false;

// dimension and position of the sqare joystick
float softReturn = 0.1;  //speed of return in position    
int joyPosX;     // X position of the joystick
int joyPosY;     // Y position of the joystick
int joyWide;     // dimension of sqare
int joyRadStk;   // radius of stick
int stickPosX = 0;
int stickPosY = 0;

void setup() {
  size(DisplayWidth, DisplayHeight); 
  ImageIcon myicon = new ImageIcon(loadBytes("gear.png"));
  frame.setIconImage(myicon.getImage());   
//  frameRate(20);  //Refresh graph rate
  smooth(); // smoothed shapes looks better!

  controlP5 = new ControlP5(this);
  ControlFont fonT = new ControlFont(createFont("Arial bold",15),15); 
  controlP5.setControlFont(fonT); 
  InitComDropdown();  
  InitButton();
  InitNumberBox();
  joyPosX   = DisplayWidth/2;       // X position of the joystick
  joyPosY   = 110+DisplayHeight/2;  // Y position of the joystick
  joyWide   = 100;  
  joyRadStk = 80;
}

void draw() 
{
  background(0);
  textFont(createFont("Arial bold",24));
  
  fill(c_red);
  stroke(255);
  text("PersonalRobot GUI V1.0", 20, 30);
  textSize(16);
  //textAlign(CENTER);
  fill(c_azure); 
  text("Drive: " + int(Drive) + "%", 180, 70);
  text("Steer: " + Steer, 180, 95);   
  text("Pos:   " + Position, 180, 120);
  text("VBatLevel: " + nf(VBatLevel,1,1) + "V", 180, 145);
  text("IBatLevel: " + nf(IBatLevel,1,1) + "A", 180, 170);
  error = binary(statusFlag,12);
  viewStatus();
  DrawJoystick();

  if (true)
  {        

      // display stick position 
      text("(" + stickPosX + "," + stickPosY + ")", 20, DisplayHeight-20);
      
      if(mousePressed) 
      {   
         // lo stick si muove solo se il dito Ã¨ entro il joystick
         if (  abs(mouseX-joyPosX)< joyWide && abs(mouseY-joyPosY) < joyWide  )
         {
             //il dito è entro il joystick!          
             stickPosX = mouseX-joyPosX;
             stickPosY = -(mouseY-joyPosY);     
         }
         else
         {
            //se il dito Ã¨ fuori il joystick lo stick ritorna in posizione
            //stickPosX +=  -stickPosX * softReturn;  //BRUTTO EFFETTO!
            //stickPosY +=  -stickPosY * softReturn;
         } 

        /*
            // lo stick si muove ovunque venga posto il dito ma non esce dal joystick
            stickPosX = constrain(mouseX-joyPosX, -joyWide, joyWide);
            stickPosY = constrain(mouseY-joyPosY, -joyWide, joyWide);
        */
      }
      else 
      {
          //se togli il dito lo stick ritorna in posizione
          stickPosX +=  -stickPosX * softReturn;
          stickPosY +=  -stickPosY * softReturn;
      }

 }
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
      { 
        // send joystick data and telemetry request
        byte outByte[] = new byte[3];        
        int  dataX = int(map(stickPosX, -joyWide, joyWide, 0, 255));
        int  dataY = int(map(stickPosY, -joyWide, joyWide, 0, 255));        
        outByte[0] = 'D';
        outByte[1] = byte(dataX);
        outByte[2] = byte(dataY);       
        serial.write(outByte);        
      }  
      time2=time1;
    }
  }   
}
