#include"FormatLastError.h"


LPSTR FormatLastError(DWORD dwMessageID)
{

	LPSTR szBuffer = NULL;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwMessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_US),
		(LPSTR)&szBuffer,
		0,
		NULL

	);
	//cout << szBuffer << endl;
	//LocalFree(szBuffer);
	return szBuffer;
}
VOID PrintLastError(DWORD dwMessageID)
{
	//DWORD dwMessageID = WSAGetLastError();
	LPSTR szMessage = FormatLastError(dwMessageID);
	//cout << "Error" << dwMessageID << ": " << szMessage << endl;
	printf("Error %i:%s", dwMessageID, szMessage);
	LocalFree(szMessage);
}

// Обработчик закрытия консоли, чтобы порты  самостоятельно не открывались
BOOL WINAPI ConsoleClosed(DWORD signal)
{
	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
	{
		g_running = false;
		return TRUE;
		//Sleep(1000); // чтобы успеть отправить 'q'  после закрытия клиента
	}
	return FALSE;
}