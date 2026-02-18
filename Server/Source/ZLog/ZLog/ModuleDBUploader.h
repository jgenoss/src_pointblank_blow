#ifndef __MODULE_DB_UPLOADER_H__
#define __MODULE_DB_UPLOADER_H__

#define MAX_FILE_BUFFER_SIZE	4096
#define MAX_FILE_LINE_SIZE		512
#define MAX_FILE_LIST_SIZE		10	// 10개 정도씩 끊어서 처리 합니다.
#define MAX_DB_QUERY_SIZE		1024

class CDBConfig;

enum DB_RECONNECT_REASON
{
	DB_RECONNECT_REASON_NONE,
	DB_RECONNECT_REASON_QUERY,
	DB_RECONNECT_REASON_BEGINTRANS,
	DB_RECONNECT_REASON_COMMITTRANS,
};

enum DB_UPDATER_STATE
{    
	DB_UPDATER_STATE_WAITING,
	DB_UPDATER_STATE_RUNNING_FILE_OPEN,
	DB_UPDATER_STATE_RUNNING_FILE_READ,
	DB_UPDATER_STATE_RUNNING_DB_TRANS,
	DB_UPDATER_STATE_RUNNING_DB_WRITE,
	DB_UPDATER_STATE_ENDING,
	DB_UPDATER_STATE_DB_ERROR,
};

enum TABLE_ID_NAME
{
	TABLE_ID_NAME_100,
	TABLE_ID_NAME_101,	
	TABLE_ID_NAME_1001,
	TABLE_ID_NAME_1002,
	TABLE_ID_NAME_1003,
	TABLE_ID_NAME_1004,
	TABLE_ID_NAME_1101,
	TABLE_ID_NAME_1102,
	TABLE_ID_NAME_1103,
	TABLE_ID_NAME_1104,
	TABLE_ID_NAME_1201,
	TABLE_ID_NAME_1202,
	TABLE_ID_NAME_1203,
	TABLE_ID_NAME_1204,
	TABLE_ID_NAME_1301,
	TABLE_ID_NAME_1302,
	TABLE_ID_NAME_1303,
	TABLE_ID_NAME_1304,
	TABLE_ID_NAME_1305,
	TABLE_ID_NAME_1401,
	TABLE_ID_NAME_1402,
	TABLE_ID_NAME_1501,
	TABLE_ID_NAME_1502,
	TABLE_ID_NAME_1503,
	TABLE_ID_NAME_1504,
	TABLE_ID_NAME_1505,
	TABLE_ID_NAME_1701,
	TABLE_ID_NAME_1702,
	TABLE_ID_NAME_1703,
	TABLE_ID_NAME_2001,
	TABLE_ID_NAME_2002,
	TABLE_ID_NAME_2003,
	TABLE_ID_NAME_3001,
	TABLE_ID_NAME_3002,
	TABLE_ID_NAME_4001,

	TABLE_ID_NAME_COUNT,
};

class CModuleDBUploader
{
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	bool					m_bIsRunning;
	INT32					m_i32ThreadIdx;

	CADODatabase*			m_pADO;
	CDBConfig*				m_pZLogDBConfig;

	DB_UPDATER_STATE		m_eDBUpdaterState;

	HANDLE					m_hFile;
	wchar_t					m_wstrFileName[ FILE_NAME_SIZE ];
	INT32					m_i32FileID;

	char					m_pReadBuffer[ MAX_FILE_BUFFER_SIZE ];
	char*					m_pLineStartPoint;
	char*					m_pReadPoint;
	INT32					m_i32ReadFileIdx;
	INT32					m_i32ReadStartIdx;
	INT32					m_i32ReadPoint;
	
	char					m_strLine[ MAX_FILE_LINE_SIZE ];

	char					m_strColumnName[ COLUMN_COUNT ][ 10 ];
	INT32					m_strColumnSize[ COLUMN_COUNT ];

	char					m_strQueryColumn[ MAX_DB_QUERY_SIZE ];
	char					m_strQueryContents[ MAX_DB_QUERY_SIZE ];
	char					m_strInsertQuery[ MAX_DB_QUERY_SIZE ];

	UINT32					m_ui32CheckTime;
	INT32					m_i32UploadCount;

	bool					m_bFinish;
	
	DB_RECONNECT_REASON		m_eReconnectReason;

	INT32					m_i32ReadIdx;
protected:

	void					_ReConnect();

	bool					_CommitFile( char* pPoint );

	bool					_OpenFile();
	bool					_ReadFile();
	bool					_WriteDB();

	bool					_GetNextLine();
	void					_MakeQuery();

public:
	CModuleDBUploader();
	~CModuleDBUploader();

	bool					Create( INT32 i32ThreadIdx );
	void					Destroy();

	void					Working();

	bool					IsWorkingFolder();
	bool					SetFile(wchar_t* pwstrFileName, INT32 i32FileID );

	wchar_t*				GetFileName()					{	return m_wstrFileName;		}
	INT32					GetFileID()							{	return m_i32FileID;			}	
	void					SetState( DB_UPDATER_STATE eState )	{	m_eDBUpdaterState = eState;	}
	DB_UPDATER_STATE		GetState()							{	return m_eDBUpdaterState;	}
	bool					GetFinish()							{	return m_bFinish;			}
};

#endif