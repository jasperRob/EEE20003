// Include necessary libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

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

// Bools for handling symbols
bool denied = false;
bool granted = false;

int hover = 0;
long lastChange = millis();

void setup() {
  // Begin
  Serial.begin(9600);
  
  // Save to EEPROM
  EEPROM.write(0, "EEE20003");
  EEPROM.write(8, 102989198);
  EEPROM.write(9, 103073746);
  EEPROM.write(10, 102098120);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  attachInterrupt(digitalPinToInterrupt(2), selectChar, RISING);
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

  // Increment hover var
  if (millis() - lastChange > 1000) {
    if (hover == 9) {
      hover = 0;
    } else {
      hover++;
    }
    lastChange = millis();
  }

  // Only add to entry if active
  if (active) {

    if (entry[8] != NULL) {
      
      String entryString(entry);
      int entryInt = entryString.toInt();

      if (entryInt == EEPROM.read(8)) {
        active = false;
        granted = true;
        msg = "Welcome Jasper!";
      }
      else if (entryInt == EEPROM.read(9)) {
        active = false;
        granted = true;
        msg = "Welcome Joel!";
      }
      else if (entryInt == EEPROM.read(10)) {
        active = false;
        granted = true;
        msg = "Welcome Jack!";
      }
      else {
        msg = "No Match Found.";
        denied = true;
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
  display.print("New Char: ");
  display.print(hover);
  // Print Current entry
  display.setCursor(1, 14);
  display.print(entry);
  // Print Message
  display.setCursor(1, 28);
  display.print(msg);
  // Draw Cross if Denied
  if (denied) {
    display.drawLine(96, 32, 120, 8, WHITE);
    display.drawLine(120, 32, 96, 8, WHITE);
  }
  // Draw Tick if Granted
  if (granted) {
    display.drawLine(96, 23, 105, 30, WHITE);
    display.drawLine(105, 30, 120, 6, WHITE);
  }
  // Send to Display
  yield();
  display.display();
  // Delay 10 millis
  delay(10);
}

void selectChar() {
  entry[counter] = hover;
  counter++;
}

/*
Used to compare two char arrays
*/
// compares two different arrays and see if they are equal - J.F
bool isEqual(char x[], char y[], int len) {
  for (int i = 0; i < len; i++) {
    if (!(x[i] == y[i])) {
      return false;
    }
  }
  return true;
}

void menu(){ // menu function from serial monitor to select the 3 option choices 
    int dec;
    Serial.println("Option 1 - Change 3 members passcodes. ");
    Serial.println("Option 2 - [D/HD] The data from the additional sensor. ");
    Serial.println("Option 3 - Interrupt-driven programming. ");
    while(Serial.available() == 0){ // Wait for User to Input Data
    }
    dec = Serial.parseInt();
    switch(dec){
      case 1:
        //function 1
        break;
      case 2:
        //function 2
        break;
     case 3:
        //function 3
        break;\
    default:
        //something 
        break;
    }
  }
    
