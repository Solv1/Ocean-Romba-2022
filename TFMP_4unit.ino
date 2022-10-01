//Using I2C Addresses 0x10, 0x14, 0x18 and 0x1C

#include <Wire.h>         // Arduino standard I2C/Two-Wire Library
#include "printf.h"
#include <Servo.h>        //Servo library
#include <TFMPI2C.h>      // TFMini-Plus I2C Library v1.7.0 
#include "NewPing.h"      //library for USS

#define TRIGGER_PIN 3     //pin used for usds
#define ECHO_PIN 3        //pin used for usds
#define MAX_DISTANCE 400  //max distance for usds

#define servoPin 11       //servo pin used for steering craft (180 deg)
#define servoPin2 6       //servo pin used for belt system (180 deg)
#define servoPin3 5       //servo pin used for belt system (360 deg)
#define servoRight 30     //30
#define servoMid 90       //87
#define servoLeft 140
#define SLAVE_ADDR 9
#define ANSWERSIZE 1

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //create a sonar obj
TFMPI2C tfmP1;            // Create a TFMini-Plus I2C object
Servo servo;              //create servo obj for steering craft (180 deg)
Servo servo2;             //create servo obj for belt system (180 deg)
Servo servo3;             //create servo obj for belt system (360 deg)
Servo thruster;           //create thruster obj 

byte thrusterPin = 10;
int duration = 0;
int distance = 0;

// Initialize data variables
int16_t tfDist1 = 0;      // Distance to object in centimeters
int16_t tfDist2 = 0;      // Distance to object in centimeters
int16_t tfDist3 = 0;      // Distance to object in centimeters
int16_t tfDist4 = 0;      // Distance to object in centimeters
char distStr[6];          // distance data string
boolean oilFlag = false;  // Passed Oil Flag from Oil Sensor

unsigned long previousMillis = 0UL;
const long interval = 5000UL;
  
void commands( uint8_t addr)
{
    printf("Set up device at address 0x%2x\r\n", addr);
    // - - Perform a system reset - - - - - - - - - - -
    printf( "System reset: ");
    if( tfmP1.sendCommand( SOFT_RESET, 0, addr) )
    {
        printf( "passed.\r\n");
    }
    else tfmP1.printReply();
    delay(500);  //  Wait for device to complete Reset

    // - - Set the data frame-rate to 100 - - - - - - - - -
    //  Frame rate directly proportional to noisiness of data
    printf( "Data-Frame rate: ");
    if( tfmP1.sendCommand( SET_FRAME_RATE, FRAME_100, addr))
    {
        printf( "%2uHz.\r\n", FRAME_100);
    }
    else tfmP1.printReply();
    delay(500);                    // wait for half a second.
}

void enable_belt(int distance)
{
  //Will add more code here once I'll know how the belt fits together.
  if (distance <= 10) //10 cm
  {
    //turn off belt
  }
  else
  {
    if (Wire.available()) 
    {
      oilFlag = Wire.read();
    }
    while(oilFlag) {
      //turn on belt
      if (!oilFlag) 
      {
        //no more oil
        break;
      }
      //servo2.write();
      //servo3.write(); 
    }
  }
}

void setup()
{
    Serial.begin(115200);   // Initialize terminal serial port
    printf_begin();          // Initialize printf library.
    delay(20);
    servo.attach(servoPin,544,2400);
    servo2.attach(servoPin2,544,2400);
    servo3.attach(servoPin3,544,2400);
    thruster.attach(thrusterPin);
    delay(500);

    if (tfmP1.sendCommand( SET_I2C_MODE,0))
    {
      printf( "mode set.\r\n");
    }
    else tfmP1.printReply();
  
    printf("save settings: ");
  
    if(tfmP1.sendCommand( SAVE_SETTINGS,0))
    {
      printf("saved.\r\n");
    }
    else tfmP1.printReply();

    // Recover I2C Bus from any data hang.
    // This function includes `Wire.begin()`
    tfmP1.recoverI2CBus();
    //Wire.begin();            // Initialize two-wire interface
    Wire.setClock( 400000);  // Set I2C bus speed to 'Fast' if
                               // your Arduino supports 400KHz.

    // Send lidar addresses to interface with
    commands( 0x10);
    commands( 0x14);
    commands( 0x18);
    commands( 0x1C);
}

void loop()
{
    unsigned long currentMillis = millis();
    
    tfmP1.getData( tfDist1, 0x10);  // Get a frame of data (left lidar)
    delay(30);
    tfmP1.getData( tfDist2, 0x14);  // Get a frame of data (left front lidar)
    delay(30);
    tfmP1.getData( tfDist3, 0x18);  // Get a frame of data (right lidar)
    delay(30);
    tfmP1.getData( tfDist4, 0x1C);  // Get a frame of data (right front lidar)
    
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(0);
    Wire.endTransmission();
    Wire.requestFrom(SLAVE_ADDR,ANSWERSIZE);

    duration = sonar.ping();
    delay(15);
    distance = round((duration / 2) * 0.0343); //get distance measurement for level-sensor

    if (Wire.available()) 
    {
      oilFlag = Wire.read();
    }

    if (!oilFlag) {
      if ((tfDist2 <= 62 && tfDist4 <= 62 && tfDist2 != 0 && tfDist4 != 0) || (tfDist3 <= 62 && tfDist4 <= 62 && tfDist3 != 0 && tfDist4 != 0) || (tfDist3 <= 62 && tfDist2 >= 62 && tfDist4 >= 62 && tfDist3 != 0 && tfDist2 != 0 && tfDist4 != 0) || (tfDist4 <= 62 && tfDist4 != 0))
      {
        servo.write(servoRight);
        thruster.writeMicroseconds(1650);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1650);
        }
      }
      else if ((tfDist1 <= 62 && tfDist2 <= 62 && tfDist1 != 0 && tfDist2 != 0) || (tfDist1 <= 62 && tfDist2 >= 62 && tfDist4 >= 62 && tfDist1 != 0 && tfDist2 != 0 && tfDist4 != 0 ) || (tfDist2 <= 62 && tfDist2 != 0)) 
      {
        servo.write(servoLeft);
        thruster.writeMicroseconds(1650);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1650);
        }
      }
      else
      {
        printf("Distance1: %d\n",tfDist1);
        printf("Distance2: %d\n",tfDist2);
        printf("Distance3: %d\n",tfDist3);
        printf("Distance4: %d\n",tfDist4);
        servo.write(servoMid);
        thruster.writeMicroseconds(1650);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1650);
        }
      }
    }
    else {
      thruster.writeMicroseconds(1500);
      if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
      {
        previousMillis = currentMillis;
        thruster.writeMicroseconds(1500);
      }
      printf("calling enable_belt fnc\n");
      //call enable_belt(distance);
    }
}
/*
  //SECTION COMMENTED OUT BELOW WAS FOR TESTING PURPOSES,
  //AND MAY STILL BE NEEDED FOR NOW.
    
    if (tfDist1 <= 31 && tfDist1 != 0) 
    {
      printf("0x10 (RIGHT FRONT SENSOR) is less than 31cm\n");
      printf("Distance1: %d\n",tfDist1);
      servo.write(servoRight);
      thruster.writeMicroseconds(1650);
      if (currentMillis - previousMillis > interval) //wait delay (is interruptable)
      {
        previousMillis = currentMillis;
        thruster.writeMicroseconds(1650);
      }
    }
    else if(tfDist2 <= 31 && tfDist2 != 0)
    {
      printf("0x14 (FRONT LEFT SENSOR) is less than 31cm\n");
      printf("Distance2: %d\n",tfDist2);
      servo.write(servoLeft);
      thruster.writeMicroseconds(1650);
      if (currentMillis - previousMillis > interval) //should wait for 10 seconds before turning off thruster.
      {
        previousMillis = currentMillis;
        thruster.writeMicroseconds(1650);
      }
    }
    else if(tfDist3 <= 31 && tfDist3 != 0)
    {
      printf("0x18 (RIGHT SENSOR) is less than 31cm\n");
      printf("Distance3: %d\n",tfDist3);
      servo.write(servoLeft);
      thruster.writeMicroseconds(1650);
      if (currentMillis - previousMillis > interval) //should wait for 10 seconds before turning off thruster.
      {
        previousMillis = currentMillis;
        thruster.writeMicroseconds(1650);
      }
    }
    else if(tfDist4 <= 31 && tfDist4 != 0)
    {
      printf("0x1C (RIGHT SENSOR) is less than 31cm\n");
      printf("Distance4: %d\n",tfDist4);
      servo.write(servoRight);
      thruster.writeMicroseconds(1650);
      if (currentMillis - previousMillis > interval) //should wait for 10 seconds before turning off thruster.
      {
        previousMillis = currentMillis;
        thruster.writeMicroseconds(1650);
      }
    }
    else
    {
      printf("Distance1: %d\n",tfDist1);
      printf("Distance2: %d\n",tfDist2);
      printf("Distance3: %d\n",tfDist3);
      printf("Distance4: %d\n",tfDist4);
      servo.write(servoMid);
      thruster.writeMicroseconds(1650);
      if (currentMillis - previousMillis > interval) //should wait for 10 seconds before turning off thruster.
      {
        previousMillis = currentMillis;
        thruster.writeMicroseconds(1650);
      }
    }
}
*/
