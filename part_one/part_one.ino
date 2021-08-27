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
char password1[8] = { "102989198" };
char password2[8] = { "103073746" };
char password3[8] = { "102098120"};
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

    if (new_key != NO_KEY && new_key != key) {
      key = new_key;
      entry[i] = key;
      i++;
      Serial.println(entry);
    }

    if (entry[8] != NULL) {
      if (isEqual(entry, password1, 9)) {
        active = false;
        Serial.println("Match Found!");
      }
      if (isEqual(entry, password2, 9)) {
        active = false;
        Serial.println("Match Found!");
      }
      if (isEqual(entry, password3, 9)) {
        active = false;
        Serial.println("Match Found!");
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

bool isEqual(char x[], char y[], int len) {
  for (int i = 0; i < len; i++) {
    if (!(x[i] == y[i])) {
      return false;
    }
  }
  return true;
}
