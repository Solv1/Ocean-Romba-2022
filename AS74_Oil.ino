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



OilSense Sensor(IR_RANGE,UV_RANGE,RELAY_PIN);
BlueTooth ble(Serial1,BLUEFRUIT_MODE_PIN);

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


void setup() {
  Serial.begin(115200);
  ble.setup();
  Sensor.calibration();

  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void loop(){
  ble.cmdCheck(&Sensor);
  oilFlagByte = Sensor.oilCheck();
  delay(1000);
}
