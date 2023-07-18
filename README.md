# Garden Monitor

## Description

This project is mainly created to rehabilitate a garden fountain. All system is in a waterproof box, protected of the rain and the wind. The 4 LEDs and the 4x7 Segment display will be visible to maintain the feedback with the user.

## Components used

The components used in this systems are shown below:

- Raspberry Pi Pico W
- 2 x LM2596
- TP4056
- 2 x 3.7 Li rechargeable batteries
- 12V Water Pump
- 12V Submersible LEDs
- Relay
- Blue LED
- Yellow LED
- Red LED
- Green LED
- DHT11 sensor
- IR Receiver
- 74HC595 Shifter module
- 4-7segment display
- Buzzer
- Switch
- Solar Panel

## Performance

The system will be controlled with a IR remote control, but only enabling 3 buttons:

- Power ON/OFF: This button will turn ON/OFF the water pump of the fountain and the sumbmersible LEDs in it.
- Button '1': This button activates the humidity mode, if it was not activated yet, turning ON the blue LED on the box and displaying the ambient humidity (%).
- Button '2': This button activates the temperature mode, if it was not activated yet, turning ON the yellow LED on the box and displaying the temperature (°C). 

## Specifications

## Diagrams

### Block Diagram

![Garden monitor block diagram](/media/blockDiagram.jpeg)

### Flowchart

![Garden monitor flow chart](/media/flowChart.jpeg)

### Connections diagram

![Garden monitor connections diagram](/media/connectionDiagram.jpg)