#ifndef _S2MO_DAT_FILE_H_
#define _S2MO_DAT_FILE_H_

#include "S2MOMD5.h"

#define	MAX_DAT_FILE_NAME		64

enum FILE_CONTROL
{
	FILE_CONTROL_READ = 0,
	FILE_CONTROL_WRITE
};

class S2MO_EXPORT_BASE S2MODatFile
{
private:
	char					m_strMD5Key[NET_MD5_KEY_SIZE+1];	
	char					m_strDatFileName[MAX_DAT_FILE_NAME];
	HANDLE					m_hFile;			

public:
	S2MODatFile();
	~S2MODatFile();

	BOOL					Create(char* strDatFileName);
	void					Destory();

	HANDLE					GetHandle(void)				{	return m_hFile;	  };
	char*					GetMD5Key(void);
	
	BOOL					IsSameMD5Key(char* strMD5Key)	
	{
		return ::strcmp( m_strMD5Key, strMD5Key ) == 0 ? TRUE : FALSE ; 
	}

	void					SetMD5Key(char* strMD5Key)
	{
		::strcpy_s( m_strMD5Key, NET_MD5_KEY_SIZE, strMD5Key ); 
		m_strMD5Key[NET_MD5_KEY_SIZE] = 0;
	}


	void					ControlDatFile(FILE_CONTROL eFileControl, void* pData, INT32 i32DataSize, INT32* i32Length );
	
	T_RESULT				SaveDatFile();			 
	T_RESULT				LoadDatFile();				 
	
	virtual void			ProcessDatFile(FILE_CONTROL eFileControl) = 0;

	void					MakeMD5Key(void);

private:
	T_RESULT				_MakeDatFile(void);
	void					_DeleteDatFile(void);
	void					_MakeMD5Key(void);
	BOOL					_FindDatFile(void);

};

#endif

