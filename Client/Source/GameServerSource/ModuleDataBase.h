#ifndef __MODULE_DATABASE_
#define __MODULE_DATABASE_

typedef struct _DataQuery
{
	char	_pQuery[SQL_STRING_COUNT];
}DB_QUERY;

class CModuleDataBase : public i3Thread
{
	I3_CLASS_DEFINE( CModuleDataBase );
private:		
	HANDLE				m_hKillEvent;
	BOOL				m_bIsRunning; 
protected:
	INT32			m_iThreadCount; 

	DB_QUERY	*	m_pMainQuery;
	INT32			m_iMainReadIdx;
	INT32			m_iMainWriteIdx;

	//Nc Module not support 

	DB_QUERY	*	m_pModuleZPQuery;
	INT32			m_iModuleZPReadIdx;
	INT32			m_iModuleZPWriteIdx;

	DB_QUERY	**	m_ppUserQuery;
	INT32		*	m_pUserReadIdx;
	INT32		*	m_pUserWriteIdx;

	//thai Module Not support 

public: 
	CModuleDataBase(void); 
	virtual ~CModuleDataBase(void); 

	BOOL OnCreate_M(INT32 iThreadCount);
	void OnDestroy_M(void);

	virtual UINT32 OnRunning(void * pUserData); 

	void WriteUpdateQuery_MAU(INT32 iWorkThread, char * pQuery); //////
};

extern CModuleDataBase * g_pModuleDataBase; 

#endif
