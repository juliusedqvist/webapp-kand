#include <Arduino.h>




#define DriveF 9
#define DriveB 10
#define Feedb1 2 //The motors channel B, I think. It is synchronius with channel Z
#define Feedb2 5 //The motors channel A, I think
#define FeedbVarv 3 //The motors channel Z
#define FeedbHitEnd 4




#ifndef DEVICE_ID
#define DEVICE_ID 0 // fallback ID
#endif




char buffer[64];
String incomingCommand = "";




//0: Stand still
//1: Move to targetLocationNumber
//2: Reset
int16_t missionIndex = 0;

//used for resume
int16_t savedMissionIndex = 0;






float delayTime = 20.0;


volatile int32_t locationNumber = 0; //Goes from 0 to somewhere between 10k and 20k
int32_t prevLocationNumber = locationNumber;
int32_t longagoPositionOne = locationNumber; //0 to 1.5s ago
int32_t longagoPositionTwo = locationNumber; //1.5 to 3s ago
int32_t longagoPositionThree = locationNumber; //3 to 4.5s ago
int16_t loopsPerLongagoPositionUpdate = 1500/delayTime;
int16_t counter = 0;


int32_t targetLocationNumber = 0;










float P = 0.00035;
float I = 0.000000005;
float D = 0.07;
float generalSpeedFactor = 0.35; //0.45
int16_t forwardsMargin = 25;
int16_t backwardsMargin = 5;



float antistuckCurrentPWMBonus = 0;
float integral = 0;

float PWMFraction = 0.0;
int16_t movementDir = 0; //-1 for backwards, +1 for forwards, 0 for standing still




//used for detecting loose cables
int16_t locationNumberPreviousVarvInterrupt = locationNumber;
int16_t numberOfSusVarvInterrupts = 0;




// put function declarations here:
void Feedb1INTERRUPT();
void FeedbVarvINTERRUPT();


void setup() {
  pinMode(DriveF, OUTPUT);
  pinMode(DriveB, OUTPUT);
  pinMode(Feedb1, INPUT);
  pinMode(Feedb2, INPUT);
  pinMode(FeedbVarv, INPUT);
  pinMode(FeedbHitEnd, INPUT_PULLUP);




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
        targetLocationNumber = atol(incomingCommand.c_str());
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
		savedMissionIndex = 0;
		numberOfSusVarvInterrupts = 0;
		Serial.println("done");
	  }
	 
	  //If we are stuck against something but very close to target location, react quickly:
	  if((abs(locationNumber - targetLocationNumber) < 100 && abs(locationNumber-longagoPositionTwo) < 25)){
		missionIndex = 0;
		savedMissionIndex = 0;
		numberOfSusVarvInterrupts = 0;
		Serial.println("done");
	  }
	 
	  //If we are stuck against something and not close to the target location, react slowly. Max allowed location diff is high to account for that the programs believed position often drifts when the robot is pushing against something it cant move.
	  if((antistuckCurrentPWMBonus > 0.8 && abs(locationNumber-longagoPositionThree) < 150)){
		missionIndex = 0;
		savedMissionIndex = 0;
		numberOfSusVarvInterrupts = 0;
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
	  if(digitalRead(FeedbHitEnd) == LOW){
		missionIndex = 0;
		savedMissionIndex = 0;
		locationNumber = 0;
		integral = 0;
		antistuckCurrentPWMBonus = 0;
		longagoPositionThree = 0;
		longagoPositionTwo = 0;
		longagoPositionOne = 0;
		numberOfSusVarvInterrupts = 0;
		Serial.println("done");
        speedNDir = 0;
	  } else{
		speedNDir = -0.6;
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
    locationNumber += 1;
  } else{
    locationNumber -= 1;
  }
}


void FeedbVarvINTERRUPT(){
	locationNumber = round(locationNumber/1000.0)*1000.0;
	if(locationNumber - locationNumberPreviousVarvInterrupt < 200){
		numberOfSusVarvInterrupts += 1;
		if(numberOfSusVarvInterrupts > 100 && missionIndex == 1){
			detachInterrupt(digitalPinToInterrupt(FeedbVarv));
			Serial.println("fuck :VarvInterruptSpam");
			missionIndex = 0;
		}
	}
	locationNumberPreviousVarvInterrupt = locationNumber;
}

