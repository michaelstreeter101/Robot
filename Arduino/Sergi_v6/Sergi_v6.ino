// Copyright (c) 2021 M. Streeter
// trying to do a quick circle on 2021-07-15

#include <SabertoothSimplified.h>

SabertoothSimplified ST; // We'll name the Sabertooth object ST.
                         // For how to configure the Sabertooth, see the DIP Switch Wizard for
                         //   http://www.dimensionengineering.com/datasheets/SabertoothDIPWizard/start.htm
                         // Be sure to select Simplified Serial Mode for use with this library.
                         // This sample uses a baud rate of 9600.
                         //
                         // Connections to make:
                         //   Arduino TX->1  ->  Sabertooth S1
                         //   Arduino GND    ->  Sabertooth 0V
                         //   Arduino VIN    ->  Sabertooth 5V (OPTIONAL, if you want the Sabertooth to power the Arduino)
                         //
                         // If you want to use a pin other than TX->1, see the SoftwareSerial example.
                                        
void setup()
{
  SabertoothTXPinSerial.begin(9600); // This is the baud rate you chose with the DIP switches.
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  ST.motor(1, 0);    // Stop. In an emergency,
  ST.motor(2, 0);    // just hit the 'reset' button on the Arduino.
}

void flash( int count)
{
  for(int c=count; c--; c==0){
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(500);                       // wait for a second
  }
}

void stop(){
  ST.motor(1, 0);    // Stop.
  ST.motor(2, 0);    // Stop.  
}

void turnLeft(){
  ST.motor(1, 16);
  ST.motor(2, 36);
  delay(3667);
  stop();  
}

void turnRight(){
  ST.motor(1, 36);
  ST.motor(2, 16);
  delay(3000);
  stop();  
}

void rotateLeft(){
  ST.motor(1, -16);
  ST.motor(2, 16);
  delay(3000);
  stop();    
}

void rotateRight(){
  ST.motor(1, 16);
  ST.motor(2, -16);
  delay(3000);
  stop();    
}

void forward(int d){
  int speed = 32;
  ST.motor(1, speed);
  ST.motor(2, speed*0.93);
  delay(d);
  stop();  
}

void backward(int d){
  int speed = -32;
  ST.motor(1, speed);
  ST.motor(2, speed*0.93);
  delay(d);
  stop();  
}

void loop()
{
  // motor 2 is the left one
  flash(10);
  forward(8500);
  turnLeft();
  forward(3000);
  stop();
  
}
