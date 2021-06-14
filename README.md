PIUIO Board Clone
===========

This is an clone of the original Pump It Up USB IO Board. It's fully compatible with the original one.
This uses V-USB Library for an emulated USB interface at ATMEGA328. Other devices may be compatible too.

WARN: This is the SIMPLE branch. This is intented to start users who just want to put hook their DIY pads to original game machines.

Contains files for two types of PIUIO:
 - In the main folder, the normal PIUIO - provides pad input (5 inputs per player)
 - In the button-board folder, code to clone a PIU button board (4 menu buttons and menu button lights per player)


The files in this repo are code for the ATMEGA328, you can load it on an Arduino (or alternatively, a bare ATMEGA328P chip), but you can't do this through the Arduino IDE.
(TODO: Write instructions on how to do this - for now, if you go into the Arduino IDE and enable verbose output for compile/upload, you can find some of the commands you'll need to use for this)




PS: The V-USB folder (a.k.a. usbdrv) is intact and original. Keeping the licenses of it. It is not my property.

