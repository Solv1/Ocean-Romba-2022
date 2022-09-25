//Using I2C Addresses 0x10, 0x14, 0x18 and 0x1C

#include <Wire.h>     // Arduino standard I2C/Two-Wire Library
#include "printf.h"
#include <Servo.h>    //Servo library
#include <TFMPI2C.h>  // TFMini-Plus I2C Library v1.7.0
#include <SoftwareSerial.h> //Serial Connection Libary 

#define servoPin 11
#define servoRight 30
#define servoMid 87
#define servoLeft 140

TFMPI2C tfmP1;        // Create a TFMini-Plus I2C object
Servo servo;          //create servo obj
Servo thruster;       //create thruster obj
SoftwareSerial oilSerial(2,3); // RX, TX Using Pins other then 0 and 1 because of the USB Sieral connection present. 

byte thrusterPin = 10;

// Initialize data variables
int16_t tfDist1 = 0;  // Distance to object in centimeters
int16_t tfDist2 = 0;  // Distance to object in centimeters
int16_t tfDist3 = 0;  // Distance to object in centimeters
int16_t tfDist4 = 0;  // Distance to object in centimeters
char distStr[6];      // distance data string
boolean oilFlag = false; // Passed Oil Flag from Oil Sensor

unsigned long previousMillis = 0UL;
const long interval = 5000UL;

void oilSenseRoutine(){
//Called when Oil Sense Flag is Set TRUE
  
}
  
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

void setup()
{
    oilSerial.begin(115200); // Init OilSensor Serial Port
    Serial.begin(115200);   // Initialize terminal serial port
    printf_begin();          // Initialize printf library.
    delay(20);
    servo.attach(servoPin,544,2400); //we are using this pin for pwm
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
//    Wire.begin();            // Initialize two-wire interface
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

    if(oilSerial.available())
    {
      oilFlag = oilSerial.read();  
    }
    if(oilFlag)
    {
      oilSenseRoutine();    
    }
    unsigned long currentMillis = millis();
    
    tfmP1.getData( tfDist1, 0x10);  // Get a frame of data (left lidar)
    delay(30);
    tfmP1.getData( tfDist2, 0x14);  // Get a frame of data (left front lidar)
    delay(30);
    tfmP1.getData( tfDist3, 0x18);  // Get a frame of data (right lidar)
    delay(30);
    tfmP1.getData( tfDist4, 0x1C);  // Get a frame of data (right front lidar)
   
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
  
  //SECTION COMMENTED OUT BELOW WAS FOR TESTING PURPOSES,
  //AND MAY STILL BE NEEDED FOR NOW.
    /*
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
