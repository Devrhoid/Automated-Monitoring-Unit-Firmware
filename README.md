# Automated-Monitoring-Unit-Firmware

This repository Contains the C code for the embedded firmware that was loaded onto the units used for the demonstration.

The code flow is as follows:

-Configure Ports
-Initialise RX and TX of USARTO using the Sensor Baud Rate
-Initialise RX and TX of USART1 using the Modem Baud Rate
-Turn on the modem
-Configure the Sensor 
-Continuously Read the sensor and upload the data. 

