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

## timer-multi-tasking-v-1_1

This sketch is an extended version of timer-multi-tasking-v_1_0. Instead of preventing the update of the servo a reset is executed each time the button is pressed. This behavior is triggered by an external interrupt handler registered on pin 2.

To compile this sketch you must install this Arduino libraries first:
- [Sweeper2 Arduino lib] (https://github.com/Eden33/arduino-lib-sweeper2)
- [Flasher2 Arduino lib] (https://github.com/Eden33/arduino-lib-flasher2)

![timer-multi-tasking-v_1_1.gif](timer-multi-tasking-v-1_1/timer-multi-tasking-v-1_1.gif)

![timer-multi-tasking-v_1_1.png](timer-multi-tasking-v-1_1/timer-multi-tasking-v-1_1_bb.png)

## distance-measurement-v-1_0

This sketch uses an HC-SR04 to measure the distance to objects in front of it.
The distance is printed to the serial output.

![distance-measurement-v-1_0_bb.png](distance-measurement-v-1_0/distance-measurement-v-1_0_bb.png)

## membran-keypad-v-1_0 

This sketch uses a 4x4 membran keyboard. 
It captures the key pressed and prints it to the serial output.

![distance-measurement-v-1_0_bb.png](membran-keypad-v-1_0/membran-keypad-v-1_0_bb.png)

## temperature-and-humidity-v-1_0

This sketch uses a DHT11 temperature and humidity sensor. It reads in a 2 seconds interval the temperature and humidity from the sensor and prints it to the serial output.

![temperature-and-humidity-v-1_0_bb.png](temperature-and-humidity-v-1_0/temperature-and-humidity-v-1_0_bb.png)

## joystick-v-1_0

This sketch uses a KY-023 joystick. It demonstrates:
- How to read data from the joystick and print it to serial out.
- How to use registers to change the pin mode and write or read to/from the pins.

![joystick-v-1_0_bb.png](joystick-v-1_0/joystick-v-1_0_bb.png)

## infrared-send-receive-v-1_0

This sketch decodes the NEC protocol it receives from a remote control and prints the pressed button to the serial out. The infrared receiver used is a AX-1838HS on a breakout board. To catch the NEC data we use a timer 2 interrupt in CTC mode.

![infrared-send-receive-v-1_0_bb.png](infrared-send-receive-v-1_0/infrared-send-receive-v-1_0_bb.png)

## infrared-send-receive-v-1_1

This sketch is a copy of the sketch you can find in infrared-send-receive-v-1_0 (= approx. 95 % of the code is the same). However, to capture the NEC data we use an external interrupt handler registered on pin 2. Each time the pin changes from HIGH to LOW or vice versa we remember the timing using the state machine.

![infrared-send-receive-v-1_1_bb.png](infrared-send-receive-v-1_1/infrared-send-receive-v-1_1_bb.png)

## led-display-8x8-v-1_0

This sketch uses a MAX7219 and a 8x8 LED matrix. The LED states are pushed via SPI (Serial Peripheral Interface) to the MAX7219 who orchestrates the LEDs.

![led-display-8x8-v-1_0.gif](led-display-8x8-v-1_0/led-display-8x8-v-1_0.gif)

## pull-up-pull-down-resistor-v-1_0

Understand pull-up/pull-down resistors.

Pull down resistor:

![pull-down-external.jpg](pull-up-pull-down-resistor-v-1_0/pull-down-external.jpg)

Pull up resistor:

![pull-up-external.jpg](pull-up-pull-down-resistor-v-1_0/pull-up-external.jpg)

Pull up resistor (use build in resistor part of the pin):

![pull-up-external.jpg](pull-up-pull-down-resistor-v-1_0/pull-up-internal.jpg)
