#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <vector>
#include <string.h>
#ifdef _WIN32
	#include <Windows.h>
#endif

#ifdef __linux__
	#include <dirent.h>
	#include <sys/types.h>
    #include <time.h>
#endif
#include "String.h"

/*
=================================================

Common functionalities grouped together

===================================================
*/

class Common
{
public:
	enum { CM_WARNING = 1, CM_ERROR, CM_FATAL };

	void 				FatalError(qStr msg);
	void 				Error(qStr msg);
	void 				Warning(qStr msg);
	std::vector<qStr>	ListFiles(const char *szPath);
	qStr				GetTime();

};

#endif /* !_COMMON_H */
