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
  lcd.begin(16, 2);
  lcd.print("Encoder");

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
  
void loop() {
  bool A = digitalRead(outputA), B = digitalRead(outputB);

  if (B != prevB) counter += (B-prevB) * (A ? +1 : -1);
  else if (A != prevA) counter += (A-prevA) * (B ? -1 : +1);
  
  prevA = A;
  prevB = B;

  // PRINTING THE VALUES OF THE COUNTER VARIABLE TO THE MONITOR 
//  Serial.println(counter);
  Serial.print("Position: ");
  Serial.println(counter);
  lcd.setCursor(0, 1);
  lcd.setCursor(8,1);
  
  bps = counter/60.0;
  a = (.001) * bps * 2 * PI;
  if(bps != previousBps)
  {
    lcd.clear();
    outputString = strcat("BPM", counter);
    lcd.print(outputString);
  }

  x = a * millis();//for each run through of the loop, x is reset based on the current time value
  Serial.println(x);
  myservo.write(avgValue + (amplitude * sin(x)));//sets pump power to desired value based on avgValue, amplitude, and time passed   

  previousBps = bps;
}
