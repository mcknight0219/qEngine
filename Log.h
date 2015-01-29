#ifndef _LOG_H
#define _LOG_H

#include <ostream>
#include <iostream>
// We don't use our File system because we want to
// take advantage of same interface of both file and
// console. 
#include <fstream>
#include "String.h"

enum LOG_LEVEL { L_NORMAL, L_WARNING, L_FATAL };
/*
================================================

Class definition for logging. User can choose to
output logging information to the screen or file

================================================
*/
class Log
{
public:
					Log();
					Log(const char * file);
					~Log();

public:
	void 			SetLevel(LOG_LEVEL lev) { level = lev; }
	LOG_LEVEL		GetLevel() const { return level; }

	void 			BeginLog(LOG_LEVEL l, const qStr msg);
	void 			LogNormal(const char * fmt, ...);
	void 			LogWarning(const char * fmt, ...);
	void 			LogFatal(const char * fmt, ...);

	// Only viable for file log
	// Reset the log content
	void			Clear();
	qStr			GetTimestamp() const;

private:
	bool 			logToFile;
	std::ostream * 	output;
	LOG_LEVEL		level;

private:
	// Disable copy and assign constructor
	Log& 			operator=(const Log&) { return *this; /* Silent compiler */}
	Log(const Log&) {}
};

#endif /* !_LOG_H */