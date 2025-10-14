#include <windows.h>
#include <iostream>
#include <ostream>

struct ThreadStruct {
	const char * pipeName;
	std::ostream * stream;
};

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    auto lpstruct = (ThreadStruct*)lpParam;
	auto pipeName = lpstruct->pipeName;
	auto cout = (lpstruct->stream);
	
    HANDLE hPipe = CreateNamedPipe(
        pipeName,                        // Pipe name
        PIPE_ACCESS_INBOUND,           	 // Read access
        PIPE_TYPE_BYTE | PIPE_WAIT,      // Byte-type pipe and blocking mode
        1,                               // Max. instances
        1024,                            // Output buffer size
        1024,                            // Input buffer size
        0,                               // Client time-out
        NULL);                           // Default security attributes

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error creating named pipe: " << GetLastError() << std::endl;
        return 1;
    }

   // std::cout << "Named pipe created. Waiting for a client to connect..." << std::endl;

    // Wait for a client to connect
    if (ConnectNamedPipe(hPipe, NULL) == FALSE) {
        std::cerr << "Error connecting to named pipe: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }
   // std::cout << "Client connected." << std::endl;

    // Buffer to store incoming data
    char buffer[128];
    DWORD bytesRead;

    // Read from the pipe
    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        do {
			buffer[bytesRead] = '\0'; // Null-terminate the string
			*cout << buffer ;
		} while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL));
    } else {
        std::cerr << "Error reading from named pipe: " << GetLastError() << std::endl;
    }

    // Close the pipe
    CloseHandle(hPipe);
    return 0;
}

int main() {
	ThreadStruct * str = new ThreadStruct();
	str->pipeName = R"(\\.\pipe\MyNamedPipe1)";
	str->stream = &std::cout;
	
	
	ThreadStruct * str2 = new ThreadStruct();
	str2->pipeName = R"(\\.\pipe\MyNamedPipe2)";
	str2->stream = &std::cerr;
	
	
	HANDLE hThread1 = CreateThread(
        NULL,               // Default security attributes
        0,                  // Default stack size
        ThreadFunction,     // Thread function
        (void*)str,               // Parameter to thread function
        0,                  // Default creation flags
        NULL);              // Receive thread identifier
		
	
		
	HANDLE hThread2 = CreateThread(
        NULL,               // Default security attributes
        0,                  // Default stack size
        ThreadFunction,     // Thread function
        (void*)str2,               // Parameter to thread function
        0,                  // Default creation flags
        NULL);              // Receive thread identifier

    if (hThread1 == NULL || hThread2 == NULL) {
        std::cerr << "Error creating thread: " << GetLastError() << std::endl;
        return 1;
    }
	
	HANDLE threads[2];
	threads[0] = hThread1;
	threads[1] = hThread2;
	
	WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    // Close the thread handle
    CloseHandle(hThread1);
    CloseHandle(hThread2);
    return 0;
}