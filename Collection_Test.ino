/* Basic Test Code for the Pump System
/ Written by Cameron Garcia CpE 191
/ October 8th, 2022*/

#define RELAY_PIN 2

void setup() {
  pinMode(RELAY_PIN, OUTPUT);  
  Serial.begin(115200);
}

void loop() {
  Serial.println("Pumping Water... \n");
  digitalWrite(RELAY_PIN, HIGH);
  delay(5000);
  Serial.println("Water Pump Turned off\n");
  digitalWrite(RELAY_PIN,LOW);
  delay(5000);

}
