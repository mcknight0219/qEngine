#include "String.h"

qStr::qStr() : data(NULL), len(0)
{
}

qStr::qStr(const char *s)
{
	size_t l = strlen(s);
	data = (char*)malloc(l+1);
	memcpy(data, s, l + 1); // Copy the '\0' too
	len = l;
}

qStr::qStr(const char *s, int l)
{
	int nLen = strlen(s);
	if( l <= 0 || l > nLen )
		l = nLen;

	data = (char*)malloc(l + 1);
	memcpy(data, s, l);
	data[l] = '\0';
	len = l;
}

qStr::qStr(const qStr& other)
{
	data = (char*)malloc(other.len + 1);
	memcpy(data, other.Ptr(), other.len + 1);
	len = other.len;
}

qStr::~qStr()
{
	if (data) {
		free(data);
	}
}

void qStr::Clear()
{
	data[0] = '\0';
	len = 0;
}

#ifdef _WIN32
LPWSTR qStr::GetWideStr()
{
	LPWSTR wBuf = new WCHAR[256];
	MultiByteToWideChar(CP_ACP, 0, data, -1, wBuf, 256);
	
	return wBuf;
}
#endif

void qStr::ToLower()
{
	char *offset = data;
	if( len ) {
		while( *offset ) {
			if( *offset >= 'A' && *offset <= 'Z' )
				*offset = *offset + ('a' - 'A');
			offset++;
		}
	}
}

void qStr::ToUpper()
{
	char *offset = data;
	if( len ) {
		while( *offset ) {
			if( *offset >= 'a' && *offset <= 'z' )
				*offset = *offset + ('A' - 'a');
			offset++;
		}
	}
}

void qStr::ConcatSelf(const char *s)
{
	int newLen = len + strlen(s);
	if( data ) {
		data = static_cast<char*>(realloc(data, newLen+1));
	} else {
		data = static_cast<char*>(malloc(newLen+1));
	}

	memcpy(data +len, s, strlen(s));
	data[newLen] = '\0';
	len = newLen;
}

void qStr::ConcatSelf(const qStr& other)
{
	ConcatSelf(other.Ptr());
}

void qStr::ConcatSelf(const int n)
{
	char sNum[256];
	sprintf(sNum, "%d", n);

	Concat(sNum);
}

qStr qStr::Concat(const char *s)
{
	qStr sNew(data);
	sNew.ConcatSelf(s);
	return sNew;
}

bool qStr::Contains(const char *s)
{
	if( strlen(s) > len )
		return false;

	for( size_t i = 0; i < len-strlen(s); ++i ) {
		if( !memcmp(s, data+i, strlen(s)))
			return true;
	}
	return false;
}

bool qStr::EndsWith(const char *s)
{
	int diff = len - strlen(s);
	if (diff < 0) {
		return false;
	}

	char *orig = data + diff;
	char *match = const_cast<char*>(s);
	while (*match != '\0' && *match++ == *orig++);

	return *match == '\0';
}

qStr& qStr::operator=(const qStr& other)
{
	if( this == &other )
		return *this;
	if (data)
		free(data);
	len = other.len;
	data = (char*)malloc(len + 1);
	memcpy(data, other.data, len + 1); /* Copy '\0' as well */
	return *this;
}

bool qStr::operator==(const char *other)
{
	if( !other)
		return false;

	if( len != strlen(other))
		return false;

	return !strcmp(other, data);
}

bool qStr::operator!=(const char *other)
{
	return !operator==(other);
}

void qStr::RemoveQuotes()
{
	char *start = data;
	char *end = data + len - 1;
	while( *start == '\'' || *start == '"' )
		start++;
	while( *end == '\'' || *end == '"')
		end--;

	int nlen = end - start + 1;
	memmove(data, start, nlen);
	data[nlen] = '\0';
	len = nlen;
}

// Make sure it's a path before you call this!!!
qStr qStr::GetFileName() const
{
	char *sName, *sExt;
	char *end = data + len;
	while( end >= data && *end != '/' && *end != '\\') {
		end--;
	}

	if( end < data ) {
		sName = data;
	} else {
		sName = end + 1;
	}
	// Next remove extension
	sExt = strstr(sName, ".");
	return qStr(sName, sExt ? sExt - sName : 0);
}

bool qStr::IsAbsolutePath() const 
{
	if( len <= 1 ) {
		return false;
	}
#ifdef _WIN32
	if( data[0] == 'C' || data[0] == 'D' || data[0] == 'E' && data[1] == ':')
		return true;
#elif __linux__
	if( data[0] == '/' )
		return true;
#endif
	return false;
}

qStr qStr::GetFileExtension() const
{
	if( len <= 1) {
		return "";
	}

	char * end = data + len;
	while( end >= data && *end != '.' ) {
		end--;
	}

	if( end < data ) {
		return "";
	}
	return qStr(end + 1);
}

void qStr::AppendExtension(const char * ext)
{
    if( !ext || ext[0] == '\0' )
        return;

    if( ext[0] != '.' ) {
        this->ConcatSelf(".");
    }
    this->ConcatSelf(ext);
}


#ifdef _WIN32
// Caller must release the memory
LPSTR qStr::GetCStr(LPWSTR wStr)
{
	if (wStr == NULL)
		return NULL;

	char buff[256];
	wcstombs(buff, wStr, 256);
	return _strdup(buff);
}
#endif

// Insert prefix at the front
void qStr::Insert(const char *prefix)
{
	if( !prefix || !strlen(prefix) ) {
		return;
	}
	// Probably very bad efficiency
	qStr sPrefix = prefix;
	sPrefix.ConcatSelf(*this);
	//fprintf(stderr, "sPrefix: %s\n", sPrefix.Ptr());
	*this = sPrefix;
}

// Change path separator in the string
// to accormodate system difference
void qStr::AdjustSep()
{

}

bool qStr::IsOnlyFileName(const char * name)
{
    char * p = const_cast<char *>(name);
    while( *p != 0 ) {
        if( *p == '/' || *p == '\\' )
            return false;
    }
    // don't care if there is extension
    return true;
}
