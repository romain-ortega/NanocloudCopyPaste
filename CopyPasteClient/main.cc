#include <iostream>
#include <windows.h> 
#include <conio.h>
#include <tchar.h>
#include <strsafe.h>
#include <atlstr.h>
#include "stdafx.h"
#include "CopyPaste.h"

#define BUFSIZE 2048
#define PIPE_NAME "\\\\.\\pipe\\photon_pipe"

int wmain(int argc, wchar_t *argv[])
{
	HANDLE hPipe;
	LPTSTR lpvMessage;
	TCHAR  chBuf[BUFSIZE];
	BOOL   fSuccess = FALSE;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename = TEXT(PIPE_NAME);

	if (argc == 1) {
		wchar_t *clipboard_content = GetClipboardText();

		if (wcslen(clipboard_content) == 0)
			return (1);

		//std::cout << clipboard_content;
		lpvMessage = clipboard_content;
	}
	else {
		auto success = SetClipboardText(argv[1]);
		lpvMessage = (success) ? (TEXT("true")) : (TEXT("false"));
	}

	// Try to open a named pipe; wait for it, if necessary. 
	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

							// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			std::cout << "Could not open pipe." << std::endl;
			return -1;
		}

		// All pipe instances are busy, so wait for 5 seconds. 

		if (!WaitNamedPipe(lpszPipename, 5000))
		{
			std::cout << "Could not open pipe: 5 second wait timed out." << std::endl;
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle
		&dwMode,  // new pipe mode
		NULL,     // don't set maximum bytes
		NULL);    // don't set maximum time
	if (!fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed."));
		return -1;
	}

	// Send a message to the pipe server. 

	cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
	//_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed.\n"));
		return -1;
	}

	do
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			BUFSIZE * sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		std::cout << "ReadFile from pipe failed." << std::endl;
		return -1;
	}
	CloseHandle(hPipe);

	return 0;
}
