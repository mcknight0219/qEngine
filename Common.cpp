#include "Common.h"

void Common::Warning(qStr msg)
{
#ifdef _WIN32
	LPWSTR text = msg.GetWideStr();
	MessageBox(NULL, text, L"QEngine: Warning", MB_ICONWARNING | MB_OK);
#endif
#ifdef __linux__
	fprintf(stdout, msg.Ptr());
#endif
}

void Common::Error(qStr msg)
{
#ifdef _WIN32
	LPWSTR text = msg.GetWideStr();
	MessageBox(NULL, text, L"QEngine: Error", MB_ICONERROR | MB_OK);
#endif
#ifdef __linux__
	fprintf(stderr, msg.Ptr());
#endif
	// We don't terminate program, let caller decide what to do
}

// it's something serious!
void Common::FatalError(qStr msg)
{
#ifdef _WIN32
	LPWSTR text = msg.GetWideStr();
	MessageBox(NULL, text, L"QEngine: Fatal", MB_ICONSTOP | MB_OK);

	PostQuitMessage(CM_FATAL);
#endif
#ifdef __linux__
	fprintf(stderr, msg.Ptr());
	abort(); // nothing matters anymore
#endif
}

std::vector<qStr> Common::ListFiles(const char *szPath)
{
	std::vector<qStr> vFiles;
#ifdef _WIN32
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	qStr path(szPath);
	path.ConcatSelf("\\*");
	LPWSTR wPath = path.GetWideStr();
	hFind = FindFirstFile(wPath, &ffd);
	if( hFind == INVALID_HANDLE_VALUE ) {
		return vFiles;
	} 

	qStr dirPath = szPath;
	dirPath.ConcatSelf("\\");
	do {
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			continue;
		}
		char *fname = qStr::GetCStr(ffd.cFileName);
		qStr sName = fname;
		sName.Insert(dirPath.Ptr());
		vFiles.push_back(sName);

	} while( FindNextFile(hFind, &ffd) != 0 );

	FindClose(hFind);
#else
	DIR *dp = opendir(szPath);
	if( !dp ) {
		return vFiles;
	}
	struct dirent *ep;
	while( ep = readdir(dp) ) {
		if( !strcmp(ep->d_name, ".") || !strcmp(ep->d_name, "..") ) {
			continue;
		}
		qStr sName = szPath;
		sName.ConcatSelf("/");
		sName.ConcatSelf(ep->d_name);
		vFiles.push_back(sName);
	}

	closedir(dp);
#endif
	return vFiles;
}

// Log class has the same functionality
qStr Common::GetTime()
{
	char stamp[256];
	const char * fmt = "%d_%d_%d_%d_%d_%d";
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