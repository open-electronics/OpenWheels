/*
  PersonalRobot Encoders
 */


void initEncoder()
{
    // Initialize Encoders
  pinMode(encoderLeftPinA, INPUT_PULLUP); // turn on pullup resistor
  pinMode(encoderLeftPinB, INPUT_PULLUP); // turn on pullup resistor  
  pinMode(encoderRightPinA, INPUT_PULLUP); // turn on pullup resistor
  pinMode(encoderRightPinB, INPUT_PULLUP); // turn on pullup resistor   
  attachInterrupt(0, EncoderLeft, FALLING); // encoder pin 2 interrupt 0 (ENC1) 
  attachInterrupt(1, EncoderRight, FALLING); // encoder pin 3 interrupt 1 (ENC2)
}


void EstimatePos()
{
  // FOR EMG30 motors with 360CounterWheelsTurn
  //encoderPos = 2 * (encoderCounterLeft+encoderCounterRight);  // DEG ENG30
  //encoderDif = 2 * (encoderCounterLeft-encoderCounterRight);  // DEG EMG30
  
  // FOR EW360CPR motor with 25000 CounterWheelsTurn
  encoderPos =  (encoderCounterLeft+encoderCounterRight) / 70;  // DEG EW360CPR
  encoderDif =  (encoderCounterLeft-encoderCounterRight) / 70;  // DEG EW360CPR  
  
  wheelsPos = float(encoderPos);  // position wheels DEG
  wheelsVel = 0.95 * wheelsVel + 0.05 * float(encoderPos-encoderPosPre)/dt; //velocity wheels DEG/sec filtered 
  encoderPosPre = encoderPos;
}

void EncoderLeft()
{
  // If pinA and pinB are both high or both low, it is spinning
  // forward. If they're different, it's going backward.
/*
//     if( (PINL & 0b00000001) == (PINE & 0b00010000) ) //PINL - The Port L Input Pins Register - read only (bit0 PL0)
//if (digitalRead(encoderLeftPinA) == digitalRead(encoderLeftPinB))
        encoderCounterLeft--;
     else
        encoderCounterLeft++;
*/        
  
     if(PINL & 0b00000001)  //PINL - The Port L Input Pins Register - read only (bit0 PL0)
        encoderCounterLeft--;
     else
        encoderCounterLeft++;
} 

void EncoderRight()
{
  // If pinA and pinB are both high or both low, it is spinning
  // forward. If they're different, it's going backward.
     if(PINL & 0b00000010)  //PINL - The Port L Input Pins Register - read only (bit1 PL1)
        encoderCounterRight++;
     else
        encoderCounterRight--;  
}

