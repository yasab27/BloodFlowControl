// DEFING VARIABLES FROM RGB LCD LIBRARY TO OUTPUT INFORMATION TO SCREENS
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// DEFINING OUTPUTS USED BY THE ROTARY ENCODER 
#define outputA 6
#define outputB 7
#define buttonInput 8
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
  // Configuring the rotary encoder pin to read 
  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);

  // Setting pin 13 as just a 5 V power supply since all other power sources on the ARDUINO are being used
  pinMode (13, OUTPUT);
  digitalWrite(13, HIGH);

  // Configuring initial LCD Screen output
  lcd.begin(0, 0);
  lcd.print("NBPM:");

  // Setting up button input button
  pinMode(buttonInput, INPUT_PULLUP);
  
  // Configuring pump level
  myservo.attach(PUMPPIN);//initializes peristaltic control pin
  bps = 1.5; //user sets beats per second
  a = (.001) * bps * 2 * PI;//initializes constant using bps
  x = a * millis();//initializes x
  avgValue = 135;//initializes avgValue
  amplitude = 180-avgValue;//initializes amplitude

  
 Serial.begin (9600);
} 

int counter = 60;
bool prevA = 1, prevB = 1;

int pushEvent; // digital input value from pin 13. Due to the Pull up resistor maintaing a constant 5 V in the pin , a value of LOW means 
               // the button is currently being pushed as the button's closing offers a path to ground where as HIGH means the button has offered no additional path for the voltage and
               // the reading is stuck at 5V. 
int statusMemory; // This integer stores the value of the pin 100 seconds after the pushEvent. 
int mode = 1; // This integer corresponds to the mode of the system. If the mode reads 1, then that means that the green Light is no and red light is off and if the mode reads 0, that means 
              // the green light is off and the red light is on. 

int currentBPM = 100;
void loop() {



  // We start by reading the pushEvent on every loop. If the user does not push the button, the pin will read HIGH, and if the user pushes the button, then the pin will read LOW 
  pushEvent = digitalRead(buttonInput);// read the push button value

  // Here we check if the user pushed the pin (pushEvent == Low) and also that if the user is not holding down the pin via the statusMemory. As the statusMemory will hold information on the
  // state of the digital input from the last loop iteration, if it reads HIGH, then that means that a few seconds ago the pin was not being pushed and therefore this change to the state is novel and should
  // be recorded. 
  if(pushEvent == LOW && statusMemory == HIGH){
//    mode = 1 - mode; // Flip the value of the mode from 1 to 0 if it was 1 before and from 0 to 1 if it was 0 before. This toggles whether the red or green light is on. 
//    delay(100); // Delay the loop for 100 ms. This gives time for the user to lift their finger from the button. 
  }    

  // Store the value of the digitalInput pushEvent 100 seconds after the pushing. If this reads 1, this indicates the user has succesfully let go of the button. 
  statusMemory = pushEvent;

  // If the circuit is in mode 1, the greenLight should be turne don and the red light will be turned off. 
  if(pushEvent == LOW){
    Serial.println("Mode 1");
    lcd.clear();
    currentBPM = counter;
  }else{
    // If the circuit is is not in mode 1, meaning it is in mode 0, then turn off the green light and turn on the re light instead. 
    Serial.println("Mode 2");
  }     

  bool A = digitalRead(outputA), B = digitalRead(outputB);

  if (B != prevB) counter += (B-prevB) * (A ? +1 : -1);
  else if (A != prevA) counter += (A-prevA) * (B ? -1 : +1);
  
  prevA = A;
  prevB = B;

  // PRINTING THE VALUES OF THE COUNTER VARIABLE TO THE MONITOR 
  //  Serial.println(counter);
  // Serial.print("Position: ");
  // Serial.println(counter);

  
  bps = currentBPM/60.0;
  a = (.001) * bps * 2 * PI;
  if(bps != previousBps)
  {
    lcd.setCursor(0, 0);
    lcd.print("NBPM:");

    lcd.setCursor(5,0);
    //  lcd.clear();
    //  outputString = strcat("BPM", counter);
    lcd.print(counter);

    if(counter < 100)
    { 
      lcd.setCursor(7,0);
      lcd.print(" ");
    }
    
    lcd.setCursor(8,0);
    lcd.print("CBPM:");
    lcd.setCursor(13,0);
    lcd.print(currentBPM);
    
  }

  x = a * millis();//for each run through of the loop, x is reset based on the current time value
  Serial.println(x);
  myservo.write(avgValue + (amplitude * sin(x)));//sets pump power to desired value based on avgValue, amplitude, and time passed   

  previousBps = bps;

}
