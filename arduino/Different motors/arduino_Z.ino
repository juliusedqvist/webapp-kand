#include <Arduino.h>




#define DriveF 10
#define DriveB 9
#define Feedb1 2 //The motors channel B, I think. It is synchronius with channel Z
#define Feedb2 5 //The motors channel A, I think
#define FeedbVarv 3 //The motors channel Z
#define FeedbHitEnd A5
#define lockPin 8




#ifndef DEVICE_ID
#define DEVICE_ID 2 // fallback ID
#endif


char buffer[64];
String incomingCommand = "";


//0: Stand still
//1: Move to targetLocationNumber
//2: Reset
int missionIndex = 0;

//used for resume
int savedMissionIndex = 0;






float delayTime = 20.0;


volatile long locationNumber = 0; //Goes from 0 to something HUGE (>100k? >50k?)
long prevLocationNumber = locationNumber;
long longagoPositionOne = locationNumber; //0 to 2s ago
long longagoPositionTwo = locationNumber; //2 to 4s ago
long longagoPositionThree = locationNumber; //4 to 6s ago
int loopsPerLongagoPositionUpdate = 2000/delayTime;
int counter = 0;


long targetLocationNumber = 0;










float P = 0.0003;
float I = 0.000000005;//.00000025;
float D = 0;//.085;
float generalSpeedFactor = 0.85;
int forwardsMargin = 225; //1mm
int backwardsMargin = 225;


float antistuckCurrentPWMBonus = 0;
float integral = 0;

float PWMFraction = 0.0;
int movementDir = 0; //-1 for backwards, +1 for forwards, 0 for standing still






// put function declarations here:
void Feedb1INTERRUPT();
void FeedbVarvINTERRUPT();


void setup() {
  pinMode(DriveF, OUTPUT);
  pinMode(DriveB, OUTPUT);
  pinMode(Feedb1, INPUT);
  pinMode(Feedb2, INPUT);
  pinMode(FeedbVarv, INPUT);
  pinMode(FeedbHitEnd, INPUT);
  pinMode(lockPin, OUTPUT);




  pinMode(4, INPUT); //Just for safety - voltages will be applied to these, but will not be used.
  pinMode(6, INPUT); //Just for safety - voltages will be applied to these, but will not be used.
  pinMode(7, INPUT); //Just for safety - voltages will be applied to these, but will not be used.
  pinMode(13, OUTPUT); //Permanently high
  digitalWrite(13, HIGH);


  attachInterrupt(digitalPinToInterrupt(Feedb1), Feedb1INTERRUPT, RISING);
  attachInterrupt(digitalPinToInterrupt(FeedbVarv), FeedbVarvINTERRUPT, RISING);




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
		savedMissionIndex = 2;
      } else if(incomingCommand.equalsIgnoreCase("STOP")){
        missionIndex = 0;
		Serial.println("stopped");
      } else if(incomingCommand.equalsIgnoreCase("RESUME")){
        missionIndex = savedMissionIndex;
      } else if(incomingCommand.equalsIgnoreCase("REQUEST_POS")){
        Serial.print("Current position: ");
        Serial.println(locationNumber);
      } else {
        missionIndex = 1;
		savedMissionIndex = 1;
        targetLocationNumber = atoi(incomingCommand.c_str());
      }



      incomingCommand = ""; // Reset buffer
    } else {
      incomingCommand += received;
    }
  }




  counter += 1;
  if(counter % loopsPerLongagoPositionUpdate == 0){
    longagoPositionThree = longagoPositionTwo;
    longagoPositionTwo = longagoPositionOne;
    longagoPositionOne = locationNumber;
  }


	float speedNDir = 0;
	if(missionIndex == 0){
		digitalWrite(lockPin, LOW); //Turn the lock ON
	} else if(missionIndex == 1){
		digitalWrite(lockPin, HIGH); //Turn the lock OFF
      float e = targetLocationNumber - locationNumber;
      float derivative = (locationNumber - prevLocationNumber)/delayTime;
      speedNDir = I*integral + P*e + D*derivative + antistuckCurrentPWMBonus;
   
   
      if(1000*derivative < 250){
		  Serial.println("Antistuck action");
        if(abs(locationNumber - targetLocationNumber) < 6000){
          if(locationNumber > targetLocationNumber){
            antistuckCurrentPWMBonus = antistuckCurrentPWMBonus - 0.2*delayTime/1000;// * (1+antistuckCurrentPWMBonus);
          } else{
            antistuckCurrentPWMBonus = antistuckCurrentPWMBonus + 0.2*delayTime/1000;// * (1-antistuckCurrentPWMBonus);
          }
        }
      }
      if(abs(locationNumber - targetLocationNumber) > 8000){
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
		savedMissionIndex = 0;
		Serial.println("done");
      }
     
      //If we are stuck against something and not close to the target location, react slowly. Max allowed location diff is high to account for that the programs believed position often drifts when the robot is pushing against something it cant move.
      if((antistuckCurrentPWMBonus > 0.8 && abs(locationNumber-longagoPositionThree) < 1000)){
        missionIndex = 0;
		savedMissionIndex = 0;
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
      if(analogRead(FeedbHitEnd) > 150){
        missionIndex = 0;
		savedMissionIndex = 0;
        locationNumber = 0;
        integral = 0;
        antistuckCurrentPWMBonus = 0;
		longagoPositionThree = 0;
		longagoPositionTwo = 0;
		longagoPositionOne = 0;
		Serial.println("done");
        speedNDir = 0;

		digitalWrite(lockPin, LOW); //Turn the lock ON
      } else{

		digitalWrite(lockPin, HIGH); //Turn the lock OFF
        speedNDir = -0.4;
      }
    }


 




  PWMFraction = abs(speedNDir);
  if(PWMFraction > 1) PWMFraction = 1;
  movementDir = 0;
  if(speedNDir > 0.0) movementDir = 1;
  if(speedNDir < 0.0) movementDir = -1;


  if(movementDir == 1){
    analogWrite(DriveF, 255); //Driving and braking simultaneously was reccomended by the data sheet when controlling it via PWM
    analogWrite(DriveB, 255 - PWMFraction*generalSpeedFactor*255.0);
  } else if (movementDir == -1){
    analogWrite(DriveF, 255 - PWMFraction*generalSpeedFactor*255.0);
    analogWrite(DriveB, 255);
  } else{
    analogWrite(DriveF, 0);
    analogWrite(DriveB, 0);
  }
  if(missionIndex == 1){
    delay(delayTime);
  }
}








void Feedb1INTERRUPT(){
  if(digitalRead(Feedb2) == HIGH){
    locationNumber -= 1;
  } else{
    locationNumber += 1;
  }
}


void FeedbVarvINTERRUPT(){
  locationNumber = round(locationNumber/1000.0)*1000.0;
}


