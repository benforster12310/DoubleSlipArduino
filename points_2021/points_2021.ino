// define how long to turn the relays on for in milliseconds
const int ON_TIME = 100;
// define the pins array
// 2-5 = [2=TOPLEFTBUTTON][3=TOPRIGHTBUTTON][4=BOTTOMLEFTBUTTON][5=BOTTOMRIGHTBUTTON]
// 6-9 = [6=LEFTPOINTMOVEIN][7=LEFTPOINTMOVEOUT][8=RIGHTPOINTMOVEIN][9=RIGHTPOINTMOVEOUT]
int pins[2][8] = {
    {2,3,4,5,6,7,8,9},
    {10,11,12,14,15,16,17,18}
};
// define the pointPositions array
// 0 = IN, 1 = OUT
// [1=LEFTPOINT][2=RIGHTPOINT]
int pointPositions[2][2] = {
    {0,0},
    {0,0}
};
// then define the buttonState array
//[1=LEFT[0=none][1=top][2=bottom]][2=RIGHT[0=none][1=top][2=bottom]]
int buttonState[2][2] = {
    {0,0},
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
    }
    Serial.println("Delaying Startup By 3s To Allow CDU To Charge Before Demanding Point Movement");
    delay(3000);
    // initialise the points in position: OUT
    for(int outerPins = 0; outerPins < (sizeof(pins) / sizeof(pins[0])); outerPins++) {
        movePoint(outerPins, 0, 0);
        movePoint(outerPins, 1, 0);
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
    pointPositions[point][side] = position;
    Serial.print("THE POINT:");
    Serial.print(point);
    Serial.print(", SIDE:");
    Serial.print(side);
    Serial.println(" RELAY HAS BEEN RELEASED");
    // then set the correct buttonState side to 0
    buttonState[point][side] = 0;
    Serial.print("THE BUTTON STATE OF THE LEFT SIDE IS: ");
    Serial.print(buttonState[point][0]);
    Serial.print(", THE BUTTON STATE OF THE RIGHT SIDE IS: ");
    Serial.println(buttonState[point][1]);    
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
        Serial.print("POINT ALREADY SET ");
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
        // then delay by 10
        delay(10);
    }   
}
