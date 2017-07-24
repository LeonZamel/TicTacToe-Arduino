// Set up variables
int selPosition = 0;
int prevSelPosition = 0;

int player = 1;
boolean won = false;

int firstLed = 12;
int sensorPin = A5;
int sensorValue = 0;
int playerLed = 3;
int buttonPin = 2;
int buttonValue = 0;

int grid[3][3] = {
  {0,0,0},
  {0,0,0},
  {0,0,0},
};

// Timers
// One for player
unsigned long lastPollP = 0;
// One for selection
unsigned long lastPollS = 0;

int cycleP = LOW;
int cycleS = LOW;

void switchPlayer() {
  if(player == 1) player = 2;
  else player = 1;
}

int getGridValue(int pos) {
  return grid[pos / 3][pos % 3];
}

void setGridValue(int pos, int value) {
  grid[pos / 3][pos % 3] = value;
}

// Called when a player has won
void win(int led1, int led2, int led3) {
  switchPlayer();
  for(int led = 0; led <= 8; led++) {
    setGridValue(led, 0);
  }
  setGridValue(led1, player);
  setGridValue(led2, player);
  setGridValue(led3, player);
}

void setup() {
  for(int led = (int)(firstLed - 8); led <= firstLed; led++) {
    pinMode(led, OUTPUT);
  }
  pinMode(playerLed, OUTPUT);
  
  pinMode(buttonPin, INPUT);
}

void loop() {
  // Update timers
  if((unsigned long)(millis() - lastPollS) >= 200) {
      cycleS = !cycleS;
      lastPollS = millis();
    }
  if((unsigned long)(millis() - lastPollP) >= 350) {
      cycleP = !cycleP;
      lastPollP = millis();
  }
  // Update player LED
  if(player == 1) {
      digitalWrite(playerLed, HIGH);
  } else {
      digitalWrite(playerLed, cycleP);
  }
  // Update LEDs in the grid
  for(int led = 0; led <= 8; led++) {
    if(getGridValue(led) == 0) {
        digitalWrite(12 - led, LOW);
    } else if(getGridValue(led) == 1) {
        digitalWrite(12 - led, HIGH);
    } else if(getGridValue(led) == 2) {
        digitalWrite(12 - led, cycleP);
    } else if(getGridValue(led) == 3) {
        digitalWrite(12 - led, cycleS);
    }
  }
  
  if(!won) {
    sensorValue = analogRead(sensorPin);
    buttonValue = digitalRead(buttonPin);
  
    prevSelPosition = selPosition;

    // Assign reag value to a position in the grid
    for(int pos = 1; pos <= 9; pos++) {
      if(sensorValue < (int)(((float)pos) / 9.0 * 1023)) {
        selPosition = (int)pos - 1;
        break;
      }
    }

    // Check for button
    if(buttonValue == 1) {
      if(getGridValue(selPosition) != 1 && getGridValue(selPosition) != 2) {
        setGridValue(selPosition, player);
        switchPlayer();
      }
    } else {
      // Chack if selection changed
      if(getGridValue(selPosition) == 0) {
        setGridValue(selPosition, 3);
      }
      if(selPosition != prevSelPosition && getGridValue(prevSelPosition) == 3) {
        setGridValue(prevSelPosition, 0);
      }
    }

    // Check winning conditions
    for(int i = 0; i <= 2; i++) {
      // Horizontal
      if(getGridValue(3*i) == getGridValue(3*i+1) && getGridValue(3*i) == getGridValue(3*i+2) && getGridValue(3*i) != 0) {
        won = true;
        win(3*i, 3*i+1, 3*i+2);
        break;
      }
      
      // Vertical 
      else if(getGridValue(i) == getGridValue(i+3) && getGridValue(i) == getGridValue(i+6) && getGridValue(i) != 0) {
        won = true;
        win(i, i+3, i+6);
        break;
      }
    }

    // Diagonal
    if(!won) {
      // Top-right bottom-left
      if(getGridValue(0) == getGridValue(4) && getGridValue(0) == getGridValue(8) && getGridValue(0) != 0) {
        won = true;
        win(0, 4, 8);
      }
      // Top-left bottom-right
      else if(getGridValue(2) == getGridValue(4) && getGridValue(2) == getGridValue(6) && getGridValue(2) != 0) {
        won = true;
        win(2, 4, 6);
      }
    }
  }
}
