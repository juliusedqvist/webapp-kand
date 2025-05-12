#include <Arduino.h>


#define ZDriveF 9
#define ZDriveB 10
#define ZFeedb1 2 //The motors channel B, I think. It is synchronius with channel Z
#define ZFeedb2 5 //The motors channel A, I think
#define ZFeedbVarv 3 //The motors channel Z
//#define ZFeedbHitEnd 0


#define lampDebug 6


#ifndef DEVICE_ID
#define DEVICE_ID 0 // fallback ID
#endif


char buffer[64];
String incomingCommand = "";


//0: Stand still
//1: Move to targetLocationNumber
//2: Reset
int missionIndex = 1;



float locationNumber = 0; //GOES FROM 0 TO ca 23000
float varvNumber = 0;
float varvNumber2 = 0;
int prevLocationNumber = locationNumber;


int targetLocationNumber = 8000;




float P = 0.0004;
float I = 0.000000005;//.00000025;
float antistuckCurrentPWMBonus = 0;
float D = 0.045; //this is close to the dynamic friction constant, it seems
float integral = 0;


float delayTime = 20.0;
float PWMFraction = 0.0;
float generalSpeedFactor = 0.5;
int movementDir = 0; //-1 for backwards, +1 for forwards, 0 for standing still




// put function declarations here:
void ZFeedb1INTERRUPT();
void ZFeedbVarvINTERRUPT();


void setup() {
  pinMode(ZDriveF, OUTPUT);
  pinMode(ZDriveB, OUTPUT);
  pinMode(lampDebug, OUTPUT);
  pinMode(ZFeedb1, INPUT);
  pinMode(ZFeedb2, INPUT);
  pinMode(ZFeedbVarv, INPUT);
 // pinMode(ZFeedbHitEnd, INPUT);


  attachInterrupt(digitalPinToInterrupt(ZFeedb1), ZFeedb1INTERRUPT, RISING);
  attachInterrupt(digitalPinToInterrupt(ZFeedbVarv), ZFeedbVarvINTERRUPT, RISING);


  Serial.begin(9600);
  while (!Serial); // Wait for serial port to connect (for some boards)
  sprintf(buffer, "ID:%d", DEVICE_ID);
  Serial.println(buffer);
}



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

      if(incomingCommand.equalsIgnoreCase("RESET")){
        missionIndex = 2;
      } else if(incomingCommand.equalsIgnoreCase("STOP")){
        missionIndex = 0;
      } else {
        missionIndex = 1;
      }


      incomingCommand = ""; // Reset buffer
    } else {
      incomingCommand += received;
    }
  }




  float speedNDir = 0;
  if(missionIndex == 1){
    float e = targetLocationNumber - locationNumber;
    float derivative = (locationNumber - prevLocationNumber)/delayTime;
    speedNDir = I*integral + P*e + D*derivative + antistuckCurrentPWMBonus;
  
  
    if(prevLocationNumber != locationNumber){
      integral = integral + e*delayTime;
    }
    if(1000*derivative < 250){
      if(abs(locationNumber - targetLocationNumber) < 1000){
        if(locationNumber > targetLocationNumber){
          antistuckCurrentPWMBonus = antistuckCurrentPWMBonus - 0.2*delayTime/1000;// * (1+antistuckCurrentPWMBonus);
        } else{
          antistuckCurrentPWMBonus = antistuckCurrentPWMBonus + 0.2*delayTime/1000;// * (1-antistuckCurrentPWMBonus);
        }
      }
    }
    prevLocationNumber = locationNumber;
   
    if(locationNumber > targetLocationNumber - 5 && speedNDir > 0){
      speedNDir = 0;
      integral = 0;
      antistuckCurrentPWMBonus = 0;
    }
    if(locationNumber < targetLocationNumber + 5 && speedNDir < 0){
      speedNDir = 0;
      integral = 0;
      antistuckCurrentPWMBonus = 0;
    }
    if(abs(locationNumber - targetLocationNumber) < 5 && locationNumber - prevLocationNumber == 0){
      if(targetLocationNumber != 1000){
        targetLocationNumber = 1000;
      } else{
        targetLocationNumber = 9000;
      }
    }
    if(abs(locationNumber - targetLocationNumber) > 2000){
      antistuckCurrentPWMBonus = 0;
    }

  
  } else if(missionIndex == 2){
   /* if(digitalRead(ZFeedbHitEnd)){
      missionIndex = 0;
      locationNumber = 0;
      varvNumber = 0;
      integral = 0;
      antistuckCurrentPWMBonus = 0;
    }*/
    speedNDir = -0.4;
  }


 


  analogWrite(lampDebug, 0.11*varvNumber2*1000*0.7);
  if(locationNumber + 5 > targetLocationNumber && locationNumber - 5 < targetLocationNumber){
    analogWrite(lampDebug, 0);
  }


  PWMFraction = abs(speedNDir);
  if(PWMFraction > 1) PWMFraction = 1;
  movementDir = 0;
  if(speedNDir > 0.0) movementDir = 1;
  if(speedNDir < 0.0) movementDir = -1;

  if(movementDir == 1){
    analogWrite(ZDriveF, 255); //Driving it and braking simultaneously is was reccomended by data sheet
    analogWrite(ZDriveB, 255 - PWMFraction*generalSpeedFactor*255.0);
  } else if (movementDir == -1){
    analogWrite(ZDriveF, 255 - PWMFraction*generalSpeedFactor*255.0);
    analogWrite(ZDriveB, 255);
  } else{
    analogWrite(ZDriveF, 0);
    analogWrite(ZDriveB, 0);
  }
  delay(delayTime);
}




void ZFeedb1INTERRUPT(){
  if(digitalRead(ZFeedb2) == HIGH){
    locationNumber += 1;
  }
  if(digitalRead(ZFeedb2) == LOW){
    locationNumber -= 1;
  }
}


void ZFeedbVarvINTERRUPT(){
  //first, tell if it's valid
  if(abs(1000*varvNumber - locationNumber) > 300){
    if(locationNumber > 1000*varvNumber){
      varvNumber += 1;
    }
    if(locationNumber < 1000*varvNumber){
      varvNumber -= 1;
    }
  }
    
  if(digitalRead(ZFeedb2) == HIGH){
    varvNumber2 += 1;
  }
  if(digitalRead(ZFeedb2) == LOW){
    varvNumber2 -= 1;
  }
  
  Serial.print(varvNumber);
  Serial.print(" ");
  Serial.print(varvNumber2);
  Serial.print(" ");
  Serial.println(locationNumber);
 // locationNumber = 1000.0*round(locationNumber/1000.0);
  locationNumber = 1000*varvNumber;
}