# Open Weather Station
A Homemade weather station using common components like Arduino, Rasbperry Pi and a set of Sensors. 
The Arduino get measures of all sensors and sends by Serial Port to the Raspberry Pi that saves these measures in a database.
The database is read for Python scripts to build and save charts inside a webserver folder to be shown on a Webpage.
A bash script is used to take pictures using a webcam periodicaly.
A LAMP server was used in this project.

## Sensors
- Temperature and Humidity (DHT22)
- Temperature and Atmospheric Pressure (BMP180)
- Luminance (BH1750)
- Light Level (LDR)

![sensor-probe](/images/sensor-probe.jpeg)

Sensor Probe

![arduino-box](/images/arduino-box.jpeg)

Arduino Uno box
