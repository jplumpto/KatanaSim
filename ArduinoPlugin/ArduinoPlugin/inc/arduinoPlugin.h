//---------------------------------------------------------------------
// Arduino plugin header file
//---------------------------------------------------------------------
#ifndef _ARDUINO_PLUGIN_H_
#define _ARDUINO_PLUGIN_H_

// Disable depreciation warning.
#pragma warning( disable : 4996 )

//XML Reading
#include "../inc/xmlConfig.h"

//X-Plane Includes
#include "XPLM/XPLMProcessing.h"
#include "XPLM/XPLMDataAccess.h"
#include "XPLM/XPLMUtilities.h"
#include "XPLM/XPLMGraphics.h"

//Arduino Includes
#include <stdio.h>
#include <tchar.h>

#include <Windows.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define ARDUINO_WAIT_TIME 2000


//--------------Arduino Stuff ----------------
struct ArduinoStates {
	//Test Variable to Ensure alignment of data
	UINT16 startVar;

	//Controls
	UINT16 throttle;
	UINT16 propSpeed;
	UINT16 pitch;
	UINT16 roll;
	UINT16 yaw;
	UINT16 carbHeat;
	UINT16 lBrake;
	UINT16 rBrake;
	UINT16 choke;
	UINT16 pBrake;

	//Starter
	UINT8 igniterState;
	UINT8 ignitionPos;

	//Switches
	UINT8 fuelState;
	UINT8 strobeState;
	UINT8 landingState;
	UINT8 taxiState;
	UINT8 positionState;
	UINT8 avMasState;
	UINT8 generatorState;
	UINT8 batteryState;
	UINT8 trimUpState;
	UINT8 trimDownState;

	//Test Variable to Ensure alignment of data
	UINT16 endVar;
};

BOOL comm_init();
void InitializeStateMemory();
int read_states();
void clear_buffer();
int find_start(UINT8 *outBuffer, UINT8 *inBuffer, DWORD maxPosition);
BOOL send_current_cmd();
BOOL send_update_cmd();
BOOL cancel_update_cmd();
void update_controls();
float invert_control(float percent, int positive, int invert);
void update_buttons();
void update_fan_speed();
void update_trim_position(float trimIncrement);
void update_trim_display();
void sleep();
struct ArduinoStates create_states();

//--------------Serial Stuff-----------------

void comminit(char* commport, int baudrate);
void commopen();
DWORD commopen_internal();
void commclose();
DWORD commread( LPVOID pB, DWORD n );
DWORD commwrite( const void* lpBuf, DWORD dwCount );
void commflush();
void commstats(COMSTAT& stat);

//---------------X-Plane Stuff ---------------

float ArduinoFlightLoopCallback(
  float                inElapsedSinceLastCall,    
  float                inElapsedTimeSinceLastFlightLoop,    
  int                  inCounter,    
  void *               inRefcon);

int ArduinoCommandCB(
			  XPLMCommandRef inCommand, 
			  XPLMCommandPhase inPhase, 
			  void * inRefcon);

void ArduinoDataRefs();

void UpdateStates();






#endif
