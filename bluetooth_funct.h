#ifndef bluetooth_funct.h
#define bluetooth_funct.h
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"


#include <Arduino.h>

#include <Regexp.h>
#include "sensor_funct.h"

#define BUFSIZE 128 //Size of Read Buffer
#define VERBOSE_MODE true //for debugging

#define BLUEFRUIT_RX_PIN 19
#define BLUEFRUIT_TX_PIN 18
#define BLUEFRUIT_CTS_PIN 13
#define BLUEFRUIT_RTS_PIN 12


#define FACTORYRESET_ENABLE 1
#define MIN_FIRM "0.6.6"
#define MODE_LED_SET "MODE"



class BlueTooth : public Adafruit_BluefruitLE_UART{
  public:
    using Adafruit_BluefruitLE_UART::Adafruit_BluefruitLE_UART;
    void cmdCheck(OilSense* sensor_to_check,TinyGPSPlus* gps);
    bool setup();
    void blueToothSend(char* s);
  private:
   

};
#endif