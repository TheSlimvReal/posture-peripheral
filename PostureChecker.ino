#include <ArduinoBLE.h>


BLEService newService("180A"); // creating the service

BLEStringCharacteristic resistanceBroadcast("2A58", BLERead | BLENotify, 4); // creating the Analog Value characteristic
int analogPin = 0;
int Vin = 3.3;
int R1 = 800;

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
        int resistance = getResistance();
        resistanceBroadcast.writeValue(String(resistance));
        delay(1000);
    }
    
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());

  }
}

int getResistance() {
  int raw = analogRead(analogPin);
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