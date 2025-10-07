# Game Timer

## Game Timer for 2 to 4 players.
* Set player count and timer interval at startup.
* Randomizer picks who is first to play.

## Hardware
This is built on the [Lilygo T-Encoder-Pro](https://github.com/Xinyuan-LilyGO/T-Encoder-Pro/tree/arduino-esp32-libs_V2.0.14). 

## Build
* Gnu Makefile
  * make Build_Serial
    * There are other options but that is the only one needed here.
    * You may have to edit the Makefile to change the COM3 port to your configuration.
* Compiles with arduino-cli
    * Install Espressif ESP32 platform libraries.
    * All other libraries needed are included in this project.
* Files are included for Visual SlickEdit for editing, compiling, and loading.
