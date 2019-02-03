#include <Stepper.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "rgb_lcd.h"
#define PN532_IRQ (2)
#define PN532_RESET (3)
//Servo servo;
rgb_lcd lcd;
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
Stepper steppermotor(32, 8, 10, 9, 11);
uint32_t keys[10];
uint8_t sizeofkeys = 0;
bool wasUnlocked = false;
void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.setRGB(131, 7, 255);
  nfc.begin();
  Serial.println("LockMate init!");
  uint32_t versiondata = nfc.getFirmwareVersion();
  if(!versiondata) {
    Serial.print("Fatal error: NFC hardware is missing! Halting.");
    while(1);
  }
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  nfc.SAMConfig();
  Serial.println("LockMate ready for key card. Startup complete.");
}

uint32_t readCard() {
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.println("Key detected");
    if (uidLength == 4) {
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];  
      cardid <<= 8;
      cardid |= uid[3]; 
      Serial.println(cardid);
      return cardid;
    }
  }
}

bool checkForCard(uint32_t num) {
  for(int x = 0; x < 5; x++) {
    if(num == keys[x]) {
      return true;
    }
  }
  return false;
}
void loop() {
  lcd.print("Please Tap Card");
  if(wasUnlocked) {
    lcd.setRGB(131, 7, 255);
    wasUnlocked = false;
  }
  uint32_t card = readCard();
  delay(200);
  if(card == 2527921356) {
    addCard();
  } else if(checkForCard(card)) {
    lcd.setRGB(0, 255, 0);
    lcd.clear();
    lcd.print("Card Accepted.");
    Serial.println("Card " + String(card) + " identified. Unlocking.");
    steppermotor.setSpeed(1000);  
    steppermotor.step(340);
    delay(5000);
    steppermotor.setSpeed(1000);  
    steppermotor.step(340);
    wasUnlocked = true;
    lcd.clear();
    //servo.detach();
  } else {
    lcd.clear();
    lcd.setRGB(255, 0, 0);
    lcd.print("Access Denied.");
    Serial.println("Card not recognized or not in system.");
    delay(2500);
    wasUnlocked = true;
    lcd.clear();
  }
}

void addCard() {
  lcd.setRGB(0, 0, 255);
  lcd.clear();
  lcd.print("Tap To Add Card");
  delay(150);
  Serial.println("Programming card detected. \nTap card against sensor to add to system. \nYou may add up to 10 cards. \nTap programming card when done.");
  while(true) {
    lcd.print("Tap To Add Card");
    uint32_t card = readCard();
    delay(200);
    if(card == 2527921356) {
      Serial.println("Returning to lock screen.");
      lcd.clear();
      lcd.print("Done adding card");
      delay(1500);
      lcd.setRGB(131, 7, 255);
      lcd.clear();
      return;
    } else if(sizeofkeys == 10) {
        lcd.clear();
        lcd.print("Card bank full.");
        delay(1500);
        Serial.println("Card bank full.");
        lcd.setRGB(131, 7, 255);
        lcd.clear();
        return;
    }
    bool isCardPresent = checkForCard(card);
    if(isCardPresent) {
      Serial.println("Card is already present in system. Returning to lock screen.");
      lcd.clear();
      lcd.print("Card is in sys.");
      delay(1500);
      lcd.setRGB(131, 7, 255);
      lcd.clear();
      return;
    } else {
      keys[sizeofkeys] = card;
      sizeofkeys++;
      Serial.println("Card " + String(card) + " added to system.");
      lcd.clear();
      lcd.print("Card added.");
      delay(1500);
      lcd.clear();
    }
  }
}
