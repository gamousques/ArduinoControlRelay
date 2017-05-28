
// Set up constants for the pin numbers.

// Initialize Timer vvalues
long previousMillis = 0;        // will store last time LED was updated
long interval = 50000;           // interval at which to blink (milliseconds) 2 mins

//Set Pin definiton

//Pin for LED 
const int WorkingLedPin =  5;    // LED pin

//Pins for Relay
const int  CHRelay1 = 7;  // Connect Digital Pin 8 on Arduino to CHRelay1 on Relay Module
const int  CHRelay2 = 8; // Connect Digital Pin 8 on Arduino to CHRelay2 on Relay Module


//Pin for buttons 
const int UPbuttonPin = 2;  // pushbutton 1 pin
const int DOWNbuttonPin = 3;  // pushbutton 2 pin
const int STOPbuttonPin = 4;  // pushbutton 3 pin

// Buttons names
const int UP = 1;
const int DOWN = 2;
const int STOP = 3;

// Buttons push indicators
bool UPStatus = false;
bool DOWNStatus = false;
bool STOPStatus = false;
bool WORKStatus = false;

// Deboubce definitions 
#define DEBOUNCE 3  // how many ms to debounce, 5+ ms is usually plenty
 
//define the buttons that we'll use.
byte buttons[] = {UPbuttonPin, DOWNbuttonPin, STOPbuttonPin}; 
int buttonsName[] = {UP, DOWN, STOP};  

//determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
 
//track if a button is just pressed, just released, or 'currently pressed' 
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
byte previous_keystate[NUMBUTTONS], current_keystate[NUMBUTTONS];
 

void setup()
{
  Serial.begin(9600); //set up serial port
  
  // Set up the LED pin to be an output:
  pinMode(WorkingLedPin, OUTPUT);   

  //Set relays
  pinMode(CHRelay1, OUTPUT);
  pinMode(CHRelay2, OUTPUT);
  digitalWrite(CHRelay1, LOW);  
  digitalWrite(CHRelay2, LOW);  

  //Turn OFF any power to the Relay channels
  Serial.println("High Relays OFF");
  digitalWrite(CHRelay1,HIGH);
  digitalWrite(CHRelay2,HIGH);


  //Set buttons
  byte i;

  Serial.print("Button checker with ");
  Serial.print(NUMBUTTONS, DEC);
  Serial.println(" buttons");
  // Make input & enable pull-up resistors on switch pins
  for (i=0; i< NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    previous_keystate[i] = current_keystate[i] = HIGH;
//    digitalWrite(buttons[i], HIGH);
  }

  //Test WORK Led 
  digitalWrite(WorkingLedPin, HIGH);  // turn the LED on
  delay(3000);
  digitalWrite(WorkingLedPin, LOW);  // turn the LED off


  Serial.println("Setup ok");
}


void loop()
{
  byte thisSwitch = thisSwitch_justPressed();


  switch (buttonsName[thisSwitch])
  {  
  case UP: 
    Serial.println("switch just pressed UP" );  
    UPStatus = true;
    break;
  case DOWN: 
    Serial.println("switch just pressed DOWN");    
    DOWNStatus = true;
    break;
  case STOP: 
    Serial.println("switch just pressed STOP");
    STOPStatus = true;
    UPStatus = false;
    DOWNStatus = false;
    break; 
  default:

    break;
  }

  if (DOWNStatus == true && WORKStatus == false) 
  {
    Serial.println("En DOWN" );  
    setRelayDown(true);
    setWorkLed(true);
    WORKStatus = true;
       
    UPStatus = false;
    STOPStatus = false;
    resetCountDown();
    
  } else if (UPStatus == true && WORKStatus == false)
  {
    Serial.println("En UP" );  
    setRelayUp(true); 
    setWorkLed(true);
    WORKStatus = true;  

    DOWNStatus = false;
    STOPStatus = false;
    resetCountDown();
        
  } else if (STOPStatus == true && WORKStatus == true)
  {
    Serial.println("En STOP" ); 
    resetCountDown(); 
    reset();
  }

  if(checkCountDown() == true && WORKStatus == true)
  {
    Serial.println("Timer Off" );  
    reset();
  }


}

void reset()
{
    setRelayUp(false);
    setRelayDown(false);  
    setWorkLed(false);
    DOWNStatus = false;
    UPStatus = false;
    WORKStatus = false;
    STOPStatus = false;
}

void resetCountDown()
{

  previousMillis =millis();
}

bool checkCountDown()
{
  bool retVal = false;
  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
    retVal = true;
 
  }   

  return retVal;
}

void setWorkLed(bool st)
{
  if(st == true)
  {
      digitalWrite(WorkingLedPin, HIGH);  // turn the LED on
  } else {
      digitalWrite(WorkingLedPin, LOW);  // turn the LED off
  }

}

void setRelayUp(bool st)
{
  if(st == true)
  {
     digitalWrite(CHRelay2, LOW);   //Relay ON 
  } else {
     digitalWrite(CHRelay2, HIGH);  //Relay off
  }
}

void setRelayDown(bool st)
{
  if(st == true)
  {
     digitalWrite(CHRelay1, LOW);   //Relay ON
       
  } else {
     digitalWrite(CHRelay1, HIGH);  //Relay off
  }

}

void check_switches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;
  if (millis() < lasttime) {
    // we wrapped around, lets just try again
    lasttime = millis();
  }
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       //when we start, we clear out the "just" indicators
    justreleased[index] = 0;
    currentstate[index] = digitalRead(buttons[index]);   //read the button
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        justreleased[index] = 1; // just released
      }
      pressed[index] = !currentstate[index];  //remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index]; //keep a running tally of the buttons
  }
}


byte thisSwitch_justPressed() {
  byte thisSwitch = 255;
  check_switches();  //check the switches &amp; get the current state
  for (byte i = 0; i < NUMBUTTONS; i++) {
    current_keystate[i]=justpressed[i];
    if (current_keystate[i] != previous_keystate[i]) {
      if (current_keystate[i]) thisSwitch=i;
    }
    previous_keystate[i]=current_keystate[i];
  }  
  return thisSwitch;
}
  

