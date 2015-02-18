int flipped;
int rawDiff;
int lastRawDiff;
int lastRawPos;
int rawPos;
int lastRawOffset;
int rawOffset;
int lastLastRawPos;
int updatedPos;
int flips;
int offset;


void init_counter(void){
	flipped = 0;
	rawDiff = 0;
	lastRawDiff = 0;
	rawPos = 0;
	lastRawPos = 0;
	rawOffset = 0;
	lastRawOffset = 0;
	lastLastRawPos = 0;
	updatedPos = 0;
	flips = 0;
	offset = 0;
}

int update_pos(int voltage){
	rawPos = voltage;

    rawDiff = rawPos - lastRawPos;          //difference btwn current raw position and last raw position
    lastRawDiff = rawPos - lastLastRawPos;  //difference btwn current raw position and last last raw position
    rawOffset = abs(rawDiff);
    lastRawOffset = abs(lastRawDiff);
        
    // Update position record-keeping vairables
    lastLastRawPos = lastRawPos;
    lastRawPos = rawPos;
          
    // Keep track of flips over 180 degrees
    if((lastRawOffset > 700) && (!flipped)) { // enter this anytime the last offset is greater than the flip threshold AND it has not just flipped
    	if(lastRawDiff > 0) {        // check to see which direction the drive wheel was turning
              flips--;              // cw rotation 
            } 
            else {                     // if(rawDiff < 0)
              flips++;              // ccw rotation
            }
          // check to see if the data was good and the most current offset is above the threshold
        updatedPos = rawPos + flips*800; // update the pos value to account for flips over 180deg using the most current offset 
        offset = rawOffset;
        flipped = 1;        // set boolean so that the next time through the loop won't trigger a flip
    } 
    else {                        // anytime no flip has occurred
        updatedPos = rawPos + flips*800; // need to update pos based on what most recent offset is 
        flipped = 0;
    }
    return updatedPos;
}

