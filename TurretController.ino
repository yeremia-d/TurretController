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
  //Serial.println("Pan Position:" + panCurrentPos);
  //Serial.println("Tilt Position:" + tiltCurrentPos);

  Serial.println("Ready for Data");
  
}

void loop() {
  getSerialCommands();

  if(newCommand == true) {
    Serial.println(command[1]);
    newCommand = false;
    }
  // Get Instructions from serial

  
  
  

  

  //

  

  

  // Handle Laser
  
  
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

    if(recievingInProgress == true) {
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
  
