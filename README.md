Author: Vincent Gijsen

*work in progress!*

# Repo to hold work in progress 

Melbus port, do note, only for HU-850, i wasn't able to sniff other radios...

## HW
* stm32f103 (bluepill)
* BM20 bluetooth module AKA IS2020 from microchip

## Software

* freertos
* libopencm3
* heavily inspired/copy-paste of initial is2020 library from https://github.com/tomaskovacik/IS2020 for structure


## IDE

* stm32workbench

# Working
* bluetooth comms with IS2020
* extract meta from songs running
* event driven updates due to song-end, user-actions on phone, ...
* control of music playback
* console (via STM32f103 usb virtual hu-interface to send HU-keys and get single line display back (tested with SCREEN terminal)



# Todo
* implement melbus logic for initialisation and generating of text-sending reqto HU850
* improve statemachine for 
  * settings menu
  * voice-assistent
  * ..
