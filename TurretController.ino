/*
  == COMMAND PROTOCOL ==
  Correction commands contain 5 characters enclosed in square brackets
  [12345]
  Value 1 - Magnitude of movment in pan (0-9)
  Value 2 - Magnitude of movement in tilt (0-9)
  Value 3 - Direction of pan movement (1 for left, 0 for right relative to camera frame)
  Value 4 - Direction of tile (1 for up, 0 for down relative to camera frame)
  Value 5 - Laser Activation (1 to activate, 0 to Deactivate) 
*/

#include <ax12.h> //include the ArbotiX DYNAMIXEL library

const int tiltUpperLimit = 1000;
const int tiltLowerLimit = 3050;
const int panUpperLimit = 4090;
const int panLowerLimit = 0;

int isLaserActive;
int panCurrentPos;
int tiltCurrentPos;

int panVelocity = 0;
int tiltVelocity = 0;
int panDirection = 1;
int tiltDirection = 1;

const byte commandSize = 6;
char command[commandSize];
bool newCommand = false;

void setup()
{
  dxlInit(1000000);  //start dynamixel library at 1mbps to communicate with the servos
  Serial.begin(9600); //start serial at 9600bps

  pinMode(0, OUTPUT); // Sets the laser pinmode as output

  
  
  dxlServoReport(); //scan for all servos

  // Initialize Positions
  dxlSetGoalPosition(1,0);
  dxlSetGoalPosition(2,2048);
  delay(2000);

  panCurrentPos = dxlGetPosition(1);
  tiltCurrentPos = dxlGetPosition(2);

  Serial.println("Ready for Data");
  
}

void loop() {
  getSerialCommands();

  if(newCommand) processNewCommand();

  updateTurretPosition();

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

  panVelocity   = command[0] - '0';
  tiltVelocity  = command[1] - '0';
  panDirection  = command[2] - '0';
  tiltDirection = command[3] - '0';
  isLaserActive = command[4] - '0';
  newCommand    = false;

  // Sets the laser activation
  setLaserActivation();
  
}

void setLaserActivation() {
  if(isLaserActive == 1) {
    // activate Laser
    Serial.println("Activate Laser");
    digitalWrite(0, HIGH);
  }
  else {
    // deactivate laser
    Serial.println("Deactivate Laser");
    digitalWrite(0, LOW);
  }  
}

void updateTurretPosition() {

  int currentPanPosition = dxlGetPosition(1);
  int currentTiltPosition = dxlGetPosition(2);
  int newPanPosition = dxlGetPosition(1);
  int newTiltPosition = dxlGetPosition(2);

  if(currentPanPosition >= panLowerLimit && currentPanPosition <= panUpperLimit) {
    if(panDirection == 1) newPanPosition +=  (4*panVelocity);
    if(panDirection == 0) newPanPosition -=  (4*panVelocity); 
  }

  if(currentTiltPosition >= tiltUpperLimit && currentTiltPosition <= tiltLowerLimit) {
    if(tiltDirection == 1) newTiltPosition -= (5 * tiltVelocity);
    if(tiltDirection == 0) newTiltPosition += (5 * tiltVelocity);
  }

  if(newTiltPosition < tiltUpperLimit) {newTiltPosition = tiltUpperLimit + 2; tiltVelocity = 0;}
  if(newTiltPosition > tiltLowerLimit) {newTiltPosition = tiltLowerLimit - 2; tiltVelocity = 0;}

  if(newPanPosition < panLowerLimit) {newPanPosition = panLowerLimit + 2; panVelocity = 0;}
  if(newPanPosition > panUpperLimit) {newPanPosition = panLowerLimit - 2; panVelocity = 0;}
  
  dxlSetGoalPosition(1, newPanPosition);
  dxlSetGoalPosition(2, newTiltPosition);

  delay(1);
}
  
