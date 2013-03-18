/*
 KatanaSim
 
 Reads the states of devices connected to Arduino and sends to PC.
 For calibration, sends string of control inputs.
 For xplaneplugin, sends uint8_ts of all states. 
 
 Updated: February 26, 2013
 
 */
//#include <Wire.h>
#include <string.h>

// fuelState; strobeState; landingState; taxiState; positionState; avMasState; generatorState; batteryState;
//Define Switch BIT flags
const uint8_t SWITCHES_CLEAR             =   0;   // 0x00000000
const uint8_t SWITCHES_FUELPUMP_ON       =   1;   // 0x00000001
const uint8_t SWITCHES_STROBELIGHT_ON    =   2;   // 0x00000010
const uint8_t SWITCHES_LANDINGLIGHT_ON   =   4;   // 0x00000100
const uint8_t SWITCHES_TAXILIGHT_ON      =   8;   // 0x00001000
const uint8_t SWITCHES_NAVLIGHT_ON       =   16;  // 0x00010000
const uint8_t SWITCHES_AVIONICSMASTER_ON =   32;  // 0x00100000
const uint8_t SWITCHES_GENERATOR_ON      =   64;  // 0x01000000
const uint8_t SWITCHES_BATTERY_ON        =   128; // 0x10000000


//Structure should match structure on computer end
#pragma pack(1)
struct ArduinoStates {
  //Test Variable for alignment
  uint16_t startVar;
  //uint16_t packetCount;
    
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
  
  //Circuit breakers
  uint32_t cbStates;
  
  //Switches
  uint8_t switchStates; // fuelState; strobeState; landingState; taxiState; positionState; avMasState; generatorState; batteryState;
  uint8_t trimSwitchPos; // 0 - Trim Button Not Pressed; 1 - Trim Up; 2 - Trim Down
  uint8_t flapSwitchPos; // 0 - No Flaps; 1 - T/0 Flaps; 2 - Full Flaps
  
  //Test Variable for alignment
  uint8_t endVar;
}; //ArduinoStates structure

String currCmd = "\0";
String updateDate = "February 26, 2013";
int  updateXplane = 0;
int delayLength = 0;
unsigned long lastTime = 0;

ArduinoStates * states = NULL;
int hitloop = 0;
unsigned long nbChar = 0;
unsigned long nbCali = 0;

/*    Pin locations     */
//CBs
int CB[28]; //Initiated in function

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
const int trimTabPin = 5;
const int flapsDisplayPin = 6;

const int starterPin = 2;
const int leftMagPin = 15;
const int rightMagPin = 14;
const int fuelPumpPin = 44;
const int strobePin = 45;
const int landingPin = 46;
const int taxiPin = 47;
const int positionPin = 48;
const int avionicsMasterPin = 49;
const int generatorPin = 50;
const int batteryPin = 51;
const int trimUpPin = 3;
const int trimDownPin = 4;

//Functions
void * create_state();
void update_controls();
void update_switches();
void update_starter();
void send_state();
void send_calibration();
void send_test_data();
void update_ventilation_speed(char *buff);
void update_trim_display(char *buff);
void update_flaps_display(char *buff);
void update_stall_warning(char *buff);
void random_test();
void snprintSwitches(char *buff, int maxChars);

//Circuit Breakers
void initiate_breakers();
void update_breakers();
int get_breaker(int x);

void setup() {
  Serial.begin(9600);
  create_state();
  initiate_breakers();
  
  pinMode(fanPin,OUTPUT);
  pinMode(trimTabPin,OUTPUT);
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
         currCmd += "\0";
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
        //states->packetCount = 0;
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
      }else if (action == "V") //Updates Ventilation Speed
      {
        //currCmd = "V:%d;"; // where %d is 0 - 255
        if ( currCmd[1] == ':')
        {
          buff[0] = currCmd[2];
          buff[1] = currCmd[3];
          buff[2] = currCmd[4];
          buff[3] = currCmd[5];
          buff[4] = currCmd[6];
          update_ventilation_speed(buff);
        } //if
      }else if (action == "T")  //Updates the trim display
      {
        //currCmd = "T:%d;"; // where %d is 0 - 255

        int i = 0;
        int j = 2;
        int length = currCmd.length();
        
        if ( currCmd[1] == ':')
        {
          for (i = 0, j = 2; j < length - 1; i++, j++)
          {
            buff[i] = currCmd[j];
            
          }  // for
          
          buff[i] = '\0';
          
          update_trim_display(buff);
        } //if
        
      }else if (action == "F")  //Updates the Flaps display
      {
        //currCmd = "F:%d;"; // where %d is 0 - 255

        int i = 0;
        int j = 2;
        int length = currCmd.length();
        
        if ( currCmd[1] == ':')
        {
          for (i = 0, j = 2; j < length - 1; i++, j++)
          {
            buff[i] = currCmd[j];
            
          }  // for
          
          buff[i] = '\0';
          
          update_flaps_display(buff);
        } //if
        
      }else if (action == "S")  //Updates stall warning
      {
        buff[0] = currCmd[2];
        buff[1] = currCmd[3];
        buff[2] = currCmd[4];
        buff[3] = currCmd[5];
        buff[4] = currCmd[6];
        update_stall_warning(buff);
      }else if (action == "R")  //Prints to screen last update of Arduino Code
      {
        Serial.println(updateDate);
      }else if (action == "A")  //Prints to screen last update of Arduino Code
      {
        send_test_data();
      }else if (action == "Z")
      {
        updateXplane = 2;
      }

      currCmd = "\0";
    }
    
  } //if serial available 
  
  if (updateXplane == 1){
    //Send current states
    send_states();
    delay(0);
    //delay(delayLength);
  } else if (updateXplane == 2)
  {
    random_test();
    delay(0);
  } //else if
  
}





