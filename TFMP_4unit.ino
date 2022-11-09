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
#define servoPin2 6       //servo pin used for belt system (360 deg)
#define servoPin3 5       //servo pin used for lift system (360 deg)
#define servoRight 15     //30
#define servoMid 90       //90
#define servoMidd2 135
#define servoRMidd2 45
#define servoLeft 180     //140
#define SLAVE_ADDR 9
#define ANSWERSIZE 1

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //create a sonar obj
TFMPI2C tfmP1;            // Create a TFMini-Plus I2C object
Servo servo;              //create servo obj for steering craft (180 deg)
Servo beltServo;             //create servo obj for belt system (360 deg)
Servo liftServo;             //create servo obj for belt system (360 deg)
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
    Serial.print("Running the lift Servo!\n");
    liftServo.write(20); //run the lift servo
    delay(10000);
    Serial.print("Stopping the lift Servo!\n");
    liftServo.write(90); //stop the lift servo
    Serial.print("Running the belt Servo!\n");
    beltServo.write(1); //run the belt servo
    delay(60000);
    Serial.print("Stopping the belt Servo!\n");
    beltServo.write(90); //stop the belt servo
    Serial.print("Lifting back the lift Servo!\n");
    liftServo.write(160); //lift back up the belt system
    delay(11000);
    Serial.print("Stopping the lift Servo!\n");
    liftServo.write(90); //stop the lift servo
    /*
    if (Wire.available()) 
    {
      oilFlag = Wire.read();
    }
    if ((!oilFlag) || (distance <= 10)) 
    {
      //no more oil or tank is full
      //turn off belt and raise belt back up
      servo3.write(90); //stop the 360 degree servo
      //might have to add a delay here? Need to test to verify.
      //servo2.write(20); //pick up the belt (180 degree servo)
      break;
    }
    */
}

void setup()
{
    Serial.begin(115200);   // Initialize terminal serial port
    printf_begin();          // Initialize printf library.
    delay(20);
    servo.attach(servoPin,544,2400);
    beltServo.attach(servoPin2);
    liftServo.attach(servoPin3);
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
    delay(30);
    //distance values will show 0 if distance is too far.
    
    //Wire.beginTransmission(SLAVE_ADDR);
    //Wire.write(0);
    //Wire.endTransmission();
    //Wire.requestFrom(SLAVE_ADDR,ANSWERSIZE);

    //duration = sonar.ping();
    //delay(15);
    //distance = round((duration / 2) * 0.0343); //get distance measurement for level-sensor

    if (Wire.available()) 
    {
      oilFlag = Wire.read();
    }

    if (!oilFlag) {
      if ((tfDist4 <= 120 && tfDist2 > tfDist4 && tfDist2 != 0 && tfDist4 != 0) || (tfDist3 <= 20 && tfDist4 <= 120 && tfDist3 != 0 && tfDist4 != 0))
      {
        printf("Turn Servo Right");
        servo.write(servoRight);
        thruster.writeMicroseconds(1305);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1305);
        }
      }
      else if ((tfDist2 <= 120 && tfDist2 < tfDist4 && tfDist2 != 0 && tfDist4 != 0)||(tfDist1 <= 20 && tfDist2 <= 120 && tfDist1 != 0 && tfDist2 != 0)) 
      {
        printf("Turn Servo Left");
        servo.write(servoLeft);
        thruster.writeMicroseconds(1305);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1305);
        }
      }
      else if (tfDist1 <= 90 && tfDist2 <= 120 && tfDist4 <= 120 && tfDist1 != 0 && tfDist2 != 0 && tfDist4 != 0)
      {
        printf("Turn Servo Left");
        servo.write(servoLeft);
        thruster.writeMicroseconds(1305);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1305);
        }
      }
      else if (tfDist3 <= 90 && tfDist2 <= 120 && tfDist4 <= 120 && tfDist3 != 0 && tfDist2 != 0 && tfDist4 != 0)
      {
        printf("Turn Servo Right");
        servo.write(servoRight);
        thruster.writeMicroseconds(1305);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1305);
        }
      }
      else if (tfDist1 <= 20 && tfDist2 >= 120 && tfDist4 >= 120 && tfDist1 != 0 && tfDist2 != 0 && tfDist4 != 0)
      {
        printf("Turn Servo A Bit Left");
        servo.write(servoMidd2);                      //turns servo a bit left.
        thruster.writeMicroseconds(1305);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1305);
        }
      }
      else if (tfDist3 <= 20 && tfDist2 >= 120 && tfDist4 >= 120 && tfDist3 != 0 && tfDist2 != 0 && tfDist4 != 0)
      {
        printf("Turn Servo A Bit Right");
        servo.write(servoRMidd2);                     //turns servo a bit right.
        thruster.writeMicroseconds(1305);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1305);
        }
      }
      else
      {
        printf("Distance1: %d\n",tfDist1);
        printf("Distance2: %d\n",tfDist2);
        printf("Distance3: %d\n",tfDist3);
        printf("Distance4: %d\n",tfDist4);
        servo.write(servoMid);
        thruster.writeMicroseconds(1400);
        if(currentMillis - previousMillis > interval) //wait delay (is interruptable)
        {
          previousMillis = currentMillis;
          thruster.writeMicroseconds(1400);
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
