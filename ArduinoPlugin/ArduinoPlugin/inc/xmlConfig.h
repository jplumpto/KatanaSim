//---------------------------------------------------------------------
// XML Config header file
//---------------------------------------------------------------------
#ifndef _XML_CONFIG_H_
#define _XML_CONFIG_H_

// Disable depreciation warning.
#pragma warning( disable : 4996 )

#include <stdio.h> 
#include <conio.h> 
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


class XmlConfig {
public:
	XmlConfig();                            // constructor; initialize the list to be empty
	void Open(const char *filename);         // Opens the specified Config file and returns the parameters

	char ArduinoPort[100];
	int Delay;
	int CommFlush;
	int ThrottleMin;
	int ThrottleMax;
	int ThrottleInvert;
	int PitchMin;
	int PitchMax;
	int PitchInvert;
	int RollMin;
	int RollMax;
	int RollInvert;
	int YawMin;
	int YawMax;
	int YawInvert;
	int LeftBrakeMin;
	int LeftBrakeMax;
	int LeftBrakeInvert;
	int RightBrakeMin;
	int RightBrakeMax;
	int RightBrakeInvert;
	int PropSpeedMin;
	int PropSpeedMax;
	int PropSpeedInvert;
	int CarbHeatMin;
	int CarbHeatMax;
	int CarbHeatInvert;
	int ChokeMin;
	int ChokeMax;
	int ChokeInvert;
	int ParkBrakeMin;
	int ParkBrakeMax;
	int ParkBrakeInvert;

	int IsOpen;

private:
};



#endif
