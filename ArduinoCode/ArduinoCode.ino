/*
 KatanaSim
 
 Reads the states of devices connected to Arduino and sends to PC.
 For calibration, sends string of control inputs.
 For xplaneplugin, sends bytes of all states. 
 
 Updated: January 24, 2013
 
 */
#include <Wire.h>
#include <string.h>

String currCmd = "\0";
String updateDate = "January 24, 2013";
int  updateXplane = 0;
int delayLength = 0;

/*    Pin locations     */
//Controls
const int pitchPin = A0;   //Cable 1
const int rollPin = A1;    //Cable 2
const int yawPin = A2;     //Cable 3
const int pBrakePin = A3;
const int propSpeedPin = A4;
const int chokePin = A5;
const int throttlePin = A6;
const int carbHeatPin = A7;
const int lBrakePin = A8;
const int rBrakePin = A9;


//Digital Pins
const int fanPin = 13;
const int trimTabPin = 14;

const int starterPin = 2;
const int leftMagPin = 53;
const int rightMagPin = 52;
const int fuelPin = 44;
const int strobePin = 45;
const int landingPin = 46;
const int taxiPin = 47;
const int positionPin = 48;
const int avMasterPin = 49;
const int generatorPin = 50;
const int batteryPin = 51;
const int trimUpPin = 3;
const int trimDownPin = 4;


void * create_state();
void update_controls();
void update_switches();
void update_starter();
void send_state();
void send_calibration();
void send_test_data();
void update_fan_speed(char *buff);
void update_trim_display(char *buff);
void update_stall_warning(char *buff);

struct ArduinoStates * states = NULL;
int hitloop = 0;
unsigned long nbChar = 0;
unsigned long nbCali = 0;


//Structure should match structure on computer end
#pragma pack(1)
struct ArduinoStates {
  //Test Variable for alignment
  uint16_t startVar;
  uint16_t packetCount;
  unsigned long elapsedTime;
  
  //controls
  uint16_t throttle;
  uint16_t propSpeed;
  uint16_t pitch;
  uint16_t roll;
  uint16_t yaw;
  uint16_t carbHeat;
  uint16_t lBrake;
  uint16_t rBrake;
  uint16_t choke;
  uint16_t pBrake;
  
  //Starter
  uint8_t igniterState;
  uint8_t ignitionPos;
  
  //Switches
  uint8_t fuelState;
  uint8_t strobeState;
  uint8_t landingState;
  uint8_t taxiState;
  uint8_t positionState;
  uint8_t avMasState;
  uint8_t generatorState;
  uint8_t batteryState;
  uint8_t trimUpState;
  uint8_t trimDownState;
  
  //Test Variable for alignment
  uint16_t endVar;
};

void setup() {
  Serial.begin(9600);
  create_state();
  
  pinMode(fanPin,OUTPUT);
}


void loop() {
      
  if (Serial.available() > 0)
  {
    String action;
    char buff[1000];
    
    
    while( Serial.available() )
    {
      char temp = (char) Serial.read();
      currCmd += temp;
      
      if(temp == ';') 
      { 
         break;
      }
    }
    
    int strEndIndex;
    if( (strEndIndex = currCmd.indexOf(';')) != -1)
    {
      action = currCmd.substring(0,1);
      
      if ( action == "C")  //Used for calibration of controls
      {
        send_calibration();
      }else if (action == "U") //X-Plane Initiate Update Request
      {
        states->packetCount = 0;
        updateXplane = 1;
        delayLength = 0;
        if( currCmd[1] == ':' )
        {
          buff[0] = currCmd[2];
          buff[1] = currCmd[3];
          buff[2] = currCmd[4];
          buff[3] = '\0';
          if(sscanf(buff,"%d",&delayLength) == 0)
          {
             delayLength = 0;
             Serial.print("Failed to read delay length in: ");
             Serial.println(buff);
             updateXplane = 0;          
          }
        }
      }else if (action == "L") //X-Plane Send Current Request
      {
        send_states();
      }else if (action == "X") //Cancel X-Plane Update
      {
        updateXplane = 0;
      }else if (action == "F") //Updates Fan Speed
      {
        buff[0] = currCmd[2];
        buff[1] = currCmd[3];
        buff[2] = currCmd[4];
        buff[3] = currCmd[5];
        buff[4] = currCmd[6];
        update_fan_speed(buff);
      }else if (action == "T")  //Updates the trim display
      {
        buff[0] = currCmd[2];
        buff[1] = currCmd[3];
        buff[2] = currCmd[4];
        buff[3] = currCmd[5];
        buff[4] = currCmd[6];
        update_trim_display(buff);
      }else if (action == "S")  //Updates stall warning
      {
        buff[0] = currCmd[2];
        buff[1] = currCmd[3];
        buff[2] = currCmd[4];
        buff[3] = currCmd[5];
        buff[4] = currCmd[6];
        update_stall_warning(buff);
      }else if (action == "V")  //Prints to screen last update of Arduino Code
      {
        Serial.println(updateDate);
      }else if (action == "A")  //Prints to screen last update of Arduino Code
      {
        send_test_data();
      }else if (action == "Z")
      {
        Serial.println(nbChar);
      }

      currCmd = "\0";
    }
    
  } //if serial available 
  
  if (updateXplane == 1){
    //Send current states
    send_states();
    //delay(delayLength);
  }
  
}

//Sends current state of everything
void send_states()
{
  update_controls();
  update_starter();
  update_switches();
  
  states->elapsedTime = millis();
  states->packetCount++;
  
  Serial.write((uint8_t*)states,nbChar);
}

//Updates state of controls
void update_controls()
{
  states->throttle = analogRead(throttlePin);
  states->propSpeed = analogRead(propSpeedPin); 
  states->pitch = analogRead(pitchPin);
  states->roll = analogRead(rollPin);
  states->yaw = analogRead(yawPin);
  states->carbHeat = analogRead(carbHeatPin);
  states->lBrake = analogRead(lBrakePin);
  states->rBrake = analogRead(rBrakePin);
  states->pBrake = analogRead(pBrakePin);
  states->choke = analogRead(chokePin);
}

//Update state of igniter and ignition position
void update_starter()
{
  int leftMag = digitalRead( leftMagPin );
  int rightMag = digitalRead( rightMagPin );
  states->ignitionPos = 0 + (rightMag == 0) + 2 * (leftMag == 0); //Matches X-Plane Dataref
  states->igniterState = digitalRead( starterPin );
}

//Updates state of switches
void update_switches()
{
  states->fuelState = digitalRead( fuelPin );
  states->strobeState = digitalRead( strobePin );
  states->landingState = digitalRead( landingPin );
  states->taxiState = digitalRead( taxiPin );
  states->positionState = digitalRead( positionPin );
  states->avMasState = digitalRead( avMasterPin );
  states->generatorState = digitalRead( generatorPin );
  states->batteryState = digitalRead( batteryPin );
  states->trimUpState = digitalRead( trimUpPin );
  states->trimDownState = digitalRead( trimDownPin );
}

//Sends to PC current state of calibration components
void send_calibration()
{
  char output[256];
  update_controls();
  //Var1:value;Var2:value;Var3:value...
  snprintf( output, 256, "PI:%d;RL:%d;YW:%d;TH:%d;LB:%d;RB:%d;ES:%d;CH:%d;CK:%d;PB:%d",states->pitch,states->roll,states->yaw,states->throttle,states->lBrake,states->rBrake,states->propSpeed,states->carbHeat,states->choke,states->pBrake );
  Serial.println( output );
}

//Sends to PC current state of all components
void send_test_data()
{
  char output[256];
  update_controls();
  update_starter();
  update_switches();
  states->elapsedTime = millis();

  //Var1:value;Var2:value;Var3:value...
  snprintf( output, 256, "startVar:%d;ElapsedTime:%lu",states->startVar,states->elapsedTime);
  Serial.println( output );
  snprintf( output, 256, "PI:%d;RL:%d;YW:%d;TH:%d;LB:%d;RB:%d;ES:%d;CH:%d;CK:%d;PB:%d;",states->pitch,states->roll,states->yaw,states->throttle,states->lBrake,states->rBrake,states->propSpeed,states->carbHeat,states->choke,states->pBrake );
  Serial.println( output );
  snprintf( output, 256, "Ignition:%d;Igniter:%d;",states->ignitionPos,states->igniterState );
  Serial.println( output );
  snprintf( output, 256, "Fuel:%d;Strobe:%d;Land:%d;Taxi:%d;Position:%d;Avionics:%d;Gen:%d;Batt:%d;",states->fuelState,states->strobeState,states->landingState,states->taxiState,states->positionState,states->avMasState,states->generatorState,states->batteryState );
  Serial.println( output );
  snprintf( output, 256, "endVar:%d",states->endVar);
  Serial.println( output );
}


//Update the speed of the fan
void update_fan_speed(char *buff)
{
  int val = 255; //255 is max output value
  int success = 0;
  char *entry;
  
//  Serial.print("Updating fan speed: ");
//  Serial.println(buff);
  
  entry = strtok(buff,";"); //Should return 'F'
  success = sscanf(buff,"%d",&val);
  
  if (success == 1)
  {
    //analogWrite(fanPin,val);
    Serial.print("Fan speed set to following percent: ");
    Serial.println(val);
  } 
}


//Update the trim display
void update_trim_display(char *buff)
{
  int val = 255;
  
  analogWrite(trimTabPin,val);
}

//Update the stall warning
void update_stall_warning(char *buff)
{
  int val = 255; //255 is max output value
  int success = 0;
  char *entry;
  
//  Serial.print("Updating fan speed: ");
//  Serial.println(buff);
  
  entry = strtok(buff,";"); //Should return 'F'
  success = sscanf(buff,"%d",&val);
  
  if (success == 1)
  {
    //analogWrite(fanPin,val);
    Serial.print("Stall warning set to following percent: ");
    Serial.println(val);
  } 
}


//Initialize State variable
void * create_state(){
  nbChar = sizeof(ArduinoStates);
  states = (ArduinoStates *) malloc(nbChar);

  if (states)
  {
      states->startVar = 0xAAA;
      states->packetCount = 0;
      states->elapsedTime = 0;
      
      states->throttle = 0;
      states->propSpeed = 0; 
      states->pitch = 0;
      states->roll = 0;
      states->yaw = 0;
      states->carbHeat = 0;
      states->lBrake = 0;
      states->rBrake = 0;
      states->choke = 0;
      states->pBrake = 0;
      
      states->fuelState = 0;
      states->strobeState = 0;
      states->landingState = 0;
      states->taxiState = 0;
      states->positionState = 0;
      states->avMasState = 0;
      states->generatorState = 0;
      states->batteryState = 0;
      states->trimUpState = 0;
      states->trimDownState = 0;
      
      states->endVar = 0xFFF;
  }
  
}

