#pragma once
#include <time.h>
#include <iostream>

#define TRED FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE TNORMAL | FOREGROUND_INTENSITY
#define TBLUE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TGRAY FOREGROUND_BLUE | FOREGROUND_GREEN// | FOREGROUND_INTENSITY

#define USE_CONSOLE_EXTERN						//�Ƿ�ʹ�ÿ���̨��չ�� ʹ�õĻ����������ɫ����֧��LINUX (δʵ��)
#define CONSOLE_MESSAGE_LEVEL 5
//#define TRACE_THREADID_INFO

#pragma warning(disable: 4312)
#pragma warning(disable: 4996)
class CConsole
{
public:
	CConsole();
	~CConsole();

	//������Ϣ��ʾ����,0=ֻ��ʾ��ͨ��Ϣ,1=��ʾ������Ϣ����ͨ��Ϣ,
	//2=��ʾ����,�ɹ���Ϣ��01,3=��ʾ������Ϣ����������Ϣ
	void SetLevel(int nOutputLevel){m_nOutputLevel = nOutputLevel;}
	int  GetLevel(){return m_nOutputLevel;}
	void SetFileLevel(int nFileLevel, const TCHAR* lpszFilePath = NULL);
	int  GetFileLevel(){return m_nLogFileLevel;}


	inline void Notice(const TCHAR* source, const TCHAR * format, ...)
	{
		EnterCriticalSection(&m_lock);

		if(m_nOutputLevel >= 1)
		{
			va_list ap;
			va_start(ap, format);
			Color(TNORMAL);
			OutputTime();
			fputs("N ", stdout);
			OutputSource(source);
			Color(TNORMAL);
			_vtprintf_s(format, ap);
			putchar('\n');
			va_end(ap);
			Color(TNORMAL);
		}
		if(m_nLogFileLevel >= 2)
		{
			TCHAR buf[4096];

			SYSTEMTIME tm;
			GetLocalTime(&tm);
			_stprintf_s(buf, _countof(buf), _T("[%4d-%02d-%02d %02d:%02d]��NOTICE�� "), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute);
			_ftprintf_s(m_file, buf);

			va_list ap;
			va_start(ap, format);
			_vstprintf(buf, format, ap);
			va_end(ap);
			_ftprintf_s(m_file, source);
			putc(' ', m_file);
			_ftprintf_s(m_file, buf);
			putc('\n', m_file);
			fflush(m_file);
		}

		LeaveCriticalSection(&m_lock);
	}

	void Trace(const TCHAR * source, const TCHAR * format, ...)
	{
		if(m_nOutputLevel >= 5)
		{
			EnterCriticalSection(&m_lock);

			va_list ap;
			va_start(ap, format);
			Color(TGRAY);
			OutputTime();
			fputs("T ", stdout);
			OutputSource(source);
			Color(TGRAY);

			_vtprintf_s(format, ap);
			putchar('\n');
			va_end(ap);
			Color(TNORMAL);
			LeaveCriticalSection(&m_lock);
		}
	}
	void Debug(const TCHAR * source, const TCHAR * format, ...)
	{
		if(m_nOutputLevel >= 3)
		{
			EnterCriticalSection(&m_lock);

			va_list ap;
			va_start(ap, format);
			Color(TBLUE);
			OutputTime();
			fputs("D ", stdout);
			OutputSource(source);
			Color(TBLUE);

			_vtprintf_s(format, ap);
			putchar('\n');
			va_end(ap);
			Color(TNORMAL);
			LeaveCriticalSection(&m_lock);
		}
		if(m_nLogFileLevel >= 3)
		{
			TCHAR buf[4096];

			SYSTEMTIME tm;
			GetLocalTime(&tm);
			_stprintf_s(buf, _countof(buf), _T("[%4d-%02d-%02d %02d:%02d]��DEBUG�� "), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute);
			_ftprintf_s(m_file, buf);

			va_list ap;
			va_start(ap, format);
			_vstprintf(buf, format, ap);
			va_end(ap);
			_ftprintf_s(m_file, source);
			putc(' ', m_file);
			_ftprintf_s(m_file, buf);
			putc('\n', m_file);
			fflush(m_file);
		}
	}

	void Warning(const TCHAR * source, const TCHAR * format, ...)
	{
		if(m_nOutputLevel >= 2)
		{	
			EnterCriticalSection(&m_lock);
			va_list ap;
			va_start(ap, format);
			Color(TYELLOW);
			OutputTime();
			fputs("W ", stdout);
			OutputSource(source);
			Color(TYELLOW);
			_vtprintf_s(format, ap);
			putchar('\n');
			va_end(ap);
			Color(TNORMAL);
			LeaveCriticalSection(&m_lock);		
		}
		if(m_nLogFileLevel >= 2)
		{
			TCHAR buf[4096];
			SYSTEMTIME tm;
			GetLocalTime(&tm);
			_stprintf_s(buf, _countof(buf), _T("[%4d-%02d-%02d %02d:%02d]��WARNING�� "), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute);
			_ftprintf_s(m_file, buf);

			va_list ap;
			va_start(ap, format);
			_vstprintf(buf, format, ap);
			va_end(ap);
			_ftprintf_s(m_file, source);
			putc(' ', m_file);
			_ftprintf_s(m_file, buf);
			putc('\n', m_file);
			fflush(m_file);
		}
	}

	void Success(const TCHAR * source, const TCHAR * format, ...)
	{
		if(m_nOutputLevel >= 2)
		{
			EnterCriticalSection(&m_lock);
			va_list ap;
			va_start(ap, format);
			Color(TGREEN);
			OutputTime();
			fputs("S ", stdout);
			OutputSource(source);
			Color(TGREEN);
			_vtprintf_s(format, ap);
			putchar('\n');
			va_end(ap);
			Color(TNORMAL);
			LeaveCriticalSection(&m_lock);
		}
		if(m_nLogFileLevel >= 2)
		{
			TCHAR buf[4096];
			SYSTEMTIME tm;
			GetLocalTime(&tm);
			_stprintf_s(buf, _countof(buf), _T("[%4d-%02d-%02d %02d:%02d]��SUCCESS�� "), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute);
			_ftprintf_s(m_file, buf);

			va_list ap;
			va_start(ap, format);
			_vstprintf(buf,format, ap);
			va_end(ap);
			_ftprintf_s(m_file, source);
			putc(' ', m_file);
			_ftprintf_s(m_file, buf);
			putc('\n', m_file);
			fflush(m_file);
		}
	}

	void Error(const TCHAR * source, const TCHAR * format, ...)
	{
		if(m_nOutputLevel >= 1)
		{
			EnterCriticalSection(&m_lock);
			va_list ap;
			va_start(ap, format);
			Color(TRED);
			OutputTime();
			fputs("E ", stdout);
			OutputSource(source);
			Color(TRED);
			_vtprintf_s(format, ap);
			_puttchar('\n');
			va_end(ap);
			Color(TNORMAL);
			LeaveCriticalSection(&m_lock);
		}
		if(m_nLogFileLevel >= 1)
		{
			TCHAR buf[4096];
			SYSTEMTIME tm;
			GetLocalTime(&tm);
			_stprintf_s(buf, _countof(buf), _T("[%4d-%02d-%02d %02d:%02d]��ERROR�� "), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute);
			_ftprintf_s(m_file, buf);

			va_list ap;
			va_start(ap, format);
			_vstprintf(buf, /*4096,*/ format, ap);
			va_end(ap);
			_ftprintf_s(m_file, source);
			putc(' ', m_file);
			_ftprintf_s(m_file, buf);
			putc('\n', m_file);
			fflush(m_file);
		}

	}

	void Print(const TCHAR * format, ...)
	{
		if(m_nOutputLevel >= 4)
		{
			EnterCriticalSection(&m_lock);
			va_list ap;
			va_start(ap, format);
			Color(TNORMAL);
			_vtprintf_s(format, ap);
			//putchar('\n');
			va_end(ap);
			Color(TNORMAL);
			LeaveCriticalSection(&m_lock);
		}

		if(m_nLogFileLevel >= 4)
		{
			TCHAR buf[4096];
			SYSTEMTIME tm;
			GetLocalTime(&tm);
			_stprintf_s(buf, _countof(buf), _T("[%4d-%02d-%02d %02d:%02d]��PRINT�� "), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute);
			_ftprintf_s(m_file, buf);

			va_list ap;
			va_start(ap, format);
			_vstprintf(buf, format, ap);
			va_end(ap);
			_ftprintf_s(m_file, buf);
			putc('\n', m_file);
			fflush(m_file);
		}
	}

	void Line()
	{
		EnterCriticalSection(&m_lock);
		putchar('\n');
		LeaveCriticalSection(&m_lock);
	}

	long	m_nStdHandle;
	void Color(unsigned int color)
	{
		HANDLE h = (HANDLE)m_nStdHandle;
		SetConsoleTextAttribute(h, (short)color);
	}
private:
	void OutputSource(const TCHAR* source)
	{
#ifdef TRACE_THREADID_INFO
		TCHAR buf[16];
		sprintf_s(buf, _countof(buf), _T("[T:%04d] "), GetCurrentThreadId());
		fputs(buf, stdout);
#endif

		if(*source)
		{

			Color(TWHITE);
			_fputts(source, stdout);
			putchar(':');
			int n = (int)_tcslen(source);
			for(int i=n; i<4; i++)
				fputs(" ", stdout);		
			putchar(' ');
		}
	}

	void OutputTime();

	long	m_nOutputLevel;
	long	m_nLogFileLevel;

	FILE *m_file;
	CRITICAL_SECTION m_lock;
};

extern CConsole theConsole;