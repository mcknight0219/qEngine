
#include <stdarg.h>
#include <ios>
#include <istream>

#include "Log.h"

#ifdef _WIN32
	#include <windows.h>
#endif

#ifdef __linux__
	#include <sys/time.h>
#endif

Log::Log()
{
	logToFile = false;
	output = &std::cout;
	SetLevel(L_NORMAL);
	BeginLog(L_NORMAL, "Logging initialized: Output to screen");
}

Log::Log(const char *file)
{
	if( !file || file[0] == '\0' ) {
		goto FALLBACK;
	}
	output = new std::ofstream(file, std::ofstream::out | std::ofstream::app);
	SetLevel(L_NORMAL);

	if( !output->good() ) 
		goto FALLBACK;
	logToFile = true;
	BeginLog(L_NORMAL, "Logging initialized: Output to file");

FALLBACK:
	// cannot open file, fallback to logging to screen
	logToFile = false;
	output = &std::cout;
	BeginLog(L_NORMAL, "Logging initialized: Output to screen");
}

Log::~Log()
{
	// close the file stream if possible
	if( logToFile ) {
		dynamic_cast<std::ofstream*>(output)->close();
	}
}

qStr Log::GetTimestamp() const
{
	char stamp[256];
	const char * fmt = "[%d-%d-%d %d:%d:%d] ";
#ifdef _WIN32
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(stamp, fmt, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
#else
	time_t raw;
	time(&raw);
	struct tm * local = localtime( &raw );
	
	sprintf(stamp, fmt, local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
#endif

	return qStr(stamp);
}

void Log::BeginLog(LOG_LEVEL l, const qStr msg)
{
	qStr ts = GetTimestamp();
	if( l >= level ) {
		*output << ts.Ptr() << msg.Ptr() << std::endl;
	}
}

void Log::LogNormal(const char * fmt, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	int b = vsprintf(buf, fmt, ap);
	va_end(ap);

	// overflow? truncate
	if( b == 1024 ) {
		buf[1024 - 1] = 0;
	}
	BeginLog(L_NORMAL, qStr(buf)); 
}

void Log::LogWarning(const char * fmt, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	int b = vsprintf(buf, fmt, ap);
	va_end(ap);

	// overflow? truncate
	if( b == 1024 ) {
		buf[1024 - 1] = 0;
	}
	BeginLog(L_WARNING, qStr(buf));
}

void Log::LogFatal(const char * fmt, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	int b = vsprintf(buf, fmt, ap);
	va_end(ap);

	// overflow? truncate
	if( b == 1024 ) {
		buf[1024 - 1] = 0;
	}
	BeginLog(L_FATAL, qStr(buf));
}

void Log::Clear()
{
	if( logToFile ) {
		output->seekp(std::ios::beg);
		BeginLog(level, "Logging reset for file stream");
	}
}


