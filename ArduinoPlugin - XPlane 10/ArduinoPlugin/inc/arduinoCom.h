//---------------------------------------------------------------------
// Arduino Communications header file
//---------------------------------------------------------------------
#ifndef _ARDUINO_COM_H_
#define _ARDUINO_COM_H_

//X-Plane debug
#include "XPLM/XPLMUtilities.h"

//Arduino Includes
#include <stdio.h>
#include <tchar.h>

#include <Windows.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define ARDUINO_WAIT_TIME 2000

//Enums for sending commands to Arduino
#define TRIM_DISPLAY 0
#define FLAPS_DISPLAY 1
#define FAN_SPEED 2
#define GENERATOR_WARNING 3
#define FUEL_PRESSURE_WARNING 4

const UINT8 SWITCHES_CLEAR             =   0;   // 0x00000000
const UINT8 SWITCHES_FUELPUMP_ON       =   1;   // 0x00000001
const UINT8 SWITCHES_STROBELIGHT_ON    =   2;   // 0x00000010
const UINT8 SWITCHES_LANDINGLIGHT_ON   =   4;   // 0x00000100
const UINT8 SWITCHES_TAXILIGHT_ON      =   8;   // 0x00001000
const UINT8 SWITCHES_NAVLIGHT_ON       =   16;  // 0x00010000
const UINT8 SWITCHES_AVIONICSMASTER_ON =   32;  // 0x00100000
const UINT8 SWITCHES_GENERATOR_ON      =   64;  // 0x01000000
const UINT8 SWITCHES_BATTERY_ON        =   128; // 0x10000000


//--------------Arduino Stuff ----------------
struct ArduinoStates {
	//Test Variable to Ensure alignment of data
	UINT16 startVar;
	//UINT16 packetCount;

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

	//Circuit breakers
	UINT32 cbStates;

	//Switches
	UINT8 switchStates; // fuelState; strobeState; landingState; taxiState; positionState; avMasState; generatorState; batteryState;
	UINT8 trimSwitchPos; // 0 - Trim Button Not Pressed; 1 - Trim Up; 2 - Trim Down
	UINT8 flapSwitchPos; // 0 - No Flaps; 1 - T/0 Flaps; 2 - Full Flaps

	//Test Variable to Ensure alignment of data
	UINT8 endVar;
}; //struct ArduinoStates



class ArduinoCom {

public:
	ArduinoCom(char* commport, int baudrate); //Constructor opens communication, serial in this format
	~ArduinoCom();  //Destructor closes handle

	bool IsOpen() {return m_bCommOpened;} //Returns whether or not communication port is open
	bool Initiate();  //Sends command to Arduino to start sending packets
	void SendState(int updatingState, int value);  //For updating things such as Trim or Flaps display
	bool RecvCurrentState(struct ArduinoStates* currentState);  //Updates the structure with the latest data
	void ClearBuffer();  //Clears buffer
	//And so on as required...

private:
	//Serial Variables
	volatile bool	m_bStop;
	bool			m_bAsync;
	HANDLE			m_hArduino;
	char			m_sCommport[80];
	bool			m_bCommOpened;
	int				m_iBaudrate;

	//Serial Read Variables
	UINT8*	_buffer;
	//DWORD	_nTotalBytesRead;
	//int		_startRead;
	//DWORD	_nStateSize;


	//--------------Serial Stuff-----------------
	void commopen();
	DWORD commopen_internal();
	void commclose();
	DWORD commread( LPVOID pB, DWORD n );
	DWORD commwrite( const void* lpBuf, DWORD dwCount );
	void commflush();
	void commstats(COMSTAT& stat);	

	void clear_buffer();
	int find_start(UINT8 *inBuffer, DWORD maxPosition);
	bool send_current_cmd();
	bool send_update_cmd();
	bool cancel_update_cmd();


}; //ArduinoCom Class


#endif //_ARDUINO_COM_H_