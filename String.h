#ifndef _MY_STRING_H
#define _MY_STRING_H

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <string.h>	// for memcpy
#endif

#include <stdlib.h>
#include <stdio.h>


#ifdef _WIN32
	#define PATH_SEPRATOR	'\\'
#elif __linux__
	#define PATH_SEPRATOR	'/'
#endif

class qStr
{
public:
			qStr();
			qStr(const char *s);
			qStr(const qStr& other);
			qStr(const char *s, int l);
			//qStr(const std::string& other);
			~qStr();

	qStr&			operator=(const qStr& other);
	const size_t	Length() const { return len; }
	const char*		Ptr() const { return data; }
	void			Clear();
	bool			Empty() { return !len;  };
	void			ToLower();
	void			ToUpper();
	void			ConcatSelf(const qStr& other);
	void			ConcatSelf(const int num);
	void			ConcatSelf(const char *str);
	qStr			Concat(const char *str);
	bool			Contains(const char *s);
	bool			EndsWith(const char *s);
	void			Insert(const char *prefix);
	void			RemoveQuotes();
	// Use them carefully !!!
	int				ToInteger() { return atoi(data); }
	float			ToFloat() { return static_cast<float>(atof(data)); }

	// Compare with raw string
	bool			operator==(const char *other);
	bool			operator!=(const char *other);
#ifdef _WIN32
	LPWSTR			GetWideStr();
	static LPSTR	GetCStr(LPWSTR wStr);
#endif
	// Bad, bad, bad. But I don't really want another Path class
	qStr			GetFileName() const;
	qStr			GetFileExtension() const;
	void			AppendExtension(const char *);
	bool			IsAbsolutePath() const;
    static bool     IsOnlyFileName(const char *name);
	void			AdjustSep();

private:
	char	*data;
	size_t	len;
};


#endif /* !_STRING_H */
