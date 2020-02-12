# arduino-playground

This project contains the following Arduino sketches:
* <a href="#simple-multi-tasking">simple-multi-tasking</a>
* <a href="#timer-multi-tasking-v-1_0">timer-multi-tasking-v-1_0</a>
* <a href="#timer-multi-tasking-v-1_1">timer-multi-tasking-v-1_1</a>
* <a href="#distance-measurement-v-1_0">distance-measurement-v-1_0</a>
* <a href="#membran-keypad-v-1_0">membran-keypad-v-1_0</a>
* <a href="#temperature-and-humidity-v-1_0">temperature-and-humidity-v-1_0</a>
* <a href="#joystick-v-1_0">joystick-v-1_0</a>
* <a href="#infrared-send-receive-v-1_0">infrared-send-receive-v-1_0</a>
* <a href="#infrared-send-receive-v-1_1">infrared-send-receive-v-1_1</a>
* <a href="#led-display-8x8-v-1_0">led-display-8x8-v-1_0</a>
* <a href="#pull-up-pull-down-resistor-v-1_0">pull-up-pull-down-resistor-v-1_0</a>
* <a href="#imu-v-1_0">imu-v-1_0</a>

## simple-multi-tasking

[Link to the code.](./simple-multi-tasking)

Simple multitasking example that uses the main application loop of the arduino.
In each loop we we trigger an update request on the LEDs and the servo connected.

To compile this sketch you must install this Arduino libraries first:
- [Sweeper2 Arduino lib] (https://github.com/Eden33/arduino-lib-sweeper2)
- [Flasher2 Arduino lib] (https://github.com/Eden33/arduino-lib-flasher2)

<img src="./simple-multi-tasking/simple-multi-tasking_bb.png" width="500" alt="simple-multi-tasking_bb.png">

## timer-multi-tasking-v-1_0

[Link to the code.](./timer-multi-tasking-v-1_0)

This sketch uses a timer interrupt service routine to update the LEDs and the servo in a 1 millisecond interval. The servo is not updated in case the button connected to pin 2 is pressed. 

<img src="./timer-multi-tasking-v-1_1/timer-multi-tasking-v-1_1_bb.png" width="500" alt="timer-multi-tasking-v-1_1_bb.png">

To compile this sketch you must install this Arduino libraries first:
- [Sweeper2 Arduino lib] (https://github.com/Eden33/arduino-lib-sweeper2)
- [Flasher2 Arduino lib] (https://github.com/Eden33/arduino-lib-flasher2)

## timer-multi-tasking-v-1_1

[Link to the code.](./timer-multi-tasking-v-1_1)

This sketch is an extended version of <a href="#timer-multi-tasking-v-1_0">timer-multi-tasking-v-1_0</a> and requires same breaboard configuration. Instead of preventing the update of the servo a reset is executed each time the button is pressed. This behavior is triggered by an external interrupt handler registered on pin 2.

![timer-multi-tasking-v_1_1.gif](timer-multi-tasking-v-1_1/timer-multi-tasking-v-1_1.gif)

To compile this sketch you must install this Arduino libraries first:
- [Sweeper2 Arduino lib] (https://github.com/Eden33/arduino-lib-sweeper2)
- [Flasher2 Arduino lib] (https://github.com/Eden33/arduino-lib-flasher2)

## distance-measurement-v-1_0

[Link to the code.](./distance-measurement-v-1_0)

This sketch uses an HC-SR04 to measure the distance to objects in front of it.
The distance is printed to the serial output.

<img src="./distance-measurement-v-1_0/distance-measurement-v-1_0_bb.png" width="500" alt="distance-measurement-v-1_0_bb.png">

## membran-keypad-v-1_0 

[Link to the code.](./membran-keypad-v-1_0)

This sketch uses a 4x4 membran keyboard. 
It captures the key pressed and prints it to the serial output.

<img src="./membran-keypad-v-1_0/membran-keypad-v-1_0_bb.png" width="300" alt="membran-keypad-v-1_0_bb.png">

## temperature-and-humidity-v-1_0

[Link to the code.](./temperature-and-humidity-v-1_0)

This sketch uses a DHT11 temperature and humidity sensor. It reads in a 2 seconds interval the temperature and humidity from the sensor and prints it to the serial output.

<img src="./temperature-and-humidity-v-1_0/temperature-and-humidity-v-1_0_bb.png" width="500" alt="temperature-and-humidity-v-1_0_bb.png">

## joystick-v-1_0

[Link to the code.](./joystick-v-1_0)

This sketch uses a KY-023 joystick. It demonstrates:
- How to read data from the joystick and print it to serial out.
- How to use registers to change the pin mode and write or read to/from the pins.

<img src="./joystick-v-1_0/joystick-v-1_0_bb.png" width="400" alt="joystick-v-1_0_bb.png">

## infrared-send-receive-v-1_0

[Link to the code.](./infrared-send-receive-v-1_0)

This sketch decodes the NEC protocol it receives from a remote control and prints the pressed button to the serial out. The infrared receiver used is a AX-1838HS on a breakout board. To catch the NEC data we use a timer 2 interrupt in CTC mode.

<img src="./infrared-send-receive-v-1_0/infrared-send-receive-v-1_0_bb.png" width="400" alt="infrared-send-receive-v-1_0_bb.png">

## infrared-send-receive-v-1_1

[Link to the code.](./infrared-send-receive-v-1_1)

This sketch is a copy of the sketch you can find in <a href="#infrared-send-receive-v-1_0">infrared-send-receive-v-1_0</a> (= approx. 95 % of the code is the same). However, to capture the NEC data we use an external interrupt handler registered on pin 2. Each time the pin changes from HIGH to LOW or vice versa we remember the timing using the state machine.

<img src="./infrared-send-receive-v-1_1/infrared-send-receive-v-1_1_bb.png" width="400" alt="infrared-send-receive-v-1_1_bb.png">

## led-display-8x8-v-1_0

[Link to the code.](./led-display-8x8-v-1_0)

This sketch uses a MAX7219 and a 8x8 LED matrix. The LED states are pushed via SPI (Serial Peripheral Interface) to the MAX7219 who orchestrates the LEDs.

![led-display-8x8-v-1_0.gif](led-display-8x8-v-1_0/led-display-8x8-v-1_0.gif)

## pull-up-pull-down-resistor-v-1_0

[Link to the code.](./pull-up-pull-down-resistor-v-1_0)

Understand pull-up/pull-down resistors.

Pull down resistor:

<img src="./pull-up-pull-down-resistor-v-1_0/pull-down-external.jpg" width="500" alt="pull-down-external.jpg">

Pull up resistor:

<img src="./pull-up-pull-down-resistor-v-1_0/pull-up-external.jpg" width="500" alt="pull-up-external.jpg">

Pull up resistor (use build in resistor part of the pin):

<img src="./pull-up-pull-down-resistor-v-1_0/pull-up-internal.jpg" width="500" alt="pull-up-internal.jpg">

## imu-v-1_0

[Link to the code.](./imu-v-1_0)

This sketch uses a GY-521 IMU device (= Inertia Measurement Unit) that ships with a gyroscope and accelerometer which are build in sensors (so called MEMS = Micro Elecrical Mechanical Systems) available on the device. The GY-521 uses a standard I2C bus for data transmission. The Arduino is a master and the GY-521 is a slave on that bus. The raw data the GY-521 returns to the Arduino is used to calculate the current orientation of the breadboard (= X- and Y-axis). Last but not least the calculated data is visualized in real time with help of a LCD 1602 module which is also part of the breadboard configuration.

![imu-v-1_0.gif](imu-v-1_0/imu-v-1_0.gif)

<img src="./imu-v-1_0/imu-v-1_0_bb.png" width="400" alt="imu-v-1_0_bb.png">

## pir-v-1_0

[Link to the code.](./pir-v-1_0)

This sketch uses a HC-SR501 PIR sensor (= Passive Infrared Sensor) to detect motion. The current status is displayed on a LCD 1602 module also part of the breadboard configuration. Once the Arduino is powered on 60 seconds are granted to the PIR sensor to bootstrap its functionality (in this timeframe the sensor may output wrong results according to the data sheet; therefore we wait). After that time period "No motion." will be displayed on the LCD display in case no motion is detected. Once motion is detected "Motion detected." will be displayed on the LCD display for 3 seconds. After the 3 seconds the PIR sensor will be idle (= blocks further motion detection) for another 3 seconds which is also displayed on the LCD display indicated by a countdown. Once the idle time is over next motion can be detected.

The PIR configuration required is described with help of images you can find in the sketch.

![pir-v-1_0.gif](pir-v-1_0/pir-v-1_0.gif)

<img src="./pir-v-1_0/pir-v-1_0_bb.png" width="400" alt="pir-v-1_0_bb.png">