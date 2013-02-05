#include "../inc/arduinoCom.h"

/*Public Functions*/

//Constructor opens communication, serial in this format
ArduinoCom::ArduinoCom(char* commport, int baudrate)
{
	//commclose();

	strncpy_s(m_sCommport,80,commport,80);
	m_iBaudrate = baudrate;

	m_bCommOpened = FALSE;
	m_bAsync = FALSE;
	m_hArduino = INVALID_HANDLE_VALUE;
	m_bStop = TRUE;

	_nStateSize = sizeof(ArduinoStates);

	commopen();

	_buffer = (UINT8*)malloc(2*sizeof(struct ArduinoStates));
	//_buffer = new ArduinoStates();

} //Constructor

//Destructor closes handle
ArduinoCom::~ArduinoCom()
{
	cancel_update_cmd();
	commclose();
	free(_buffer);
	//delete _buffer;

}  //Destructor

//Sends command to Arduino to start sending packets
bool ArduinoCom::Initiate()
{
	//Clear any remnants from last run
	cancel_update_cmd();
	_nTotalBytesRead = 0;
	_startRead = 0;

	return send_update_cmd();

} //Initiate

//For updating things such as Trim or Flaps display
void ArduinoCom::SendState(int updatingState, int value)
{
	char sBuf[25];

	switch ( updatingState )
	{
	case TRIM_DISPLAY:
		sprintf_s(sBuf,25,"T:%d;",value);
		break;
	case FLAPS_DISPLAY: //Currently not configured on Arduino
		sprintf_s(sBuf,25,"F:%d;",value);
		break;
	case FAN_SPEED:
		sprintf_s(sBuf,25,"F:%d;",value);
		break;
	default: //Not a command Arduino recognises
		return;
	} //Switch

	commwrite(sBuf, static_cast<DWORD>(strlen(sBuf)));

} //SendState

//Updates the structure with the latest data, if buffer is complete
bool ArduinoCom::RecvCurrentState(ArduinoStates* currentState)
{


	// read some data (already have _nTotalBytesRead - _startRead of buffer)
	_nTotalBytesRead += commread(_buffer + _nTotalBytesRead, _nStateSize - _nTotalBytesRead + _startRead);

	//	search for the start of the frame in the buffer
	_startRead = find_start(_buffer, _nStateSize);

	//	if the start of the frame was not found
	if (_startRead == -1)
	{
		// toss the data that was just read and reset the buffer
		_nTotalBytesRead = 0;
		_startRead = 0;

		return FALSE;

	}  //if

	//if are buffer isn't big enough to fit our frame
	if (_nTotalBytesRead - _startRead < _nStateSize)
	{
		// need to return again to get full frame
		return FALSE;

	}  //if

	//if we find the end of the buffer
	if (((ArduinoStates *)(_buffer + _startRead))->endVar != 0xFFF)
	{
		// didn't find the end of the buffer so we can't use the data
		// toss the data that was just read and reset the buffer
		_nTotalBytesRead = 0;
		_startRead = 0;

		return FALSE;

	}  // if

	//	copy the buffer to the frame
	memcpy(currentState, _buffer + _startRead, _nStateSize);
	commflush();

	// reset the buffer
	_startRead = 0;
	_nTotalBytesRead = 0;

	return true;


} //RecvCurrentState






/*Private Functions*/



//-----------------Arduino Stuff --------------------------------------------------------

#pragma region ArduinoSend
//Sends command to Arduino asking it to send updates.
bool ArduinoCom::send_current_cmd()
{
	char sBuf[5] = {"L;"};

	if(commwrite(sBuf, static_cast<DWORD>(strlen(sBuf))) == 0)
		return FALSE;

	// Event has occurred.
	return TRUE;
}

//Sends command to Arduino asking it to send updates.
bool ArduinoCom::send_update_cmd()
{
	char sBuf[32]; 
	sprintf_s(sBuf,25,"U;");

	if(commwrite(sBuf, static_cast<DWORD>(strlen(sBuf))) == 0)
		return FALSE;

	// Event has occurred.
	return TRUE;
}

//Sends command to Arduino asking it to cancel updates.
bool ArduinoCom::cancel_update_cmd()
{
	char sBuf[5] = {"X;"};

	if(commwrite(sBuf, static_cast<DWORD>(strlen(sBuf))) == 0)
		return FALSE;

	Sleep(20);
	commflush();

	// Event has occurred.
	return TRUE;
}
#pragma endregion


//Finds 0xAAA in buffer, which marks start of the structure
int ArduinoCom::find_start(UINT8 *inBuffer, DWORD maxPosition)
{
	for (int i=_startRead;i<(int)maxPosition;i++)
	{
		UINT16 *tmpPtr = (UINT16*) (inBuffer+i);

		if (0xAAA == tmpPtr[0])
		{
			return i;
		}
	}

	return -1;
}

void ArduinoCom::clear_buffer()
{
	commflush();
}

DWORD ArduinoCom::commread(void* lpBuf, DWORD dwCount)
{
	//Validate our parameters
	if (!m_bCommOpened)
	{
		return 0;
	}

	DWORD dwBytesRead = 0;
	if (!ReadFile(m_hArduino, lpBuf, dwCount, &dwBytesRead, NULL))
	{
		return 0;
	}

	return dwBytesRead;
}


/*--------------------------SERIAL STUFF------------------------------------------------
Note: Using WinBase.h (Windows API) for serial communications
*/
void ArduinoCom::commclose() {
	if ( m_hArduino ) {
		if ( CloseHandle(m_hArduino) ) {
			m_hArduino = INVALID_HANDLE_VALUE;
			m_bCommOpened = FALSE;
		}
	}
}

void ArduinoCom::commopen() {
	while(m_bCommOpened == FALSE)
	{
		DWORD retval;

		printf("Opening Com Port... ");

		retval = commopen_internal();
		if (retval)
		{
			int n;
			LPSTR lpMsgBuf;
			char lpErrorBuf[1024];

			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0,
				NULL);

			for (n=0;lpMsgBuf[n]!=0;n++)
			{
				if (lpMsgBuf[n]=='\n' || lpMsgBuf[n]=='\r' )
				{
					sprintf_s(lpErrorBuf,1024,"\rError %d: %s\n", retval, lpMsgBuf);
					XPLMDebugString(lpErrorBuf);
				}
			}

			LocalFree( lpMsgBuf );		

			return;

		}
	}
}

DWORD ArduinoCom::commopen_internal() {
	DCB dcb;
	COMMTIMEOUTS ct;

	if ( m_hArduino != INVALID_HANDLE_VALUE ) {
		return (DWORD) -1;
	}
	m_bCommOpened = FALSE;
	m_hArduino = CreateFile( m_sCommport,
		GENERIC_READ|GENERIC_WRITE,
		0, /*not shared*/
		0, /*no security descriptor*/
		OPEN_EXISTING,
		m_bAsync?FILE_FLAG_OVERLAPPED:0,
		NULL );
	if ( INVALID_HANDLE_VALUE == m_hArduino ) {
		return GetLastError();
	}
	if ( !GetCommState(m_hArduino, &dcb) ) {
		DWORD dwE = GetLastError();
		commclose();
		return dwE;
	}

	dcb.StopBits = 0;
	dcb.Parity = 0;
	dcb.BaudRate = m_iBaudrate;
	dcb.ByteSize = 8;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutxCtsFlow = FALSE;	// no flow control
	dcb.fOutxDsrFlow = FALSE;	// no flow control
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;

	if ( !SetCommState(m_hArduino, &dcb) ) {
		DWORD dwE = GetLastError();
		commclose();
		return dwE;
	}

	if ( !GetCommTimeouts(m_hArduino, &ct) ) {
		DWORD dwE = GetLastError();
		commclose();
		return dwE;
	}
	ct.ReadIntervalTimeout = MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = MAXDWORD;
	ct.ReadTotalTimeoutConstant = 100; // ms
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;
	if ( !SetCommTimeouts(m_hArduino, &ct) ) {
		DWORD dwE = GetLastError();
		commclose();
		return dwE;
	}

	m_bCommOpened = TRUE;
	return 0;
}



DWORD ArduinoCom::commwrite(const void* lpBuf, DWORD dwCount)
{
	//Validate our parameters
	if (!m_bCommOpened)
	{
		return 0;
	}

	DWORD dwBytesWritten = 0;
	if (!WriteFile(m_hArduino, lpBuf, dwCount, &dwBytesWritten, NULL))
	{
		return 0;
	}

	return dwBytesWritten;
}





void ArduinoCom::commflush()
{
	if (!m_bCommOpened)
	{
		return;
	}


	if (!FlushFileBuffers(m_hArduino))
	{
		return;
	}

	if (!PurgeComm(m_hArduino,PURGE_TXCLEAR))
	{
		return;
	}

	if (!PurgeComm(m_hArduino,PURGE_RXCLEAR))
	{
		return;
	}
}

void ArduinoCom::commstats(COMSTAT& stat)
{

	if (!m_bCommOpened)
	{
		return;
	}

	DWORD dwErrors;
	if (!ClearCommError(m_hArduino, &dwErrors, &stat))
	{
		return;
	}

}