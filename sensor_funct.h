#ifndef sensor_funct.h
#define sensor_funct.h
#include "Arduino.h"
#include "SparkFun_AS7265X.h"
/* Sensor Fucntions for the Ocean Roomba 
/Writen by Cameron Garcia CPE 191 October 2022
*/
class OilSense
{
  public:
    OilSense(float ir, float uv, int pump_pin);
    void pumpWater(int duration);
    int oilCheck();
    void calibration();
    float getIR();
    float getUV();
    AS7265X Sensor; 
  private:
    int pump_pin;
    float ir_range;
    float uv_range;
    float ir_cal;
    float uv_cal;
    float curIR;
    float curUV;
    int oilVal;
    
};


#endif