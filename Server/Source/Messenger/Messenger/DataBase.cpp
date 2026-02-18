#include "pch.h"
#include "DataBase.h"
#include "NoteBox.h"
#include "ConnectedUser.h"

CDataBase * g_pDataBase = NULL;

I3_CLASS_INSTANCE( CDataBase, i3ElementBase);

//Thread Proc
static void _DefThreadProc( INT32 * pi32TheadIdx )
{
	g_pDataBase->OnWorking( *pi32TheadIdx );
}

CDataBase::CDataBase(void)
{
	// DB per Thread
	m_ppADOdb_MESS			= NULL;
	m_ppADOdb_USER			= NULL;

	// Thread variable
	m_pHThread				= NULL;
	m_pi32ThreadIdx			= NULL;

	// RingBuffer
	m_ppI_FriendList		= NULL;
	m_ppO_FriendList		= NULL;
	m_ppI_FriendControl		= NULL;
	m_ppO_FriendControl		= NULL;


#if defined( USE_BLOCK_LIST ) 
	m_ppI_BlockList			= NULL;
	m_ppO_BlockList			= NULL;
	m_ppI_BlockControl		= NULL;
	m_ppO_BlockControl		= NULL;
#endif

	m_ppI_NoteList			= NULL;
	m_ppO_NoteList			= NULL;

	m_ppI_NoteCommand		= NULL;
	m_ppO_NoteCommand		= NULL;

#ifdef USE_CHANGE_NICK	
	m_ppI_ChangeNick		= NULL;
	m_ppO_ChangeNick		= NULL;
#endif
#ifdef USE_CHANGE_RANK	
	m_ppI_ChangeRank		= NULL;
	m_ppO_ChangeRank		= NULL;
#endif
	m_ppI_ClanGroupNote		= NULL;
	m_ppO_ClanGroupNote		= NULL;

#ifdef USE_MESS_FIND
	m_ppI_UserInfo = NULL;
	m_ppO_UserInfo = NULL;
#endif

	m_i32OutFriendListThreadIdx		= 0;
	m_i32OutFriendControlThreadIdx	= 0;
	
#if defined( USE_BLOCK_LIST )
	m_i32OutBlockListThreadIdx		= 0;
	m_i32OutBlockControlThreadIdx	= 0;
#endif

	m_i32PopIdxNoteList				= 0;
	m_i32PopIdxNoteCommand			= 0;

#ifdef USE_CHANGE_NICK	
	m_i32OutChangeNickThreadIdx		= 0;
#endif
#ifdef USE_CHANGE_RANK	
	m_i32OutChangeRankThreadIdx		= 0;
#endif
	m_i32PopIdxClanGroupNoteCommand	= 0;

#ifdef USE_MESS_FIND
	m_iPopIdxUserInfo				= 0;
#endif

	for( INT32 i = 0; i < DB_THREAD_COUNT; ++i )
	{
		m_apMessDBConfig[i] = NULL;
		m_apUserDBConfig[i] = NULL;
	}
}

CDataBase::~CDataBase(void)
{
	OnDestroy();
}

BOOL CDataBase::OnCreate(void)
{
	m_i32OutFriendListThreadIdx		= 0;
	m_i32OutFriendControlThreadIdx	= 0;
#if defined( USE_BLOCK_LIST )
	m_i32OutBlockListThreadIdx		= 0;
	m_i32OutBlockControlThreadIdx	= 0;
#endif
	m_i32PopIdxNoteList				= 0;
	m_i32PopIdxNoteCommand			= 0;

#ifdef USE_MESS_FIND
	m_iPopIdxUserInfo				= 0;
#endif

	//==================================
	// Create RingBuffer
	//==================================

	//Friend
	m_ppI_FriendList	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 
	m_ppO_FriendList	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 
	m_ppI_FriendControl	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 
	m_ppO_FriendControl	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 

	//Block
#if defined( USE_BLOCK_LIST )
	m_ppI_BlockList		= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 
	m_ppO_BlockList		= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 
	m_ppI_BlockControl	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 
	m_ppO_BlockControl	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 
#endif


	//Note
	m_ppI_NoteList		= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT); 
	m_ppO_NoteList		= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
	m_ppI_NoteCommand	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
	m_ppO_NoteCommand	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);

#ifdef USE_CHANGE_NICK	
	// Change Nick
	m_ppI_ChangeNick	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
	m_ppO_ChangeNick	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
#endif
#ifdef USE_CHANGE_RANK	
	// Change Rank	
	m_ppI_ChangeRank	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
	m_ppO_ChangeRank	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
#endif
	// Clan Group Note
	m_ppI_ClanGroupNote	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
	m_ppO_ClanGroupNote	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);

#ifdef USE_MESS_FIND
	// User Search
	m_ppI_UserInfo	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
	m_ppO_UserInfo	= (Local_CRingBuffer **)i3MemAlloc( sizeof(Local_CRingBuffer *) * DB_THREAD_COUNT);
#endif

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		//GetFriendList
		m_ppI_FriendList[i] = new Local_CRingBuffer;
		if( !m_ppI_FriendList[i]->OnCreate( sizeof(IN_FRIEND_LIST), DB_BUFFER_FRIEND_LIST_COUNT ) )	return FALSE; 

		m_ppO_FriendList[i] = new Local_CRingBuffer;
		if( !m_ppO_FriendList[i]->OnCreate( sizeof(OUT_FRIEND_LIST), DB_BUFFER_FRIEND_LIST_COUNT ) ) return FALSE; 

		//AuthFriend
		m_ppI_FriendControl[i] = new Local_CRingBuffer;
		if( !m_ppI_FriendControl[i]->OnCreate( sizeof(IN_FRIEND_CONTROL), DB_BUFFER_COUNT ) )		return FALSE; 
		m_ppO_FriendControl[i] = new Local_CRingBuffer;
		if( !m_ppO_FriendControl[i]->OnCreate( sizeof(OUT_FRIEND_CONTROL), DB_BUFFER_COUNT ) )		return FALSE; 


		// Block
#if defined( USE_BLOCK_LIST )
		m_ppI_BlockList[i]		= new Local_CRingBuffer;
		m_ppO_BlockList[i]		= new Local_CRingBuffer;
		m_ppI_BlockControl[i]	= new Local_CRingBuffer;
		m_ppO_BlockControl[i]	= new Local_CRingBuffer;
		if( !m_ppI_BlockList[i]->OnCreate( sizeof(IN_BLOCK_LIST), DB_BUFFER_BLOCK_LIST_COUNT ) )	return FALSE; 
		if( !m_ppO_BlockList[i]->OnCreate( sizeof(OUT_BLOCK_LIST), DB_BUFFER_BLOCK_LIST_COUNT ) )	return FALSE; 
		if( !m_ppI_BlockControl[i]->OnCreate( sizeof(IN_BLOCK_CONTROL), DB_BUFFER_COUNT ) )			return FALSE; 
		if( !m_ppO_BlockControl[i]->OnCreate( sizeof(OUT_BLOCK_CONTROL), DB_BUFFER_COUNT ) )		return FALSE; 
#endif
			
		//Note
		m_ppI_NoteList[i]	= new Local_CRingBuffer;
		m_ppO_NoteList[i]	= new Local_CRingBuffer;
		m_ppI_NoteCommand[i]= new Local_CRingBuffer;
		m_ppO_NoteCommand[i]= new Local_CRingBuffer;

		if( !m_ppI_NoteList[i]->OnCreate( sizeof(IN_NOTE_LIST),			DB_BUFFER_COUNT ) )			return FALSE;
		if( !m_ppO_NoteList[i]->OnCreate( sizeof(OUT_NOTE_LIST),		DB_BUFFER_COUNT ) )			return FALSE;
		if( !m_ppI_NoteCommand[i]->OnCreate( sizeof(IN_NOTE_COMMAND),	DB_BUFFER_COUNT ) )			return FALSE;
		if( !m_ppO_NoteCommand[i]->OnCreate( sizeof(OUT_NOTE_COMMAND),	DB_BUFFER_COUNT ) )			return FALSE;


#ifdef USE_CHANGE_NICK
		// Change Nick
		m_ppI_ChangeNick[i]	= new Local_CRingBuffer;
		m_ppO_ChangeNick[i]	= new Local_CRingBuffer;
#endif
#ifdef USE_CHANGE_RANK
		// Change Rank
		m_ppI_ChangeRank[i]	= new Local_CRingBuffer;
		m_ppO_ChangeRank[i]	= new Local_CRingBuffer;
#endif
		// Clan Group Note
		m_ppI_ClanGroupNote[i]	= new Local_CRingBuffer;
		m_ppO_ClanGroupNote[i]	= new Local_CRingBuffer;

#ifdef USE_CHANGE_NICK
		// Change Nick
		if( !m_ppI_ChangeNick[i]->OnCreate( sizeof(IN_CHANGE_NICK),			DB_BUFFER_COUNT ) )		return FALSE;
		if( !m_ppO_ChangeNick[i]->OnCreate( sizeof(OUT_CHANGE_NICK),		DB_BUFFER_COUNT ) )		return FALSE;
#endif
#ifdef USE_CHANGE_RANK
		// Change Rank
		if( !m_ppI_ChangeRank[i]->OnCreate( sizeof(IN_CHANGE_RANK),			DB_BUFFER_COUNT ) )		return FALSE;
		if( !m_ppO_ChangeRank[i]->OnCreate( sizeof(OUT_CHANGE_RANK),		DB_BUFFER_COUNT ) )		return FALSE;
#endif
		// Clan Group Note
		if( !m_ppI_ClanGroupNote[i]->OnCreate( sizeof(IN_NOTE_COMMAND),		DB_BUFFER_COUNT ) )		return FALSE;
		if( !m_ppO_ClanGroupNote[i]->OnCreate( sizeof(OUT_NOTE_COMMAND),	DB_BUFFER_COUNT ) )		return FALSE;

#ifdef USE_MESS_FIND
		//User Search
		m_ppI_UserInfo[i]= new Local_CRingBuffer;
		m_ppO_UserInfo[i]= new Local_CRingBuffer;

		if( !m_ppI_UserInfo[i]->OnCreate( sizeof(IN_USER_INFO_SEARCH),		DB_BUFFER_COUNT ) )		return FALSE;
		if( !m_ppO_UserInfo[i]->OnCreate( sizeof(OUT_USER_INFO_SEARCH),		DB_BUFFER_COUNT ) )		return FALSE;
#endif

	}

	//==================================
	// Create Thread & Database
	//==================================	

	// 1. Memory Aloc.
	m_ppADOdb_MESS	= (CADODatabase **)i3MemAlloc( sizeof(CADODatabase *) * DB_THREAD_COUNT);
	m_ppADOdb_USER	= (CADODatabase **)i3MemAlloc( sizeof(CADODatabase *) * DB_THREAD_COUNT);

	m_pi32ThreadIdx		= (INT32 *)i3MemAlloc( sizeof(INT32) * DB_THREAD_COUNT ); 
	m_pHThread			= (HANDLE*)i3MemAlloc( sizeof(HANDLE) * DB_THREAD_COUNT );
	m_pi32LogIdx		= (INT32*)i3MemAlloc( sizeof(INT32) * DB_THREAD_COUNT );

	// 2. DB Connect & Create Threads.


	for( INT32 i=0; i < DB_THREAD_COUNT; i++)
	{
		m_ppADOdb_MESS[i] = CADODatabase::NewObject();
		m_ppADOdb_MESS[i]->Create(TRUE);

		char szMessDBName[DB_STRING_LENGTH] = {0, };
		i3String::Format(szMessDBName, DB_STRING_LENGTH, "PBMess%02d", i);
		m_apMessDBConfig[i] = g_pConfig->GetDatabaseConfig(szMessDBName);
		if( !m_apMessDBConfig[i] )
			return FALSE;

		if(!m_ppADOdb_MESS[i]->Connect(m_apMessDBConfig[i]->GetDataSource(), m_apMessDBConfig[i]->GetCatalog(), m_apMessDBConfig[i]->GetAccount(), m_apMessDBConfig[i]->GetPassword()) )
			return FALSE;
		
		m_ppADOdb_USER[i] = CADODatabase::NewObject();
		m_ppADOdb_USER[i]->Create(TRUE);

		char szUserDBName[DB_STRING_LENGTH] = {0, };
		i3String::Format(szUserDBName, DB_STRING_LENGTH, "PBUser%02d", i);
		m_apUserDBConfig[i] = g_pConfig->GetDatabaseConfig(szUserDBName);
		if( !m_apUserDBConfig[i] )
			return FALSE;

		if(!m_ppADOdb_USER[i]->Connect(m_apUserDBConfig[i]->GetDataSource(), m_apUserDBConfig[i]->GetCatalog(), m_apUserDBConfig[i]->GetAccount(), m_apUserDBConfig[i]->GetPassword()) )
			return FALSE;

		m_pi32ThreadIdx[i]	= i;
		
		DWORD dwThreadID;
		m_pHThread[i] = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , &m_pi32ThreadIdx[i] , 0 , &dwThreadID);
		if( m_pHThread[i] == NULL )
			return FALSE; 
	
		if( !SetThreadPriority( m_pHThread[i], THREAD_PRIORITY_NORMAL) )
			return FALSE;
	}

	return TRUE; 
}

void CDataBase::OnDestroy(void)
{
	// Delete Threads & DB & RingBuffer
	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		if(m_pHThread[i] != NULL)TerminateThread(m_pHThread[i], 0); 

		if(m_ppADOdb_USER != NULL)
		{
			m_ppADOdb_USER[i]->Close();
			I3_SAFE_RELEASE( m_ppADOdb_USER[i] );
		}

		if(m_ppADOdb_MESS != NULL)
		{
			m_ppADOdb_MESS[i]->Close();
			I3_SAFE_RELEASE( m_ppADOdb_MESS[i] );
		}

		I3_SAFE_DELETE( m_ppI_FriendList[i] );
		I3_SAFE_DELETE( m_ppO_FriendList[i] );
		I3_SAFE_DELETE( m_ppI_FriendControl[i] );
		I3_SAFE_DELETE( m_ppO_FriendControl[i] );

#if defined( USE_BLOCK_LIST )
		I3_SAFE_DELETE( m_ppI_BlockList[i] );
		I3_SAFE_DELETE( m_ppO_BlockList[i] );
		I3_SAFE_DELETE( m_ppI_BlockControl[i] );
		I3_SAFE_DELETE( m_ppO_BlockControl[i] );
#endif


		I3_SAFE_DELETE( m_ppI_NoteList[i] );
		I3_SAFE_DELETE( m_ppO_NoteList[i] );
		I3_SAFE_DELETE( m_ppI_NoteCommand[i] );
		I3_SAFE_DELETE( m_ppO_NoteCommand[i] );

#ifdef USE_CHANGE_NICK	
		// Change Nick
		I3_SAFE_DELETE( m_ppI_ChangeNick[i] );
		I3_SAFE_DELETE( m_ppO_ChangeNick[i] );
#endif
		//-----------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------
#ifdef USE_CHANGE_RANK	
		// Change Rank	
		I3_SAFE_DELETE( m_ppI_ChangeRank[i] );
		I3_SAFE_DELETE( m_ppO_ChangeRank[i] );
#endif
		//-----------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------
		// Clan Group Note
		I3_SAFE_DELETE( m_ppI_ClanGroupNote[i] );
		I3_SAFE_DELETE( m_ppO_ClanGroupNote[i] );

#ifdef USE_MESS_FIND
		I3_SAFE_DELETE( m_ppI_UserInfo[i] );
		I3_SAFE_DELETE( m_ppO_UserInfo[i] );
#endif
	}

	// Free memory.
	I3MEM_SAFE_FREE( m_pi32ThreadIdx );
	I3MEM_SAFE_FREE( m_pi32LogIdx );
	I3MEM_SAFE_FREE( m_pHThread );
	I3MEM_SAFE_FREE(m_ppADOdb_USER);
	I3MEM_SAFE_FREE(m_ppADOdb_MESS);

	I3MEM_SAFE_FREE(m_ppI_FriendList);
	I3MEM_SAFE_FREE(m_ppO_FriendList);
	I3MEM_SAFE_FREE(m_ppI_FriendControl);
	I3MEM_SAFE_FREE(m_ppO_FriendControl);

#if defined( USE_BLOCK_LIST )
	I3MEM_SAFE_FREE(m_ppI_BlockList);
	I3MEM_SAFE_FREE(m_ppO_BlockList);
	I3MEM_SAFE_FREE(m_ppI_BlockControl);
	I3MEM_SAFE_FREE(m_ppO_BlockControl);
#endif

	I3MEM_SAFE_FREE( m_ppI_NoteList );
	I3MEM_SAFE_FREE( m_ppO_NoteList );
	I3MEM_SAFE_FREE( m_ppI_NoteCommand );
	I3MEM_SAFE_FREE( m_ppO_NoteCommand );
	I3MEM_SAFE_FREE( m_ppI_ChangeNick );
	I3MEM_SAFE_FREE( m_ppO_ChangeNick );
	I3MEM_SAFE_FREE( m_ppI_ChangeRank );
	I3MEM_SAFE_FREE( m_ppO_ChangeRank );
	I3MEM_SAFE_FREE( m_ppI_ClanGroupNote );
	I3MEM_SAFE_FREE( m_ppO_ClanGroupNote );
	I3MEM_SAFE_FREE( m_ppI_UserInfo );
	I3MEM_SAFE_FREE( m_ppO_UserInfo );

	return; 
}

void CDataBase::OnWorking(INT32 i32ThreadIdx)
{
	wchar_t wszThreadName[DB_STRING_LENGTH];
	i3String::Format(wszThreadName, DB_STRING_LENGTH, L"DATABASE%02d", i32ThreadIdx);
	m_pi32LogIdx[i32ThreadIdx] = g_pLog->AddLogThread(wszThreadName, 0, GetCurrentThreadId());

	BOOL bContinue = TRUE;
	INT32 i32WorkCount;

	while( bContinue )
	{
		i32WorkCount = 0;

		//User Search
	#ifdef USE_MESS_FIND
		i32WorkCount += Working_UserSearch(i32ThreadIdx);
	#endif
		//////////////////////////////////////////////////////////////////////////
		i32WorkCount += Working_Friend(i32ThreadIdx);

#if defined( USE_BLOCK_LIST )
		i32WorkCount += Working_Block(i32ThreadIdx);
#endif
		i32WorkCount += Working_Note(i32ThreadIdx);

		//////////////////////////////////////////////////////////////////////////
#ifdef USE_CHANGE_NICK
		i32WorkCount += WorkingChangeNick(i32ThreadIdx);
#endif
#ifdef USE_CHANGE_RANK
		i32WorkCount += WorkingChangeRank(i32ThreadIdx);
#endif
		//////////////////////////////////////////////////////////////////////////
		i32WorkCount += Working_Clan_Group_Note(i32ThreadIdx);
		//////////////////////////////////////////////////////////////////////////
		
		if(i32WorkCount == 0)Sleep(1);
	}

	return;
}

void CDataBase::ErrorDBMess( wchar_t* wstrSql, INT32 i32ThreadIdx )
{
	g_pLog->WriteLog( L"[ErrorDBMess] %s, Error Code : %08x", wstrSql, m_ppADOdb_MESS[i32ThreadIdx]->GethrLastError() );
	
	HRESULT EventType = m_ppADOdb_MESS[i32ThreadIdx]->ReConnect(
																	m_apMessDBConfig[i32ThreadIdx]->GetDataSource(),
																	m_apMessDBConfig[i32ThreadIdx]->GetCatalog(),
																	m_apMessDBConfig[i32ThreadIdx]->GetAccount(),
																	m_apMessDBConfig[i32ThreadIdx]->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBMess] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBMess] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CDataBase::ErrorDBUser( wchar_t* wstrSql, INT32 i32ThreadIdx )
{
	g_pLog->WriteLog( L"[ErrorDBUser] %s, Error Code : %08x", wstrSql, m_ppADOdb_USER[i32ThreadIdx]->GethrLastError() );
	
	HRESULT EventType = m_ppADOdb_USER[i32ThreadIdx]->ReConnect(
																	m_apUserDBConfig[i32ThreadIdx]->GetDataSource(),
																	m_apUserDBConfig[i32ThreadIdx]->GetCatalog(),
																	m_apUserDBConfig[i32ThreadIdx]->GetAccount(),
																	m_apUserDBConfig[i32ThreadIdx]->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBUser] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBUser] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_MESS_FIND
BOOL CDataBase::InUserSearch(INT64 i64UID, TTCHAR * pstrNick)
{
	INT32 i32ThreadIdx = (UINT32)(i64UID % DB_THREAD_COUNT);
	IN_USER_INFO_SEARCH * pUserSearch = (IN_USER_INFO_SEARCH *)m_ppI_UserInfo[i32ThreadIdx]->PushPointer(); 
	if(pUserSearch == NULL)return FALSE;

	pUserSearch->i64UID = i64UID;
	i3String::Copy(pUserSearch->strNick, pstrNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);

	m_ppI_UserInfo[i32ThreadIdx]->PushPointerIdx();
	return TRUE; 
}

OUT_USER_INFO_SEARCH * CDataBase::GetOutUserSearch(void)
{
	OUT_USER_INFO_SEARCH * pUserSearch = NULL; 

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_iPopIdxUserInfo++; 
		if(m_iPopIdxUserInfo >= DB_THREAD_COUNT)m_iPopIdxUserInfo = 0; 
		if( m_ppO_UserInfo[m_iPopIdxUserInfo]->GetBufferCount() > 0)
		{
			pUserSearch = (OUT_USER_INFO_SEARCH *)m_ppO_UserInfo[m_iPopIdxUserInfo]->Pop(); 
			break;
		}
	}
	return pUserSearch;
}

// 유저찾기 NEW 버전. 2012.06.07 노준영
INT32 CDataBase::Working_UserSearch(INT32 i32ThreadIdx)
{

	INT32 i32WorkCount = 0;

	if( m_ppI_UserInfo[i32ThreadIdx]->GetBufferCount() > 0)
	{
		i32WorkCount++;

		OUT_USER_INFO_SEARCH * pBufferOut = (OUT_USER_INFO_SEARCH*)m_ppO_UserInfo[i32ThreadIdx]->PushPointer();
		if( NULL == pBufferOut )
		{
			g_pLog->WriteLog( L"UserSearch Output Buffer Overflow"  );
			goto ExitErrorUserSearch;
		}

		// 1. Get data from ring buffer that has Input data.
		IN_USER_INFO_SEARCH * pBufferIn = (IN_USER_INFO_SEARCH *)m_ppI_UserInfo[i32ThreadIdx]->Pop();
		if( NULL == pBufferIn )
		{
			g_pLog->WriteLog( L"UserSearch Input Buffer NULL"  );
			goto ExitErrorUserSearch;
		}

		pBufferOut->i32Reault	= EVENT_ERROR_SUCCESS;
		pBufferOut->i64UID		= pBufferIn->i64UID;

		if( FALSE == _GetUIDByNick(i32ThreadIdx, pBufferIn->strNick, &pBufferOut->i64SearchUID ))
		{
			pBufferOut->i32Reault = EVENT_ERROR_NEW_USER_FIND_FUSER_NOT_FIND_DB;
		}

//ExitUserSearch:
		m_ppO_UserInfo[i32ThreadIdx]->PushPointerIdx();
		m_ppI_UserInfo[i32ThreadIdx]->PopIdx();
	}

ExitErrorUserSearch:
	return i32WorkCount;
}


/* 유저 찾기 OLD 버전
INT32 CDataBase::Working_UserSearch(INT32 i32ThreadIdx)
{
	INT32 i32WorkCount = 0;

	if( m_ppI_UserInfo[i32ThreadIdx]->GetBufferCount() > 0)
	{
		i32WorkCount++;

		OUT_USER_INFO_SEARCH * pBufferOut = (OUT_USER_INFO_SEARCH*)m_ppO_UserInfo[i32ThreadIdx]->PushPointer();
		if( pBufferOut == NULL)
		{
			g_pLog->WriteLog(LOG_DB_DATABASE_IDX, "UserSearch Output Buffer Overflow"  );
			goto ExitErrorUserSearch;
		}

		// 1. Get data from ring buffer that has Input data.
		IN_USER_INFO_SEARCH * pBufferIn = (IN_USER_INFO_SEARCH *)m_ppI_UserInfo[i32ThreadIdx]->Pop();

		pBufferOut->i32Reault	= EVENT_ERROR_FAIL; 
		pBufferOut->i64UID		= pBufferIn->i64UID;

		//_GetUIDByNick(INT32 iThreadCount, const char* NickName, UINT32* outUID)
		if( _GetUIDByNick(i32ThreadIdx, pBufferIn->strNick, &pBufferOut->i64SearchUID ))
		{
			pBufferOut->i32Reault = EVENT_ERROR_SUCCESS;
		}

//ExitUserSearch:
		m_ppO_UserInfo[i32ThreadIdx]->PushPointerIdx();
		m_ppI_UserInfo[i32ThreadIdx]->PopIdx();
	}

ExitErrorUserSearch:
	return i32WorkCount;
}
*/
#endif