#include <Arduino.h>




#define ZDriveF 9
#define ZDriveB 10
#define ZFeedb1 2 //The motors channel B, I think. It is synchronius with channel Z
#define ZFeedb2 5 //The motors channel A, I think
#define ZFeedbVarv 3 //The motors channel Z
#define ZFeedbHitEnd 4




#ifndef DEVICE_ID
#define DEVICE_ID 0 // fallback ID
#endif




char buffer[64];
String incomingCommand = "";




//0: Stand still
//1: Move to targetLocationNumber
//2: Reset
int missionIndex = 0;






float delayTime = 20.0;
int delayloops = 0;


volatile long locationNumber = 0; //Goes from 0 to somewhere between 10k and 20k
long prevLocationNumber = locationNumber;
long longagoPositionOne = locationNumber; //0 to 1.5s ago
long longagoPositionTwo = locationNumber; //1.5 to 3s ago
long longagoPositionThree = locationNumber; //3 to 4.5s ago
int loopsPerLongagoPositionUpdate = 1500/delayTime;
int counter = 0;


long targetLocationNumber = 0;










float P = 0.00035;
float I = 0.000000005; //.00000025;
float antistuckCurrentPWMBonus = 0;
float D = 0.05;
float integral = 0;
float generalSpeedFactor = 0.45;


int forwardsMargin = 25;
int backwardsMargin = 5;




float PWMFraction = 0.0;
int movementDir = 0; //-1 for backwards, +1 for forwards, 0 for standing still






// put function declarations here:
void ZFeedb1INTERRUPT();
void ZFeedbVarvINTERRUPT();


void setup() {
  pinMode(ZDriveF, OUTPUT);
  pinMode(ZDriveB, OUTPUT);
  pinMode(ZFeedb1, INPUT);
  pinMode(ZFeedb2, INPUT);
  pinMode(ZFeedbVarv, INPUT);
  pinMode(ZFeedbHitEnd, INPUT);




  pinMode(4, INPUT); //Just for safety - voltages will be applied to these, but will not be used.
  pinMode(6, INPUT); //Just for safety - voltages will be applied to these, but will not be used.
  pinMode(7, INPUT); //Just for safety - voltages will be applied to these, but will not be used.
  pinMode(13, OUTPUT); //Permanently high
  digitalWrite(13, HIGH);


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


      if(incomingCommand.equalsIgnoreCase("RESET")){
        missionIndex = 2;
      } else if(incomingCommand.equalsIgnoreCase("STOP")){
        missionIndex = 0;
      } else if(incomingCommand.equalsIgnoreCase("RESUME")){
        missionIndex = 1;
      } else {
        missionIndex = 1;
        targetLocationNumber = atoi(incomingCommand.c_str());
      }


      incomingCommand = ""; // Reset buffer
    } else {
      incomingCommand += received;
    }
  }




  counter += 1;
  if(counter % 125 == 0){
    longagoPositionThree = longagoPositionTwo;
    longagoPositionTwo = longagoPositionOne;
    longagoPositionOne = locationNumber;
  }


  float speedNDir = 0;
  if(delayloops > 0){
    delayloops -= 1;
  } else{
    if(missionIndex == 1){
      float e = targetLocationNumber - locationNumber;
      float derivative = (locationNumber - prevLocationNumber)/delayTime;
      speedNDir = I*integral + P*e + D*derivative + antistuckCurrentPWMBonus;
   
   
      if(1000*derivative < 250){
        if(abs(locationNumber - targetLocationNumber) < 1000){
          if(locationNumber > targetLocationNumber){
            antistuckCurrentPWMBonus = antistuckCurrentPWMBonus - 0.2*delayTime/1000;// * (1+antistuckCurrentPWMBonus);
          } else{
            antistuckCurrentPWMBonus = antistuckCurrentPWMBonus + 0.2*delayTime/1000;// * (1-antistuckCurrentPWMBonus);
          }
        }
      }
      if(abs(locationNumber - targetLocationNumber) > 2000){
        antistuckCurrentPWMBonus = 0;
      }
     
      if(locationNumber > targetLocationNumber - backwardsMargin && speedNDir > 0){
        speedNDir = 0;
        integral = 0;
        antistuckCurrentPWMBonus = 0;
      }
      if(locationNumber < targetLocationNumber + forwardsMargin && speedNDir < 0){
        speedNDir = 0;
        integral = 0;
        antistuckCurrentPWMBonus = 0;
      }


      //If we are standing still and at the correct location:
      if((locationNumber > targetLocationNumber - backwardsMargin && locationNumber < targetLocationNumber + forwardsMargin && locationNumber == prevLocationNumber)){
        missionIndex = 0;
		Serial.println("done");
      }
     
      //If we are stuck against something but very close to target location, react quickly:
      if((abs(locationNumber - targetLocationNumber) < 100 && abs(locationNumber-longagoPositionTwo) < 25)){
        missionIndex = 0;
		Serial.println("done");
      }
     
      //If we are stuck against something and not close to the target location, react slowly. Max allowed location diff is high to account for that the programs believed position often drifts when the robot is pushing against something it cant move.
      if((antistuckCurrentPWMBonus > 0.8 && abs(locationNumber-longagoPositionThree) < 150)){
        missionIndex = 0;
		Serial.println("fuck");
        //GRAB
        //REPORT BACK THAT AN UNCERTAIN GRAB WAS PERFORMED
        //REPORT THAT IT IS LIKELY THAT DRIFT HAS OCCURED
      }




     
      if(prevLocationNumber != locationNumber){
        integral = integral + e*delayTime;
      }
      prevLocationNumber = locationNumber;


    } else if(missionIndex == 2){
      if(digitalRead(ZFeedbHitEnd) == HIGH){
        missionIndex = 0;
        locationNumber = 0;
        integral = 0;
        antistuckCurrentPWMBonus = 0;
        delayloops = 50;
      } else{
        speedNDir = -0.6;
      }
    }
 
  }


 




  PWMFraction = abs(speedNDir);
  if(PWMFraction > 1) PWMFraction = 1;
  movementDir = 0;
  if(speedNDir > 0.0) movementDir = 1;
  if(speedNDir < 0.0) movementDir = -1;


  if(movementDir == 1){
    analogWrite(ZDriveF, 255); //Driving and braking simultaneously was reccomended by the data sheet when controlling it via PWM
    analogWrite(ZDriveB, 255 - PWMFraction*generalSpeedFactor*255.0);
  } else if (movementDir == -1){
    analogWrite(ZDriveF, 255 - PWMFraction*generalSpeedFactor*255.0);
    analogWrite(ZDriveB, 255);
  } else{
    analogWrite(ZDriveF, 0);
    analogWrite(ZDriveB, 0);
  }
  if(missionIndex != 2 || delayloops != 0){
    delay(delayTime);
  }
}








void ZFeedb1INTERRUPT(){
  if(digitalRead(ZFeedb2) == HIGH){
    locationNumber += 1;
  } else{
    locationNumber -= 1;
  }
}


void ZFeedbVarvINTERRUPT(){
  locationNumber = round(locationNumber/1000.0)*1000.0;
}


