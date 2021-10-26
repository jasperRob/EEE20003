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
  writeToEEPROM(1, "EEE20003", 8);
  writeToEEPROM(9, "102989198", 9);
  writeToEEPROM(18, "103073746", 9);
  writeToEEPROM(27, "102098120", 9);

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
  Serial.println("Finished Starting Up! Provide a Password...");
}

void loop() {
  
  // start menu
  menu();

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

      // First position integer stores total number of saved passwords
      int numPasswords = EEPROM.read(0);

      for (int i = 0; i < numPasswords; i++) {
        char pw[9];
        int pos = (i+1) * 9;
        readFromEEPROM(pos, pw, 9);

        if (isEqual(entry, pw, 9)) {
          active = false;
          granted = true;
          msg = "Welcome Person!";
        }
      }

      if (!granted) {
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
  if (millis() - debounce > 100){
    // This converts it to an ascii char
    char c = (char) (hover + 48);
    Serial.println(c);
    // print entry
    if (counter < 9) {
      entry[counter] = c;
      counter++;
    }
    debounce = millis();
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

void changePassword(int pos){
  String message; //string from serial 
  int msg_len;    //total length of string
  int dec; // user address in the EEPROM
  switch(pos){
      case 1:
        dec = 9;
        break;
      case 2:
        dec = 18;
        break;
     case 3:
        dec = 27;
        break;
    default:
        Serial.println("Please Select a user from 1-3.");
        break;
    } 
  Serial.print("Please enter a 9 alphanumeric length passcode to change User: ");
  Serial.println(pos);
  do {                    // do while used to read strings instead of  parseInt()
    message = Serial.readString();
  } while (message == ""); // read from serial monitor 
  Serial.println(message);
  msg_len = message.length() ; // //total length of string 
  char char_array[msg_len];   // char array made from length of message
  message.toCharArray(char_array, msg_len);  // convert message into char array with length
  for (int i = 0; i<sizeof(char_array); i++)    // refering to Wk5 lab notes
  {
    Serial.print(char_array[i]); //display each 
  }
  writeToEEPROM(dec,char_array,9);
  char pw[9];
  readFromEEPROM(dec,pw , 9);
  Serial.println(pw);
}

void menu(){ // menu function from serial monitor to select the 3 option choices 
   
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
  
    //main menu
    
    int dec;
    Serial.println("Option 1 - Change 3 members passcodes. ");
    Serial.println("Option 2 - [D/HD] The data from the additional sensor. ");
    Serial.println("Option 3 - Interrupt-driven programming. ");
    while(Serial.available() == 0){ // Wait for User to Input Data
    }
    dec = Serial.parseInt();
    switch(dec){
      case 1:
        String choice;
        Serial.println("Please select which user passcode to change (1-3) ");
        while (Serial.available() == 0 ) { // wait for input
        }
        choice =  Serial.readStringUntil('\n'); // read string until meet newline character 
        if(choice == 1){
          changePassword(1);
        }
        else if (choice == 2){
          changePassword(2);
        }
        else if (choice == 3){
          changePassword(3);
        }
        else{
          Serial.println("INCORRECT INPUT. Please select 1-3. ");
          break;
        }
        break;
      case 2:
        //function 2
        Serial.println("Additional Sensor");
        break;
     case 3:
        //function 3
        Serial.println("Finished Starting Up! Provide a Password...");
        
        break;
    default:
        //something 
        break;
    }
  }

