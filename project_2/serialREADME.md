##Mode Data:
To send mode data, write a string to the serial port for form:
```
m#
```
'm' is a character which tells the microcontroler what data the following numbers should be used as. 
The number after the character denotes which mode the microcontroler should use. These are:
    * 0: Spring mode
    * 1: Damper mode
    * 2: Texture mode
    * 3: Wall mode
For example, writing the string: 
```
'm2\n'
``` 
would set the mode to texture mode.
