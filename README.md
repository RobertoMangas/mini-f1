Build of a RC car and its controller from scratch, using scrap and spare pieces like PVC or rim spokes from a bike. Giving a new life to those elements by reusing them.

Both traction and direction systems will provide the car with a movement exactly like any other commercial product. Apart from doing what the user wants,
the project will also collect telemetry in three ways:
- Acceleration over two axes.
- Remaining battery percentage.

This telemetry will be sent and displayed to the user on a OLED display embedded on the controller.
Data is managed by two nRF24L01 transciever modules, one acting as an emitter and the other as a receiver.

Both controller (emitter) and car (receiver) have their own optimized PCB board for all their respectives components and requirements. I soldered all the componets that are part of the PCB boards: resistors, capacitors, switches and headers. 

In terms of components we have:
 - Controller: Arduino NANO Every manages two joysticks; the 164x32 OLED Display and the radio 
 module.
 - Car: Arduino UNO R3 manages the accelerometer (MPU6050 module); servo (steering); DC 12T 
 brushed motor (traction); motor driver (BTS7960) and the radio module.

Software:
 - Arduino IDE 2.2.1 to develop functionalities and algorithms.
 - EasyEDA for the PCB boards design.
