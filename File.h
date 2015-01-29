#ifndef _FILE_H
#define _FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>	// for std::wstring

#ifdef _WIN32
#include <Windows.h>
#endif

#include "String.h"
#include "Common.h"

#define MAX_TOKEN_LENGTH (256)

/*
==================================================

File handling class. Tend to overcome the gap between
Linux interface and Windows interface

==================================================
*/

typedef unsigned char byte;
class File
{
public:
			File();
			File(const qStr fileName);
			File(const qStr fileName, qStr mode);
			virtual ~File();

	bool	Open(const qStr fileName, const qStr mode);
	void	Write(const unsigned char *, int, int);
	void	Close();
	void	UploadToRAM();
	bool	Exist();
	size_t	GetSize();
	qStr	GetName() { return name; }
    void    Rewind();
    bool	Good() { return isGood; }
    bool 	Loaded() { return isLoaded; }
    
    static void CreateDir(const char *);
    static bool	DirExist(const char *);

private:
	void	Init(const qStr fileName, const qStr mode);
	// If has access to write
	bool	HasPermission();

protected:
	// Internal file position
	byte		*ptrCurrent;
	byte		*ptrBeg;
	byte		*ptrEnd;

private:
	FILE		*handler;
	qStr		name;
	size_t		size;
	byte		*ptr;
	bool		writeable;
	bool		isGood;
	bool		isLoaded;
};


/*
========================================================

Simple lexer that is capable of reading a token at a time.
User should know the structure of the file to be parsed.

==========================================================
*/
class LexerFile : public File 
{
public:
            LexerFile(const qStr& path);
            ~LexerFile() {}
    bool    MoreToken();
    void    ReadToken();
    qStr	TokenValue() { return currentToken; }
private:
    LexerFile() {}

    void SkipWS();
	bool IsWhiteCharacter(const char c);

private:
    qStr			currentToken;	
};

#endif /* !_FILE_H */
