/*
  OpenWheels Graphics
  by Mirco Segatello 2012
*/

void InitNumberBox()
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


void InitButton()
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


void InitSlider()
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


void graphGauge()
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
  text(nfp(Angle,1,1) + "°", posX+20, posY-Diameter/2-20 );
  // Draw the gauges indicator. Will display the Angle
  strokeWeight(3);
  float Angle_radians = radians(Angle)-HALF_PI;
  line( posX, posY, posX+(60*cos(Angle_radians)), posY+(60*sin(Angle_radians)) );  
}  

void graphGrid()
{
  //Draw Grid for Roll Graph
  int posX = 500;
  int posY = 400;
  int spanY = 200;  //same as graphRoll
  textSize(20);  
  fill(c_red);
  text("  0°", posX, posY);
  text("+30°", posX, posY-spanY/2);
  text("-30°", posX, posY+spanY/2);
  stroke(c_green);
  strokeWeight(2);  
  line( posX, posY-spanY/2, 10, posY-spanY/2);  
  line( posX, posY+spanY/2, 10, posY+spanY/2); 
}

void graphRoll(float inData, int[] graphBuffer, color c_color)
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


void InitComDropdown()
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
  //We also need how many characters there is in a single port name, we´ll store the chars here for counting later.
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

void controlEvent(ControlEvent theEvent) {
  //select com from list 
  if(theEvent.isGroup() && theEvent.name().equals("portComList"))   
  { 
      println("Select portComList"+"   value = "+theEvent.group().value()); // for debugging    
      InitSerial(theEvent.group().value()); // initialize the serial port selected    
  }
}


