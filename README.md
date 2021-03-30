# Prototype-of-a-neon-sign

Small prototype of implementing a text-based neon sign on the Arduino Mega development platform. Text
should scroll from the bottom of the screen to the top, where the text eventually disappears.
The prototype uses a 128x160 pixel TFT screen connected to the SPI bus.

The text used in the prototype is programmed with a terminal program for the ATMega2560 microcontroller, which
stores the data in the EEPROM.
At startup, the last text entered is displayed.

![Circuit](https://github.com/oskara-ai/Prototype-of-a-neon-sign/blob/main/Images/Kuva2.png)



In the picture is a circuit of the prototype.
