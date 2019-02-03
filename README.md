# LockMate

LockMate is a NFC door lock that was designed with an Arduino Uno, a 
standard Arduino expansion board, a servo motor, an Adafruit PN532 NFC 
breakout board, and a 16x2 RGB LCD screen.

# Install instructions

1. Stack expansion board on top of Arduino, and then place PN532 on top 
of expansion board

2. Connect LCD screen to I2C port on expansion board

3. Connect servo motor as shown:

[//]: # (TODO: Add annotated image)

4. Flash .ino sketch to board

5. Enjoy!

# Side notes

If you look over the code, you'll notice we used the Stepper library. 
That doesn't mean you should connect a stepper motor. When building this 
project, please make sure you use a servo.
