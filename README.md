# ArduiNao-RMM
Robots can express emotions for better Human Robot Interaction. In this field, the Nao robot is a platform widely used. This robot mainly expresses emotions by gestures and colored led eyes, but due to its white flat and inanimate face it cannot express facial expressions. In this work we propose a pluggable eyebrows device with two separated degrees of freedom.
# PCB
NAO robot has a USB port behind the head of the robot opening a possibility to connect the robot with external hardware. The Arduino-based PCB is designed with the dimension of 46mm x 15mm with a USB connector and headers to connect two servo motors. The gerber files were generated using the SparkFun's CAM file settings.
# Arduino 
A firmware is uploaded to the ATmega328P microcontroller of the PCB which manages the data received from NAO robot via the USB communication. The data is then translated into PWM values to control the positions of two servo motors. The firmware is programmed
using serial and servo libraries of Arduino.
