##Mode Data:
To send mode data, write a string to the serial port for form:
```m#```
'm' is a character which tells the microcontroler what data the following numbers should be used as. 
The number after the character denotes which mode the microcontroler should use. These are:
    -0: Spring mode
    -1: Damper mode
    -2: Texture mode
    -3: Wall mode
For example, writing the string: 
```'m2\n'``` 
would set the mode to texture mode.

##Setting Data
There are multiple settings in the different modes which the user can change! These include:
* Setting the spring constant for the spring mode
* Setting the amount of damping for damper mode
* Setting the texture for texture mode
* Setting the bounds in which a wall occurs in wall mode
To define each of these, follow the relevent serial protocal directives presented in the sections below. Each of these values has a defult value the system will use on startup. This default value will be used until a new one is defined and sent by the user.

#Spring mode user changeable settings
In spring mode, the spring constant can be defined by the user. To change this, write a 's' character followed by the decired spring constant. Finally, end the transmission with a new line character. An example transmission of: 
```
's123\n'
```
would set the spring constant to be 123. 

#Damper mode user changeable settings
In damper mode, the amount of damping can be changed. To change the amount of damping, write a 'd' character followed by the amount of damping the user desired and finally end the serial transmission by writing a new line character. An example transmission of: 
```
'd287\n'
```
Would set the amount of damping to 287.

#Texture mode user changeable settings
In texture mode, the texture is user configureable. To change this, write a 't' followed by the desired texture represented as elements in a binary array. Finally, close the transmission with a new line character. The texture should be shorter than 12 elements long. An example of re-defining the texture is:
```
't1100101\n'
```
In this example, the texture would be high, high, low, low, high, low, high, then loop back around. 

Aditionally, the magnitude of the texture can be audjusted by the user. To change this write an 'n' followed by the desired magnitude value, concluded by a new line character. For example, 
```
'n60\n'
```
would set the magnitude of damping used  on the texture to be 60.

#Wall mode user changeable settings
In wall mode, the position of the walls can be written. To change this, write a 'w' character followed by the values of the wall position desired. Each value should be three digits long. Aditionally, the order in which these should be written is: smllerLowerAngle, largerLowerAngle, smallerHigherAngle, largerHigherAngle. Because of this order, the values should always increase in value. If the two middle values are the same, or largerLowerAngle is larger than smallerHigherAngle, the system will always be in wall mode. In some cases, the user may want this, so there is no safe ga\uard against this occuring. An example transmission is:
```
'w000010070090\n'
```
This would create "walls" between 0 and 10 degrees and between 70 and 90 degrees. 

Aditionally, the magnitude of force used to create the walls can be changed. To change this, write an 'f' character followed by the desired value and a new line character. An example transmission: 
```
'f65\n'
```
would set the magnitude of the force used to create the walls to 65. 

