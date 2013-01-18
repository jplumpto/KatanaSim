#include "../inc/arduinoPlugin.h"

ArduinoStates currentState;
ArduinoStates lastState;
XmlConfig *configFile;
float MAX_PROP_SPEED_RADS = 2550;
float IDLE_PROP_SPEED_RADS = 0;
float PROP_SPEED_RANGE_RADS = 0;
const float MAX_TRIM_DEFLECTION = 0.5f;
const float MIN_TRIM_DEFLECTION = -0.5f;
const float MAX_AIRSPEED = 120.0f;

//Arduino Communications
ArduinoCom	*_arduino	= NULL;
bool		_updating	= FALSE;

//Position Variables
float			_trimPosition = 0.0f;
float			_airspeed	  = 0.0f;

//X-Plane Switch Refs
XPLMDataRef _fuel = NULL;
XPLMDataRef _strobe = NULL;
XPLMDataRef _landing = NULL;
XPLMDataRef _taxi = NULL;
XPLMDataRef _position = NULL;
XPLMDataRef _masterAv = NULL;
XPLMDataRef _generator = NULL;
XPLMDataRef _battery = NULL;

//X-Plane StarterControl Refs
XPLMDataRef _ignitionPos = NULL;
XPLMDataRef _igniterState = NULL;

//X-Plane ParamRefs
XPLMDataRef _trimRef = NULL;
XPLMDataRef _airspeedRef = NULL;

//X-Plane Control Refs
XPLMDataRef _throttle = NULL;
XPLMDataRef _pitch = NULL;
XPLMDataRef _roll = NULL;
XPLMDataRef _yaw = NULL;
XPLMDataRef _propSpeed = NULL;
XPLMDataRef _lBrake = NULL;
XPLMDataRef _rBrake = NULL;
XPLMDataRef _carbHeat = NULL;
XPLMDataRef _parkBrake = NULL;

//X-Plane Prop Refs
XPLMDataRef _maxPropSpeed = NULL;
XPLMDataRef _idlePropSpeed = NULL;


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
	_fuel = NULL;
	_strobe = NULL;
	_landing = NULL;
	_taxi = NULL;
	_position = NULL;
	_masterAv = NULL;
	_generator = NULL;
	_battery = NULL;

	_igniterState = NULL;
	_ignitionPos = NULL;

	_trimRef = NULL;
	_airspeedRef = NULL;

	/*------Controls--------*/
	_throttle = NULL;
	_propSpeed = NULL;
	_pitch = NULL;
	_roll = NULL;
	_yaw = NULL;
	_lBrake = NULL;
	_rBrake = NULL;
	_carbHeat = NULL;
	_parkBrake = NULL;

	XPLMUnregisterFlightLoopCallback(ArduinoFlightLoopCallback, NULL);
	_arduino->~ArduinoCom();

}


/**
\brief called when users reenables this plugin.
registers MyPosFlightLoopCallback.
*/
PLUGIN_API int XPluginEnable(void)
{
	char filename[1000];
	char *path;

	path = getenv("XPlanePlugin");
	sprintf(filename,"%s\\ArduinoConfig.xml",path);

	configFile = new XmlConfig();
	configFile->Open(filename);

	ArduinoDataRefs();
	InitializeStateMemory();

	_arduino = new ArduinoCom(configFile->ArduinoPort,9600);
	if(!_arduino->IsOpen())
	{
		return -1;
	}

	// enable arduino flight loop
	XPLMRegisterFlightLoopCallback(		
		ArduinoFlightLoopCallback,					/* Callback */
		1.0f/200.0f,									/* Interval */
		NULL);										/* refcon not used. */

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
	MAX_PROP_SPEED_RADS = XPLMGetDataf(_maxPropSpeed);
	IDLE_PROP_SPEED_RADS = XPLMGetDataf(_idlePropSpeed);
	PROP_SPEED_RANGE_RADS = MAX_PROP_SPEED_RADS - IDLE_PROP_SPEED_RADS;
	_trimPosition = XPLMGetDataf(_trimRef);
	_airspeed = XPLMGetDataf(_airspeedRef);
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
	_fuel = XPLMFindDataRef("sim/cockpit/engine/fuel_pump_on");
	_strobe = XPLMFindDataRef("sim/cockpit/electrical/strobe_lights_on");
	_landing = XPLMFindDataRef("sim/cockpit/electrical/landing_lights_on");
	_taxi = XPLMFindDataRef("sim/cockpit/electrical/taxi_light_on");
	_position = XPLMFindDataRef("sim/cockpit/electrical/nav_lights_on");
	_masterAv = XPLMFindDataRef("sim/cockpit/electrical/avionics_on");
	_generator = XPLMFindDataRef("sim/cockpit/electrical/generator_on");
	_battery = XPLMFindDataRef("sim/cockpit/electrical/battery_on");
	
	/*------ Others ---------*/
	_ignitionPos = XPLMFindDataRef("sim/cockpit/engine/ignition_on");
	_igniterState = XPLMFindDataRef("sim/cockpit/engine/igniters_on");
	_trimRef = XPLMFindDataRef("sim/flightmodel/controls/elv_trim");
	_airspeedRef = XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");
	
	/*-----  Controls  -----*/
	_throttle = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro");
	_propSpeed = XPLMFindDataRef("sim/flightmodel/engine/ENGN_prop");
	_pitch = XPLMFindDataRef("sim/joystick/yoke_pitch_ratio");
	_roll = XPLMFindDataRef("sim/joystick/yoke_roll_ratio");
	_yaw = XPLMFindDataRef("sim/joystick/yoke_heading_ratio");
	_lBrake = XPLMFindDataRef("sim/flightmodel/controls/l_brake_add");
	_rBrake = XPLMFindDataRef("sim/flightmodel/controls/r_brake_add");
	_carbHeat = XPLMFindDataRef("sim/flightmodel/engine/ENGN_heat");
	_parkBrake = XPLMFindDataRef("sim/flightmodel/controls/parkbrake");

	/* ------Prop Speed Refs ------------ */
	_maxPropSpeed = XPLMFindDataRef("sim/aircraft/controls/acf_RSC_redline_prp");
	_idlePropSpeed = XPLMFindDataRef("sim/aircraft/controls/acf_RSC_idlespeed_prp");
}



/**
	\brief not used.
*/
PLUGIN_API void XPluginReceiveMessage(
												  XPLMPluginID	inFromWho,
												  long			inMessage,
												  void *			inParam)
{

}


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
		if (_arduino->Initiate())
		{
			_updating = TRUE;
			return -1;
		} else 
		{
			return 2.0f;
		}
	}

	//Read bytes from Arduino
	if ( _arduino->LatestStates(&currentState) )
	{
		//If buffer was properly filled, update states
		UpdateStates();
	}

	/* return -1 so we get called every frame
	   -2 would be every other frame
	   1.0 would be once every second */
	return -1;
}    


void UpdateStates()
{
	update_controls();
	update_buttons();

	//Every X cycles, update fan speed
	if (XPLMGetCycleNumber() % 500 == 0)
	{
		update_fan_speed();
	}
}

void update_controls()
{
	/* Ratios */
	float throttle = 1.0f * ( currentState.throttle - configFile->ThrottleMin) / (configFile->ThrottleMax - configFile->ThrottleMin);
	float propSpeedRatio = 1.0f * (currentState.propSpeed - configFile->EngineMin) / (configFile->EngineMax - configFile->EngineMin);
	float pitch = 2.0f * (currentState.pitch - configFile->PitchMin) / (configFile->PitchMax - configFile->PitchMin) - 1.0f;
	float roll = 2.0f * (currentState.roll - configFile->RollMin) / (configFile->RollMax - configFile->RollMin) - 1.0f;
	float yaw = 2.0f * (currentState.yaw - configFile->YawMin) / (configFile->YawMax - configFile->YawMin) - 1.0f;
	float lBrake = 1.0f * ( currentState.lBrake - configFile->LeftBrakeMin) / (configFile->LeftBrakeMax - configFile->LeftBrakeMin);
	float rBrake = 1.0f * ( currentState.rBrake - configFile->RightBrakeMin) / (configFile->RightBrakeMax - configFile->RightBrakeMin);
	float carbHeat = 1.0f * ( currentState.carbHeat - configFile->CarbHeatMin) / (configFile->CarbHeatMax - configFile->CarbHeatMin);

	/* Correct for inverted values where needed  */
	throttle = invert_control(throttle,1,configFile->ThrottleInvert);
	propSpeedRatio = invert_control(propSpeedRatio,1,configFile->EngineInvert);
	pitch = invert_control(pitch,0,configFile->PitchInvert);
	roll = invert_control(roll,0,configFile->RollInvert);
	yaw = invert_control(yaw,0,configFile->YawInvert);
	lBrake = invert_control(lBrake,1,configFile->LeftBrakeInvert);
	rBrake = invert_control(rBrake,1,configFile->RightBrakeInvert);
	carbHeat = invert_control(carbHeat,1,configFile->CarbHeatInvert);

	/* Propeller Speed needs to be converted from ratio to rad/s */
	float propSpeed = propSpeedRatio * PROP_SPEED_RANGE_RADS + IDLE_PROP_SPEED_RADS;

	/* Update controls */
	XPLMSetDatavf(_throttle,&throttle,0,1);
	XPLMSetDatavf(_carbHeat,&carbHeat,0,1);
	XPLMSetDataf(_propSpeed,propSpeed);
	XPLMSetDataf(_pitch,pitch);
	XPLMSetDataf(_roll,roll);
	XPLMSetDataf(_yaw,yaw);
	XPLMSetDataf(_lBrake,lBrake);
	XPLMSetDataf(_rBrake,rBrake);
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


void update_buttons()
{
	/* Starter Switch */
	int ignitionPos = currentState.ignitionPos;
	int igniterState = currentState.igniterState;
	
	if (lastState.ignitionPos != ignitionPos)
	{
		XPLMSetDatavi(_ignitionPos,&ignitionPos,0,1);
		lastState.ignitionPos = ignitionPos;
	}
	if (lastState.igniterState != igniterState)
	{
		XPLMSetDatavi(_igniterState,&igniterState,0,1);
		lastState.igniterState = igniterState;
	}
	
	//Change state of Fuel Pump for engine 1
	if (lastState.fuelState != currentState.fuelState)
	{
		int entry = (int)currentState.fuelState;
		XPLMSetDatavi(_fuel,&entry,0,1);
		lastState.fuelState = currentState.fuelState;
	}

	//Change state of Strobe light
	if (lastState.strobeState != currentState.strobeState)
	{
		XPLMSetDatai(_strobe,currentState.strobeState);
		lastState.strobeState = currentState.strobeState;
	}

	//Change state of landing light
	if (lastState.landingState != currentState.landingState)
	{
		XPLMSetDatai(_landing,currentState.landingState);
		lastState.landingState = currentState.landingState;
	}

	//Change state of taxi light
	if (lastState.taxiState != currentState.taxiState)
	{
		XPLMSetDatai(_taxi,currentState.taxiState);
		lastState.taxiState = currentState.taxiState;
	}

	//Change state of position light
	if (lastState.positionState != currentState.positionState)
	{
		XPLMSetDatai(_position,currentState.positionState);
		lastState.positionState = currentState.positionState;
	}

	//Change state of avionics master
	if (lastState.avMasState != currentState.avMasState)
	{
		XPLMSetDatai(_masterAv,currentState.avMasState);
		lastState.avMasState = currentState.avMasState;
	}

	//Change state of generator for engine 1
	if (lastState.generatorState != currentState.generatorState)
	{
		int entry = (int)currentState.generatorState;
		XPLMSetDatavi(_generator,&entry,0,1);
		lastState.generatorState = currentState.generatorState;
	}

	//Change state of battery
	if (lastState.batteryState != currentState.batteryState)
	{
		XPLMSetDatai(_battery,currentState.batteryState);
		lastState.batteryState = currentState.batteryState;
	}

	//Trim Up Button pressed (holding button does not cause continuous increase)
	if (currentState.trimUpState == 1 && lastState.trimUpState == 0)
	{
		update_trim_position(0.05f);
		lastState.trimUpState = 1;
	} else if (currentState.trimUpState == 0 && lastState.trimUpState == 1)
	{
		lastState.trimUpState = 0;
	}

	//Trim Dn Button pressed (holding button does not cause continuous decrease)
	if (currentState.trimDownState == 1 && lastState.trimDownState == 0)
	{
		update_trim_position(-0.05f);
		lastState.trimDownState = 1;
	} else if (currentState.trimDownState == 0 && lastState.trimDownState == 1)
	{
		lastState.trimDownState = 0;
	}
}


//Change trim state in X-Plane
void update_trim_position(float trimIncrement)
{
	float trim = _trimPosition + trimIncrement;

	//Ensure change in trim can be made
	if (MIN_TRIM_DEFLECTION < trim && trim < MAX_TRIM_DEFLECTION)
	{
		_trimPosition = trim;
		XPLMSetDataf(_trimRef,_trimPosition);
		update_trim_display();
	}
}

//Send command to arduino to update trim tab display
void update_trim_display()
{
	int trimValue = 0;

	//Want value 0 - 255
	trimValue = (int) (255 * (_trimPosition - MIN_TRIM_DEFLECTION) / (MAX_TRIM_DEFLECTION-MIN_TRIM_DEFLECTION));

	_arduino->SendState(TRIM_DISPLAY,trimValue);
}

//Sends command to update ventilation fan speed based on airspeed
void update_fan_speed()
{
	int fanSpeed = 0;

	_airspeed = XPLMGetDataf(_airspeedRef);
	fanSpeed =  max((int)(_airspeed * 255 / MAX_AIRSPEED), 50 );

	_arduino->SendState(FAN_SPEED,fanSpeed);
}


//Ensure no null pointers in structure
struct ArduinoStates create_states(){

	struct ArduinoStates state;

	state.igniterState = -1;
	state.ignitionPos = -1;
	state.fuelState = -1;
	state.strobeState = -1;
	state.landingState = -1;
	state.taxiState = -1;
	state.positionState = -1;
	state.avMasState = -1;
	state.generatorState = -1;
	state.batteryState = -1;
	state.trimDownState = 0;
	state.trimUpState = 0;

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