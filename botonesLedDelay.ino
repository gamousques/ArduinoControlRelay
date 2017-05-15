
// First we'll set up constants for the pin numbers.
// This will make it easier to follow the code below.

long previousMillis = 0;        // will store last time LED was updated
long interval = 60000;           // interval at which to blink (milliseconds) 2 mins


#define CH1 8   // Connect Digital Pin 8 on Arduino to CH1 on Relay Module
#define CH2 7   // Connect Digital Pin 8 on Arduino to CH2 on Relay Module


const int UPbuttonPin = 2;  // pushbutton 1 pin
const int DOWNbuttonPin = 3;  // pushbutton 2 pin
const int STOPbuttonPin = 4;  // pushbutton 3 pin

const int WorkingLedPin =  12;    // LED pin

const int UP = 1;
const int DOWN = 2;
const int STOP = 3;

bool UPStatus = false;
bool DOWNStatus = false;
bool STOPStatus = false;
bool WORKStatus = false;

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
  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);
  digitalWrite(CH1, LOW);  
  digitalWrite(CH2, LOW);  

  //Turn OFF any power to the Relay channels
  Serial.println("High Relays OFF");
  digitalWrite(CH1,HIGH);
  digitalWrite(CH2,HIGH);


  //Set buttons
  byte i;

  Serial.print("Button checker with ");
  Serial.print(NUMBUTTONS, DEC);
  Serial.println(" buttons");
  // Make input & enable pull-up resistors on switch pins
  for (i=0; i< NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
//    digitalWrite(buttons[i], HIGH);
  }

  //Test WORKING LED 
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
    SetRelayDown(true);
    SetWorkLed(true);
    WORKStatus = true;
       
    UPStatus = false;
    STOPStatus = false;
    ResetCountDown();
    
  } else if (UPStatus == true && WORKStatus == false)
  {
    Serial.println("En UP" );  
    SetRelayUp(true); 
    SetWorkLed(true);
    WORKStatus = true;  

    DOWNStatus = false;
    STOPStatus = false;
    ResetCountDown();
        
  } else if (STOPStatus == true && WORKStatus == true)
  {
    Serial.println("En STOP" ); 
    ResetCountDown(); 
    Reset();
  }

  if(CheckCountDown() == true && WORKStatus == true)
  {
    Serial.println("Timer Off" );  
    Reset();
  }

}

void Reset()
{
    SetRelayUp(false);
    SetRelayDown(false);  
    SetWorkLed(false);
    DOWNStatus = false;
    UPStatus = false;
    WORKStatus = false;
    STOPStatus = false;
}
void ResetCountDown()
{

  previousMillis =millis();
}

bool CheckCountDown()
{
  bool retVal = false;
  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
    retVal = true;
 
  }   
     Serial.print("TIMER ");
    Serial.println((currentMillis - previousMillis));
  return retVal;
}

void SetWorkLed(bool st)
{
  if(st == true)
  {
      digitalWrite(WorkingLedPin, HIGH);  // turn the LED on

  } else {
      digitalWrite(WorkingLedPin, LOW);  // turn the LED off

  }

}

void SetRelayUp(bool st)
{
  if(st == true)
  {
     digitalWrite(CH2, LOW);   //Relay ON 
  } else {
     digitalWrite(CH2, HIGH);  //Relay off
  }
}

void SetRelayDown(bool st)
{
  if(st == true)
  {
     digitalWrite(CH1, LOW);   //Relay ON
       
  } else {
     digitalWrite(CH1, HIGH);  //Relay off
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
  
   /*
  int button1State, button2State , button3State;  // variables to hold the pushbutton states

  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);

  if ((button1State == LOW) && (DOWN == false)){
       UP = true;
       digitalWrite(ledVerdePin, HIGH);  // turn the LED on
       delay(6000);
       digitalWrite(ledVerdePin, LOW);  // turn the LED off
       UP = false;
  } else if ((button2State == LOW) && (UP == false)){
       DOWN = true;
       digitalWrite(ledRojoPin, HIGH);  // turn the LED on
       delay(6000);
       digitalWrite(ledRojoPin, LOW);  // turn the LED off
       DOWN = false;
  } else if ((button2State == LOW) ){
       DOWN = false;
       UP = false;
  }
 

  // Don't forget that we use = when we're assigning a value,
  // and use == when we're testing a value for equivalence.
}
*/
