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


//--------------Arduino Stuff ----------------
struct ArduinoStates {
	//Test Variable to Ensure alignment of data
	UINT16 startVar;
	UINT16 packetCount;
	unsigned long   elapsedTime;

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
}; //struct ArduinoStates



class ArduinoCom {

public:
	ArduinoCom(char* commport, int baudrate); //Constructor opens communication, serial in this format
	~ArduinoCom();  //Destructor closes handle

	bool IsOpen() {return m_bCommOpened;} //Returns whether or not communication port is open
	bool Initiate();  //Sends command to Arduino to start sending packets
	void SendState(int updatingState, int value);  //For updating things such as Trim or Flaps display
	bool RecvCurrentState(struct ArduinoStates* currentState);  //Updates the structure with the latest data
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
	ArduinoStates*	_buffer;
	DWORD			_bytesRead;
	int				_startRead;
	DWORD			_nStateSize;


	//--------------Serial Stuff-----------------
	void commopen();
	DWORD commopen_internal();
	void commclose();
	DWORD commread( LPVOID pB, DWORD n );
	DWORD commwrite( const void* lpBuf, DWORD dwCount );
	void commflush();
	void commstats(COMSTAT& stat);	

	void clear_buffer();
	int find_start(UINT8 *outBuffer, UINT8 *inBuffer, DWORD maxPosition);
	bool send_current_cmd();
	bool send_update_cmd();
	bool cancel_update_cmd();


}; //ArduinoCom Class


#endif //_ARDUINO_COM_H_