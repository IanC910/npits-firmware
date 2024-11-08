Capstone Company 10 Embedded

Setup to be done on the Pi:

For I2C:
$ sudo raspi-config -> enable i2c
$ sudo apt install i2c-tools
$ sudo apt install libi2c-dev

For GPIO:
$ sudo apt install gpiod
$ sudo apt install libgpiod2
$ sudo apt install libgpiod-dev