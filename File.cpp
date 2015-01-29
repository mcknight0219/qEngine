#include "File.h"

File::File(const qStr fileName, const qStr mode)
{
	Init(fileName, mode);
}

File::File(const qStr fileName)
{
	Init(fileName, "r");
}

// not very useful
File::File()
{
	Init("", "r");
}

File::~File()
{
	if( ptr)
		delete[] ptr;
	fclose(handler);
}

inline bool File::Exist()
{
	if( name.Empty() )
		return false;
#ifdef __linux__
	struct stat st;
	if( stat(name.Ptr(), &st) || !S_ISREG(st.st_mode) )
		return false;
	return true;
#endif
#ifdef _WIN32
	WIN32_FIND_DATA ffd;
	int len = MultiByteToWideChar(CP_ACP, 0, name.Ptr(), -1, NULL, 0);
	wchar_t *buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, name.Ptr(), -1, buf, len);
	std::wstring ws(buf);
	delete[] buf;
	HANDLE hFind = FindFirstFile(ws.c_str(), &ffd);
	return hFind != INVALID_HANDLE_VALUE;
#endif
}

void File::Init(const qStr fileName, const qStr mode)
{
	name = fileName;
	isGood = false;
	
	qStr ms = mode;
	ms.ToLower();
	writeable = false;
	if( ms.Contains("w") ) {
		writeable = true;
	}
	handler = fopen(name.Ptr(), mode.Ptr());
	if (!handler) {
		isGood = false;
		return;
	}
	// get file size
	size = GetSize();

	isGood = true;
	isLoaded = false;
	ptr = ptrCurrent = ptrBeg = ptrEnd = 0;

}

size_t File::GetSize()
{
	if (isGood)
		return size;

	fseek(handler, 0, SEEK_END);
	size_t sz = ftell(handler);
	fseek(handler, 0, SEEK_SET);

	return sz;
}

// Buffer the file content
void File::UploadToRAM()
{
	if (isLoaded || !isGood)
		return;
	ptr = new byte[size];
	if (!ptr) {
		printf("Oops. Cannot allocate memory");
		isLoaded = false;
		return;
	}
	size_t bytesRead = fread(ptr, sizeof(byte), size, handler);
	if (bytesRead != size) {
		printf("An error occurred during reading from file");
		if (ptr)
			delete[] ptr;
		isLoaded = false;
		return;
	}

	ptrBeg = ptrCurrent = ptr;
	ptrEnd = ptrBeg + size;
}

LexerFile::LexerFile(const qStr& path) : File(path)
{
	// we only work in buffered content
	if( Good() )
		UploadToRAM();
}

void LexerFile::SkipWS()
{
	while( ptrCurrent < ptrEnd && IsWhiteCharacter(*ptrCurrent) ) {
		ptrCurrent++;
	}
}

bool LexerFile::IsWhiteCharacter(const char c)
{
	return (c == '\n' || c == ' ' || c =='\t' || c == '\0' ||
		c == '\r' || c == '(' || c ==')' || c == ';' || c ==':' || c == ',');
}

void LexerFile::ReadToken()
{
	SkipWS();
	if( ptrCurrent >= ptrEnd ) {
		currentToken.Clear();
		return;
	}

	char token[MAX_TOKEN_LENGTH];
	int j = 0;
	while( ptrCurrent < ptrEnd && !IsWhiteCharacter(*ptrCurrent) ) {
		token[j++] = *ptrCurrent;
		ptrCurrent++;
	}
	token[j] = '\0';
	currentToken = qStr(token);
}

bool LexerFile::MoreToken()
{
	return ptrCurrent < ptrEnd;
}

#ifdef _WIN32
bool File::DirExist(const char * path)
{
	DWORD attr = GetFileAttributesA(path);
	if( attr == INVALID_FILE_ATTRIBUTES)
		return false;

	return attr & FILE_ATTRIBUTE_DIRECTORY;
}

#elif __linux__
bool File::DirExist(const char * path)
{
	struct stat st;
	stat(path, &st);

	return st.st_mode & S_IFDIR;
}
#endif

// Create any missing directory along the path
void File::CreateDir(const char * path)
{
	char *data = strdup(path), *cur;
	cur = data;

	while( *cur ) {
		if( *cur == '/' ) {
			*cur = '\0';
#ifndef _WIN32
			mkdir(data, S_IRUSR | S_IWUSR);
#else
			CreateDirectory(qStr(data).GetWideStr(), NULL);
#endif
			*cur = '/';
		}
		cur++;
	}
#ifndef _WIN32
			mkdir(data, S_IRUSR | S_IWUSR);
#else
	CreateDirectory(qStr(data).GetWideStr(), NULL);
#endif


}

void File::Write(const unsigned char * data, const int count, const int sz)
{
	if( !handler )
		return;

	fwrite(data, sz, count, handler);
}

void File::Close()
{
	if( handler )
		fclose(handler);

	ptr = ptrCurrent = ptrBeg = ptrEnd = 0;
	isGood = isLoaded = false;
}
