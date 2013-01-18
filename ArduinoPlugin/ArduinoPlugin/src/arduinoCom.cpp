#include "../inc/arduinoCom.h"

/*Public Functions*/

//Constructor opens communication, serial in this format
ArduinoCom::ArduinoCom(char* commport, int baudrate)
{
	commclose();

	strncpy_s(m_sCommport,80,commport,80);
	m_iBaudrate = baudrate;

	m_bCommOpened = FALSE;
	m_bAsync = FALSE;
	m_hArduino = INVALID_HANDLE_VALUE;
	m_bStop = TRUE;

	commopen();

	_buffer = (UINT8*)malloc(2*sizeof(struct ArduinoStates));
} //Constructor

//Destructor closes handle
ArduinoCom::~ArduinoCom()
{
	cancel_update_cmd();
	commclose();
	free(_buffer);

}  //Destructor

//Sends command to Arduino to start sending packets
bool ArduinoCom::Initiate()
{
	//Clear any remnants from last run
	cancel_update_cmd();
	_bytesRead = 0;
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
bool ArduinoCom::LatestStates(ArduinoStates* currentState)
{
	DWORD nbChar = sizeof(ArduinoStates);
	DWORD nbRead = 0;
	UINT8* tempPtr = _buffer + _bytesRead;

	// Read up to 2 times intended struct size, ensuring that struct
	// should fit in the buffer
	nbRead = commread(tempPtr,nbChar - _startRead); 

	//Add # bytes read to bytes already in buffer
	_bytesRead += nbRead;  

	//check if buffer is full
	if (_bytesRead >= nbChar) 
	{
		//Find location of 0xAAA in buffer
		UINT8* stPtr = NULL;

		//Find location of StartVariable
		int startPosition = find_start(stPtr,_buffer,nbChar);
		if (startPosition == -1)
		{
			//Reset buffer size
			_bytesRead = 0;
			_startRead = 0;
			return FALSE;
		}

		//Ensure sufficient bytes have been read
		if (_bytesRead - startPosition < nbChar)
		{
			_startRead = startPosition;
			return FALSE;
		}

		//Reset buffer size
		_bytesRead = 0;
		_startRead = 0;

		//Check that test variables matches
		if ((currentState->startVar != 0xAAA) || (currentState->endVar != 0xFFF))
		{
			return FALSE;
		}

		//Copy buffer values to global structure
		memcpy(currentState,_buffer + startPosition,nbChar);

		//Buffer is full and data is properly aligned
		return TRUE;
	} 

	//Buffer is not full yet
	return FALSE;


} //LatestStates






/*Private Functions*/



//-----------------Arduino Stuff --------------------------------------------------------

#pragma region ArduinoSend
//Sends command to Arduino asking it to send updates.
bool ArduinoCom::send_current_cmd()
{
	char sBuf[5] = {"L;"};
	//DWORD dwCommEvent;
	DWORD dwReadSize = sizeof(struct ArduinoStates);

	if(commwrite(sBuf, static_cast<DWORD>(strlen(sBuf))) == 0)
		return FALSE;

	// Event has occurred.
	return TRUE;
}

//Sends command to Arduino asking it to send updates.
bool ArduinoCom::send_update_cmd()
{
	char sBuf[32]; 
	DWORD dwReadSize = sizeof(struct ArduinoStates);

	//sprintf(sBuf,"U:%03d;",configFile->Delay);
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
	//DWORD dwCommEvent;
	DWORD dwReadSize = sizeof(struct ArduinoStates);

	if(commwrite(sBuf, static_cast<DWORD>(strlen(sBuf))) == 0)
		return FALSE;

	Sleep(10);
	commflush();

	// Event has occurred.
	return TRUE;
}
#pragma endregion


//Finds 0xAAA in buffer, which marks start of the structure
int ArduinoCom::find_start(UINT8 *outBuffer, UINT8 *inBuffer, DWORD maxPosition)
{
	for (int i=_startRead;i<(int)maxPosition;i++)
	{
		UINT16 *tmpPtr = (UINT16*) (inBuffer+i);

		if (tmpPtr[0] = 0xAAA)
		{
			outBuffer = inBuffer+i;			
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
					printf("\rError %d: %s\n", retval, lpMsgBuf);
			}

			LocalFree( lpMsgBuf );		

			Sleep(ARDUINO_WAIT_TIME);  //Arduino has boot up time

			return;

		}
	}
}

DWORD ArduinoCom::commopen_internal() {
	DCB dcb;
	COMMTIMEOUTS ct;

	if ( m_hArduino != INVALID_HANDLE_VALUE ) {
		return -1;
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