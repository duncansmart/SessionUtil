// SessionUtil.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void printError(TCHAR *messageFormat)
{
    LPWSTR pErrorMsg;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, (LPWSTR)&pErrorMsg, 0, NULL);
    wprintf(messageFormat, pErrorMsg);
    LocalFree(pErrorMsg);
}

int findSession(TCHAR *userToFind)
{
    int sessionId = -1;

    WTS_SESSION_INFO *sessions; 
    DWORD count;
    if (!WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessions, &count))
    {
        printError(L"WTSEnumerateSessions failed: %s\n");
        return -1;
    }

    for (DWORD i = 0; i < count; i++)
    {
        WCHAR *sessionUser; 
        DWORD bytes;
        if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessions[i].SessionId, WTSUserName, &sessionUser, &bytes))
        {
            printError(L"WTSQuerySessionInformation error: %s\n");
            continue;
        }

#if DEBUG
        wprintf(L"  * Session %d: \n      user: %s\n      state: %d\n      name: %s\n", 
            sessions[i].SessionId, 
            sessionUser, 
            sessions[i].State, 
            sessions[i].pWinStationName);
#endif

        if (sessionUser != NULL && _wcsicmp(userToFind, sessionUser) == 0)
            sessionId = sessions[i].SessionId;

        WTSFreeMemory(sessionUser);

        if (sessionId != -1)
            break;
    }
    WTSFreeMemory(sessions);

    return sessionId;
}

int switchUser(TCHAR *username, TCHAR *password)
{
    int sessionId = findSession(username);

    if (password == NULL)
        password = L"";

    if (sessionId == -1) 
    {	
        wprintf(L"Session for %s not found\n", username);
        return 2;
    }

    if(!WTSConnectSession(sessionId, WTSGetActiveConsoleSessionId(), password, true))
    {
        wprintf(L"SessionId: %d\n", sessionId);
        printError(L"WTSConnectSession error: %s\n");
        return 2;
    }

    return 0;
}

int logoffUser(TCHAR *userToLogoff)
{
    wprintf(L"Logging off %s\n", userToLogoff);

    int sessionId = findSession(userToLogoff);

    if (sessionId != -1)
    {
        wprintf(L"  session %d\n", sessionId);
        if (!WTSLogoffSession(WTS_CURRENT_SERVER_HANDLE, sessionId, TRUE))
        {
            printError(L"WTSLogoffSession failed: %s");
            return 3;
        }
        else 
            wprintf(L"  ...done.\n");
    }
    else 
    {
        wprintf(L"  session not found.\n");
        return 2;
    }

    return 0;
}

void printUsage()
{

    wprintf(
    L"Usage:\n\
    sessionutil switch\n\
    sessionutil switch <username> <password>\n\
    \n\
    sessionutil logoff\n\
    sessionutil logoff <username> [<username2> ...]\n\
    \n\
    sessionutil sleep\n\
    "
    );
}

void showBalloonTip(TCHAR* msg)
{
	static bool nidset = false;
	static NOTIFYICONDATA nid = {};

	if (nidset == false)
	{
		nid.cbSize = sizeof(nid);
		nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);

		GUID tipGuid = {0x30375b04, 0x977b, 0x410f, { 0x9e, 0xb7, 0x46, 0xed, 0xb5, 0xf7, 0xf4, 0xe2 }};
		nid.guidItem = tipGuid;

		nid.uFlags = NIF_INFO | NIF_ICON | NIF_GUID;
		nidset = true;
	}

	wcsncpy_s(nid.szInfo, msg, ARRAYSIZE(nid.szInfo));

	Shell_NotifyIcon(nidset ? NIM_MODIFY : NIM_ADD, &nid);

	//Shell_NotifyIcon(NIM_DELETE, &nid) ? S_OK : E_FAIL;
}

int _tmain(int argc, TCHAR* argv[])
{
	showBalloonTip(L"Hello world!");
	Sleep(2000);
	showBalloonTip(L"Hello world!\nagain!");
	Sleep(1000);

	return 0;

    if (argc == 1)
    {
        printUsage();
        return 1;
    }

	if (_wcsicmp(argv[1], L"SWITCH") == 0)
    {
        if (argc > 2)
            return switchUser(argv[2], argv[3]);
        else // go to Switch User screen
            WTSDisconnectSession(WTS_CURRENT_SERVER_HANDLE,  WTS_CURRENT_SESSION, false);
    }
    else if (_wcsicmp(argv[1], L"LOGOFF") == 0)
    {
        if (argc > 2)
        {
            for (int i = 2; i < argc; i++)
                logoffUser(argv[i]);
        }
        else 
        {
            WTSLogoffSession(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, true);
        }
    }
    else if (_wcsicmp(argv[1], L"SLEEP") == 0)
    {
        SetSuspendState(/*bHibernate*/ FALSE, 
            /*bForce*/ FALSE, 
            /*bWakeupEventsDisabled*/ FALSE);
        // Go to Switch User screen
        WTSDisconnectSession(WTS_CURRENT_SERVER_HANDLE,  WTS_CURRENT_SESSION, false);
    }
    else 
    {
        printUsage();
    }
}

