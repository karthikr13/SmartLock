
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoBLE.h>

BLEService newService("19B10010-E8F2-537E-4F6C-D104768A1214"); // creating the service

BLEByteCharacteristic switchChar("19B10010-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite); // creating the LED characteristic
BLEByteCharacteristic accessChar("ACCESS-CODE", BLERead | BLEWrite);

#include <Firebase_Arduino_WiFiNINA.h>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status
byte mac[6];

#define RST_PIN         6          // Configurable, see typical pin layout above
#define SS_PIN          7         // Configurable, see typical pin layout above

#define FIREBASE_HOST "smartlock-96c43-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "2EpydZnzzRuiFtvcJxAuTMXLWYoV7NBuHkGZui6K"
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
char server[] = "www.google.com";
String mac_addr;
FirebaseData firebaseData;
//IPAddress server(8,8,8,8);
WiFiClient client;

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

//https://forum.arduino.cc/t/convert-mac-address-to-string/644713/2
String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%02X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

String uid2String(byte ar[]) {
  String s;
  for (int i = 0; i < 4; i++)
  {
    char buf[3];
    sprintf(buf, "%02X", ar[i]);
    s += buf;
    if (i < 3) s += '-';
  }
  return s;
}

void bluetooth_setup(){
  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected
  pinMode(ledPin, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  //initialize BLE library
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("MKR WiFi 1010"); //Setting a name that will appear when scanning for bluetooth devices
  BLE.setAdvertisedService(newService);

  newService.addCharacteristic(switchChar); //add characteristics to a service
  newService.addCharacteristic(accessChar);

  BLE.addService(newService);  // adding the service

  switchChar.writeValue(0); //set initial value for characteristics

  BLE.advertise(); //start advertising the service
  Serial.println("Bluetooth device active, waiting for connections...");
}

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");

  printWifiStatus();

  bluetooth_setup();
  
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  /*Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }*/
  
  Serial.println("\nConnecting to Firebase");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, ssid, pass);
  Serial.println("Connected");
  /*String path = "/";
  Serial.println("Setting");
  if(Firebase.setInt(firebaseData, path + "/Int/Data1", 1)) {
    Serial.println("Inserted");
  }*/
}

// Our tag: 47 93 24 B5 (all hex values)
String check_db(String path) {
  Serial.print("Accessing Firebase DB at URL ");
  Serial.println(path);
  String out;
  if(Firebase.getString(firebaseData, path)) {
    Serial.println("Got data from DB: ");
    out = firebaseData.stringData();
    return out;
  }
  return "NO_ACCESS"; 
}

void loop() {
    // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  WiFi.macAddress(mac);
  mac_addr = mac2String(mac);
  byte uid[4] = {mfrc522.uid.uidByte[0], mfrc522.uid.uidByte[1], mfrc522.uid.uidByte[2], mfrc522.uid.uidByte[3]};
  String uidString = uid2String(uid);
  Serial.print("UID: ");
  Serial.println(uidString);
  String path = "/" + mac_addr + "/" + uidString;
  if (state){ //nonzero value => registering state
    //write to db
//    String path = "/";
    Serial.println("Setting");
//    firebaseData =  
    if(Firebase.setInt(firebaseData, path + "/Int/Data1", 1)) {
      
      Serial.println("Inserted");
    }
  } else { //zero => reading
    // Dump debug info about the card; PICC_HaltA() is automatically called
    //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    byte uid[4] = {mfrc522.uid.uidByte[0], mfrc522.uid.uidByte[1], mfrc522.uid.uidByte[2], mfrc522.uid.uidByte[3]};
    String uidString = uid2String(uid);
    Serial.print("UID: ");
    Serial.println(uidString);
    String path = "/" + mac_addr + "/" + uidString;
    String access_level = check_db(path);
    Serial.println(access_level);
  }

  // bluetooth
  while (central.connected()) { // while the central is connected:
      long currentMillis = millis();

      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        
        if (switchChar.written()) {
          if (switchChar.value() == 1) {   // 1 => reading value
            Serial.println("Reading state");
            state = 0;
            digitalWrite(LED_BUILTIN, HIGH);         // will turn the LED on
          } else {                              // a 0 value
            Serial.println(F("Registering State"));
            state = 1;
            digitalWrite(LED_BUILTIN, LOW);          // will turn the LED off
          }
        }

        if(accessChar.written(){
          //this is where i would check access levels for registering new rfid
        }

      }
    }
  
  
  mfrc522.PICC_HaltA();
}
