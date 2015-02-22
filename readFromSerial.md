#Read from Serial:
To read from board over a serial connection, we must publish values to the serial conectin from the board. On the computer side, we must read these messages. 

##Board:
To print values from the board, we will use the function printf with a string input. An example of this is:
```C
printf("Moror derivative position is: %i ,  command value is: %i \n", derivative, command);
```

##Computer
To receive serial commands from the board, we will use screen. If you don't already have screen, you can install it with 
```bash
sudo apt-get install screen
```
To read vaues back from the board, use the command: 
```bash
sudo screen /dev/ttyUSB0 19200
```
The default baud rate for helloUSB is 19200,  but it is configureable. By default, the board usually shows up as /dev/ttyUSB0. However, if this doesn't work, try looking for the board with the command: 
```bash
dmesg | grep tty
```
