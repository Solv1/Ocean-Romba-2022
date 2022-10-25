#include "sensor_funct.h"
#include "SparkFun_AS7265X.h"
/* Sensor Fucntions for the Ocean Roomba 
/Writen by Cameron Garcia CPE 191 October 2022
*/

OilSense::OilSense(float ir, float uv, int pp)
{
  pump_pin = pp;
  pinMode(pump_pin, OUTPUT);
  oilVal = 0;
  ir_range = ir;
  uv_range = uv;
  ir_cal = 0;
  uv_cal = 0;
  curUV = 0;
  curIR = 0;
  AS7265X Sensor; 

}
float OilSense::getIR(){
  return curIR;
}
float OilSense::getUV(){
  return curUV;
}
void OilSense::pumpWater(int duration)//--> Pump water w. pump at pump_pin
{
   digitalWrite(pump_pin, HIGH);
   delay(duration * 1000);
   digitalWrite(pump_pin, LOW);

}
void OilSense::calibration(){//--> Calibrates sensor and sets values intial compariator values based on clean water
  if(Sensor.begin() == false){
    Serial.println("Check Sensor Wiring"); // Intializes AS7265X Sensor
  }
  else{
  OilSense::pumpWater(5);
  Serial2.println("Calibration in progress please wait...");
  Sensor.disableIndicator();
  for(int i = 0; i < 5; i++){
    OilSense::pumpWater(1);
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
  
  uv_cal = uv_cal / 5;
  ir_cal = ir_cal / 5;

  Serial.println("Calibration Done: Values are --> UV 410nm: ");
  Serial.println(uv_cal);
  Serial.println(" IR 860nm: ");
  Serial.println(ir_cal);
  }
}
int OilSense::oilCheck()//-->Checks for Oil based on Calibration values and Defined Ranges
{


    OilSense::pumpWater(5);
    Sensor.enableBulb(AS7265x_LED_UV); //UV LED
    Sensor.takeMeasurements();
    Sensor.disableBulb(AS7265x_LED_UV);

    curUV = Sensor.getCalibratedA(); // 410nm

    Sensor.enableBulb(AS7265x_LED_IR); //IR LED
    Sensor.takeMeasurements();
    Sensor.disableBulb(AS7265x_LED_IR);

    curIR = Sensor.getCalibratedW(); //860nm
    
// Logic Section of my Oil Sensing Code with Constants Obtained From a Black Sensing Box with Light Cut to a Minimum
  if(curIR > (ir_cal+ir_range)) { //For more information on how I got these constant values please check my speadsheet.
      Serial.println(ir_cal);
      Serial.println(curIR);
      Serial.println("\nOil Detected with the 860nm wavelength\n");
      oilVal = 1;
  }
  else if((curUV > (uv_cal + uv_range))){ 
    Serial.println(uv_cal);
    Serial.println(curUV);
    Serial.println("\nOil Detected with the 410nm wavelength\n");
    oilVal = 1;
  }
  else{
    Serial.println("\nOil Not Detected Running Check Again...\n");
    oilVal = 0;
  }
  return oilVal;

}
