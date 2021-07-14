// Copyright (c) 2021 M. Streeter
// This version did a 3 point turn on 2021-07-10

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

void loop()
{
  flash(10);
  ST.motor(1, 20);  // Go forward at 16.
  ST.motor(2, 16);  // Go forward at 16.
  delay(8000);       // Wait 8 seconds.
  
  ST.motor(1, 0);    // Stop.
  ST.motor(2, 0);    // Stop.
  delay(1000);       // Wait 1 seconds.

  ST.motor(1, -32);  // Go forward at 16.
  ST.motor(2, -16);  // Go backward at 16.
  delay(8000);       // Wait 4 seconds.
  
  ST.motor(1, 0);    // Stop.
  ST.motor(2, 0);    // Stop.

  ST.motor(1, 20);  // Go forward at 16.
  ST.motor(2, 16);  // Go forward at 16.
  delay(8000);       // Wait 8 seconds.

  ST.motor(1, 0);    // Stop.
  ST.motor(2, 0);    // Stop.
  delay(20000);       // Wait 20 seconds.
}
