// LogOffUser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//using namespace std;

void printError(TCHAR *messageFormat)
{
	LPWSTR pErrorMsg;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, (LPWSTR)&pErrorMsg, 0, NULL);
	wprintf(messageFormat, pErrorMsg);
	LocalFree(pErrorMsg);
}

void logoffUser(TCHAR *userToLogoff)
{
	WTS_SESSION_INFO *sessions;
	DWORD count;
	if (!WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessions, &count))
	{
		printError(L"WTSEnumerateSessions failed: %s\n");
		return;
	}
	wprintf(L"Logging off %s\n", userToLogoff);
	bool loggedOff = false;
	for (DWORD i = 0; i < count; i++)
	{
		WCHAR *sessionUser; 
		DWORD bytes;
		if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessions[i].SessionId, WTS_INFO_CLASS::WTSUserName, &sessionUser, &bytes))
		{
			printError(L"WTSQuerySessionInformation error: %s\n");
			continue;
		}

		//wprintf(L"* Session %d: \n    user: %s\n    state: %d\n    name: %s\n", sessions[i].SessionId, sessionUser, sessions[i].State, sessions[i].pWinStationName);

		if (sessionUser != NULL && _wcsicmp(userToLogoff, sessionUser) == 0)
		{
			wprintf(L"  Session %d: %s\n", sessions[i].SessionId, sessionUser);
			if (!WTSLogoffSession(WTS_CURRENT_SERVER_HANDLE, sessions[i].SessionId, TRUE))
				printError(L"WTSLogoffSession failed: %s");
			else 
			{
				wprintf(L"  ...done.");
				loggedOff = true;
			}
		}

		WTSFreeMemory(sessionUser);
	}
	if (!loggedOff)
		wprintf(L"  session not found.\n");

	WTSFreeMemory(sessions);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1)
	{
		wprintf(L"Usage: LogOffUser <username> [<username2>...]\n");
		return 1;
	}

	for (int i = 1; i < argc; i++)
	{
		logoffUser(argv[i]);
	}

}

