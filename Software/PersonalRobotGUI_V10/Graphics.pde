/*
 OpenWheels-GUI V2.0
 Graphic Interface
*/

void   viewStatus()
{
  String errorMessage = "Status: ";
  //the char of 0 index is in left position, last bit is StandBy
  if (error.charAt(0) == '1')   errorMessage +=  "Connect...  ";
  else if (error.charAt(1) == '1')  errorMessage += "---  "; 
  else if (error.charAt(11) == '0')   errorMessage += "StandBy  "; 
  else if (error.charAt(11) == '1')   errorMessage += "GO  ";   
  

  if (error.charAt(10) == '1')   errorMessage += "VBat_under  ";
  if (error.charAt(9) == '1')   errorMessage += "VBat_over  ";
  if (error.charAt(8) == '1')   errorMessage += "IBat_over  ";
  if (error.charAt(7) == '1')   errorMessage += "Steer_error  ";
  if (error.charAt(6) == '1')   errorMessage += "Acc_error  ";
  if (error.charAt(5) == '1')   errorMessage += "Gyro_error  ";
  if (error.charAt(4) == '1')   errorMessage += "Angle_error  ";
  if (error.charAt(3) == '1')   errorMessage += "EEPROM_error  ";
  if (error.charAt(2) == '1')   errorMessage += "Speed_error  ";   
 
  
  text(errorMessage, 20, DisplayHeight-50);
}


void InitNumberBox()
{
  // Initialize number box
  conf_KP = controlP5.addNumberbox("KP")
    .setPosition(500,50) 
    .setColorBackground(c_red)    
    .setSize(60,22)
    .setRange(0,255)
    .setDecimalPrecision(0)
   //      .setScrollSensitivity(5.1)//?
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right
 
  conf_KI = controlP5.addNumberbox("KI")
    .setPosition(500,105) 
    .setColorBackground(c_red)    
    .setSize(60,22)
    .setRange(0,255)
    .setDecimalPrecision(0) 
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right
    
   conf_KD = controlP5.addNumberbox("KD")
    .setPosition(500,160) 
    .setColorBackground(c_red)    
    .setSize(60,22)
    .setRange(0,255)
    .setDecimalPrecision(0)
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right

   conf_STEER = controlP5.addNumberbox("STEER")
    .setPosition(500,215) 
    .setColorBackground(c_red)    
    .setSize(60,22)
    .setRange(0,255)
    .setDecimalPrecision(0)
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right 
    
  conf_KO = controlP5.addNumberbox("KO")
    .setPosition(500,270) 
    .setColorBackground(c_red)    
    .setSize(60,22)
    .setRange(0,255)
    .setDecimalPrecision(0)
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right
 
   conf_KV = controlP5.addNumberbox("KV")
    .setPosition(500,325) 
    .setColorBackground(c_red)    
    .setSize(60,22)
    .setRange(0,255)
    .setDecimalPrecision(0) 
    .setMultiplier(1) // set the sensitifity of the numberbox
    .setDirection(Controller.HORIZONTAL); // change the control direction to left/right   
}


void InitButton()
{
  // Set zero acc e gyro
  controlP5.addButton("OFFSET")
    // .setValue(0)
     .setPosition(380,50)
     .setSize(90,22)
     .setColorBackground(c_blue)
     ;  
     
  // Read PID parameter
  controlP5.addButton("READ")
     .setPosition(380,105)
     .setSize(90,22)
     .setColorBackground(c_blue)     
     ;       

  // Write PID parameter
  controlP5.addButton("WRITE")
     .setPosition(380,160)
     .setSize(90,22)
     .setColorBackground(c_blue)     
     ;       
     
  // Save parameter
  controlP5.addButton("SAVE")
     .setPosition(380,215)
     .setSize(90,22)
     .setColorBackground(c_blue)     
     ;   

  // Reset alam error
  controlP5.addButton("RESET")
     .setPosition(380,270)
     .setSize(90,22)
     .setColorBackground(c_blue)     
     ;       
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
    ellipse(stickPosX+joyPosX, -stickPosY+joyPosY, joyRadStk, joyRadStk);  // Draw stick
}


void InitComDropdown()
{
  // Initialize portCommList 
  int posX = 20;
  int posY = 70;
  PortsList = controlP5.addDropdownList("portComList",posX,posY,120,84);    
  //Set the background color of the list (you wont see this though).
  PortsList.setBackgroundColor(color(200));
  //Set the height of each item when the list is opened.
  PortsList.setItemHeight(22);
  //Set the height of the bar itself.
  PortsList.setBarHeight(22);
  //Set the lable of the bar when nothing is selected.
  PortsList.captionLabel().set("COM port");
  //Set the top margin of the lable.
  PortsList.captionLabel().style().marginTop = 3;
  //Set the left margin of the lable.
  PortsList.captionLabel().style().marginLeft = 3;
  //Set the top margin of the value selected.
  PortsList.valueLabel().style().marginTop = 3;
  //Store the Serial ports in the string comList (char array).
  comList = serial.list();
  
  println(comList);
  if (comList.length!=0)
  { 
    //We need to know how many ports there are, to know how many items to add to the list, so we will convert it to a String object (part of a class).
    String comlist = join(comList, ",");
    //We also need how many characters there is in a single port name, we´ll store the chars here for counting later.
    String COMlist = comList[0];
    //Here we count the length of each port name.
    int size2 = COMlist.length();
    //Now we can count how many ports there are, well that is count how many chars there are, so we will divide by the amount of chars per port name.
    int size1 = comlist.length() / size2;
    //Now well add the ports to the list, we use a for loop for that. How many items is determined by the value of size1.
    for(int i=0; i< size1; i++)  //con dispositivo BT errore perchè troppe porte
//    for(int i=0; i< 7; i++) //se sono disponibili più di 9 porte
    {
      //This is the line doing the actual adding of items, we use the current loop we are in to determin what place in the char array to access and what item number to add it as.
      PortsList.addItem(comList[i],i);
    }
  }  
  //Set the color of the background of the items and the bar.
  PortsList.setColorBackground(color(60));
  //Set the color of the item your mouse is hovering over.
  PortsList.setColorActive(color(255,128));
}

void controlEvent(ControlEvent theEvent) {
  //select com from list 
  if(theEvent.isGroup() && theEvent.name().equals("portComList"))   
  { 
      //println("Select portComList"+"   value = "+theEvent.group().value()); // for debugging    
      InitSerial(theEvent.group().value()); // initialize the serial port selected    
  }
}


