#ifndef bluetooth_funct.h
#define bluetooth_funct.h
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
#define BLUEFRUIT_MODE_PIN 7

#define FACTORYRESET_ENABLE 1
#define MIN_FIRM "0.6.6"
#define MODE_LED_SET "MODE"

class BlueTooth : private Adafruit_BluefruitLE_UART{
  public:
    using Adafruit_BluefruitLE_UART::Adafruit_BluefruitLE_UART;
    void cmdCheck(OilSense* sensor_to_check);
    void setup();
  private:
    bool getUserInput(char buffer[], uint8_t maxSize);
    char input[BUFSIZE+1];


};
#endif