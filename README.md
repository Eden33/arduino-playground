# arduino-playground

This project contains several Arduino sketches.

## simple-multi-tasking

Simple multitasking example that uses the main application loop of the arduino.
In each loop we we trigger an update request on the LEDs and the servo connected.

To compile this sketch you must install this Arduino libraries first:
- [Sweeper2 Arduino lib] (https://github.com/Eden33/arduino-lib-sweeper2)
- [Flasher2 Arduino lib] (https://github.com/Eden33/arduino-lib-flasher2)

![simple-multi-tasking_bb.png](simple-multi-tasking/simple-multi-tasking_bb.png)

## timer-multi-tasking-v_1_0

This sketch requires same breadboard setup than sketch timer-multi-tasking-v_1_1. The sketch uses a timer interrupt service routine to update the LEDs and the servo in a 1 millisecond interval. The servo is not updated in case the button connected to pin 2 is pressed. 

To compile this sketch you must install this Arduino libraries first:
- [Sweeper2 Arduino lib] (https://github.com/Eden33/arduino-lib-sweeper2)
- [Flasher2 Arduino lib] (https://github.com/Eden33/arduino-lib-flasher2)

## timer-multi-tasking-v_1_1

This sketch is an extended version of timer-multi-tasking-v_1_0. Instead of preventing the update of the servo a reset is executed each time the button is pressed. This behavior is triggered by an external interrupt handler registered on pin 2.

To compile this sketch you must install this Arduino libraries first:
- [Sweeper2 Arduino lib] (https://github.com/Eden33/arduino-lib-sweeper2)
- [Flasher2 Arduino lib] (https://github.com/Eden33/arduino-lib-flasher2)

![timer-multi-tasking-v_1_1.gif](timer-multi-tasking-v_1_1/timer-multi-tasking-v_1_1.gif)

![timer-multi-tasking-v_1_1.png](timer-multi-tasking-v_1_1/timer-multi-tasking-v_1_1_bb.png)
