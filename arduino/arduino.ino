#include <Arduino.h>

#define ZDriveF 7
#define ZDriveB 8
#define ZFeedb1 2
#define ZFeedb2 3

#define lampDebug 10

#ifndef DEVICE_ID
#define DEVICE_ID 0 // fallback ID
#endif

char buffer[64];
String incomingCommand = "";

// put function declarations here:
void ZFeedb1INTERRUPT();

void setup() {
  // put your setup code here, to run once:
  pinMode(ZDriveF, OUTPUT);
  pinMode(ZDriveB, OUTPUT);
  pinMode(lampDebug, OUTPUT);
  pinMode(ZFeedb1, INPUT);
  pinMode(ZFeedb2, INPUT);

  attachInterrupt(digitalPinToInterrupt(ZFeedb1), ZFeedb1INTERRUPT, RISING);

  Serial.begin(9600);
  while (!Serial); // Wait for serial port to connect (for some boards)
  sprintf(buffer, "ID:%d", DEVICE_ID);
  Serial.println(buffer);
}

int locationNumber = 0; //GOES FROM 0 TO ca 23000
int missionIndex = 0;

//mission 0 constants and variables:
int chargeUpDistance = -1;
int prevLocationNumber = locationNumber;
int stoppedChargeAt = 0;

int tests[] = {500, 2000, 4000, 10000};
int testIndex = 0;
int tryIndex = 0;
int forwOrBackwNum = 0;


//mission 0 results:
int stopDist = 0;
int stopDistSum = 0;

//mission 1 constants:

//mission 2 constants.
int targetLocationNumber = 10000;

//mission 3 variables:
int counter = 0;
int goInDir = 1;

void loop() {
  


  while (Serial.available() > 0) {
    char received = Serial.read();

    // If newline, process command
    if (received == '\n') {
      incomingCommand.trim(); // Remove any extra whitespace/newlines

      if (incomingCommand.equalsIgnoreCase("TOGGLE")) {
        Serial.println("Oi, whot exactly u want me to toggle?");
      } else {
        Serial.println("Unknown command.");
      }

      incomingCommand = ""; // Reset buffer
    } else {
      incomingCommand += received;
    }
  }




  analogWrite(lampDebug, 0.011*locationNumber*0.7);
  if(locationNumber + 200 > targetLocationNumber && locationNumber - 200 < targetLocationNumber){
    analogWrite(lampDebug, 255);
  }
  
  if(missionIndex == 0){
    if(locationNumber < chargeUpDistance){
      analogWrite(ZDriveF, 200);
      analogWrite(ZDriveB, 0);
    } else{
      analogWrite(ZDriveF, 0);
      analogWrite(ZDriveB, 0);
      if(stoppedChargeAt == 0){ //Just in case we dont stop exactly at the ChargeUpDistance. StoppedChargeAt ~ ChargeUpDistance
        stoppedChargeAt = locationNumber;
      }
      if(prevLocationNumber == locationNumber){
        stopDist = locationNumber - stoppedChargeAt;
        Serial.println(prevLocationNumber);
        Serial.println(locationNumber);
        Serial.print("Stop distance: ");
        Serial.println(stopDist);
        missionIndex = 1;
        stoppedChargeAt = 0;
        delay(500);
      }
    }
    prevLocationNumber = locationNumber;
    delay(10);
  } else if(missionIndex == 1){
	Serial.print("Location number: ");
    Serial.println(locationNumber);
	if(locationNumber > stopDist){
      analogWrite(ZDriveF, 0);
      analogWrite(ZDriveB, 200);
    } else{
      analogWrite(ZDriveF, 0);
      analogWrite(ZDriveB, 0);
      delay(500);
      missionIndex = 0;
    }
  } else if(missionIndex == 2){
    if(locationNumber < targetLocationNumber - stopDist){
      analogWrite(ZDriveF, 200);
      analogWrite(ZDriveB, 0);
    } else if(locationNumber > targetLocationNumber + stopDist){
      analogWrite(ZDriveF, 0);
      analogWrite(ZDriveB, 200);
    } else {
      analogWrite(ZDriveF, 0);
      analogWrite(ZDriveB, 0);
    }
    delay(10);
  } else if (missionIndex == 3){
    
    analogWrite(ZDriveF, 0);
    analogWrite(ZDriveB, 0);

    delay(250);

    analogWrite(ZDriveF, 0);
    analogWrite(ZDriveB, 200);

    delay(750);
    
    analogWrite(ZDriveF, 0);
    analogWrite(ZDriveB, 0);

    delay(250);

    analogWrite(ZDriveF, 200);
    analogWrite(ZDriveB, 0);

    delay(750);
  } else if(missionIndex == 4){
    Serial.print("Currently the counter is ");
    Serial.println(counter);
    delay(500);
    counter = counter + 1;
  }
}
  /*
  delay((int)dt*1000);
  if(counter > 100){
    referenceLum = 200;
  }

  double e = referenceLum - estimatedLum;
  E_integral = E_integral + e*dt;
  double E_derivative = (e-E_previous)/dt;
  double toChange = P*e + I*E_integral + D*E_derivative;
  E_previous = e;
*/
  // put your main code here, to run repeatedly: Check which arduino and change in platformio.ini depending 
  // which one you have
 /* for (int i = 0; i <= 255; i++) {
    analogWrite(ZDriveF, (int)(127.5*sin(i/40.585)+127.5));
    analogWrite(ZDriveB, (int)(127.5*cos(1.571+i/40.585)+127.5));
    delay(10);
  }*/
/*
  if(status == 1){
    digitalWrite(3, HIGH);
    digitalWrite(5, LOW);
  }
  if(status == -1){
    digitalWrite(3, LOW);
    digitalWrite(5, HIGH);
  }
*/
/*
  digitalWrite(3, HIGH);
  analogWrite(5, 25);
  delay(DEL);
  digitalWrite(3, LOW);
  analogWrite(5, 255);
  delay(DEL);
  DEL = DEL - 10;
  if(DEL < 5){
    DEL = 255;
  }
  if(digitalRead(ZFeedb2) == HIGH){
   // DEL = 50;
  }*/
  /*digitalWrite(3, HIGH);
  digitalWrite(5, LOW);
  delay(1600);
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
  delay(400);
  digitalWrite(3, LOW);
  digitalWrite(5, HIGH);
  delay(1600);
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
  delay(400);*/


void ZFeedb1INTERRUPT(){
  if(digitalRead(ZFeedb2) == HIGH){
    locationNumber += 1;
  }
  if(digitalRead(ZFeedb2) == LOW){
    locationNumber -= 1;
  }
}

// put function definitions here:
/*double sin(double x) {
  int zone = 0;
  while(x > 1.571){
    x = x - 3.1415;
    zone = zone + 1;
  }
  double toRet = x - x*x*x/6 + x*x*x*x*x/120 - x*x*x*x*x*x*x/5040;
  if(zone % 2 == 1) toRet = -toRet;
  return toRet; 
}*/
