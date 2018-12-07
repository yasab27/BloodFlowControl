// DEFING VARIABLES FROM RGB LCD LIBRARY TO OUTPUT INFORMATION TO SCREENS
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// Define the switch pin
#define switchInput 6

#include <Encoder.h>
Encoder myEnc(2, 3);

//
//// DEFINING OUTPUTS USED BY THE ROTARY ENCODER
//#define outputA 2
//#define outputB 3
#define buttonInput 4
// DEFINING PUMP LIBRARIES AND VARIABLES
#include <Servo.h>

Servo myservo;
float x;//function of time that is fed into the sin function
float a;//constant that converts period of sine wave from 2 PI milliseconds into 1 / bps
float bps;//variable controlling beats per second of the pump
int avgValue;//average power value of the pump, should be between 90 and 180
int amplitude;//amplitude of fluctuation of power level: max power value = avgValue + amplitude (should be 180), min power value = avgValue - amplitude
#define PUMPPIN 9    //peristaltic pump control pin, connect to arduino digital pin 9

int power;//power level of pump, between 90 and 180
int previousBps;

void setup() {
  
  // Setting pin 13 as just a 5 V power supply since all other power sources on the ARDUINO are being used
  pinMode (13, OUTPUT);
  digitalWrite(13, HIGH);

  // Switch input
  pinMode(switchInput, INPUT);

  // Configuring initial LCD Screen output
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("NBPM:");

  // Setting up button input button
  pinMode(buttonInput, INPUT_PULLUP);
  //

  // Configuring pump level
  myservo.attach(PUMPPIN);//initializes peristaltic control pin
  bps = 1.5; //user sets beats per second
  a = (.001) * bps * 2 * PI;//initializes constant using bps
  x = a * millis();//initializes x
  avgValue = 160;//initializes avgValue
  amplitude = 180 - avgValue; //initializes amplitude

  // Beginning the Serial Print
  //Serial.begin (9600);
}

volatile float counter = 60;

///////////////////////////////////////////////////////////////////////////////////////////
int pushEvent; // digital input value from pin 13. Due to the Pull up resistor maintaing a constant 5 V in the pin , a value of LOW means
// the button is currently being pushed as the button's closing offers a path to ground where as HIGH means the button has offered no additional path for the voltage and
// the reading is stuck at 5V.
int statusMemory; // This integer stores the value of the pin 100 seconds after the pushEvent.
int mode = 1; // This integer corresponds to the mode of the system. If the mode reads 1, then that means that the green Light is no and red light is off and if the mode reads 0, that means
// the green light is off and the red light is on.

int currentBPM = 100;
//////////////////////////////////////////////////////////////////////////////////////////////

int previousCounter;

int currentFlowRate;

long oldPosition  = -999;

// Variables to control whether the device is in constant or pulsatile mode
int switchPinState;
int previousPinState;

int previousFlowRate;
void loop() {

     long newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      Serial.println(newPosition);
      counter = newPosition;
    }
  // Read and print the current switch state
  if (digitalRead(switchInput) != previousPinState) {
    // SET DEFAULT VALUES FOR CHANGE
    counter = 20;
    switchPinState = digitalRead(switchInput);
    Serial.println(switchPinState);
    lcd.clear();
  }
  previousPinState = switchPinState;

  if (switchPinState == 1)
  {

    // Read the value of the encoder 


    
    // PULSATILE FLOW
    // We start by reading the pushEvent on every loop. If the user does not push the button, the pin will read HIGH, and if the user pushes the button, then the pin will read LOW
    pushEvent = digitalRead(buttonInput);// read the push button value

    // If the circuit is in mode 1, the greenLight should be turne don and the red light will be turned off.
    if (pushEvent == LOW) {
      lcd.clear();
      currentBPM = counter;

      lcd.setCursor(0, 0);
      lcd.print("New BPM:");

      lcd.setCursor(8, 0);
      //  lcd.clear();
      //  outputString = strcat("BPM", counter);
      lcd.print(int(counter));

      if (counter < 100)
      {
        lcd.setCursor(10, 0);
        lcd.print(" ");
      }

      lcd.setCursor(0, 1);
      lcd.print("Cur. BPM:");
      lcd.setCursor(9, 1);
      lcd.print(currentBPM);
      //      Serial.println(currentBPM);
    }


    bps = currentBPM / 60.0;
//    Serial.println(bps);
    a = (0.001) * bps * 2 * PI;
    if (counter != previousCounter)
    {
      lcd.setCursor(0, 0);
      lcd.print("New BPM:");

      lcd.setCursor(8, 0);
      //  lcd.clear();
      //  outputString = strcat("BPM", counter);
      lcd.print(int(counter));

      if (counter < 100)
      {
         lcd.setCursor(10, 0);
         lcd.print(" ");
      }

      lcd.setCursor(0, 1);
      lcd.print("Cur. BPM:");
      lcd.setCursor(9, 1);
      lcd.print(currentBPM);


    }

    x = a * millis();//for each run through of the loop, x is reset based on the current time value
    Serial.println(avgValue + (amplitude * sin(x)));
    myservo.write(avgValue + (amplitude * sin(x)));//sets pump power to desired value based on avgValue, amplitude, and time passed

    previousCounter = counter;
  } else {

    // We start by reading the pushEvent on every loop. If the user does not push the button, the pin will read HIGH, and if the user pushes the button, then the pin will read LOW
    pushEvent = digitalRead(buttonInput);// read the push button value
    int currentFlowMinute;
    // If the circuit is in mode 1, the greenLight should be turne don and the red light will be turned off.
    if (pushEvent == LOW) {
      lcd.clear();
      currentFlowMinute = counter;
      currentFlowRate = (counter/60.0 + .7999) / .008018;

      lcd.setCursor(0, 0);
      lcd.print("New FLR:");

      lcd.setCursor(8, 0);
      lcd.print(int(counter));

      if (counter < 100)
      {
        lcd.setCursor(10, 0);
        lcd.print(" ");
      }

      lcd.setCursor(0, 1);
      lcd.print("Cur FLR:");
      lcd.setCursor(8, 1);
      lcd.print(int(currentFlowMinute));
    }

    if (((counter/60.0 + .7999) / .008018) != previousFlowRate)
    {
      lcd.setCursor(0, 0);
      lcd.print("New FLR:");

      lcd.setCursor(8, 0);
      //  lcd.clear();
      //  outputString = strcat("BPM", counter);
      lcd.print(int(counter));

      if (counter < 100)
      {
        lcd.setCursor(10, 0);
        lcd.print(" ");
      }

      lcd.setCursor(0,1);
      lcd.print("Cur FLR:");
      lcd.setCursor(8, 1);
      lcd.print(int(currentFlowMinute));

      lcd.setCursor(12,0);
      lcd.print("mL/m");

      lcd.setCursor(12,1);
      lcd.print("mL/m");
    }

    myservo.write(currentFlowRate);   //set motor to speed

    previousFlowRate = currentFlowRate;
  }

}
