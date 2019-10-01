
#include <EEPROM.h>     // We are going to read and write TAG's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <Servo.h>
#define COMMON_ANODE
#ifdef COMMON_ANODE
#define LED_ON LOW
#define LED_OFF HIGH
#else
#define LED_ON HIGH
#define LED_OFF LOW
#endif

#define redLed 7    // Set Led Pins
#define greenLed 6
#define yellowLed 5

#define relay 4     // Set Relay Pin
#define wipeMode 3     // Button pin for WipeMode

boolean match = false;          // initialize card match to false
boolean programMode = false;  // initialize programming mode to false
boolean replaceMaster = false;

int successRead;    // Variable integer to keep if we have Successful Read from Reader
//FOR BYTE 
byte storedCard[4];   // Store ID
byte readCard[4];   //  read from RFID Module
byte masterCard[4];   // Store master card
//FOR 7 BYTE
byte storedCard7[7];   // Store ID
byte readCard7[7];   //  read from RFID Module
byte masterCard7[7];   // Store master card

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
void initialLED_config(/* arguments */){
  digitalWrite(redLed, LED_OFF);
  digitalWrite(greenLed, LED_OFF);
  digitalWrite(yellowLed, LED_OFF);
}
void set_protocall()
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

//Setup
void setup() {
  //Arduino Pin Configuration
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(wipeMode, INPUT_PULLUP);   // Enable pin's pull up resistor
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  initialLED_config();


  //Protocol Configuration
  set_protocall();
  Serial.println(F("Rfid auth"));
  showRFIDdetails();
  //Wipe Code if Button Pressed
  if (digitalRead(wipeMode) == LOW) {  //Reads the value from a specified digital pin, either HIGH or LOW.
    digitalWrite(redLed, LED_ON); // Lights the led if the above condition is satisfied
    /*strings ends up getting stored in RAM by default The F() macro makes sure they stay in Flash memory.
      Just an optimization done to make sure it doesnt end up using the ram insted.
      Flash memory in this case is the EEPROM of size 1MB
    */
    Serial.println(F("Wipe Button is Pressed"));
    Serial.println(F("You have 15 secs to Cancel the Command"));
    Serial.println(F("This will be delete all records and cannot be undone"));
    delay(15000);
    if (digitalRead(wipeMode) == LOW) {    //if button still pressed
      Serial.println(F("Starting Wiping EEPROM"));
      for (int i = 0; i < EEPROM.length(); i++) {    //Loop end of EEPROM address
        if (EEPROM.read(i) == 0) { //if it is already empty get out of the wipe loop
          break;            
        }
        else {
          /*
           * Syntax EEPROM.write(addr, val);
           * EEPROM.length() is used to iterate over the memory
           * Offical description
           * EEPROM.length()
            This function returns an unsigned int containing the number of cells in the 
            EEPROM. Not all devices have the same size EEPROM, 
            this can be useful for writing code portable to different Arduinos.
           */
          EEPROM.write(i, 0);
        }
      }
      Serial.println(F("EEPROM Successfully Wiped"));
      LED_eepromwipe(); // Call the function that will disply the led combination for wipeMode
    }
    else {
      Serial.println(F("Wiping Cancelled"));
      digitalWrite(redLed, LED_OFF); // Changed the ON status from line 70 to OFF as the WipeMode was cancelled
    }
  }
  if (EEPROM.read(1) != 01) {
    Serial.println(F("No Master Card is Defined"));
    Serial.println(F("Scan A TAG to Define as Master Card"));
    do {
      successRead = get_ID();            // sets successRead to 1 when we get read from reader otherwise 0
      digitalWrite(yellowLed, LED_ON);    // Visualize Master Card need to be defined
      delay(200);
      digitalWrite(yellowLed, LED_OFF);
      delay(200);
    }
    while (!successRead);                  // Program will not go further while you not get a successful read
    //To Read the rfid card and check wether the id tag is of 4byte or 7byte
    for (int i = 0; i < mfrc522.uid.size; i++) {
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
    for ( int j = 0; j < mfrc522.uid.size; j++ ) {        // Loop 4 times
      EEPROM.write( 2 + j, readCard[j] );  // Write scanned PICC's UID to EEPROM, start from address 3
    }
    EEPROM.write(1, 01);                  // Write to EEPROM we defined Master Card.
    Serial.println(F("Master Card is Defined Sucessfully"));
  }
  Serial.println(F("-------------------"));
  Serial.println(F("Master Card's UID ="));
  for ( int i = 0; i < mfrc522.uid.size; i++ ) {// Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2 + i);    
    Serial.print(masterCard[i], HEX);
  }
  Serial.println("");
  Serial.println(F("-------------------"));
  Serial.println(F("Everything is Ready"));
  Serial.println(F("Waiting for TAGs to be scanned"));
  LED_cycle_config();    // Everything ready lets give user some feedback by cycling leds
}


//MAIN
void loop () {
  do {
    successRead = get_ID();  // sets successRead to 1 when we get read from reader otherwise 0
    if (digitalRead(wipeMode) == LOW) {
      digitalWrite(redLed, LED_ON);
      digitalWrite(greenLed, LED_OFF);
      digitalWrite(yellowLed, LED_OFF);
      Serial.println(F("Wipe Button is Pressed"));
      Serial.println(F("Master Card will be Erased! in 10 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 9 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 8 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 7 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 6 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 5 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 4 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 3 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 2 seconds"));
      delay(1000);
      Serial.println(F("Master Card will be Erased! in 1 seconds"));
      delay(1000);
      Serial.println(F("PUFF!! Details erased"));



      if (digitalRead(wipeMode) == LOW) {
        EEPROM.write(0, 0);                  // Reset Number.
        Serial.println(F("Restart device to re-program the Master Card"));
        while (1);
      }
    }
    if (programMode) {
      LED_cycle_config();              // Program Mode cycles through RGB waiting to read a new card
    }
    else {
      normal_Mode();     // Normal mode, blue Power LED is on, all others are off
    }
  }
  while (!successRead);   //the program will not go further while you not get a successful read
  if (programMode) {
    if ( isMaster(readCard) ) { //If master card scanned again exit program mode
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      return;
    }
    else {
      if ( find_ID(readCard) ) { // If scanned card is known delete it
        Serial.println(F("I know this TAG, removing........"));
        delete_ID(readCard);
        Serial.println("-----------------------------");
        Serial.println(F("Scan a TAG to ADD or REMOVE"));
      }
      else {
        Serial.println(F("I dont know this TAG, adding........"));
        write_ID(readCard);
        Serial.println(F("-----------------------------"));
        Serial.println(F("Scan a TAG to ADD or REMOVE"));
      }
    }
  }
  else {
    if ( isMaster(readCard)) {    // If scanned card == Master Card Enter program mode
      programMode = true;
      Serial.println(F(" Entered Program Mode"));
      int count = EEPROM.read(0);   // Read the first Byte of EEPROM that
      Serial.print(F("I have "));     // stores the number of ID's in EEPROM
      Serial.print(count);
      Serial.print(F(" record on EEPROM"));
      Serial.println("");
      Serial.println(F("Scan a TAG to ADD or REMOVE to EEPROM"));
      Serial.println(F("Scan Master Card again to Exit Program Mode"));
      Serial.println(F("-----------------------------"));
    }
    else {
      if ( find_ID(readCard) ) { // If not, see if the card is in the EEPROM
        Serial.println(F("Sucess"));
        auth_sucess(300);         // Open the door lock for 300 ms
      }
      else {      // If not, show that the ID was not valid
        Serial.println(F("Not valid id"));
        auth_denied();
      }
    }
  }
}
void LED_eepromwipe()
{
digitalWrite(redLed, LED_OFF);  // visualize successful wipe
delay(200);
digitalWrite(redLed, LED_ON);
delay(200);
digitalWrite(redLed, LED_OFF);
delay(200);
digitalWrite(redLed, LED_ON);
delay(200);
digitalWrite(redLed, LED_OFF);
}
// Access auth_sucess
void auth_sucess (int setDelay) {
  digitalWrite(yellowLed, LED_OFF);
  digitalWrite(redLed, LED_OFF);
  digitalWrite(greenLed, LED_ON);
  digitalWrite(relay, LOW);
  delay(setDelay);
  digitalWrite(relay, HIGH);
  delay(1000);
}
//Access auth_denied
void auth_denied() {
  digitalWrite(greenLed, LED_OFF);
  digitalWrite(yellowLed, LED_OFF);
  digitalWrite(redLed, LED_ON);
  delay(1000);
}


// Get TAG's UID
int get_ID() {
  // Getting ready for Reading TAGS
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare TAGS which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every TAGS as they have 4 byte UID
  // Until we support 7 byte TAGS
  Serial.println(F("Scanned TAG's UID:"));
  for (int i = 0; i < mfrc522.uid.size; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}



void showRFIDdetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a china dublicate?"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check connections."));
    while (true); // do not go further
  }
}

//Program Mode
void LED_cycle_config() {
  digitalWrite(redLed, LED_OFF);
  digitalWrite(greenLed, LED_ON);
  digitalWrite(yellowLed, LED_OFF);
  delay(200);
  digitalWrite(redLed, LED_OFF);
  digitalWrite(greenLed, LED_OFF);
  digitalWrite(yellowLed, LED_ON);
  delay(200);
  digitalWrite(redLed, LED_ON);
  digitalWrite(greenLed, LED_OFF);
  digitalWrite(yellowLed, LED_OFF);
  delay(200);
}

// Normal Mode Led sequence //BUG HERE
void normal_Mode () {
  digitalWrite(yellowLed, LED_ON);
  digitalWrite(redLed, LED_OFF);
  digitalWrite(greenLed, LED_OFF);
  digitalWrite(relay, HIGH);
}

//Read an ID from EEPROM
void read_ID( int number ) {
  int getUidType=0;
  for (int i = 0; i < mfrc522.uid.size; i++)
  {
    getUidType=getUidType+1;
    }
    if(getUidType==4){
      int start = (number * 4 ) + 2;    // Figure out starting position
      for ( int i = 0; i < 4; i++ ) {     // Loop 4 times to get the 4 Bytes
      storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
                    }
    else
    {
      int start = (number * 7 ) + 2;    // Figure out starting position
      for ( int i = 0; i < 7; i++ ) {     // Loop74 times to get the74 Bytes
      storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
     }
}
  

// Add ID to EEPROM
void write_ID( byte a[] ) {
  int getUidType=0;
  if ( !find_ID( a ) ) {     // Before we write to the EEPROM, check to see if we have seen this card before!
    int num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    if(getUidType==4){
        int start = ( num * 4 ) + 6;  // Figure out where the next slot starts
        num++;                // Increment the counter by one
        EEPROM.write( 0, num );     // Write the new count to the counter
    for ( int j = 0; j < 4; j++ ) {   // Loop 4 times
        EEPROM.write( start + j, a[j] );  // Write the array values to EEPROM in the right position
    }
      }
     else
     {    
      int start = ( num * 7 ) + 6;  // Figure out where the next slot starts
      num++;                // Increment the counter by one
      EEPROM.write( 0, num );     // Write the new count to the counter
      for ( int j = 0; j < 7; j++ ) {   // Loop 4 times
        EEPROM.write( start + j, a[j] );  // Write the array values to EEPROM in the right position
    }
      }

    LED_successWriteConfig();
    Serial.println(F("Succesfully added ID to EEPROM"));
  }
  else {
    LED_failedWriteConfig();
    Serial.println(F("MUDA MUDA MUDA MUDA,something wrong with ID or bad EEPROM"));
  }
}

//remove id from eeprom
void delete_ID( byte a[] ) {
  int getUidType=0;
  if ( !find_ID( a ) ) {     // Before we delete from the EEPROM, check to see if we have this card!
    LED_failedWriteConfig();      // If not
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  else {
    int num = EEPROM.read(0);   // Get the numer of used spaces, position 0 stores the number of ID cards
    int slot;       // Figure out the slot number of the card
    int start;      // = ( num * 4 ) + 6; // Figure out where the next slot starts
    int looping;    // The number of times the loop repeats
    int j;
    int count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    if(getUidType==4){

      slot = find_slot( a );   // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--; //counter --
    EEPROM.write( 0, num );   // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) {         // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( int k = 0; k < 4; k++ ) {         // Shifting loop
      EEPROM.write( start + j + k, 0);

      
    }
      }
   else{
    slot = find_slot( a );   // Figure out the slot number of the card to delete
    start = (slot * 7) + 2;
    looping = ((num - slot) * 7);
    num--; //counter --
    EEPROM.write( 0, num );   // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) {         // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 7 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( int k = 0; k < 7; k++ ) {         // Shifting loop
      EEPROM.write( start + j + k, 0);
    }
    }  
    
    
    LED_successDeleteConfig();
    Serial.println(F("Succesfully removed ID from EEPROM"));
  }
}

//byte check
boolean check_TwoBYT ( byte a[], byte b[] ) {
  if ( a[0] != NULL )       // Make sure there is something in the array first
    match = true;       // Assume they match at first
  for ( int k = 0; k < mfrc522.uid.size; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] )     // IF a != b then set match = false, one fails, all fail
      match = false;
  }
  if ( match ) {      // Check to see if if match is still true
    return true;
  }
  else  {
    return false;
  }
}

//find available slot
int find_slot( byte find[] ) {
  int count = EEPROM.read(0);       // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    read_ID(i);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( check_TwoBYT( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
      break;          // break if found
    }
  }
}

//Find in EPROM
boolean find_ID( byte find[] ) {
  int count = EEPROM.read(0);     // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    read_ID(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( check_TwoBYT( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;  // break if found
    }
    else {
    }
  }
  return false;
}

//successful write to EEPROM
void LED_successWriteConfig() {
  digitalWrite(yellowLed, LED_OFF);
  digitalWrite(redLed, LED_OFF);
  digitalWrite(greenLed, LED_OFF);
  delay(200);
  digitalWrite(greenLed, LED_ON);
  delay(200);
  digitalWrite(greenLed, LED_OFF);
  delay(200);
  digitalWrite(greenLed, LED_ON);
  delay(200);
  digitalWrite(greenLed, LED_OFF);
  delay(200);
  digitalWrite(greenLed, LED_ON);
  delay(200);
}

//indicate a failed write to EEPROM
void LED_failedWriteConfig() {
  digitalWrite(yellowLed, LED_OFF);
  digitalWrite(redLed, LED_OFF);
  digitalWrite(greenLed, LED_OFF);
  delay(200);
  digitalWrite(redLed, LED_ON);
  delay(200);
  digitalWrite(redLed, LED_OFF);
  delay(200);
  digitalWrite(redLed, LED_ON);
  delay(200);
  digitalWrite(redLed, LED_OFF);
  delay(200);
  digitalWrite(redLed, LED_ON);
  delay(200);
}

//success delete from EEPROM
void LED_successDeleteConfig() {
  digitalWrite(yellowLed, LED_OFF);
  digitalWrite(redLed, LED_OFF);
  digitalWrite(greenLed, LED_OFF);
  delay(200);
  digitalWrite(yellowLed, LED_ON);
  delay(200);
  digitalWrite(yellowLed, LED_OFF);
  delay(200);
  digitalWrite(yellowLed, LED_ON);
  delay(200);
  digitalWrite(yellowLed, LED_OFF);
  delay(200);
  digitalWrite(yellowLed, LED_ON);
  delay(200);
}

//Check if master or!
boolean isMaster( byte test[] ) {
  if ( check_TwoBYT( test, masterCard ) )
    return true;
  else
    return false;
}
