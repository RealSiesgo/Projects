#include <Wire.h>
#include <Adafruit_SSD1306.h> // OLED DISPLAY

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); // OLED library

/*
  Program Logic:
    0: None
    1: X
    2: O
*/

// Variables //
byte CurrentTurn = 1; // X gets the first turn

// Constants //
const byte DefaultDelay = 16;

// Pins //
const byte ButtonCapture = 13;
const byte ButtonChange = 12;

// Button Old States //
bool OldButtonCaptureState = false; // OFF (I'll use this later for button held stuff)

byte Positions[10][3] = {
  /*
  CaptureValues:
    0 = No Value (EMPTY SQUARE)
    1 = X        (Captured By X)
    2 = O        (Captured By O)
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
    const byte x = Positions[integer][0];
    const byte y = Positions[integer][1];
    const byte Captured = Positions[integer][2];

    display.setCursor(x, y);

    //Serial.println(String(Captured) + ", iter:" + String(integer)); // Debugging
    
    if (Captured == 1) {
      display.print("X");
    } else if (Captured == 2) {
      display.print("Y");
    } else if (Captured == 0) {
      display.setTextColor(BLACK); // Setting text color to turn off mode (BLACK)
      display.print("Y"); // Turning off pixels for shape Y

      display.setCursor(x, y); // Setting the cursor position again because display.print changed the cursor and we do not want that.
      
      display.print("X"); // Turning off pixels for shape X
      delayMicroseconds(10);

      display.setTextColor(WHITE); // Reverting the text color to its old state (WHITE)
    } else {
      // error for invalid capture number
      display.print("#"); // Error Hash
      Serial.println("[Error] INVALID CAPTURER, EXPECTED [0, 1, 2] RECIEVED: " + String(Captured));
      delay(10000);
      break;
    }
  }
}

void _ForceMove(byte Pos, byte Captured /* 1 || 2 */) { // Internal Function
  Positions[Pos][2] = 0; // Clearing the position
  updateBoard();
  Positions[Pos][2] = Captured; 
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

void loop() {
  //Serial.println(digitalRead(ButtonCapture), OldButtonCaptureState);
  if (digitalRead(ButtonCapture) && !OldButtonCaptureState) {
    OldButtonCaptureState = true; // Booleans are 1, 0 values
    _ForceMove(4, 1);

    Serial.println("ButtonCapture pressed.");
    delay(150);
  } else if (digitalRead(ButtonCapture)) {  // If the button is eld then this code will run
    Serial.println("ButtonCapture held.");h
    _ForceMove(4, 2);
    delay(500);
  } else { // if the button is not held and is not pressed then this code will run
    OldButtonCaptureState = false; // old state is now false
    _ForceMove(4, 0);
  }

  updateBoard();
  display.display(); // UPDATING THE PIXELS ON THE OLED SCREEN

  delay(33); // 60 Hz refresh rate
}
