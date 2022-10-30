/*
Written by Cameron Garcia for use with the Ocean Roomba September 2022

Purpose: As a Main Function Implementing Written Libaries for the Ocean Roomba
*/
#include "sensor_funct.h"
#include "bluetooth_funct.h"
#include <Wire.h> // I2C Wire Libary



#define SLAVE_ADDR 9
#define RELAY_PIN 7
#define IR_RANGE 15
#define UV_RANGE 150
#define BLUEFRUIT_MODE_PIN 7
#define RXPIN 51
#define TXPIN 50


OilSense Sensor(IR_RANGE,UV_RANGE,RELAY_PIN);//Oil Sensor 
BlueTooth ble(Serial1,BLUEFRUIT_MODE_PIN);//Bluetooth Module
TinyGPSPlus gps;
SoftwareSerial ss(RXPIN, TXPIN);

byte oilFlagByte = 0;
Adafruit_BluefruitLE_UART* ble_point = NULL;

void receiveEvent(){
  byte isReady = Wire.read();
  if(isReady == 1 ){//indicates oil collection routine is done.
    oilFlagByte = 0;//this starts the oil samplinging system again.
  }
}
void requestEvent(){
  Wire.write(oilFlagByte);
  Serial.println("Flag Request Event");
  
}
void gpsRead(){
    while (ss.available() > 0){
      gps.encode(ss.read());
    }  
}
bool gpsSetup(){
    gpsRead();
    if(gps.location.isValid()){
      Serial.println(F("GPS FOUND LOCATION"));
      return true;
    }
    if(millis() > 5000 && gps.charsProcessed() < 10)
    {
      return false;
    }

    return false; 

}




void setup() {
  Serial.begin(115200);
  ss.begin(9600);
  while(!ble.setup()){//Bluetooth Module Set Up
    Serial.println("Bluetooth Module not setup trying again...");
  }
  while(!Sensor.calibration()){//Intesnity Sensor Setup
    Serial.println("Intensity Sensor not setup trying again...");
  }
  while(!gpsSetup()){
    Serial.println("GPS not setup trying again...");
    delay(1000);
  }

  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void loop(){
  gpsRead();
  ble.cmdCheck(&Sensor,&gps);
  oilFlagByte = Sensor.oilCheck();
  if(oilFlagByte){
    Wire.write(oilFlagByte);
    ble.blueToothSend("Oil Detected");
  }
  delay(100);
}
