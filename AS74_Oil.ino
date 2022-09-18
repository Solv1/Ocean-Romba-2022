/*
Written by Cameron Garcia for use with the Ocean Rumba September 2022

Purpose: To interface with the AS7265X Sensor and read wavelength intensty values from samples put in front of the sensor.
Additonaly to pump water in and out of the sampling area.
*/
#include "SparkFun_AS7265X.h"

#define RELAY_PIN 7
#define LED_READY 8
#define LED_SENSE 9

AS7265X Sensor;

float uv_cal, ir_cal; // UV Wavelength Value is 410nm, IR Wavelength Value is 860nm

void pumpWater(int duration){
   digitalWrite(RELAY_PIN, HIGH);
   delay(duration * 100);
   digitalWrite(RELAY_PIN, LOW);
}

void calibration(){
  Serial.println("Calibration in progress please wait...");
  float uv_cal = 0;
  float ir_cal = 0;
  Sensor.disableIndicator();
  for(int i = 0; i < 10; i++){
    pumpWater(1);
    Sensor.enableBulb(AS7265x_LED_UV);
    Sensor.takeMeasurements();
    Sensor.disableBulb(AS7265x_LED_UV);
    uv_cal = Sensor.getCalibratedA() + uv_cal;
    Sensor.enableBulb(AS7265x_LED_IR);
    Sensor.takeMeasurements();
    Sensor.disableBulb(AS7265x_LED_IR);
    ir_cal = Sensor.getCalibratedW() + ir_cal;
    delay(1000);
    }
  
  uv_cal = uv_cal / 10;
  ir_cal = ir_cal / 10;



  Serial.println("Calibration Done: Values are --> UV 410nm: ");
  Serial.println(uv_cal);
  Serial.println(" IR 860nm: ");
  Serial.println(ir_cal);
  Serial.println(" | ");
}


void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  //pinMode(LED_READY, OUTPUT);
  //pinMode(LED_SENSE, OUTPUT);
  Serial.begin(115200);
  if (Sensor.begin() == false)  
  {
    Serial.println("Error Please Check Connection of Sensor");
    
  }
  pumpWater(10);
  calibration();

}

void loop() {
  float wave410, wave860;
  pumpWater(1);
  Sensor.enableBulb(AS7265x_LED_UV);
  Sensor.takeMeasurements();
  Sensor.disableBulb(AS7265x_LED_UV);

  wave410 = Sensor.getCalibratedA(); // 410nm

  Sensor.enableBulb(AS7265x_LED_IR);
  Sensor.takeMeasurements();
  Sensor.disableBulb(AS7265x_LED_IR);

  wave860 = Sensor.getCalibratedW(); //860nm

 // Note that orginal constant values were obtained using 10 W 40 motor oil with a white background. Results may vary with different backgrounds.
/*  if(wave410 < cal410 - 1400){
    Serial.println("Oil Detected with 410nm wavelength");
    digitalWrite(LED_SENSE,HIGH);
  }
  else if(wave435 > cal435 + 100){
    Serial.println("Oil Detected with 435nm wavelength");
    digitalWrite(LED_SENSE,HIGH);
  }
  else if(wave460 < 150){
    Serial.println("Oil Detected with 460nm Wavelength");
    digitalWrite(LED_SENSE,HIGH);
  }
  else{
    Serial.println("Oil Not Detected Yet");
    digitalWrite(LED_SENSE,LOW);
  }
*/ 
   Serial.print("UV 410nm: ");
   Serial.print(wave410);
   Serial.print(" IR 860nm: ");
   Serial.print(wave860);
   Serial.print("| ");
  
  

  //Waits 10 Seconds before next reading. Need to Increase before use on craft
  delay(10000);

 

  

}
