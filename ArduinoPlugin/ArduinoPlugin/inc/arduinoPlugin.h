//---------------------------------------------------------------------
// Arduino plugin header file
//---------------------------------------------------------------------
#ifndef _ARDUINO_PLUGIN_H_
#define _ARDUINO_PLUGIN_H_

// Disable depreciation warning.
#pragma warning( disable : 4996 )

//XML Reading
#include "../inc/xmlConfig.h"

//Arduino Communications
#include "../inc/arduinoCom.h"

//X-Plane Includes
#include "XPLM/XPLMProcessing.h"
#include "XPLM/XPLMDataAccess.h"
#include "XPLM/XPLMUtilities.h"
#include "XPLM/XPLMGraphics.h"


//Local functions
void InitializeStateMemory();
void update_controls();
float invert_control(float percent, int positive, int invert);
void update_buttons();
void update_fan_speed();
void update_trim_position(float trimIncrement);
void update_trim_display();
void sleep();
struct ArduinoStates create_states();

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
