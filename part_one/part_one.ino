#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

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
int i = 0;
// DEFAULT, It doesn't like an empty char
char key = '_';
// Bool to signify active recording of keypresses
bool active = true;

void setup() {
  // Begin
  Serial.begin(9600);
  Serial.println("Starting Up!");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(100);

  // Clear The Bootloader Screen
  display.clearDisplay();
  display.display();

  // Set Defaults for text
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
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
      entry[i] = key;
      i++;
      Serial.println(entry);
    }

    if (entry[8] != NULL) {
      if (isEqual(entry, password1, 9)) {
        active = false;
        Serial.println("Welcome Jasper!");
      }
      else if (isEqual(entry, password2, 9)) {
        active = false;
        Serial.println("Welcome Joel!");
      }
      else if (isEqual(entry, password3, 9)) {
        active = false;
        Serial.println("Welcome Jack!");
      }
      else {
        active = false;
        Serial.println("No Match Found.");
        for( int i = 0; i < sizeof(entry);  ++i )   // this is an attempt to inpout multiple passwords in succession without restarting 
            entry[i] = (char)NULL;                  // However, it does reset the entry array, but you have to push 8 times for it to say "starting up again"
            active = true;                          // After that, you can easily input another set of digits for a password, just the little blank is the issue. 
      }
      }
    }
    
  }

  // Print Current entry
  display.print(entry);
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
