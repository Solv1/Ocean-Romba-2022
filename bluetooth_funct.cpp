#include "bluetooth_funct.h"
bool BlueTooth::setup(){ // The BLE Setup Code was not fully written by me uses Adafruit UART Bluetooth.
  /* Initialise the module*/ 
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !(this->begin(VERBOSE_MODE) ))
  {
    return false;
  }
  Serial.println( ("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state*/
    Serial.println(("Performing a factory reset: "));
    if ( !this->factoryReset() ){
      Serial.println(("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit*/
  this->echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information*/ 
  this->info();

  Serial.println("Please use Connect using the Bluetooth App");
  Serial.println("Enter Commands to send to Bluefruit");
  Serial.println();

  this->verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection*/ 
  while (!this->isConnected()) {
      delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if ( this->isVersionAtLeast(MIN_FIRM) )
  {
    // Change Mode LED Activity
    this->sendCommandCheckOK("AT+HWModeLED=" MODE_LED_SET);
  }
  return true;
}


void BlueTooth::cmdCheck(OilSense* sensor_to_check,TinyGPSPlus* gps ){ 
  MatchState ms; // Reg Exp Object
  // Check for incoming characters from Bluefruit
  this->println("Waiting for Commands...");
  this->readline();
  if (strlen(this->buffer) == 0) {
    // no data
    return;
  }
  Serial.println(this->buffer);
  // Some data was found, its in the buffer
  Serial.print(("[Recv] ")); Serial.println(this->buffer);
  ms.Target(this->buffer);
  if(ms.Match("^oil$",0) == REGEXP_MATCHED){
    this->print("Current UV Reading: ");
    this->println(sensor_to_check->getUV());
    this->print("Current IR Reading: ");
    this->println(sensor_to_check->getIR());
  }
  else if(ms.Match("^gps$",0) == REGEXP_MATCHED){

    this->print(F("Location: ")); 
    if (gps->location.isValid())
    {
      this->print(gps->location.lat(), 6);
      this->print(F(","));
      this->print(gps->location.lng(), 6);
   }
    else
    {
      this->print(F("INVALID"));
    }
    
  }  
  else{
    
  }
  this->waitForOK();
  
}
void BlueTooth::blueToothSend(char* s){
  this->println(s);
}
