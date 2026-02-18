#ifndef __MODULE_FILE_MANAGER_H__
#define __MODULE_FILE_MANAGER_H__

class CDBConfig;
class CModuleDBUploader;

#define MAX_WAIT_SIZE 32

enum FILE_STATE
{
	FILELIST_STATE_EMPTY,
	FILELIST_STATE_WAIT,
	FILELIST_STATE_UPLOAD,
	
};

struct FILE_LIST
{
	wchar_t		m_wstrFileName[ MAX_PATH ];
	FILE_STATE	m_eFileState;
	INT32		m_i32FileID;
};

class CModuleFileManager
{
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	bool					m_bIsRunning;

	// For File Create
	char					m_strLogPath[ MAX_PATH ];
	UINT32					m_ui32ServerCount;
	
	// For File Write
	CADODatabase*			m_pADO;
	CDBConfig*				m_pZlogDBConfig;

	INT32					m_i32FileListEmptyCount;	//리스트상에서 전송이 종료된 파일 갯수
	INT32					m_i32FileListWaitCount;		//전송대기상태인 파일 갯수
	FILE_LIST				m_aFileList[ MAX_WAIT_SIZE ];

	INT32					m_i32UploaderCount;
	CModuleDBUploader*		m_pUploader;
	
	INT32					m_i32IDListIdx;

protected:

	void					_ReConnect();

	INT32					_GetIDListIdx();

	BOOL					_SetNextUploadFile( INT32 i32Idx );

	INT32					_CreateFileList();
	INT32					_UpdateFileList();	
	INT32					_CheckUploader();

	void					_StartFileUpload( INT32 i32Idx );
	void					_DeleteFile( INT32 i32Idx );
	
	void					_MakeDBTable( INT32 i32Idx );
	

public:
	CModuleFileManager();
	~CModuleFileManager();

	bool					Create( UINT32 ui32ServerCount, INT32 i32UploaderCount, char* pstrLogPath );
	void					Destroy();
	void					Working();

};

#endif