#include <tchar.h>
#include <strsafe.h>
#include "httplib.h"

#define SVCNAME TEXT("NvAPIIlluminationController")

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR*);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR*);
VOID SvcReportEvent(LPTSTR);

HMODULE hDll;
BOOL isHDllLoaded = false;

void handle_request(const httplib::Request& req, httplib::Response& res);

typedef void (*GetPhysicalGPUsPtr)();
typedef int (*GetIlluminationTypePtr)(int gpuIndex, int zoneIndex);
typedef int (*GetIlluminationControlModePtr)(int gpuIndex, int zoneIndex);
typedef BOOL(*SetGPUIlluminationRGBPtr)(int gpuIndex, int zoneIndex, int r, int g, int b, int brightness);
typedef BOOL(*SetGPUIlluminationColorFixedPtr)(int gpuIndex, int zoneIndex, int brightness);
typedef BOOL(*SetGPUIlluminationRGBWPtr)(int gpuIndex, int zoneIndex, int r, int g, int b, int w, int brightness);
typedef BOOL(*SetGPUIlluminationSingleColorPtr)(int gpuIndex, int zoneIndex, int brightness);

//
// Purpose: 
//   Entry point for the process
//
// Parameters:
//   None
// 
// Return value:
//   None, defaults to 0 (zero)
//
int __cdecl _tmain(int argc, TCHAR* argv[])
{
    // If command-line parameter is "install", install the service. 
    // Otherwise, the service is probably being started by the SCM.

    if (lstrcmpi(argv[1], TEXT("install")) == 0)
    {
        SvcInstall();
        return 0;
    }

    // TO_DO: Add any additional services for the process to this table.
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { (LPWSTR)SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
        { NULL, NULL }
    };

    // This call returns when the service has stopped. 
    // The process should simply terminate when the call returns.

    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
        SvcReportEvent((LPWSTR)TEXT("StartServiceCtrlDispatcher"));
    }
}

//
// Purpose: 
//   Installs a service in the SCM database
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szUnquotedPath[MAX_PATH];

    if (!GetModuleFileName(NULL, szUnquotedPath, MAX_PATH))
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    // In case the path contains a space, it must be quoted so that
    // it is correctly interpreted. For example,
    // "d:\my share\myservice.exe" should be specified as
    // ""d:\my share\myservice.exe"".
    TCHAR szPath[] = TEXT("\"C:\\NvAPIIlluminationController\\NVAPIIlluminationControlService.exe\"");

    // Get a handle to the SCM database. 

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Create the service

    schService = CreateService(
        schSCManager,              // SCM database 
        SVCNAME,                   // name of service 
        SVCNAME,                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 

    if (schService == NULL)
    {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }
    else printf("Service installed successfully\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

//
// Purpose: 
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None.
//
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Register the handler function for the service

    gSvcStatusHandle = RegisterServiceCtrlHandler(
        SVCNAME,
        SvcCtrlHandler);

    if (!gSvcStatusHandle)
    {
        SvcReportEvent((LPWSTR)TEXT("RegisterServiceCtrlHandler"));
        return;
    }

    // These SERVICE_STATUS members remain as set here

    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    // Report initial status to the SCM

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // Perform service-specific initialization and work.

    SvcInit(dwArgc, lpszArgv);
}

//
// Purpose: 
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None
//
VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // TO_DO: Declare and set any required variables.
    //   Be sure to periodically call ReportSvcStatus() with 
    //   SERVICE_START_PENDING. If initialization fails, call
    //   ReportSvcStatus with SERVICE_STOPPED.

    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.

    ghSvcStopEvent = CreateEvent(
        NULL,    // default security attributes
        TRUE,    // manual reset event
        FALSE,   // not signaled
        NULL);   // no name

    if (ghSvcStopEvent == NULL)
    {
        ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    // Report running status when initialization is complete.

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // ----------

    hDll = LoadLibrary(L"C:\\NvAPIIlluminationController\\NVAPIIlluminationControlLibrary.dll");
    isHDllLoaded = (hDll != NULL);
    if (hDll == NULL) {
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    GetPhysicalGPUsPtr GetPhysicalGPUs = reinterpret_cast<GetPhysicalGPUsPtr>(GetProcAddress(hDll, "GetPhysicalGPUs"));
    GetIlluminationTypePtr GetIlluminationType = reinterpret_cast<GetIlluminationTypePtr>(GetProcAddress(hDll, "GetIlluminationType"));
    GetIlluminationControlModePtr GetIlluminationControlMode = reinterpret_cast<GetIlluminationControlModePtr>(GetProcAddress(hDll, "GetIlluminationControlMode"));
    SetGPUIlluminationRGBPtr SetGPUIlluminationRGB = reinterpret_cast<SetGPUIlluminationRGBPtr>(GetProcAddress(hDll, "SetGPUIlluminationRGB"));
    SetGPUIlluminationColorFixedPtr SetGPUIlluminationColorFixed = reinterpret_cast<SetGPUIlluminationColorFixedPtr>(GetProcAddress(hDll, "SetGPUIlluminationColorFixed"));
    SetGPUIlluminationRGBWPtr SetGPUIlluminationRGBW = reinterpret_cast<SetGPUIlluminationRGBWPtr>(GetProcAddress(hDll, "SetGPUIlluminationRGBW"));
    SetGPUIlluminationSingleColorPtr SetGPUIlluminationSingleColor = reinterpret_cast<SetGPUIlluminationSingleColorPtr>(GetProcAddress(hDll, "SetGPUIlluminationSingleColor"));

    httplib::Server svr;

    // Gets service update/information
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        handle_request(req, res);
    });

    // Capture hDll and isHDllLoaded in the lambda capture list
    svr.Get("/getGpus", [&](const httplib::Request& req, httplib::Response& res) {
        GetPhysicalGPUs();
        handle_request(req, res);
    });

    // Capture hDll and isHDllLoaded in the lambda capture list
    svr.Get("/red", [&](const httplib::Request& req, httplib::Response& res) {
        SetGPUIlluminationRGBW(0, 0, 255, 0, 0, 0, 255);
        handle_request(req, res);
    });

    // Capture hDll and isHDllLoaded in the lambda capture list
    svr.Get("/green", [&](const httplib::Request& req, httplib::Response& res) {
        SetGPUIlluminationRGBW(0, 0, 0, 255, 0, 0, 255);
        handle_request(req, res);
    });

    // Capture hDll and isHDllLoaded in the lambda capture list
    svr.Get("/blue", [&](const httplib::Request& req, httplib::Response& res) {
        SetGPUIlluminationRGBW(0, 0, 0, 0, 255, 0, 255);
        handle_request(req, res);
    });

    std::cout << "Server listening on port 29454" << std::endl;
    svr.listen("localhost", 29454);

    // ----------

    while (1)
    {
        // Check whether to stop the service.
        BOOL libraryIsFree = FreeLibrary(hDll);
        if (libraryIsFree == 0) {
            printf("Library could not be freed. Oh no.");
            return;
        }

        WaitForSingleObject(ghSvcStopEvent, INFINITE);

        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }
}

//
// Purpose: 
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//     in milliseconds
// 
// Return value:
//   None
//
VOID ReportSvcStatus(DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED))
        gSvcStatus.dwCheckPoint = 0;
    else gSvcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    // Handle the requested control code. 

    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

        // Signal the service to stop.

        SetEvent(ghSvcStopEvent);
        ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

        return;

    case SERVICE_CONTROL_INTERROGATE:
        break;

    default:
        break;
    }

}

//
// Purpose: 
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
// 
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction)
{
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR Buffer[80];

    hEventSource = RegisterEventSource(NULL, SVCNAME);

    if (NULL != hEventSource)
    {
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

        lpszStrings[0] = SVCNAME;
        lpszStrings[1] = Buffer;

        ReportEvent(hEventSource,        // event log handle
            EVENTLOG_ERROR_TYPE, // event type
            0,                   // event category
            EVENTLOG_ERROR_TYPE,           // event identifier
            NULL,                // no security identifier
            2,                   // size of lpszStrings array
            0,                   // no binary data
            lpszStrings,         // array of strings
            NULL);               // no binary data

        DeregisterEventSource(hEventSource);
    }
}

void handle_request(const httplib::Request& req, httplib::Response& res) {
    std::string responseString = "hDll loaded is ";
    responseString += (isHDllLoaded ? "true" : ("false"));

    res.set_content(responseString, "text/plain");
}