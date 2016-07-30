WiFi A/C Controller
===================

This is a project to control an A/C unit via WiFi. A [Digistump Oak](www.digistump.com/oak) was used to receive commands via WiFi and reproduce them using Infra Red. The air conditioner is an old Frigidaire, which accepts NEC commands.

The SDK for the Oak was built using the [esp-open-sdk](http://github.com/pfalcon/esp-open-sdk) repository, which makes building the toolchain very easy. It is important to note that the `pwm` functions didn't work, which forced the NEC signals to be implemented "manually".

This project ditches the whole Particle cloud implementation by Digistump in favor of a more bare-bones approach. To program the generated firmware, use `esptool` and an Arduino board to communicate with the Oak via Serial, as [this article](http://digistump.com/wiki/oak/tutorials/serialupdate) explains.

The only supporting component needed is an Infra Red LED, plugged to Oak's pins GND and P5.

In order to connect to the WiFi, a file called `wifi_settings.h` defining the macros `WIFI_SSID` and `WIFI_PASSWORD` is expected to exist. 
