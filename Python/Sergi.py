#!/usr/bin/env/python3
# Original author: MWS
# Creation date: 2020-06-15
# Purpose: create object Wheelchair with write methods: move() and rotate() to output those signals to the PWM pins on an Arduino MEGA.
'''
Remote user -> Internet -> PC (Python) -> WiFi -> Arduino (C++) -> Sergi (Sabretooth, camera, arm, grip)
The user connects via a web interface (simple password authentication).
After logging in, PC wakes Sergi and user sees the camera view
Commands: Sabretooth - move (slowly, or n steps) back, forwards, rotate (-180..+180)
'''
import serial
import time

def test():
    return 'Sergi'

class Sabretooth:
    def __init__(self):
        return self

    def move(self, steps : int):
        print(f'Moving {abs(steps)} steps ' + 'forwards' if steps>=0 else 'backwards')
        return steps

def main():
    try:
        for i in range(10):
            with serial.Serial('/dev/cu.usbmodem14201', 9800, timeout=1) as ser:
                time.sleep(0.5)   # 0.5 seconds
                ser.write(b'H')   # send the byte-string 'H'
                time.sleep(0.5)   # 0.5 seconds
                ser.write(b'L')   # send the byte-string 'L'
    except OSError as e:
        print(e.strerror)
        if e.errno == 2:
            print('Possible cause: check the Arduino is connected to USB')



if __name__=="__main__":
    main()
