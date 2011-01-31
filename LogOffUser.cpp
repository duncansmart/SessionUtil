// LogOffUser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1)
	{
		wprintf(L"Usage: LogOffUser <username>\n");
		return 1;
	}

	TCHAR* userToLogoff = argv[1];

	PWTS_SESSION_INFO_1 pSessionInfo = 0;
	DWORD level = 1;
	DWORD count;
	if (!WTSEnumerateSessionsEx(WTS_CURRENT_SERVER_HANDLE, &level, 0, &pSessionInfo, &count))
	{
		LPWSTR pErrorMsg;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, (LPWSTR)&pErrorMsg, 0, NULL);
		wprintf(L"WTSEnumerateSessionsEx failed %d: %s\n", GetLastError(), pErrorMsg);
		LocalFree(pErrorMsg);
		return 2;
	}

	int retval = 0;

	for (DWORD i = 0; i < count; i++)
	{
		if (pSessionInfo[i].pUserName != NULL)
		{
			if (_wcsicmp(userToLogoff, pSessionInfo[i].pUserName) == 0)
			{
				wprintf(L"Logging off session %d %s\\%s\n", pSessionInfo[i].SessionId, pSessionInfo[i].pDomainName, pSessionInfo[i].pUserName);
				if (!WTSLogoffSession(WTS_CURRENT_SERVER_HANDLE, pSessionInfo[i].SessionId, TRUE))
				{
					LPWSTR pErrorMsg;
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, (LPWSTR)&pErrorMsg, 0, NULL);
					wprintf(L"Error %d: %s\n", GetLastError(), pErrorMsg);
					LocalFree(pErrorMsg);
					retval = 3;
				} 
				else 
				{
					wprintf(L"  ...done.");
				}
			}
		}
	}
	WTSFreeMemory(pSessionInfo);
	
	return retval;
}

