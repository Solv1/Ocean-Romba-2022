#include <Servo.h>

#define BUTTON_PIN 3

Servo testservo;
int pos = 0;
int lastState = HIGH;
int currentState;

void setup() {
  Serial.begin(9600);
  testservo.attach(4); //attaches servo to PIN 4
  pinMode(BUTTON_PIN, INPUT_PULLUP);//intializes button pin with a interinal pullup resister connect button to ground and the pin.
}

void loop() {
  currentState = digitalRead(BUTTON_PIN);

  if((lastState == HIGH && currentState == LOW)&& pos == 0){ // Changes at button press and the pos is 0
      pos = 180;//change this to change the angle you want to go to.
      testservo.write(pos);
      lastState = LOW;
      Serial.print("Moving the servo to: ");
      Serial.print(pos);
      Serial.print(" Degrees \n");
      delay(1000);//so the servo has time to adjust
  }
  else if(pos != 0 && (lastState == LOW && currentState == HIGH)){//when button is released and the pos is not 0
    pos = 0;
    testservo.write(pos);
    lastState = HIGH;
    Serial.println("Moving the Servo back to 0");
    delay(1000);//so the servo has time to adjust
  }
  else{
    //do nothing if something weird is happening
  }
  

}
