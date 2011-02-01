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

int logoffUser(TCHAR *userToLogoff)
{
	WTS_SESSION_INFO_1 *pSessionInfo;
	DWORD level = 1, count;
	if (!WTSEnumerateSessionsEx(WTS_CURRENT_SERVER_HANDLE, &level, 0, &pSessionInfo, &count))
	{
		printError(L"WTSEnumerateSessionsEx failed: %s\n");
		return 2;
	}

	int result = 0;

	for (DWORD i = 0; i < count; i++)
	{
		//wprintf(L"* session %d %s\n", pSessionInfo[i].SessionId, pSessionInfo[i].pUserName);
		if (pSessionInfo[i].pUserName != NULL)
		{
			if (_wcsicmp(userToLogoff, pSessionInfo[i].pUserName) == 0)
			{
				wprintf(L"Logging off session %d %s\\%s\n", pSessionInfo[i].SessionId, pSessionInfo[i].pDomainName, pSessionInfo[i].pUserName);
				if (!WTSLogoffSession(WTS_CURRENT_SERVER_HANDLE, pSessionInfo[i].SessionId, TRUE))
				{
					printError(L"WTSLogoffSession failed: %s");
					result = 3;
				}
				else
				{
					wprintf(L"  ...done.");
				}
			}
		}
	}

	WTSFreeMemory(pSessionInfo);

	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1)
	{
		wprintf(L"Usage: LogOffUser <username>\n");
		return 1;
	}

	TCHAR* userToLogoff = argv[1];

	return logoffUser(userToLogoff);
}

