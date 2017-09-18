/* PersonalRobot APP
   basato su ES12 del corso Processing Android

   Controllo da Android via BT di PersonalRobot
   Per smartphone con display FHD 1920x1080
   testato su samsung S4 con Bytes_Android.ino

   BT DAGU DG010 configurato con nome "Arduino"
   la comunicazione avviene alla velocità impostata sul modulo BT
 */

import android.content.Intent;
import android.os.Bundle;
import ketai.net.bluetooth.*;
import ketai.ui.*;
import ketai.net.*;


PFont fontMy;
KetaiBluetooth bt;
int status = 0;
String send, received = "...";
char input;
int dataX, dataY;

// dimension and position of the sqare joystick
float softReturn = 0.1;  //speed of return in position    
int joyPosX;     // X position of the joystick
int joyPosY;     // Y position of the joystick
int joyWide;     // dimension of sqare
int joyRadStk;   // radius of stick
int stickPosX = 0;
int stickPosY = 0;

long actualTime, previousTime=0;
long pollingTime = 500;  //time to send/receive data

// variable for data received
//String command="";
byte[] receivedByte={};
float IBatLevel;
int Drive;
int statusFlag;
float VBatLevel;
int Steer;
int encoderPos;

// name of the BT devices to connect
String BTname = "PRobot";

// The following code is required to enable bluetooth at startup.
void onCreate(Bundle savedInstanceState) {
  super.onCreate(savedInstanceState);
  bt = new KetaiBluetooth(this);
}

void onActivityResult(int requestCode, int resultCode, Intent data) {
  bt.onActivityResult(requestCode, resultCode, data);
}


void setup() 
{
  orientation(PORTRAIT);  //verticale
  background(0);  //uso display nero
  joyPosX   = width/2;       // X position of the joystick
  joyPosY   = 200+height/2;  // Y position of the joystick
  joyWide   = int(width/4);  
  joyRadStk = int(width/4);
  // start BT connections
  bt.start();
  // start discover devices
  bt.discoverDevices();
}


void draw() 
{
  background(0);  //uso display nero 
  textSize(60);
  fill(200); //colore del testo 

  if (status==0)  //sto cercando Arduino (is discovering)
  {
         text("Discovering devices...", 5, 100);         
         ArrayList<String> names;
         names = bt.getDiscoveredDeviceNames();
         for (int i=0; i < names.size(); i++)
         {
            String device=names.get(i);
            println("["+i+"] " + device);    
            if (device.equals(BTname))  //in elenco c'è Arduino
            {
              bt.connectToDeviceByName(BTname); //si connette al dispositivo selezionato
              status=2;
            }        
         } 
     if (!bt.isDiscovering())  //ho cercato ma non ho trovato
     {
         status=1;
     }          
  }
    
  if (status==1)
  {
     text(BTname + " no Discover!", 5, 100);    
  }  
  
  if (status==2)
  { 
     text("Try to connect " + BTname + "...", 5, 100);  
     if (bt.connectToDeviceByName(BTname))
     {
        status=3;        
     }    
  }
   
  if (status==3)
  {
      // sei ancora connesso? verifica qui 
      if (bt.connectToDeviceByName(BTname))
            text("Connected to " + BTname, 5, 100);
      else
            text("Connection lost!", 5, 100);            

      DisplayData();
      DrawJoystick();
      // display stick position 
      text("(" + stickPosX + "," + stickPosY + ")", 30, height-50);
      
      if(mousePressed) 
      {   
         // lo stick si muove solo se il dito è entro il joystick
         if (  abs(mouseX-joyPosX)< joyWide && abs(mouseY-joyPosY) < joyWide  )
         {
             //il dito è entro il joystick!          
             stickPosX = mouseX-joyPosX;
             stickPosY = mouseY-joyPosY;     
         }
         else
         {
            //se il dito è fuori il joystick lo stick ritorna in posizione
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

    
      // Send Data    
      actualTime=millis();
      if (actualTime > previousTime + pollingTime)
      {
          dataX = int(map(stickPosX, -joyWide, joyWide, 0, 255));
          dataY = int(map(stickPosY, -joyWide, joyWide, 0, 255));     
          byte[] data = {'D',byte(dataX),byte(dataY)};  //,'\r','\n'};    // \r\n solo se si vuole vada a capo su arduino serialmonitor
          bt.broadcast(data);  
          previousTime = actualTime;
      }
  }
}  

void DisplayData()
{
    textSize(60);
    fill(220); //colore del testo
    int posY=300;
    text("Drive: " + Drive, 10, posY);
    text("Steer: " + Steer, 10, posY+50);
    text("Position: " + encoderPos, 10, posY+100);
    text("VBat: " + VBatLevel, 10, posY+150);
    text("IBat: " + IBatLevel, 10, posY+200);
    text("Status: " + statusFlag, 10 , posY+250);     
}  

void DrawJoystick()
{
    noStroke();
    //stroke(255);        // Sets the color used to draw borders around shapes
    rectMode(RADIUS);     // Set to RADIUS mode
    fill(0,120,120);      // Set color of backgroud of joystick  
    rect(joyPosX, joyPosY, joyWide, joyWide, width/8);
    //  fill(0,50,50);    // Set color of center of joystick
    //  ellipse(joyPosX, joyPosY, joyRadInt, joyRadInt);  // Draw center of joystick  
    stroke(255);          // Sets the color used to draw borders around shapes (255=white or RGB)
    strokeWeight(4);      // Sets the width of the stroke used for the border around shapes    
    ellipseMode(CENTER);  // Set ellipseMode to CENTER
    fill(255,51,0);       // Set color of the stick
    ellipse(stickPosX+joyPosX, stickPosY+joyPosY, joyRadStk, joyRadStk);  // Draw stick
}

//Funzione richiamata quando arrivano dei byte da Arduino
void onBluetoothDataEvent(String who, byte[] data) 
{
    int temp;
    if (status==3)
    {      
/*        received += new String(data);  //le stringhe vengono accodate (conversione da array di byte a stringa)
        println(received); //for debug
        //received = new String(data);     //stringa ricevuta (non è detto sia tutta) (conversione da array di byte a stringa)
        //clean if string to long
        if (received.length() > 175)
        received = "Message to long!";  
*/             
        
for (int i=0; i<data.length; i++)
  receivedByte = append(receivedByte, data[i]);
  
  if (receivedByte.length == 13) //tutto arrivato
  {
    //for (int i=0; i<receivedByte.length; i++) print(receivedByte[i]);
    if (receivedByte[0]=='D' )
    {
         // parse receivedByte
         temp = (receivedByte[2]<<8) + (receivedByte[1]&0xFF);   
         IBatLevel = float(temp)/10;
         Drive = (receivedByte[4]<<8) + (receivedByte[3]&0xFF); 
         Drive = int(0.4*Drive); //scalato in % 250->100%         
         statusFlag = (receivedByte[6]<<8) + (receivedByte[5]&0xFF);     
         temp = (receivedByte[8]<<8) + (receivedByte[7]&0xFF);   
         VBatLevel = float(temp)/10;       
         Steer = (receivedByte[10]<<8) + (receivedByte[9]&0xFF);
         encoderPos = (receivedByte[12]<<8) + (receivedByte[11]&0xFF);           
    }
    receivedByte= new byte[0];
  }  

      /* // metodo alternativo per prelevare i dati come stringa
      for(int i=0; i<data.length; i++) 
       {
        command = getCommands((char)data[i], command);
       }
       */
    } 
    
    
}

/*  //metodo alternativo per prelvare tutta la stringa
    //se serve riconvertirla in array di bytes
private String getCommands(char inChar, String command)  
{    
   if(inChar == 'H')  //Input message is complete   
   { 
         println("OK->" + command); // E' arrivata tutta la stringa          
         byte[] theByteArray=command.getBytes();          // converto la stringa in array di byte
         for(int i=0; i< theByteArray.length; i++) print(theByteArray[i]);         //stampo l'array di byte
         command= "";  //azzero la stringa
         println();
    } 
   else 
   {  
      //Input message is NOT complete, keep reading
           command+=inChar;    
   }      
   return command;  
}
*/


