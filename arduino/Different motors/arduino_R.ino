#include <Arduino.h>
#include <inttypes.h>



#define DriveF 9
#define DriveB 10
#define Feedb1 2 //The motors channel B, I think. It is synchronius with channel Z
#define Feedb2 5 //The motors channel A, I think
#define FeedbVarv 3 //The motors channel Z
#define FeedbHitEnd 4
#define lampBlink 13




#ifndef DEVICE_ID
#define DEVICE_ID 0 // fallback ID
#endif




char buffer[64];
String incomingCommand = "";

float delayTime = 20.0;



//0: Stand still
//1: Move to targetLocationNumber
//2: Reset
int16_t missionIndex = 0;
int32_t targetLocationNumber = 0;

//used for resume
int16_t savedMissionIndex = 0;
















float P = 0.0015;//0.00035; //HUGE
float I = 0;//.000000005;
float D = 0.07;//0.07;
float antistuckGrowthRate = 0.45;
float antistuckMaxSpeed = 250; //the speed which, when the motor is above it, the antistuckCurrentPWMBonus does not increase
float antistuckMaxDist = 1000; //the distance which, when the motor is further away from the target location, the antistuckCurrentPWMBonus is always 0.
float generalSpeedFactor = 0.25; //0.45
int16_t forwardsMargin = 25; //less than 0.9 mm
int16_t backwardsMargin = 5; //more than 0.2mm
int16_t loopsPerLongagoPositionUpdate = 1500/delayTime;



float antistuckCurrentPWMBonus = 0;
float integral = 0;
float PWMFraction = 0.0;
int16_t movementDir = 0; //-1 for backwards, +1 for forwards, 0 for standing still





volatile int32_t locationNumber = 0; //Goes from 0 to somewhere between 10k and 20k
int32_t prevLocationNumber = locationNumber;
int32_t longagoPositionOne = locationNumber; //0 to 1.5s ago
int32_t longagoPositionTwo = locationNumber; //1.5 to 3s ago
int32_t longagoPositionThree = locationNumber; //3 to 4.5s ago
int32_t counter = 0;


int16_t lampBlinkCounter = 0;
int16_t lampBlinkCurrentlyOn = true;




//used for detecting loose cables
int32_t locationNumberPreviousVarvInterrupt = locationNumber;
int16_t numberOfSusVarvInterrupts = 0;
int16_t currentTravelDirectionTracker = 0;
int32_t timeLastDirectionSwap = counter;




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
  pinMode(lampBlink, OUTPUT); //Permanently high
  digitalWrite(lampBlink, HIGH);


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
		timeLastDirectionSwap = counter;
      } else if(incomingCommand.equalsIgnoreCase("REQUEST_POS")){
        Serial.print("Current position: ");
        Serial.println(locationNumber);
      } else {
        missionIndex = 1;
		savedMissionIndex = 1;
        targetLocationNumber = atol(incomingCommand.c_str());
		timeLastDirectionSwap = counter;
      }


      incomingCommand = ""; // Reset buffer
    } else {
      incomingCommand += received;
    }
  }





	lampBlinkCounter += 1;
	if((lampBlinkCounter > 25 && missionIndex == 1) || lampBlinkCounter > 10000){
		lampBlinkCurrentlyOn = !lampBlinkCurrentlyOn;
		if(lampBlinkCurrentlyOn) digitalWrite(lampBlink, HIGH);
		if(!lampBlinkCurrentlyOn) digitalWrite(lampBlink, LOW);
    lampBlinkCounter = 0;
	}



  float speedNDir = 0;
  
	if(missionIndex == 1){

	  float e = targetLocationNumber - locationNumber;
	  float derivative = (locationNumber - prevLocationNumber)/delayTime;
	//  if(e > 0) speedNDir = 0.5;
	  //if(e < 0) speedNDir = -0.5;
	  
//	  if(abs(e) < 1000 && antistuckCurrentPWMBonus != 0) speedNDir = antistuckCurrentPWMBonus;
	  speedNDir = I*integral + P*e + D*derivative + antistuckCurrentPWMBonus;


	  if(1000*derivative < antistuckMaxSpeed){
		if(abs(locationNumber - targetLocationNumber) < antistuckMaxDist){
		  if(locationNumber > targetLocationNumber){
				antistuckCurrentPWMBonus = antistuckCurrentPWMBonus - antistuckGrowthRate*delayTime/1000;// * (1+antistuckCurrentPWMBonus);
            } else{
				antistuckCurrentPWMBonus = antistuckCurrentPWMBonus + antistuckGrowthRate*delayTime/1000;// * (1-antistuckCurrentPWMBonus);
		  }
		}
	  }
	  if(abs(locationNumber - targetLocationNumber) > antistuckMaxDist){
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
	  if((abs(locationNumber - targetLocationNumber) < 300 && antistuckCurrentPWMBonus > 0.8)){ //untested change 05-22: from 200, 50
		missionIndex = 0;
		savedMissionIndex = 0;
		numberOfSusVarvInterrupts = 0;
		Serial.println("done");
	  }
	 
	  //If we are stuck against something and not close to the target location, react slowly. Max allowed location diff is high to account for that the programs believed position often drifts when the robot is pushing against something it cant move.
	  if(counter - timeLastDirectionSwap > 5000/delayTime && abs(locationNumber-longagoPositionThree) < 500){
		missionIndex = 0;
		savedMissionIndex = 0;
		numberOfSusVarvInterrupts = 0;
		Serial.println("error : veryStuck");  //Can be caused by loose cables or misplaced samples
	  }



		if(speedNDir < 0 && currentTravelDirectionTracker != -1){
		  timeLastDirectionSwap = counter;
		  currentTravelDirectionTracker = -1;
		}
		if(speedNDir > 0 && currentTravelDirectionTracker != 1){
		  timeLastDirectionSwap = counter;
		  currentTravelDirectionTracker = 1;
		}
		if(speedNDir == 0 && currentTravelDirectionTracker != 0){
		  timeLastDirectionSwap = counter;
		  currentTravelDirectionTracker = 0;
		}
		if(counter - timeLastDirectionSwap > 9000/delayTime){
			missionIndex = 0;
			Serial.print("error : takesTooLong");
			Serial.print(counter);
			Serial.print("  ");
			Serial.println(timeLastDirectionSwap);
		}


	 
	  counter += 1;
	  if(counter % loopsPerLongagoPositionUpdate == 0){
		longagoPositionThree = longagoPositionTwo;
		longagoPositionTwo = longagoPositionOne;
		longagoPositionOne = locationNumber;
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
		timeLastDirectionSwap = counter;
		Serial.println("done");
        speedNDir = 0;
	  } else{
		speedNDir = -1;
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
		if(numberOfSusVarvInterrupts > 100 && numberOfSusVarvInterrupts < 110 && missionIndex == 1){
//			detachInterrupt(digitalPinToInterrupt(FeedbVarv));
			Serial.println("error : varvInterruptSpam");
			missionIndex = 0;
		}
	}
	locationNumberPreviousVarvInterrupt = locationNumber;
}

