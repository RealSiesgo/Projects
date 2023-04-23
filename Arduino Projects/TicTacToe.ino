#include <Wire.h>
#include <Adafruit_SSD1306.h> // OLED DISPLAY

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); // OLED library

/*
  Program Logic:
    0: None
    1: X
    2: O
    3: Selection
*/

// Variables //
byte CurrentTurn = 1; // X gets the first turn
byte SelectedPos = 0;
bool PosFlashed = false;

// Constants //
const byte DefaultDelay = 16;

// Pins //
const byte ButtonCapture = 13;
const byte ButtonChange = 12;

// Button Old States //
bool OldButtonChangeState = false; // OFF (I'll use this later for button held stuff)

byte Board[10][3] = {
  /*
  CaptureValues:
    0 = No Value  (EMPTY SQUARE)
    1 = X         (Captured By X)
    2 = O         (Captured By O)
    3 = Selection (To show currently selected slot)
  */

  //Data: {byte_ Xpos, byte_ Ypos, byte_ CaptureValue}
  
  {8,  1, 0}, {28,  1, 0}, {48,  1, 0},
  {8, 13, 0}, {28, 13, 0}, {48, 13, 0},
  {8, 25, 0}, {28, 25, 0}, {48, 25, 0},
};

void DisplayCleanUp() {
  display.setTextColor(WHITE);
  display.setCursor(1, 1);
  display.clearDisplay();
  display.setTextSize(1);
}

void drawBoard() {
  display.drawFastVLine(20, 0, 32, WHITE);
  display.drawFastVLine(40, 0, 32, WHITE);
  display.drawFastHLine(2, 10, 56, WHITE);
  display.drawFastHLine(2, 22, 56, WHITE);
};


void updateBoard() {
  for (int integer = 0; integer <= 9; integer++) { // Updating the board with the current position data
    const byte x = Board[integer][0];
    const byte y = Board[integer][1];
    const byte Captured = Board[integer][2];

    display.setCursor(x, y);

    //Serial.println(String(Captured) + ", iter:" + String(integer)); // Debugging
    if (Captured == 0) {
      display.setTextColor(BLACK); // Setting text color to turn off mode (BLACK)
      display.print("Y"); // Turning off pixels for shape Y

      display.setCursor(x, y); // Setting the cursor position again because display.print changed the cursor and we do not want that.
      
      display.print("X"); // Turning off pixels for shape X
      delayMicroseconds(10);

      display.setTextColor(WHITE); // Reverting the text color to its old state (WHITE)
    }else if (Captured == 1) {
      display.print("X");
    } else if (Captured == 2) {
      display.print("O");
    } else if (Captured == 3) {
      display.print("%");
    } else {
      // error for invalid capture number
      display.print("#"); // Error Hash
      Serial.println("[Error] INVALID CAPTURER, EXPECTED [0, 1, 2, 3] RECIEVED: " + String(Captured));
      delay(10000);
      break;
    }
  }
}

void EndGame(byte State) {
  _ClearSelections();

  display.setTextColor(WHITE);
  display.setCursor(66, 8);
  display.setTextSize(1);

  if (State == 0) {
    display.setCursor(66, 13);
    //Serial.println("GAMESTATE: Draw");
    display.print("Draw");
  } else if (State == 1) {
    //Serial.println("X won");
    display.print("Winner:");
    display.setCursor(84, 18);
    display.print("X");
  } else if (State == 2) {
    //Serial.println("O won");
    display.print("Winner:");
    display.setCursor(84, 18);
    display.print("O");
  }
  
  display.display();

  delay(9e6);
}

void _ClearSlot(byte Pos) {
  Board[Pos][2] = 0;
}

void _ClearSelections() {
  for (byte i = 0; i < 9; i++) {
    if (Board[i][2] == 3) {
      display.setTextColor(BLACK);
      display.setCursor(Board[i][0], Board[i][1]);
      display.print("%");
      Board[i][2] = 0;

      display.setTextColor(WHITE);
    }
  }
}

void _SelectPos(byte Pos) {
  Serial.println(String("Data: ") + Board[Pos][2] + String(", for pos: ") + Pos);
  _ClearSelections();

  if (Board[Pos][2] != 0) {
    Serial.println("NOT EMPTY");
    Serial.println(Board[Pos][2]);
    for (int i = Pos; i < 9; i++) {
      if (Board[i][2] == 0) {
        Serial.println(String("changed to pos" ) + Pos + String(", ") + i);
        Pos = i;
        break;
      }
    }

    if (Board[Pos][2] != 0) {
      for (int i = 0; i < 9; i++) {
        if (Board[i][2] == 0) {
          Serial.println(String("changed to pos" ) + Pos + String(", ") + i);
          Pos = i;
          break;
        }
      }
    }

    if (Board[Pos][2] != 0) {
      EndGame(0); // no slots left, its a draw.
    }
  }

  if (Pos > 8) {
    Pos = 0;
  };
  
  display.setCursor(Board[Pos][0], Board[Pos][1]);
  display.print("%");
  Board[Pos][2] = 3;

  SelectedPos = Pos;
}

void _ForceMove(byte Pos, byte Captured /* 1 || 2 */) { // Internal Function
  _ClearSlot(Pos); // Clearing the slot
  updateBoard();
  Board[Pos][2] = Captured; 
}

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(ButtonCapture, INPUT);
  pinMode(ButtonChange, INPUT);

  DisplayCleanUp();

  Serial.println("Writing to OLED.");

  display.setTextSize(2);
  display.println("TicTacToe!"); // BootUp Text
  display.println("SyntaxMenace"); // Author
  Serial.println("Sent: Connected!"); // debugging
  display.display();
  delay(1000);

  DisplayCleanUp();

  drawBoard();
  display.display();
}

void CheckGameStatus() {
  for (byte Check = 1; Check <= 2; Check++) {
    byte LineLength = 0;

    for (byte y = 0; y <= 6; y+=3) { // Horizontal && Vertical Check

      for (byte x = y; x <= (y + 2); x++) {
        String temp = String("y: valy, y: valx, data: valdat"); // DEBUG
        temp.replace("valy", String(y));
        temp.replace("valx", String(x));
        temp.replace("valdat", String(Board[x][2]));
        if (Board[x][2] == Check) {
          LineLength++;
        }
      }

      //Serial.println(String("Length ") + String(Check) + ": " + LineLength);
    }

    if (LineLength == 3) {
      Serial.println(Check + String(" won because horizontal or vertical"));
      EndGame(Check);
    }
    
    LineLength = 0;

    for (byte pos = 0; pos <= 8; pos += 4) { // diagonal check type: ⟍ (left to right)
      if (Board[pos][2] == Check) {
        LineLength++;
        //Serial.println(Check + String(" diag ltr found at:") + pos);
      }
    }

    if (LineLength == 3) {
      EndGame(Check);
    }

    LineLength = 0;

    for (byte pos = 2; pos <= 6; pos += 2) { // diagonal check type: ⟋ (right to left)
      if (Board[pos][2] == Check) {
        LineLength++;
        //Serial.println(Check + String(" diag rtl found at:") + pos);
      }
    }

    if (LineLength == 3) {
      EndGame(Check);
    }

    LineLength = 0;

  }
}

void loop() {
  CheckGameStatus();

  const bool ButtonChangeState = digitalRead(ButtonChange);
  const bool ButtonCaptureState = digitalRead(ButtonCapture);


  if (ButtonChangeState && !OldButtonChangeState) {
    _SelectPos(SelectedPos + 1);

    Serial.println(String("Changed Pos") + SelectedPos);

    delay(150);
  } else if (ButtonChangeState) {  // If the change position button is held then this code will run
    _SelectPos(SelectedPos + 1);

    Serial.println(String("Held Changed Pos") + SelectedPos);

    delay(300);
  } else if (ButtonCaptureState) {
    _ForceMove(SelectedPos, CurrentTurn);
    
    if (CurrentTurn == 2) {
      CurrentTurn = 1;
    } else {
      CurrentTurn = 2;
    }

    for (byte i = 0; i < 9; i++) {
      if (Board[i][2] == 0) { // finding the first empty slot
        SelectedPos = i; // set the selected position to the first empty slot
        break;
      }
    }

    do {delay(100);}while (digitalRead(ButtonCapture));
  } else {
    // if no button is pressed we display the selected position to the users
    _SelectPos(SelectedPos);
  }

  OldButtonChangeState = ButtonChangeState; // Setting the old capture state after we finish with the button control

  updateBoard();
  display.display(); // UPDATING THE PIXELS ON THE OLED SCREEN

  delay(16); // refresh rate
}
