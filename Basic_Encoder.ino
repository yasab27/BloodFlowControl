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

#define PUMPPIN 9    //peristaltic pump control pin, connect to arduino digital pin 9
int power;//power level of pump, between 90 and 180



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
  myservo.attach(PUMPPIN);
  power = 180;

  
 Serial.begin (9600);
} 

int counter = 150;
bool prevA = 1, prevB = 1;
  
void loop() {
  bool A = digitalRead(outputA), B = digitalRead(outputB);

  if (B != prevB) counter += (B-prevB) * (A ? +1 : -1);
  else if (A != prevA) counter += (A-prevA) * (B ? -1 : +1);
  else return; //nothing changed: exit

  
  prevA = A;
  prevB = B;

  // PRINTING THE VALUES OF THE COUNTER VARIABLE TO THE MONITOR 
  Serial.println(counter);
  Serial.print("Position: ");
  Serial.println(counter);
  lcd.setCursor(0, 1);
  lcd.clear();
  lcd.setCursor(8,1);


  power = counter;
  lcd.print(power);

  myservo.write(power); 
}
