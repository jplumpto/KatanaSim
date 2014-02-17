#include "../inc/arduinoPlugin.h"

ArduinoStates currentState;
ArduinoStates lastState;
XmlConfig *configFile;
float MAX_PROP_SPEED_RADS = 2550;
float MIN_PROP_SPEED_RADS = 0;
float PROP_SPEED_RANGE_RADS = 0;
float MAX_TRIM_DEFLECTION = 1.0f;
float MIN_TRIM_DEFLECTION = -1.0f;
const float MAX_AIRSPEED = 120.0f;
int _iLoopCalls = 0;

//Flap Constants
const float TAKEOFF_FLAPS_RATIO = 0.375f;
const int NO_FLAPS = 0;
const int TAKEOFF_FLAPS = 1;
const int FULL_FLAPS = 2;

//X-Plane Debugging Window
static XPLMWindowID	gWindow = NULL;
static char gTempBuffer[256];
static float PreviousElapsedTimeSinceLastFlightLoop = 0;
static float	TargetFrequency = 10.0f;
static float	TargetPeriod = 1.0f / TargetFrequency;

//Arduino Communications
ArduinoCom	*_arduino	= NULL;
bool		_updating	= FALSE;

//Position Variables
float			_trimPosition = 0.0f;
float			_airspeed	  = 0.0f;

#pragma region Datarefs
//X-Plane Switch Refs
XPLMDataRef _fuelPumpDataref = NULL;
XPLMDataRef _strobeLightDataref = NULL;
XPLMDataRef _landingLightDataref = NULL;
XPLMDataRef _taxiLightDataref = NULL;
XPLMDataRef _navLightDataref = NULL;
XPLMDataRef _avionicsMasterDataref = NULL;
XPLMDataRef _generatorDataref = NULL;
XPLMDataRef _batteryDataref = NULL;

//X-Plane StarterControl Refs
XPLMDataRef _ignitionPositionDataref = NULL;
XPLMDataRef _igniterStateDataref = NULL;

//X-Plane ParamRefs
XPLMDataRef _flapSwitchPositionDataref = NULL;
XPLMDataRef _flapsPositionDataref = NULL;
XPLMDataRef _trimPositionDataref = NULL;
XPLMDataRef _indicatedAirspeedDataref = NULL;
XPLMDataRef _busVoltsDataref = NULL;
XPLMDataRef _maxTrimDataref = NULL;
XPLMDataRef _minTrimDataref = NULL;

//X-Plane Control Refs
XPLMDataRef _throttleRatioDataref = NULL;
XPLMDataRef _pitchControlRatioDataref = NULL;
XPLMDataRef _rollControlRatioDataref = NULL;
XPLMDataRef _yawControlRatioDataref = NULL;
XPLMDataRef _commandedPropSpeedDataref = NULL;
XPLMDataRef _leftBrakeRatioDataref = NULL;
XPLMDataRef _rightBrakeRatioDataref = NULL;
XPLMDataRef _carbHeatRatioDataref = NULL;
XPLMDataRef _parkingBrakeRatioDataref = NULL;

//X-Plane Prop Refs
XPLMDataRef _maxPropSpeedDataref = NULL;
XPLMDataRef _minPropSpeedDataref = NULL;

//X-Plane Annunciator datarefs
XPLMDataRef _generatorWarningDataref = NULL;
XPLMDataRef _fuelPressureWarningDataref = NULL;

#pragma region CircuitBreakerDatarefs
XPLMDataRef _cb1Datarefs[2];
XPLMDataRef _cb2Datarefs[3];
XPLMDataRef _cb3Dataref		= NULL;
XPLMDataRef _cb4Dataref		= NULL;
XPLMDataRef _cb5Dataref		= NULL;
XPLMDataRef _cb6Dataref		= NULL;
XPLMDataRef _cb7Dataref		= NULL;
XPLMDataRef _cb8Dataref		= NULL;
XPLMDataRef _cb9Dataref		= NULL;
XPLMDataRef _cb10Dataref	= NULL;
XPLMDataRef _cb11Dataref	= NULL;
XPLMDataRef _cb12Dataref	= NULL;
XPLMDataRef _cb13Dataref	= NULL;
XPLMDataRef _cb14Dataref	= NULL;
XPLMDataRef _cb15Dataref	= NULL;
XPLMDataRef _cb16Dataref	= NULL;
XPLMDataRef _cb17Dataref	= NULL;
XPLMDataRef _cb18Dataref	= NULL;
XPLMDataRef _cb19Dataref	= NULL;
XPLMDataRef _cb20Dataref	= NULL;
XPLMDataRef _cb21Dataref	= NULL;
XPLMDataRef _cb22Dataref	= NULL;
XPLMDataRef _cb23Dataref	= NULL;
XPLMDataRef _cb24Dataref	= NULL;
XPLMDataRef _cb25Dataref	= NULL;
XPLMDataRef _cb26Dataref	= NULL;
#pragma endregion


#pragma endregion //Datarefs
/**
\brief plugin constructor
Registers MyPosFlightLoopCallback. 
\param outName Name of the plugin (ArduinoPlugin).
\param outSig Signature of the plugin (ArduinoPlugin).
\param outDesc Description of plugin to be displayed in X-Plane.
*/
PLUGIN_API int XPluginStart(
									 char *		outName,
									 char *		outSig,
									 char *		outDesc)
{
	strcpy(outName, "ArduinoPlugin");
	strcpy(outSig, "ArduinoPlugin");
	strcpy(outDesc, "Sends inputs from Arduino to X-Plane");

	return 1;
}


/**
\brief plugin destructor
Unregisters MyPosFlightLoopCallback. 
*/
PLUGIN_API void	XPluginStop(void)
{
	
	XPLMUnregisterFlightLoopCallback(ArduinoFlightLoopCallback, NULL);
	

}


/**
\brief called when users disables this plugin.
Unregisters MyPosFlightLoopCallback.
*/
PLUGIN_API void XPluginDisable(void)
{
	_updating = FALSE;

	// Should nullify all registered datarefs
	_fuelPumpDataref		= NULL;
	_strobeLightDataref		= NULL;
	_landingLightDataref	= NULL;
	_taxiLightDataref		= NULL;
	_navLightDataref		= NULL;
	_avionicsMasterDataref	= NULL;
	_generatorDataref		= NULL;
	_batteryDataref			= NULL;

	_igniterStateDataref	= NULL;
	_ignitionPositionDataref = NULL;

	_trimPositionDataref	= NULL;
	_indicatedAirspeedDataref = NULL;
	_flapsPositionDataref	= NULL;
	_flapSwitchPositionDataref = NULL;
	_busVoltsDataref = NULL;
	_maxTrimDataref = NULL;
	_minTrimDataref = NULL;

	/*------Controls--------*/
	_throttleRatioDataref	= NULL;
	_commandedPropSpeedDataref	= NULL;
	_pitchControlRatioDataref = NULL;
	_rollControlRatioDataref = NULL;
	_yawControlRatioDataref	= NULL;
	_leftBrakeRatioDataref	= NULL;
	_rightBrakeRatioDataref	= NULL;
	_carbHeatRatioDataref	= NULL;
	_parkingBrakeRatioDataref = NULL;

	// Annunciators
	_generatorWarningDataref = NULL;
	_fuelPressureWarningDataref = NULL;

	XPLMUnregisterFlightLoopCallback(ArduinoFlightLoopCallback, NULL);
	delete _arduino;
	_arduino = NULL;


	//Debugging 
	XPLMDestroyWindow(gWindow);
}


/**
\brief called when users reenables this plugin.
registers MyPosFlightLoopCallback.
*/
PLUGIN_API int XPluginEnable(void)
{
	char filename[1024];
	

#ifdef _DEBUG
	//Need to define path ourselves (ie missing admin access)
	char sXPlanePath[512];

	XPLMGetSystemPath(sXPlanePath);
	sprintf(filename,"%sResources\\plugins\\ArduinoConfig.xml",sXPlanePath);
#else

	char *path = NULL;
	path = getenv("XPlanePlugin");
	sprintf(filename,"%s\\ArduinoConfig.xml",path);

#endif

	configFile = new XmlConfig();
	configFile->Open(filename);

	_arduino = new ArduinoCom(configFile->ArduinoPort,9600);
	if(!_arduino->IsOpen())
	{
		return -1;
	}

	ArduinoDataRefs();
	InitializeStateMemory();

	// enable arduino flight loop
	XPLMRegisterFlightLoopCallback(		
		ArduinoFlightLoopCallback,					/* Callback */
		1.0f/200.0f,									/* Interval */
		NULL);										/* refcon not used. */


	// Debugging display window
// 	gWindow = XPLMCreateWindow(
// 		50, 600, 400, 500,			/* Area of the window. */
// 		1,							/* Start visible. */
// 		MyDrawWindowCallback,		/* Callbacks */
// 		MyHandleKeyCallback,
// 		MyHandleMouseClickCallback,
// 		NULL);						/* Refcon - not used. */
	return 1;
}

/*
	\brief Initialize button states to -1 so they will be changed on startup
*/
void InitializeStateMemory()
{
	lastState = create_states();
	currentState = create_states();

	//Get current states to update components
	MAX_PROP_SPEED_RADS = XPLMGetDataf(_maxPropSpeedDataref);
	MIN_PROP_SPEED_RADS = XPLMGetDataf(_minPropSpeedDataref);
	PROP_SPEED_RANGE_RADS = MAX_PROP_SPEED_RADS - MIN_PROP_SPEED_RADS;
	_trimPosition = XPLMGetDataf(_trimPositionDataref);
	_airspeed = XPLMGetDataf(_indicatedAirspeedDataref);
	update_trim_display();
}


/*
 \brief Initialize all data refs (X-Plane data pointers)
*/
void ArduinoDataRefs()
{
	/*eg. 
	XPLMDataRef _localx = XPLMFindDataRef("sim/flightmodel/position/local_x");
	*/
	_fuelPumpDataref			= XPLMFindDataRef("sim/cockpit/engine/fuel_pump_on");
	_strobeLightDataref			= XPLMFindDataRef("sim/cockpit/electrical/strobe_lights_on");
	_landingLightDataref		= XPLMFindDataRef("sim/cockpit/electrical/landing_lights_on");
	_taxiLightDataref			= XPLMFindDataRef("sim/cockpit/electrical/taxi_light_on");
	_navLightDataref			= XPLMFindDataRef("sim/cockpit/electrical/nav_lights_on");
	_avionicsMasterDataref		= XPLMFindDataRef("sim/cockpit/electrical/avionics_on");
	_generatorDataref			= XPLMFindDataRef("sim/cockpit/electrical/generator_on");
	_batteryDataref				= XPLMFindDataRef("sim/cockpit/electrical/battery_on");
	
	/*------ Others ---------*/
	_ignitionPositionDataref	= XPLMFindDataRef("sim/cockpit/engine/ignition_on");
	_igniterStateDataref		= XPLMFindDataRef("sim/cockpit/engine/igniters_on");
	_trimPositionDataref		= XPLMFindDataRef("sim/flightmodel/controls/elv_trim");
	_indicatedAirspeedDataref	= XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");
	_flapsPositionDataref		= XPLMFindDataRef("sim/flightmodel/controls/flaprat");
	_flapSwitchPositionDataref	= XPLMFindDataRef("sim/flightmodel/controls/flaprqst");
	_busVoltsDataref			= XPLMFindDataRef("sim/cockpit2/electrical/bus_volts");
	_maxTrimDataref				= XPLMFindDataRef("sim/aircraft/controls/acf_max_trim_elev"); //Nose up
	_minTrimDataref				= XPLMFindDataRef("sim/aircraft/controls/acf_min_trim_elev"); //Nose down
	
	/*-----  Controls  -----*/
	_throttleRatioDataref		= XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro");
	_commandedPropSpeedDataref	= XPLMFindDataRef("sim/flightmodel/engine/ENGN_prop");
	_pitchControlRatioDataref	= XPLMFindDataRef("sim/joystick/yoke_pitch_ratio");
	_rollControlRatioDataref	= XPLMFindDataRef("sim/joystick/yoke_roll_ratio");
	_yawControlRatioDataref		= XPLMFindDataRef("sim/joystick/yoke_heading_ratio");
	_leftBrakeRatioDataref		= XPLMFindDataRef("sim/flightmodel/controls/l_brake_add");
	_rightBrakeRatioDataref		= XPLMFindDataRef("sim/flightmodel/controls/r_brake_add");
	_carbHeatRatioDataref		= XPLMFindDataRef("sim/flightmodel/engine/ENGN_heat");
	_parkingBrakeRatioDataref	= XPLMFindDataRef("sim/flightmodel/controls/parkbrake");

	/* ------Prop Speed Refs ------------ */
	_maxPropSpeedDataref		= XPLMFindDataRef("sim/aircraft/controls/acf_RSC_redline_prp");
	_minPropSpeedDataref		= XPLMFindDataRef("sim/aircraft/controls/acf_RSC_mingov_prp");

	/* -------- Annunciator ------------- */
	_generatorWarningDataref	= XPLMFindDataRef("sim/cockpit/warnings/annunciators/generator");
	_fuelPressureWarningDataref = XPLMFindDataRef("sim/cockpit/warnings/annunciators/fuel_pressure");

	init_circuit_breaker_datarefs();
}



#pragma region DebuggerTextWindow
/*
				Function to print messages to screen (Useful for debugging)
*/
void MyDrawWindowCallback(
						  XPLMWindowID		 inWindowID,	
						  void *			   inRefcon)
{
	int		left, top, right, bottom;
	float	color[] = { 1.0, 1.0, 1.0 };	 /* RGB White */

	XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);
	XPLMDrawTranslucentDarkBox(left, top, right, bottom);
	XPLMDrawString(color, left + 5, top - 20, gTempBuffer, NULL, xplmFont_Basic);
}

void MyHandleKeyCallback(
						 XPLMWindowID		 inWindowID,	
						 char				 inKey,	
						 XPLMKeyFlags		 inFlags,	
						 char				 inVirtualKey,	
						 void *			   inRefcon,	
						 int				  losingFocus)
{
}								  

int MyHandleMouseClickCallback(
							   XPLMWindowID		 inWindowID,	
							   int				  x,	
							   int				  y,	
							   XPLMMouseStatus	  inMouse,	
							   void *			   inRefcon)
{
	return 1;
}
#pragma endregion

/**
\param inElapsedSinceLastCall Time (sec) since last call (dt).
\param inElapsedTimeSinceLastFlightLoop Time (sec) since last flight loop.
\param inCounter
\param inRefcon Reference to class.
\return next time you want function to be called (neg = sim frames, pos= seconds).
*/
float	ArduinoFlightLoopCallback(
			float                inElapsedSinceLastCall,    
			float                inElapsedTimeSinceLastFlightLoop,    
			int                  inCounter,    
			void *               inRefcon)
{
	//Ensure that the Arduino is sending updated states
	if (!_updating)
	{
		//Get current states to update components
		MAX_PROP_SPEED_RADS = XPLMGetDataf(_maxPropSpeedDataref);
		MIN_PROP_SPEED_RADS = XPLMGetDataf(_minPropSpeedDataref);
		PROP_SPEED_RANGE_RADS = MAX_PROP_SPEED_RADS - MIN_PROP_SPEED_RADS;
		_trimPosition = XPLMGetDataf(_trimPositionDataref);
		_airspeed = XPLMGetDataf(_indicatedAirspeedDataref);
		update_trim_display();

		if (_arduino->Initiate())
		{
			_updating = TRUE;
			_iLoopCalls = 0;
			return -1;
		} else 
		{
			return 2.0f;
		}

	}

	//Update x-plane update frequency
	//float ElapsedTimeDifference = (inElapsedTimeSinceLastFlightLoop - PreviousElapsedTimeSinceLastFlightLoop);
	//float UpdateFrequency = (1.0f / ElapsedTimeDifference);
	//sprintf_s(gTempBuffer, 256, "Local Hz = %0.2f",UpdateFrequency);
	//PreviousElapsedTimeSinceLastFlightLoop = inElapsedTimeSinceLastFlightLoop;

	//Read bytes from Arduino
	if ( _arduino->RecvCurrentState(&currentState) )
	{
		//sprintf_s(gTempBuffer, 256, "Local Hz = %0.2f, CBStates #%d", UpdateFrequency, currentState.cbStates);
		_iLoopCalls++;
		
		//If buffer was properly filled, update states
		UpdateStates();

		//Clear the buffer
		_arduino->ClearBuffer();
	}

	/* return -1 so we get called every frame
	   -2 would be every other frame
	   1.0 would be once every second */
	return -1;
}    


void UpdateStates()
{
	static int loopCounter = 0;
	update_controls();

	//Alternate between updating switches and circuit breakers
	if (loopCounter == 0)
	{
		loopCounter++;
		update_switches();
	}//if
	else
	{
		loopCounter = 0;
		update_circuit_breakers();
	}//else
	
	//Every 50 cycles, update annunciators
	if ( _iLoopCalls % 50 == 0)
	{
		update_annunciators();
	}

	//Every X cycles, update fan speed
	if (_iLoopCalls == 250)
	{
		update_ventilation_speed();
		_trimPosition = XPLMGetDataf(_trimPositionDataref);
		update_trim_display();
		_iLoopCalls = 0;
	}
}
#pragma region ControlInputs
void update_controls()
{
	/* Ratios */
	float throttle = 1.0f * ( currentState.throttle - configFile->ThrottleMin) / (configFile->ThrottleMax - configFile->ThrottleMin);
	float propSpeedRatio = 1.0f * (currentState.propSpeed - configFile->PropSpeedMin) / (configFile->PropSpeedMax - configFile->PropSpeedMin);
	float pitch = 2.0f * (currentState.pitch - configFile->PitchMin) / (configFile->PitchMax - configFile->PitchMin) - 1.0f;
	float roll = 2.0f * (currentState.roll - configFile->RollMin) / (configFile->RollMax - configFile->RollMin) - 1.0f;
	float yaw = 2.0f * (currentState.yaw - configFile->YawMin) / (configFile->YawMax - configFile->YawMin) - 1.0f;
	float lBrake = 1.0f * ( currentState.lBrake - configFile->LeftBrakeMin) / (configFile->LeftBrakeMax - configFile->LeftBrakeMin);
	float rBrake = 1.0f * ( currentState.rBrake - configFile->RightBrakeMin) / (configFile->RightBrakeMax - configFile->RightBrakeMin);
	float carbHeat = 1.0f * ( currentState.carbHeat - configFile->CarbHeatMin) / (configFile->CarbHeatMax - configFile->CarbHeatMin);
	float parkBrake = 1.0f * (currentState.pBrake - configFile->ParkBrakeMin) / (configFile->ParkBrakeMax - configFile->ParkBrakeMin);

	/* Correct for inverted values where needed  */
	throttle = invert_control(throttle,1,configFile->ThrottleInvert);
	propSpeedRatio = invert_control(propSpeedRatio,1,configFile->PropSpeedInvert);
	pitch = invert_control(pitch,0,configFile->PitchInvert);
	roll = invert_control(roll,0,configFile->RollInvert);
	yaw = invert_control(yaw,0,configFile->YawInvert);
	lBrake = invert_control(lBrake,1,configFile->LeftBrakeInvert);
	rBrake = invert_control(rBrake,1,configFile->RightBrakeInvert);
	carbHeat = invert_control(carbHeat,1,configFile->CarbHeatInvert);
	
	if ( carbHeat < 0.10f )
	{
		carbHeat = 0.0f;
	}
	
	parkBrake = invert_control(parkBrake, 1, configFile->ParkBrakeInvert);

	if ( parkBrake < 0.10f )
	{
		parkBrake = 0.0f;
	}

	/* Propeller Speed needs to be converted from ratio to rad/s */ //Point_tacrad
	float propSpeed = propSpeedRatio * PROP_SPEED_RANGE_RADS + MIN_PROP_SPEED_RADS;

	/* Update controls */
	XPLMSetDatavf(_throttleRatioDataref,&throttle,0,1);
	XPLMSetDatavf(_carbHeatRatioDataref,&carbHeat,0,1);
	XPLMSetDatavf(_commandedPropSpeedDataref,&propSpeed,0,1);
	XPLMSetDataf(_pitchControlRatioDataref,pitch);
	XPLMSetDataf(_rollControlRatioDataref,roll);
	XPLMSetDataf(_yawControlRatioDataref,yaw);
	XPLMSetDataf(_leftBrakeRatioDataref,lBrake);
	XPLMSetDataf(_rightBrakeRatioDataref,rBrake);
	XPLMSetDataf(_parkingBrakeRatioDataref,parkBrake);
}

// Checks if input values need to be inverted (eg switch Left and Right), and returns value accordingly
float invert_control(float percent, int positive, int invert)
{
	if (invert)
	{
		if (positive)
		{
			percent = 1 - percent;
		} else {
			percent *= -1.0;
		}
	}
	return percent;
}

#pragma endregion //ControlInputs

#pragma region Switches
void update_switches()
{
	static int ForceSwitchUpdateCounter = 0;
	int iStateChanges = 0;
	int entry = 0;

	//Every 60 calls to update_switches, force update (ie roughly 1 Hz)
	if (ForceSwitchUpdateCounter == 30)
	{
		//Toggle bool switches in current state, forcing update
		lastState.switchStates = ~currentState.switchStates;
	} //if
	else if (ForceSwitchUpdateCounter == 60)
	{
		// Force non bool switches to update by providing absurd former states
		lastState.igniterState	= (UINT8) 6;
		lastState.ignitionPos	= (UINT8) 6;
		lastState.trimSwitchPos = (UINT8) 0;
		lastState.flapSwitchPos = (UINT8) 4;
		ForceSwitchUpdateCounter = 0; // Reset counter
	} //else if

	// Update flaps display every 5 calls (roughly 10-12 Hz)
	if ( ForceSwitchUpdateCounter % 5 == 0)
	{
		update_flaps_display();
	} //if

	/* Starter Switch */
	int ignitionPos = currentState.ignitionPos;
	int igniterState = currentState.igniterState;
	
	if (lastState.ignitionPos != (UINT8)ignitionPos)
	{
		XPLMSetDatavi(_ignitionPositionDataref,&ignitionPos,0,1);
		lastState.ignitionPos = (UINT8)ignitionPos;
	}//if
	if (lastState.igniterState != (UINT8)igniterState)
	{
		//XPLMSetDatavi(_igniterStateDataref,&igniterState,0,1);
		if (igniterState)
		{
			XPLMCommandButtonPress(xplm_joy_start_0);
		}
		else
		{
			XPLMCommandButtonRelease(xplm_joy_start_0);
		}

		lastState.igniterState = (UINT8)igniterState;
	}//if

	iStateChanges = currentState.switchStates ^ lastState.switchStates;
	
	//Change state of Fuel Pump for engine 1
	if ( iStateChanges & SWITCHES_FUELPUMP_ON )
	{
		entry = (0 != (currentState.switchStates & SWITCHES_FUELPUMP_ON));
		XPLMSetDatavi(_fuelPumpDataref,&entry,0,1);
	}//if

	//Change state of Strobe light
	if ( iStateChanges & SWITCHES_STROBELIGHT_ON)
	{
		entry = (0 != (currentState.switchStates & SWITCHES_STROBELIGHT_ON));
		XPLMSetDatai(_strobeLightDataref,entry);
	}//if

	//Change state of landing light
	if ( iStateChanges & SWITCHES_LANDINGLIGHT_ON)
	{
		entry = (0 != (currentState.switchStates & SWITCHES_LANDINGLIGHT_ON));
		XPLMSetDatai(_landingLightDataref,entry);
	}//if

	//Change state of taxi light
	if ( iStateChanges & SWITCHES_TAXILIGHT_ON)
	{
		entry = (0 != (currentState.switchStates & SWITCHES_TAXILIGHT_ON));
		XPLMSetDatai(_taxiLightDataref,entry);
	}//if

	//Change state of position light
	if ( iStateChanges & SWITCHES_NAVLIGHT_ON)
	{
		entry = (0 != (currentState.switchStates & SWITCHES_NAVLIGHT_ON));
		XPLMSetDatai(_navLightDataref,entry);
	}//if

	//Change state of avionics master
	if ( iStateChanges & SWITCHES_AVIONICSMASTER_ON)
	{
		entry = (0 != (currentState.switchStates & SWITCHES_AVIONICSMASTER_ON));
		XPLMSetDatai(_avionicsMasterDataref,entry);
	}//if

	//Change state of generator for engine 1
	if ( iStateChanges & SWITCHES_GENERATOR_ON)
	{
		entry = (0 != (currentState.switchStates & SWITCHES_GENERATOR_ON));
		XPLMSetDatavi(_generatorDataref,&entry,0,1);
	}//if

	//Change state of battery
	if ( iStateChanges & SWITCHES_BATTERY_ON)
	{
		entry = (0 != (currentState.switchStates & SWITCHES_BATTERY_ON));
		XPLMSetDatai(_batteryDataref,entry);
	}//if

	//Trim Up Button pressed (holding button does not cause continuous increase)
	if (currentState.trimSwitchPos == 1)
	{
		update_trim_position(0.025f);
		lastState.trimSwitchPos = 1;
	} else if (currentState.trimSwitchPos == 0 && lastState.trimSwitchPos == 1)
	{
		lastState.trimSwitchPos = 0;
	}//if

	//Trim Dn Button pressed (holding button does not cause continuous decrease)
	if (currentState.trimSwitchPos == 2)
	{
		update_trim_position(-0.025f);
		lastState.trimSwitchPos = 2;
	} else if (currentState.trimSwitchPos == 0 && lastState.trimSwitchPos == 2)
	{
		lastState.trimSwitchPos = 0;
	}//if

	//Update flap position switch
	if (currentState.flapSwitchPos != lastState.flapSwitchPos )
	{
		float f_flapRatio = 0.0f;
		switch (currentState.flapSwitchPos)
		{
			case TAKEOFF_FLAPS:
				f_flapRatio = TAKEOFF_FLAPS_RATIO;
				break;
			case FULL_FLAPS:
				f_flapRatio = 1.0f;
				break;
		} //Switch
		XPLMSetDataf(_flapSwitchPositionDataref,f_flapRatio);
	} //if

	//Update bitwise points for switches
	lastState.switchStates = currentState.switchStates;
	ForceSwitchUpdateCounter++; //Update counter for next call
}
#pragma endregion //Switches

#pragma region CircuitBreakers
void update_circuit_breakers()
{
	//Go through circuit breakers updating (if needed) status of failure
	//Note: in x plane, proper value is value = 6 (immediate failure)
	//      Need to set value = 6 * entry (ie = 0 or 6)
	static int ForceCBUpdateCounter = 0;
	int entry = 0;
	int iStateChanges = lastState.cbStates ^ currentState.cbStates;

	//Force update once every 20 calls 
	if (ForceCBUpdateCounter == 20)
	{
		//Toggle values to force update
		//lastState.cbStates = ~currentState.cbStates;
		ForceCBUpdateCounter = 0; //Reset counter
	}

	//CB#1
	if ((iStateChanges & (1 << 0))  || (ForceCBUpdateCounter == 0) )
	{
		entry = (0 != (currentState.cbStates & (1 << 0)));
		XPLMSetDatai(_cb1Datarefs[0], 6 * entry );
		XPLMSetDatai(_cb1Datarefs[1], 6 * entry );
	}

	//CB#2
	if ((iStateChanges & (1 << 1))  || (ForceCBUpdateCounter == 0) )
	{
		entry = (0 != (currentState.cbStates & (1 << 1)));
		XPLMSetDatai(_cb2Datarefs[0], 6 * entry );
		XPLMSetDatai(_cb2Datarefs[1], 6 * entry );
		XPLMSetDatai(_cb2Datarefs[2], 6 * entry );
	}

	//CB#3
	if ((iStateChanges & (1 << 2))  || (ForceCBUpdateCounter == 0) )
	{
		entry = (0 != (currentState.cbStates & (1 << 2)));
		XPLMSetDatai(_cb3Dataref, 6 * entry );
	}

	//CB#4
	if ((iStateChanges & (1 << 3))  || (ForceCBUpdateCounter == 0) )
	{
		entry = (0 != (currentState.cbStates & (1 << 3)));
		XPLMSetDatai(_cb4Dataref, 6 * entry );
	}

	//CB#5
	if ((iStateChanges & (1 << 4))  || (ForceCBUpdateCounter == 5) )
	{
		entry = (0 != (currentState.cbStates & (1 << 4)));
		XPLMSetDatai(_cb5Dataref, 6 * entry );
	}

	//CB#6 - Internal Lights (ie not x-plane
	//entry = (int)(currentState.cbStates & (1 << 5));
	//if ( (lastState.cbStates & (1 << 5)) != entry)
	//{
	//	XPLMSetDatai(_cb6Dataref, 6 * entry );
	//}

	//CB#7
	if ((iStateChanges & (1 << 6))  || (ForceCBUpdateCounter == 5) )
	{
		entry = (0 != (currentState.cbStates & (1 << 6)));
		XPLMSetDatai(_cb7Dataref, 6 * entry );
	}

	//CB#8
	if ((iStateChanges & (1 << 7))  || (ForceCBUpdateCounter == 5) )
	{
		entry = (0 != (currentState.cbStates & (1 << 7)));
		XPLMSetDatai(_cb8Dataref, 6 * entry );
	}

	//CB#9
	if ((iStateChanges & (1 << 8))  || (ForceCBUpdateCounter == 5) )
	{
		entry = (0 != (currentState.cbStates & (1 << 8)));
		XPLMSetDatai(_cb9Dataref, 6 * entry );
	}

	//CB#10
	if ((iStateChanges & (1 << 9))  || (ForceCBUpdateCounter == 5) )
	{
		entry = (0 != (currentState.cbStates & (1 << 9)));
		XPLMSetDatai(_cb10Dataref, 6 * entry );
	}

	//CB#11 
	if ((iStateChanges & (1 << 10))  || (ForceCBUpdateCounter == 5) )
	{
		entry = (0 != (currentState.cbStates & (1 << 10)));
		XPLMSetDatai(_cb11Dataref, 6 * entry );
	}

	//CB#12 - Battery
	if ((iStateChanges & (1 << 11))  || (ForceCBUpdateCounter == 5) )
	{
		entry = (0 != (currentState.cbStates & (1 << 11)));
		XPLMSetDatai(_cb12Dataref, 6 * entry );
	}

	//CB#13
	if ((iStateChanges & (1 << 12))  || (ForceCBUpdateCounter == 10) )
	{
		entry = (0 != (currentState.cbStates & (1 << 12)));
		XPLMSetDatai(_cb13Dataref, 6 * entry );
	}

	//CB#14
	if ((iStateChanges & (1 << 13))  || (ForceCBUpdateCounter == 10) )
	{
		entry = (0 != (currentState.cbStates & (1 << 13)));
		XPLMSetDatai(_cb14Dataref, 6 * entry );
	}

	//CB#15
	if ((iStateChanges & (1 << 14))  || (ForceCBUpdateCounter == 10) )
	{
		entry = (0 != (currentState.cbStates & (1 << 14)));
		XPLMSetDatai(_cb15Dataref, 6 * entry );
	}



	//CB#16
	/*if ((iStateChanges & (1 << 15))  || (ForceCBUpdateCounter == 50) )
	{
		entry = (0 != (currentState.cbStates & (1 << 15)));
		XPLMSetDatai(_cb16Dataref, 6 * entry );
	}*/
	
	//CB#17
	if ((iStateChanges & (1 << 16))  || (ForceCBUpdateCounter == 10) )
	{
		entry = (0 != (currentState.cbStates & (1 << 16)));
		XPLMSetDatai(_cb17Dataref, 6 * entry );
	}
	
	
	//CB#18
	if ((iStateChanges & (1 << 17))  || (ForceCBUpdateCounter == 10) )
	{
		entry = (0 != (currentState.cbStates & (1 << 17)));
		XPLMSetDatai(_cb18Dataref, 6 * entry );
	}
	
	//CB#19
	if ((iStateChanges & (1 << 18))  || (ForceCBUpdateCounter == 10) )
	{
		entry = (0 != (currentState.cbStates & (1 << 18)));
		XPLMSetDatai(_cb19Dataref, 6 * entry );
	}
	
	//CB#20
	if ((iStateChanges & (1 << 19))  || (ForceCBUpdateCounter == 15) )
	{
		entry = (0 != (currentState.cbStates & (1 << 19)));
		XPLMSetDatai(_cb20Dataref, 6 * entry );
	}

	//CB#21
	if ((iStateChanges & (1 << 20))  || (ForceCBUpdateCounter == 15) )
	{
		entry = (0 != (currentState.cbStates & (1 << 20)));
		XPLMSetDatai(_cb21Dataref, 6 * entry );
	}

	//CB#22
	if ((iStateChanges & (1 << 21))  || (ForceCBUpdateCounter == 15) )
	{
		entry = (0 != (currentState.cbStates & (1 << 21)));
		XPLMSetDatai(_cb22Dataref, 6 * entry );
	}
	//CB#23
	/*if ((iStateChanges & (1 << 22))  || (ForceCBUpdateCounter == 75) )
	{
		entry = (0 != (currentState.cbStates & (1 << 22)));
		XPLMSetDatai(_cb23Dataref, 6 * entry );
	}*/

	//CB#24
	if ((iStateChanges & (1 << 23))  || (ForceCBUpdateCounter == 15) )
	{
		entry = (0 != (currentState.cbStates & (1 << 23)));
		XPLMSetDatai(_cb24Dataref, 6 * entry );
	}
	//CB#25
	if ((iStateChanges & (1 << 24))  || (ForceCBUpdateCounter == 15) )
	{
		entry = (0 != (currentState.cbStates & (1 << 24)));
		XPLMSetDatai(_cb25Dataref, 6 * entry );
	}

	//CB#26
	if ((iStateChanges & (1 << 25))  || (ForceCBUpdateCounter == 15) )
	{
		entry = (0 != (currentState.cbStates & (1 << 25)));
		XPLMSetDatai(_cb26Dataref, 6 * entry );
	}

	ForceCBUpdateCounter++;
}
#pragma endregion //CircuitBreakers

#pragma  region SendUpdateData
//Change trim state in X-Plane
void update_trim_position(float trimIncrement)
{
	//Elevation Trim, -1 = max nose down, 1 = max nose up
	float trim = _trimPosition + trimIncrement;

	//Ensure change in trim can be made
	//if (MIN_TRIM_DEFLECTION <= trim && trim <= MAX_TRIM_DEFLECTION)
	if ( -1 <= trim && trim <= 1 )
	{
		_trimPosition = trim;
		XPLMSetDataf(_trimPositionDataref,_trimPosition);
		update_trim_display();
	}
}

//Send command to Arduino to update flaps position display
void update_flaps_display()
{
	static const float f_flapsDisplayTolerance = 0.05f;
	float f_flapsRatio = XPLMGetDataf(_flapsPositionDataref);
	float f_busVolts = 0;
	XPLMGetDatavf(_busVoltsDataref,&f_busVolts,0,1);

	// If voltage is too low (gen and batt fail) then light should be off
	if (f_busVolts < 10.0)
	{
		_arduino->SendState(FLAPS_DISPLAY,0);
		return;
	}

	//No Flaps when ratio less than f_flapsDisplayTolerance
	if (f_flapsRatio <= f_flapsDisplayTolerance)
	{
		_arduino->SendState(FLAPS_DISPLAY,17);
	} 
	else if (f_flapsRatio <= TAKEOFF_FLAPS_RATIO - 2 * f_flapsDisplayTolerance) //Transition until TAKEOFF_FLAPS_RATIO - f_flapsDisplayTolerance
	{
		_arduino->SendState(FLAPS_DISPLAY,22);
	}
	else if (f_flapsRatio <= TAKEOFF_FLAPS_RATIO + f_flapsDisplayTolerance) //Takeoff flaps
	{
		_arduino->SendState(FLAPS_DISPLAY,28);
	} 
	else if (f_flapsRatio <= 1.0f - f_flapsDisplayTolerance) //Transition
	{
		_arduino->SendState(FLAPS_DISPLAY,35);
	}
	else //Full flaps
	{
		_arduino->SendState(FLAPS_DISPLAY,45);
	} //if
}

// Update Annunciators
void update_annunciators()
{
	int annunciatorState = 0;

	// Update Generator Annunciator Warning
	annunciatorState = XPLMGetDatai(_generatorWarningDataref);
	_arduino->SendState(GENERATOR_WARNING,annunciatorState);

	// Update Fuel Pressure Annunciator Warning
	annunciatorState = XPLMGetDatai(_fuelPressureWarningDataref);
	_arduino->SendState(FUEL_PRESSURE_WARNING,annunciatorState);
}

//Send command to Arduino to update trim tab display
void update_trim_display()
{
	int trimValue = 0;
	float f_busVolts = 0;
	XPLMGetDatavf(_busVoltsDataref,&f_busVolts,0,1);

	// If voltage is too low (gen and batt fail) then light should be off
	if (f_busVolts < 10.0)
	{
		_arduino->SendState(TRIM_DISPLAY,0);
		return;
	}

	//Want value 5 - 70 (Trim has max VDC ~ 1.25 VDC)
	trimValue = (int) (65 * (-_trimPosition - MIN_TRIM_DEFLECTION) / (MAX_TRIM_DEFLECTION-MIN_TRIM_DEFLECTION) + 5);

	_arduino->SendState(TRIM_DISPLAY,trimValue);
}

//Sends command to update ventilation fan speed based on airspeed
void update_ventilation_speed()
{
	int fanSpeed = 0;

	_airspeed = XPLMGetDataf(_indicatedAirspeedDataref);
	fanSpeed =  max((int)(_airspeed * 255 / MAX_AIRSPEED), 50 );

	_arduino->SendState(FAN_SPEED,fanSpeed);
}
#pragma endregion //SendUpdate


//Ensure no null pointers in structure
struct ArduinoStates create_states(){

	struct ArduinoStates state;

	state.igniterState = (UINT8) 6;
	state.ignitionPos = (UINT8) 6;
	state.switchStates = (UINT8) 0;
	state.cbStates = (UINT32) 0;
	state.trimSwitchPos = (UINT8) 0;
	state.flapSwitchPos = (UINT8) 4;

	state.throttle = 0;
	state.propSpeed = 0;
	state.pitch = 0;
	state.roll = 0;
	state.yaw = 0;
	state.carbHeat = 0;
	state.lBrake = 0;
	state.rBrake = 0;

	return state;
}



void init_circuit_breaker_datarefs()
{
	
	_cb1Datarefs[0]	= XPLMFindDataRef("sim/operation/failures/rel_CHT_ind_0");
	_cb1Datarefs[1]	= XPLMFindDataRef("sim/operation/failures/rel_oilp_ind_0");
	
	_cb2Datarefs[0]	= XPLMFindDataRef("sim/operation/failures/rel_oilt_ind_0");
	_cb2Datarefs[1]	= XPLMFindDataRef("sim/operation/failures/rel_g_fuel");
	_cb2Datarefs[2]	= XPLMFindDataRef("sim/operation/failures/rel_g_oat");

	_cb3Dataref		= XPLMFindDataRef("sim/operation/failures/rel_ele_fuepmp0");
	_cb4Dataref		= XPLMFindDataRef("sim/operation/failures/rel_lites_land");
	_cb5Dataref		= XPLMFindDataRef("sim/operation/failures/rel_lites_taxi");
	_cb6Dataref		= XPLMFindDataRef("sim/operation/failures/rel_clights");
	_cb7Dataref		= XPLMFindDataRef("sim/operation/failures/rel_lites_nav");
	_cb8Dataref		= XPLMFindDataRef("sim/operation/failures/rel_lites_strobe");
	_cb9Dataref		= XPLMFindDataRef("sim/operation/failures/rel_startr0");
	_cb10Dataref	= XPLMFindDataRef("sim/operation/failures/rel_genera0"); //-----------NEED TO FIND SOMETHING TO FAIL!!!!
	_cb11Dataref	= XPLMFindDataRef("sim/operation/failures/rel_genera0");
	_cb12Dataref	= XPLMFindDataRef("sim/operation/failures/rel_batter0");

	_cb13Dataref	= XPLMFindDataRef("sim/operation/failures/rel_nav1");
	_cb14Dataref	= XPLMFindDataRef("sim/operation/failures/rel_nav2");
	_cb15Dataref	= XPLMFindDataRef("sim/operation/failures/rel_xpndr");
	_cb16Dataref	= XPLMFindDataRef("sim/operation/failures/rel_"); //--------WHAT IS ICS???
	_cb17Dataref	= XPLMFindDataRef("sim/operation/failures/rel_gen_avio");
	_cb18Dataref	= XPLMFindDataRef("sim/operation/failures/rel_gen_avio"); //--------FIND APPROPRIATE FAILURE!!!
	_cb19Dataref	= XPLMFindDataRef("sim/operation/failures/rel_ss_ahz");
	_cb20Dataref	= XPLMFindDataRef("sim/operation/failures/rel_ss_dgy");
	_cb21Dataref	= XPLMFindDataRef("sim/operation/failures/rel_ss_tsi");
	_cb22Dataref	= XPLMFindDataRef("sim/operation/failures/rel_flap_act");
	_cb23Dataref	= XPLMFindDataRef("sim/operation/failures/rel_"); //-------------NEED TO FAIL ELEV TRIM

	_cb24Dataref	= XPLMFindDataRef("sim/operation/failures/rel_adf1");
	_cb25Dataref	= XPLMFindDataRef("sim/operation/failures/rel_dme");
	_cb26Dataref	= XPLMFindDataRef("sim/operation/failures/rel_marker");

}