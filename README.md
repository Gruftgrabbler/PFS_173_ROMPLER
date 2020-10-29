#PFS-173 ROMPLER /  SAMPLE PLAYBACK PLAYER

## Installation

You need to have SDCC and EASYPDKPROG installed
 
'''
TODO Tutorial how to install them
'''

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