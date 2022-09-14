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

float cal410, cal435, cal460;

void pumpWater(){
   digitalWrite(RELAY_PIN, HIGH);
   delay(1000);
   digitalWrite(RELAY_PIN, LOW);
}

void calibration(){
  Serial.println("Calibration in progress please wait...");
  float a410 = 0;
  float a435 = 0;
  float a460 = 0;
  Sensor.disableIndicator();
  for(int i = 0; i < 10; i++){
    pumpWater();
    Sensor.enableBulb(AS7265x_LED_UV);
    Sensor.takeMeasurements();
    Sensor.disableBulb(AS7265x_LED_UV);
    if(i != 0){
    a410 = Sensor.getCalibratedA() + a410;
    a435 = Sensor.getCalibratedB() + a435;
    a460 = Sensor.getCalibratedC()+ a460;
    delay(10000);
    }
  }
  cal410 = a410 / 10;
  cal435 = a435 / 10;
  cal460 = a460 / 10;



  Serial.println("Calibration Done: Values are --> 410nm: ");
  Serial.println(cal410);
  Serial.println(" 435nm: ");
  Serial.println(cal435);
  Serial.print(" 460nm: ");
  Serial.print(cal460);
  Serial.print(" | ");
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(LED_SENSE, OUTPUT);
  Serial.begin(115200);
  if (Sensor.begin() == false)  
  {
    Serial.println("Error Please Check Connection of Sensor");
    
  }
  calibration();

}

void loop() {
  float wave410, wave435, wave460;
  pumpWater();
  Sensor.enableBulb(AS7265x_LED_UV);
  Sensor.takeMeasurements();
  Sensor.disableBulb(AS7265x_LED_UV);

  wave410 = Sensor.getCalibratedA();
  wave435 = Sensor.getCalibratedB();
  wave460 = Sensor.getCalibratedC();


  // Note that orginal constant values were obtained using 10 W 40 motor oil with a white background. Results may vary with different backgrounds.
  if(wave410 < cal410 - 1400){
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
  /*
  *for testing purposes
  *Serial.print(wave410);
  *Serial.print(" , ");
  *Serial.print(wave435);
  *Serial.print(" , ");
  *Serial.print(wave460);
  *Serial.print("| ");
  */
  

  //Waits 10 Seconds before next reading. Need to Increase before use on craft
  delay(10000);

 

  

}
