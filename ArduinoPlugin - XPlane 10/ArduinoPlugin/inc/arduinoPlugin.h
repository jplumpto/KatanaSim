//---------------------------------------------------------------------
// Arduino plugin header file
//---------------------------------------------------------------------
#ifndef _ARDUINO_PLUGIN_H_
#define _ARDUINO_PLUGIN_H_

// Disable: unreferenced formal parameter.
#pragma warning( disable : 4100 )

//XML Reading
#include "../inc/xmlConfig.h"

//Arduino Communications
#include "../inc/arduinoCom.h"

//X-Plane Includes
#include "XPLM/XPLMProcessing.h"
#include "XPLM/XPLMDataAccess.h"
#include "XPLM/XPLMUtilities.h"
#include "XPLM/XPLMGraphics.h"
#include "XPLM/XPLMDisplay.h" //For printing to screen... debugging purposes

//Local functions
void InitializeStateMemory();
void init_circuit_breaker_datarefs();
void update_controls();
float invert_control(float percent, int positive, int invert);
void update_switches();
void update_circuit_breakers();
void update_ventilation_speed();
void update_trim_position(float trimIncrement);
void update_trim_display();
void update_flaps_display();
void update_annunciators();
void sleep();
struct ArduinoStates create_states();

//---------------X-Plane Stuff ---------------

float ArduinoFlightLoopCallback(
  float                inElapsedSinceLastCall,    
  float                inElapsedTimeSinceLastFlightLoop,    
  int                  inCounter,    
  void *               inRefcon);

void ArduinoDataRefs();
void UpdateStates();

//--------------Debugging Text Window-------------
// static void MyDrawWindowCallback(
// 								 XPLMWindowID		 inWindowID,	
// 								 void *			   inRefcon);	
// 
// static void MyHandleKeyCallback(
// 								XPLMWindowID		 inWindowID,	
// 								char				 inKey,	
// 								XPLMKeyFlags		 inFlags,	
// 								char				 inVirtualKey,	
// 								void *			   inRefcon,	
// 								int				  losingFocus);	
// 
// static int MyHandleMouseClickCallback(
// 									  XPLMWindowID		 inWindowID,	
// 									  int				  x,	
// 									  int				  y,	
// 									  XPLMMouseStatus	  inMouse,	
// 									  void *			   inRefcon);	


#endif
