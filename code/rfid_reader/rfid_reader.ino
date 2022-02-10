#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         6          // Configurable, see typical pin layout above
#define SS_PIN          7         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

// Our tag: 47 93 24 B5 (all hex values)
// To research: 4 bytes always?
bool check_auth(MFRC522::Uid uid) {
  Serial.println("UID in hex: ");
  Serial.print(uid.uidByte[0]);
  Serial.print(" ");
  Serial.print(uid.uidByte[1]);
  Serial.print(" ");
  Serial.print(uid.uidByte[2]);
  Serial.print(" ");
  Serial.println(uid.uidByte[3]);

  return (uid.uidByte[0] == 0x47 && uid.uidByte[1] == 0x93 && uid.uidByte[2] == 0x24 && uid.uidByte[3] == 0xB5); 
  
}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  bool auth = check_auth(mfrc522.uid);
  if(auth) {
    Serial.println("ACCESS ALLOWED");
  } else {
    Serial.println("ACCESS DENIED");
  }
  mfrc522.PICC_HaltA();
}
