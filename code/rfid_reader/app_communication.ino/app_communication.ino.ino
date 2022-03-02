#include <ArduinoBLE.h>

BLEService newService("19B10010-E8F2-537E-4F6C-D104768A1214"); // creating the service

BLEByteCharacteristic switchChar("19B10010-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite); // creating the LED characteristic

const int ledPin = 2;
long previousMillis = 0;
int state = 0; //if state is 0, the mode is set to read, otherwise it is to register

void setup() {
  Serial.begin(9600);    // initialize serial communication
  while (!Serial);       //starts the program if we open the serial monitor.

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

  BLE.addService(newService);  // adding the service

  switchChar.writeValue(0); //set initial value for characteristics

  BLE.advertise(); //start advertising the service
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {

  BLEDevice central = BLE.central(); // wait for a BLE central

  if (central) {  // if a central is connected to the peripheral
    Serial.print("Connected to central: ");

    Serial.println(central.address()); // print the central's BT address

//    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection



    while (central.connected()) { // while the central is connected:
      long currentMillis = millis();

      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        
        if (switchChar.written()) {
          if (switchChar.value()) {   // any value other than 0
            Serial.println("Reading state");
            digitalWrite(LED_BUILTIN, HIGH);         // will turn the LED on
          } else {                              // a 0 value
            Serial.println(F("Registering State"));
            state = 1;
            digitalWrite(LED_BUILTIN, LOW);          // will turn the LED off
          }
        }

      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
