/*
  == COMMAND PROTOCOL ==
  Correction commands contain 5 characters enclosed in square brackets
  [1,2,3,4,5]
  Value 1 - Magnitude of movment in pan (0-9)
  Value 2 - Direction of pan movement (L, R relative to camera frame)
  Value 3 - Magnitude of movement in tilt (0,9)
  Value 4 - Direction of tile (U, D relative to camera frame)
  Value 5 - Laser Activation (A to activate, D to Deactivate) 
*/

#include <ax12.h> //include the ArbotiX DYNAMIXEL library
#include <BioloidController.h>  //include bioloid libary for poses/movements


const int tiltUpperLimit = 1000;
const int tiltLowerLimit = 3050;
const int panUpperLimit = 4090;
const int panLowerLimit = 0;

int isLaserOn;
int panCurrentPos;
int tiltCurrentPos;

const byte commandSize = 6;
char command[commandSize];
bool newCommand = false;

void setup()
{
  dxlInit(1000000);  //start dynamixel library at 1mbps to communicate with the servos
  Serial.begin(9600); //start serial at 9600bps  
  dxlServoReport(); //scan for all servos

  // Initialize Positions
  dxlSetGoalPosition(1,0);
  dxlSetGoalPosition(2,2048);
  delay(2000);

  panCurrentPos = dxlGetPosition(1);
  tiltCurrentPos = dxlGetPosition(2);

  Serial.println(tiltCurrentPos);
  Serial.println("Ready for Data");
  
}

void loop() {
  getSerialCommands();

  if(newCommand) processNewCommand();

  
  
  // put your main code here, to run repeatedly:
  //dxlSetGoalPosition(1,2048);
  //dxlSetGoalPosition(2,2048);
  //delay(1000);
  //dxlSetGoalPosition(1,panUpperLimit);
  //dxlSetGoalPosition(2,tiltUpperLimit);
  //delay(1000);
  //dxlSetGoalPosition(2,tiltLowerLimit);
  //dxlSetGoalPosition(1,panLowerLimit);
  //delay(1000);

}

void getSerialCommands() {
  static boolean recievingInProgress = false;
  static byte idx = 0;
  char startDelimiter = '[';
  char endDelimiter = ']';
  char serialData;

  while(Serial.available() > 0) {
    serialData = Serial.read();

    if(recievingInProgress) {
      if(serialData != endDelimiter) {
        command[idx] = serialData;
        idx++;
        
        if(idx >= commandSize) idx = commandSize - 1;
      }
      else {
        command[idx] = '\0';
        recievingInProgress = false;
        idx = 0;
        newCommand = true;
      }
    }
    else if(serialData == startDelimiter) { recievingInProgress = true; }
  } 
}

void processNewCommand() {

  // Initialize Temporary Pan and Tilt
  int tempPanVal = panCurrentPos;
  int tempTiltVal = tiltCurrentPos;

  // Initialize temporary Laser state
  
  // Process Pan
  if(command[1] == 'L') tempPanVal += command[0];
  if(command[1] == 'R') tempPanVal -= command[0];
  
  // Process Tilt
  if(command[3] == 'U') tempTiltVal -= command[2];
  if(command[3] == 'D') tempTiltVal += command[2];
  
  // Process Laser Activation
  if(command[4] == 'A') setLaserActivation(true);
  if(command[4] == 'D') setLaserActivation(false);

  Serial.println(command);
  
  // Move Turret
  if(tempPanVal < panUpperLimit && tempPanVal > panLowerLimit) {
    dxlSetGoalPosition(1,tempPanVal);
  }

  if(tempTiltVal < tiltUpperLimit && tempTiltVal > tiltLowerLimit) {
    dxlSetGoalPosition(2, tiltLowerLimit);
  }

  delay(1000);
  panCurrentPos = dxlGetPosition(1);
  tiltCurrentPos = dxlGetPosition(2);

  String msg = "Current Pan Position: " + panCurrentPos;
  Serial.println(msg);
  //msg = "Current Tilt Postion: " + tiltCurrentPos;
  //Serial.println(msg);
  
  // Update Pan and Tilt Position Values
  newCommand = false;
}

void setLaserActivation(boolean state) {
  if(state) {
    // activate Laser
    Serial.println("Activate Laser");
  }
  else {
    // deactivate laser
    Serial.println("Deactivate Laser");
  }  
}
  
