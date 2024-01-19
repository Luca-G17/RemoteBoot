# Remote Boot Client-Server System
This repo contains my code for my remote boot system. An ESP32-VROOM32 bridges the power pins on my PC with a transistor. The gate of this transistor is connected to pin 16 and is pulled to ground with a $10K\ohm$ resistor. A TCP server is running on the ESP32 waiting for a connection from the client program, this client program can be run remotely from the Raspberry Pi. The Raspberry Pi can be accessed from the internet via a domain name provided by a Dynamic DNS service. Once the PC is turned on it can be accessed remotely using the Raspberry Pi as an SSH proxy.

Remote Terminal $\rightarrow$ Raspberry Pi Client $\rightarrow$ ESP32 Server

## OS Switch Integration
This system also contains an integration with my existing operating system switch, which will switch between two dual boot operating systems depending on the position of a physical switch. The ESP32 has a second transistor in parallel with the switch, the gate of this transistor is connected to pin 21 on the ESP32 allowing it to control which operating system is selected.
