#ifndef __FILE_H__
#define __FILE_H__

class EXPORT_BASE NSM_cFile
{
	HANDLE					m_hFile;

public:
	NSM_cFile();
	virtual ~NSM_cFile();

	BOOL					CreateFile( const char* strFileName );
	BOOL					Read( char* pBuffer, UINT32 ui32Size );
	void					Close();

	DWORD					GetFileSize();
};

#endif