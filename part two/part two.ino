// Include necessary libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
// debounce
long debounce = millis();
// Storage for current entry
char entry[9] = { NULL };

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
  
  // Save to EEPROM (pos, item, length)
  writeToEEPROM(1, "EEE20003", 8); // DEFAULT 
  writeToEEPROM(9, "102989198", 9); // JASPER
  writeToEEPROM(18, "103073746", 9); // JOEL
  writeToEEPROM(27, "102098120", 9); // JACK

  // Save total num passwords in eeprom. max of 9
  EEPROM.write(0, 3);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  attachInterrupt(digitalPinToInterrupt(19), selectChar, RISING);
  delay(100);

  // Clear The Bootloader Screen
  display.clearDisplay();
  display.display();

  // Set Defaults for text
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  menu();
}

void loop() {

  // Only add to entry if active
  if (active) {

    // Clear Display
    display.clearDisplay();
    display.setCursor(0,0);


    char c = (char) (hover + 48);

    // Increment hover var
    if (millis() - lastChange > 5000){
      lastChange = millis();

      char c = (char) (hover + 48);
      // print entry
      if (counter < 9) {
        entry[counter] = c;
        counter++;
      }

      // Reset hover var
      hover = 0;
    }

    if (entry[8] != NULL) {

      // First position integer stores total number of saved passwords
      int numPasswords = EEPROM.read(0);

      for (int i = 0; i < numPasswords; i++) {
        char pw[9];
        int pos = (i+1) * 9;
        readFromEEPROM(pos, pw, 9);

        if (isEqual(entry, pw, 9)) {
          active = false;
          granted = true;
          msg = "Welcome User!";
        }
      }

      if (!granted) {
        msg = "No Match Found.";
        denied = true;

        for (int i = 0; i < sizeof(entry); i++) {
          entry[i] = (char)NULL;
        }
        active = true;
        counter = 0;
      }
      Serial.println(msg);
    }
    // Print Text
    display.setCursor(1, 1);
    display.print("Updating in : ");
    int timeLeft = ((5000 - (millis() - lastChange)) / 1000) + 1;
    display.print(timeLeft);
    // Print Current entry
    display.setCursor(1, 14);
    display.print(entry);
    display.print(c);
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
}

void selectChar() {
  if (millis() - debounce > 100){
    debounce = millis();

    if (hover < 9) {
      hover++;
    } else {
      hover = 0;
    }

    lastChange = millis();
  }
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

/*
Used to write a char array to EEPROM

param p   the position to write to
param c   the char array to write
param length   the length of the array
*/
void writeToEEPROM(int p, char* c, int length)
{
  for (int i = 0; i < length; i++) {
    EEPROM.write(p+i, c[i]);
  }
}

/*
Used to read a char array to EEPROM

param p   the position to read from
param c   the char array to read data into
param length   the length of the array
*/
void readFromEEPROM(int p, char* c, int length)
{
  for (int i = 0; i < length; i++) {
    char cha = (char) EEPROM.read(p+i);
    c[i] = cha;
  }
}

/* 
  Password change based on which user to change, takes in 9 alphanumeric digits and applies to the 
  respected EEPROM position to WRITE
*/

void changePassword(int pos) {
  String message; //string from serial 
  int msg_len ;    //total length of string
  char char_array[9];   // char array made from length of message
  while (msg_len != 10){
    Serial.print("Please enter a 9 alphanumeric length passcode to change User: ");
    Serial.println(pos);
    do {                    // do while used to read strings instead of  parseInt()
      message = Serial.readString();
    } while (message == ""); // read from serial monitor 
    Serial.println(message);
    msg_len = message.length() ; // //total length of string 
    char char_array[msg_len];   // char array made from length of message
    message.toCharArray(char_array, msg_len);  // convert message into char array with length
    writeToEEPROM(pos*9,char_array,9);
  }
}

/* 
  Main menu function from serial monitor to select the 3 option choices 
*/
void menu() { 
   
   // variables
   String defltpword;
   char pw[9];
  
   // loop to keep asking for correct password
   while (true){
   
    readFromEEPROM(1 ,pw , 8);
    Serial.println("Please input default Password");
    while (Serial.available() == 0 ) { // wait for input
    }
    defltpword =  Serial.readStringUntil('\n'); // read string until meet newline character
    
    if (defltpword == pw){
      Serial.println("Correct Password");
      break;
    }
    else {
      Serial.println("Incorrect Password");
      Serial.println("Please try again");
      delay(200);
    }
  }
  
  // Display options
  while (true) {
    int dec;
      Serial.println("Option 1 - Change 3 members passcodes. ");
      Serial.println("Option 2 - [D/HD] The data from the additional sensor. ");
      Serial.println("Option 3 - Interrupt-driven programming. ");
      // Wait for User to Input Data
      while (Serial.available() == 0) {}
      dec = Serial.parseInt();
      // Compare result
      if (dec == 1) {
          String choice;       
          while (Serial.read() >= 0); // do nothing
          Serial.println("Please select which user passcode to change (1-3) ");
          while (Serial.available() == 0 ) {}
          choice =  Serial.readStringUntil('\n'); // read string until meet newline character 
          Serial.println(choice);
          if (choice == "1"){
            changePassword(1);
          } else if (choice == "2"){
            changePassword(2);
          } else if (choice == "3"){
            changePassword(3);
          } else{
            Serial.println("INCORRECT INPUT. Please select 1-3. ");
          }
      } else if (dec == 2) {
        Serial.println("Temp must be 35.0 deg");
        bool temp = isCorrectTemp(35);
        if (temp = true){
        Serial.println("Temp is correct!");
        EEPROM.write(100, 1); // if the temp sensor returns true, 
        //write 1 into EEPROM address 100 to indicate it has been satisfied
        }else{
          EEPROM.write(100, 0); // else write 0 into EEPROM to indicate it has not been satisfied.
        }
        int val = EEPROM.read(100); // READ value from 100 
        if(val == 1){
          Serial.println("Added into EEPROM");
        }else{
          Serial.println("No value in EEPROM");
        }
      } else if (dec == 3) {
        Serial.println("Finished Starting Up! Provide a Password...");
        break;
      }
  }

  // End menu and enter loop
  lastChange = millis();
}

/*
  Function for temp checking
*/
bool isCorrectTemp(int temp) { 
// BOOL to confirm if temp has been met, to then write into EEPROM
  int ambient = 0.0;

  while (ambient != temp) {
    const float BETA = 3950;
    int analogValue = analogRead(A0);
    float celcius = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
    ambient = (int) celcius;
  }
  return true;
}

