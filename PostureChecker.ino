#include <ArduinoBLE.h>
#include<ArduinoJson.h>


BLEService newService("180A"); // creating the service

BLEStringCharacteristic resistanceBroadcast("2A58", BLERead | BLENotify, 256); // creating the Analog Value characteristic
int leftPin = 0;
int middlePin = 3;
int rightPin = 6;
int Vin = 3.3;
int R1 = 1800;

void setup(){
  Serial.begin(9600);
  initBLE();
}

void loop(){
  BLEDevice central = connectBLE();
  if (central) {
    // check the battery level every 200ms
    // while the central is connected:
    while (central.connected()) {
        Serial.print("Left");
        int left = getResistance(leftPin);
        Serial.print("Middle");
        int middle = getResistance(middlePin);
        Serial.print("Right");
        int right = getResistance(rightPin);
        JsonDocument data;
        data["left"] = left;
        data["middle"] = middle;
        data["right"] = right;
        char output[256];
        serializeJson(data, output);
        resistanceBroadcast.writeValue(output);
        delay(1000);
    }
    
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());

  }
}

int getResistance(int pin) {
  int raw = analogRead(pin);
  if (raw) {  
    float buffer = raw * Vin;
    float Vout = (buffer)/1024.0;
    buffer = (Vin/Vout) - 1;
    int R2 = R1 * buffer;
    Serial.print("Vout: ");
    Serial.println(Vout);
    Serial.print("R2: ");
    Serial.println(R2);
    return R2;
  }
}

void initBLE() {
  // initialize ArduinoBLE library
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }

  BLE.setLocalName("PostureChecker"); //Setting a name that will appear when scanning for Bluetooth® devices
  BLE.setAdvertisedService(newService);

  newService.addCharacteristic(resistanceBroadcast);

  BLE.addService(newService);  // adding the service

  resistanceBroadcast.writeValue("");

  BLE.advertise(); //start advertising the service
  Serial.println(" Bluetooth® device active, waiting for connections...");
}

BLEDevice connectBLE() {
  BLEDevice central = BLE.central(); // wait for a Bluetooth® Low Energy central
  if (central) {  // if a central is connected to the peripheral
    Serial.print("Connected to central: ");
    Serial.println(central.address()); // print the central's BT address
    return central;
  }
}