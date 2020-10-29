#PFS-173 ROMPLER /  SAMPLE PLAYBACK PLAYER

This project is an example of how to implement a Sample Playback Player based on an I2S PT8211 DAC on a very cheap
Padauk PFS 173 microcontroller

## Installation

You need to have SDCC and EASYPDKPROG installed
 

## Usage
Compile and upload the rompler source code:
```
make all
```

Compile and upload the sinetest source code:
```
make sinetest TODO This does not work atm
```

### Note

The author of the code has some problems using easypdkprog on a macOs machine without specifying the port directly.
You can adjust the PORT of the programmer in the MAKEFILE or simply complete remove it, to let easypdkprog find it
automatically.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change

## TODO - Checklist

- [ ] I removed the sine data from the sample.h, so the sinetest isn't working anymore
- [ ] The Interrupt Timer is intitialised with 16Mhz but my calculations use 8Mhz
- [ ] The Bandgap reference voltage level and IHRC clock speed in the makefile is not used in the code.
- [ ] The sample plays at the falling edge of the button press at the moment
- [ ] Reduce the sample size to fit in 3kW code memory
- [ ] Refactor the code for more readability and clearance
- [ ] Improve the Readme

### Optional:
- [ ] Additional Feature: Potentiometer for Sample Start Position
- [ ] Additional Feature: Potentiometer for Sample Length Reduction
- [ ] Make a YouTube Video to demonstrate the project and maybe inspire someone to do something with it