// Include necessary libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

// Define keys for keypad
const uint8_t ROWS = 4;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

uint8_t colPins[COLS] = { 5, 4, 3, 2 };
uint8_t rowPins[ROWS] = { 9, 8, 7, 6 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Storage for current entry
char entry[9] = { NULL };
// Password
char password1[9] = { "102989198" };
char password2[9] = { "103073746" };
char password3[9] = { "102098120" };
// Counter
int counter = 0;
// DEFAULT, It doesn't like an empty char
char key = '_';
// Bool to signify active recording of keypresses
bool active = true;
String msg = "";

void setup() {
  // Begin
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(100);

  // Clear The Bootloader Screen
  display.clearDisplay();
  display.display();

  // Set Defaults for text
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  Serial.println("Finished Starting Up! Provide a Password...");
}

void loop() {

  // Clear Display
  display.clearDisplay();
  display.setCursor(0,0);

  // Check for Keypresses
  char new_key = keypad.getKey();

  // Only add to entry if active
  if (active) {

    if (new_key != NO_KEY) {
      key = new_key;
      entry[counter] = key;
      counter++;
      Serial.println(entry);
      msg = "";
    }

    if (entry[8] != NULL) {
      if (isEqual(entry, password1, 9)) {
        active = false;
        msg = "Welcome Jasper!";
      }
      else if (isEqual(entry, password2, 9)) {
        active = false;
        msg = "Welcome Joel!";
      }
      else if (isEqual(entry, password3, 9)) {
        active = false;
        msg = "Welcome Jack!";
      }
      else {
        msg = "No Match Found.";
        /* 
        this is an attempt to input multiple passwords in succession without restarting
        However, it does reset the entry array, but you have to push 8 times for it to say "starting up again"
        After that, you can easily input another set of digits for a password, just the little blank is the issue. 
        */
        for (int i = 0; i < sizeof(entry); i++) {
          entry[i] = (char)NULL;
        }
        active = true;
        counter = 0;
      }
      Serial.println(msg);
    }
  }
  // Print Text
  display.setCursor(1, 1);
  display.print("Password:");
  // Print Current entry
  display.setCursor(1, 14);
  display.print(entry);
  // Print Message
  display.setCursor(1, 28);
  display.print(msg);
  // Send to Display
  yield();
  display.display();
  // Delay 10 millis
  delay(10);
}

/*
Used to compare two char arrays
*/
bool isEqual(char x[], char y[], int len) {
  for (int i = 0; i < len; i++) {
    if (!(x[i] == y[i])) {
      return false;
    }
  }
  return true;
}
