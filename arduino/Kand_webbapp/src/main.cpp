#include <Arduino.h>


volatile int32_t encodercts = 0;

void count(void) {
  if(digitalRead(3) == 1){
    encodercts += 1;
  }
  else{
    encodercts -= 1;
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), count, RISING);
  Serial.begin(57600);
}

void loop() {
  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);
  Serial.println(encodercts);
  delay(1000);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  Serial.println(encodercts);
  delay(500);
  delay(500);

}
