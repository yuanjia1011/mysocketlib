#include "StdAfx.h"
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#pragma  warning(disable:4312)
#pragma  warning(disable:4311)
#include "Console.h"

CConsole theConsole;

CConsole::CConsole()
{
	m_nLogFileLevel = 0;
	m_nOutputLevel = CONSOLE_MESSAGE_LEVEL;

	TCHAR buf[512];         
	GetConsoleTitle(buf,100);   
	HWND hwnd = ::FindWindow(NULL, buf);         
	HMENU hmenu = ::GetSystemMenu(hwnd, FALSE);         
	//::RemoveMenu(hmenu,       SC_CLOSE,MF_BYCOMMAND); 
	//::ShowWindow(hwnd, SW_SHOWNORMAL);

	//BOOL   bOK   =   ::SetConsoleCtrlHandler((PHANDLER_ROUTINE)HandlerRoutine1,TRUE);         
	m_nStdHandle   =   (long)GetStdHandle(STD_ERROR_HANDLE);         
	//int   hConHandle   =   _open_osfhandle(m_nStdHandle,_O_TEXT);         
	//FILE*   fp   =   _fdopen(hConHandle,"w");         
	//*stdout   =   *fp;         
	//setvbuf(stdout,NULL,_IONBF,0);


	InitializeCriticalSection(&m_lock);
}

CConsole::~CConsole()
{
	m_nOutputLevel = 0;
	if(m_file!=NULL)fclose(m_file);

	FreeConsole();
	DeleteCriticalSection(&m_lock);
}
//
//void CConsole::OutputSource(const TCHAR* source)
//{
//#ifdef TRACE_THREADID_INFO
//	TCHAR buf[16];
//	sprintf_s(buf, _countof(buf), "[T:%04d] ", GetCurrentThreadId());
//	fputs(buf, stdout);
//#endif
//
//	if(*source)
//	{
//
//		Color(TWHITE);
//		fputs(source, stdout);
//		putchar(':');
//		int n = (int)strlen(source);
//		for(int i=n; i<4; i++)
//			fputs(" ", stdout);		
//		putchar(' ');
//	}
//}

void CConsole::OutputTime()
{
	//Color(TWHITE);
	//TCHAR buf[256];
	//time_t t = time(NULL);
	//struct tm *tm = localtime(&t);
	//if (tm)
	//{

	//	//strftime(buf, 256, "[%H:%M:%S] ", tm);
	//	strftime(buf, 256, "[%M:%S] ", tm);
	//	//SetConsoleTextAttribute(m_nStdHandle, 4);	//���ÿ���̨������ɫ
	//	printf(buf);
	//}
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	printf("[%02d:%02d:%02d.%03d] ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
}

void CConsole::SetFileLevel(int nFileLevel, const TCHAR* lpszFilePath /*= NULL*/)
{
	if(nFileLevel > 0)
	{
		if(lpszFilePath == NULL)
		{
			TCHAR buf[512];         
			GetModuleFileName(NULL, buf, 512);
#ifdef _UNICODE
			std::wstring strFilePath = buf;
#else
			std::string strFilePath = buf;
#endif
			int pos = (int)strFilePath.rfind('\\');
			strFilePath = strFilePath.substr(pos+1);
			strFilePath = strFilePath.substr(0, strFilePath.size()-4);

			SYSTEMTIME tm;
			GetLocalTime(&tm);
			_stprintf_s(buf, _countof(buf), _T("[%4d-%02d-%02d %02d.%02d] %s.log"), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, strFilePath.c_str());
			lpszFilePath = buf;
		}

		errno_t err = _tfopen_s(&m_file, lpszFilePath, _T("w+"));
		if (err != 0)
		{
			_ftprintf_s(stderr, _T("%s: Error opening '%s': %s\n"), __FUNCTION__, lpszFilePath, strerror(errno));
		}
	}

	m_nLogFileLevel = nFileLevel;
}