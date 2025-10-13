#include "include/core_uac.h"
#include <tuple>
#include <windows.h>
#include <QRandomGenerator>
#include <ostream>
#include <iostream>
#include <fstream>
#include <istream>
#include <QString>
#define SW_HIDE SW_HIDE_QA
#define SW_NORMAL SW_NORMAL_QA
#define SW_SHOWMINIMIZED SW_SHOWMINIMIZED_QA
#include <3rdparty/WinCommander.hpp>
#undef SW_HIDE
#undef SW_NORMAL
#undef SW_SHOWMINIMIZED


QString getRandomString(int length)
{
    const QString charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";

    QString result;
    result.reserve(length);

    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(charset.length());
        result.append(charset.at(index));
    }

    return result;
}

QString getRandomPipeName()
{
    QString randomPart = getRandomString(12);
    return QStringLiteral(R"(\\.\pipe\QtPipe_%1)").arg(randomPart);
}

std::tuple<HANDLE, QString> createRandomNamedPipe(){
    QString pipeName = getRandomPipeName();

    HANDLE hPipe = CreateNamedPipe(
        (LPCWSTR)pipeName.utf16(), // Convert QString to LPCWSTR
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        4096, 4096,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        qCritical() << "Failed to create pipe";
        return std::make_tuple(INVALID_HANDLE_VALUE, pipeName);
    }

    BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!connected) {
        qCritical() << "Failed to connect pipe";
        CloseHandle(hPipe);
        return std::make_tuple(INVALID_HANDLE_VALUE, pipeName);
    }
    return std::make_tuple(hPipe, pipeName);
}

// Struct to pass parameters to the thread
struct StreamThreadParams {
    HANDLE handle;
    std::ostream* output;
};

// Thread function: read from handle, write to ostream
DWORD WINAPI streamThreadFunc(LPVOID lpParam) {
    StreamThreadParams* params = static_cast<StreamThreadParams*>(lpParam);
    HANDLE hRead = params->handle;
    std::ostream& out = *(params->output);

    const DWORD bufferSize = 1024;
    char buffer[bufferSize];
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(hRead, buffer, bufferSize, &bytesRead, NULL);
        if (!success || bytesRead == 0) {
            break;  // EOF or error
        }
        out.write(buffer, bytesRead);
        out.flush();  // Optional
    }

    delete params;  // Clean up allocated memory
    return 0;
}


//  Create a thread that reads from handle and writes to ostream
HANDLE startStreamThread(HANDLE readHandle, std::ostream& output) {
    auto* params = new StreamThreadParams{ readHandle, &output };

    HANDLE hThread = CreateThread(
        NULL,              // default security
        0,                 // default stack size
        streamThreadFunc,  // thread function
        params,            // argument
        0,                 // creation flags
        NULL               // thread ID
    );

    if (!hThread) {
        std::cerr << "Failed to create thread. Error: " << GetLastError() << "\n";
        delete params;
        return NULL;
    }

    return hThread; // You can WaitForSingleObject(hThread, ...) if needed
}

int runCoreUACMode(int argc, char *argv[]){
    if (argc < 3){
        return -1000;
    }
    QString program = argv[1];
    if (program != "sudo"){
        return -1000;
    }
    program = argv[2];
    QStringList arguments;
    for (int i = 2; i < argc; i ++){
        arguments << argv[i];
    }
    HANDLE perr, pout;
    QString perr_path, pout_path;
    std::tie(perr, perr_path) = createRandomNamedPipe();
    std::tie(pout, pout_path) = createRandomNamedPipe();
    arguments << "-redirect-error";
    arguments << perr_path;
    arguments << "-redirect-output";
    arguments << pout_path;
    int ret = (int) WinCommander::runProcessElevated(program, arguments, "", WinCommander::SW_HIDE_QA, true);
    auto th1 = startStreamThread(perr, std::cerr);
    auto th2 = startStreamThread(pout, std::cout);
    CloseHandle(perr);
    CloseHandle(pout);
    WaitForSingleObject(th1, INFINITE);
    WaitForSingleObject(th2, INFINITE);
    return ret;
}
