// initialise the timers
unsigned long TIME_NOW = 0;
// define how long to turn the relays on for in milliseconds
const int ON_TIME = 1000;
// initialise the recursionTimes function
int recursionTimes = 0;

// initalise the functions that are not above setup but need to be called from setup
void updatePointPositions();

// define the pins array
// 2-5 = [2=TOPLEFTBUTTON][3=TOPRIGHTBUTTON][4=BOTTOMLEFTBUTTON][5=BOTTOMRIGHTBUTTON]
// 6-9 = [6=LEFTPOINTMOVEIN][7=LEFTPOINTMOVEOUT][8=RIGHTPOINTMOVEIN][9=RIGHTPOINTMOVEOUT]
// 10-13 = [10=LEFTPOINTPOSITIONIN][11=LEFTPOINTPOSITIONOUT][12=RIGHTPOINTPOSITIONIN][13=RIGHTPOINTPOSITIONOUT]
int pins[1][12] = {
    {2,3,4,5,6,7,8,9,10,11,12,13}
};
// define the pointPositions array
// 0 = IN, 1 = OUT
// [1=LEFTPOINT][2=RIGHTPOINT]
int pointPositions[1][2] = {
    {0,0}
};
// then define the buttonState array
//[1=LEFT[0=none][1=top][2=bottom]][2=RIGHT[0=none][1=top][2=bottom]]
int buttonState[1][2] = {
    {0,0}
};
// setup everything
void setup() {
    // then start the serial
    Serial.begin(9600);
    // then go through the pins array
    for(int outerPins = 0; outerPins < (sizeof(pins) / sizeof(pins[0])); outerPins++) {
        // then set 0-3 to input pullup
        pinMode(pins[outerPins][0], INPUT_PULLUP);
        pinMode(pins[outerPins][1], INPUT_PULLUP);
        pinMode(pins[outerPins][2], INPUT_PULLUP);
        pinMode(pins[outerPins][3], INPUT_PULLUP);
        // then set 4-7 to output
        pinMode(pins[outerPins][4], OUTPUT);
        pinMode(pins[outerPins][5], OUTPUT);
        pinMode(pins[outerPins][6], OUTPUT);
        pinMode(pins[outerPins][7], OUTPUT);
        // then set 8-11 to input pullup
        pinMode(pins[outerPins][8], INPUT_PULLUP);
        pinMode(pins[outerPins][9], INPUT_PULLUP);
        pinMode(pins[outerPins][10], INPUT_PULLUP);
        pinMode(pins[outerPins][11], INPUT_PULLUP);
        // then update the point positions
        updatePointPositions(outerPins);
    }
}

// then define the validatPointPositions function and it takes the point id, the side of the point to check, and the expected point position
bool validatePointPositions(int point, int side, int expectedPosition) {
    // then first call the updatePointPositions and wait for it to complete
    updatePointPositions(point);
    // then once that has finished then compare them
    if(pointPositions[point][side] == expectedPosition) {
        // then return true
        return true;
    }
    else {
        // then as an error has occured then return false
        return false;
    }
}

// then define the updatePointPositions function that takes the point id
void updatePointPositions(int point) {
    // then check if anything has changed
    // READ THE LEFT SIDE FIRST
    // check if the point is IN
    if(digitalRead(pins[point][8]) == LOW) {
        // then the left point is IN so set it IN
        pointPositions[point][0] = 0;
        Serial.print("Point: ");
        Serial.print(point);
        Serial.println(", LEFT Side is IN");
    }
    // check if the point is OUT
    if(digitalRead(pins[point][9]) == LOW) {
        // then the left point is OUT so set it OUT
        pointPositions[point][0] = 1;
        Serial.print("Point: ");
        Serial.print(point);
        Serial.println(", LEFT Side is OUT");
    }

    // THEN READ THE RIGHT SIDE
    // check if the point is IN
    if(digitalRead(pins[point][10]) == LOW) {
        // then the right point is IN so set it IN
        pointPositions[point][1] = 0;
        Serial.print("Point: ");
        Serial.print(point);
        Serial.println(", RIGHT Side is IN");
    }
    if(digitalRead(pins[point][11]) == LOW) {
        // the  the right point is OUT so set it OUT
        pointPositions[point][1] = 1;
        Serial.print("Point: ");
        Serial.print(point);
        Serial.println(", RIGHT Side is OUT");
    }
}

// then define the point move function, and it takes the point, the side and the position to move it to
void movePoint(int point, int side, int position) {
    // then get the point and move it
    int index = 0;
    if(side == 0) {
        // LEFT
        if(position == 0) {
            // IN
            index = 4;
            digitalWrite(pins[point][4], HIGH);
        }
        else if(position == 1) {
            // OUT
            index = 5;
            digitalWrite(pins[point][5], HIGH);
        }
    }
    else if(side == 1) {
        // RIGHT
        if(position == 0) {
            // IN
            index = 6;
            digitalWrite(pins[point][6], HIGH);
        }
        else if(position == 1) {
            // OUT
            index = 7;
            digitalWrite(pins[point][7], HIGH);
        }
    }
    Serial.print("Point: ");
    Serial.print(point);
    Serial.print(", Side: ");
    Serial.print(side);
    Serial.print(" Has been moved to the Position: ");
    Serial.println(position);
    // then start the timer
    delay(ON_TIME);
    // then turn the pin off and break from the loop
    digitalWrite(pins[point][index], LOW);
    Serial.print("THE POINT:");
    Serial.print(point);
    Serial.print(", SIDE:");
    Serial.print(side);
    Serial.println(" RELAY HAS BEEN RELEASED");
    // then call the validatePointPositions function and give it the point id, the side, and the expected position
    // to call back with true if the point is set, false if the point isnt set
    bool callBackValue = validatePointPositions(point, side, position);
    if(callBackValue == false) {
        // then as the point has not moved then try again
        Serial.print("THE POINT:");
        Serial.print(point);
        Serial.print(", SIDE:");
        Serial.print(side);
        Serial.print(" HAS NOT MOVED SO IT ");
        if(recursionTimes < 2) {
            Serial.println("IS TRYING AGAIN");
            // then increment the recursion times
            recursionTimes++;
            // then call this function and give it the parameters
            movePoint(point, side, position);
        }
        else {
            Serial.println("CANNOT MOVE AS IT HAS BEEN TRIED 3 TIMES");
            buttonState[point][side] = 0;
            recursionTimes = 0;
        }
    }
    else {
      // then set recursionTimes to 0
      recursionTimes = 0;
      // then set the correct buttonState side to 0
      buttonState[point][side] = 0;
      Serial.print("THE BUTTON STATE OF THE LEFT SIDE IS: ");
      Serial.print(buttonState[point][0]);
      Serial.print(", THE BUTTON STATE OF THE RIGHT SIDE IS: ");
      Serial.println(buttonState[point][1]);
    }
    
}

// then define the check and move point function which takes the point, the side and the desired position
void checkAndMovePoint(int point, int side, int position) {
    // then check if the left side doesn't match
    if(pointPositions[point][side] != position) {
        // then move it
        movePoint(point, side, position);
        Serial.print("Point: ");
        Serial.print(point);
        Serial.print(", Side: ");
        Serial.print(side);
        Serial.print(", Has been requested to move to the Position: ");
        Serial.println(position);
    }
    else {
        Serial.print("POINT ALREADY SET");
        Serial.print("Point: ");
        Serial.print(point);
        Serial.print(", Side: ");
        Serial.print(side);
        Serial.print(", Has been requested to move to the Position: ");
        Serial.println(position);
    }
}

// then define the checkCombinations function and give it the outerPins parameter
void checkCombination(int point) {
    // then check that they are both not zero
    if(buttonState[point][0] != 0 && buttonState[point][1] != 0) {;
        Serial.println("The buttonStates are both not 0");
        int leftPoint = buttonState[point][0];
        int rightPoint = buttonState[point][1];
        // STATE 1 [TOPLEFT & TOPRIGHT][1&&1]
        if(leftPoint == 1 && rightPoint == 1) {
            Serial.println("STATE 1 HAS BEEN SELECTED");
            // then call the checkAndMovePoint function for both sides
            // LEFT OUT
            checkAndMovePoint(point, 0, 1);
            // RIGHT OUT
            checkAndMovePoint(point, 1, 1);
        }
        // STATE 2 [BOTTOMLEFT & BOTTOMRIGHT][2&&2]
        if(leftPoint == 2 && rightPoint == 2) {
            Serial.println("STATE 2 HAS BEEN SELECTED");
            // then call the checkAndMovePoint function for both sides
            // LEFT IN
            checkAndMovePoint(point, 0, 0);
            // RIGHT IN
            checkAndMovePoint(point, 1, 0);
        }
        // STATE 3 [TOPLEFT & BOTTOMRIGHT][1&&2]
        if(leftPoint == 1 && rightPoint == 2) {
            Serial.println("STATE 3 HAS BEEN SELECTED");
            // then call the checkAndMovePoint function for both sides
            // LEFT IN
            checkAndMovePoint(point, 0, 0);
            // RIGHT OUT
            checkAndMovePoint(point, 1, 1);
        }
        // STATE 4 [BOTTOMLEFT & TOPRIGHT][2&&1]
        if(leftPoint == 2 && rightPoint == 1) {
            Serial.println("STATE 4 HAS BEEN SELECTED");
            // then call the checkAndMovePoint function for both sides
            // LEFT OUT
            checkAndMovePoint(point, 0, 1);
            // RIGHT IN
            checkAndMovePoint(point, 1, 0);
        }
        buttonState[point][0] = 0;
        buttonState[point][1] = 0;
    }
    else {
      Serial.println("The buttonStates ARE both 0");
    }
}

// then define the loop
void loop() {
    // then check if any buttons have been pressed so start looping through the pins array
    for(int outerPins = 0; outerPins < (sizeof(pins) / sizeof(pins[0])); outerPins++) {
        // then check the left buttons
        // TOP
        if(digitalRead(pins[outerPins][0]) == LOW) {
            buttonState[outerPins][0] = 1;
            Serial.println("TOP LEFT HAS BEEN PRESSED");
        }
        // BOTTOM
        if(digitalRead(pins[outerPins][2]) == LOW) {
            buttonState[outerPins][0] = 2;
            Serial.println("BOTTOM LEFT HAS BEEN PRESSED");
        }
        // then check the right buttons
        // TOP
        if(digitalRead(pins[outerPins][1]) == LOW) {
            buttonState[outerPins][1] = 1;
            Serial.println("TOP RIGHT HAS BEEN PRESSED");
        }
        // BOTTOM
        if(digitalRead(pins[outerPins][3]) == LOW) {
            buttonState[outerPins][1] = 2;
            Serial.println("BOTTOM RIGHT HAS BEEN PRESSED");
        }
        // then call the checkCombination and pass in the point id
        checkCombination(outerPins);
        // then delay by 1000
        delay(1000);
    }   
}
