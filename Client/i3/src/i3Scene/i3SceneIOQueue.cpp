#include "i3SceneDef.h"
#include "i3SceneIOQueue.h"
#include "i3SceneGraphInfo.h"

#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3SceneIOQueue);

i3SceneIOQueue::~i3SceneIOQueue(void)
{
	INT32 i;

	for( i = 0; i < MAX_IOTICKET; i++)
	{
		I3_SAFE_RELEASE( m_Queue[i].m_pSgInfo);
	}

	I3_SAFE_RELEASE( m_pSignal_Kick);
	I3_SAFE_DELETE( m_pMutex);
}

bool i3SceneIOQueue::Create(void)
{
	// Background Thread를 Wake-Up 시키기 위한 Signal
	m_pSignal_Kick = i3Signal::new_object();
	m_pSignal_Kick->Create( true, true);				// Auto-Reset Object

	m_pMutex = new i3Mutex2;
	NetworkDump_Malloc( m_pMutex, sizeof( i3Mutex2), __FILE__, __LINE__);

	return true;
}

void i3SceneIOQueue::ForceRaise( void)
{
	m_pSignal_Kick->Raise();
}

LOADSTATE i3SceneIOQueue::getLoadState( INT32 hndl)
{
	return m_Queue[ hndl].m_State;
}

i3SceneGraphInfo *	i3SceneIOQueue::PopLoadedScene( INT32 hndl)
{
	i3Monitor2 mon( m_pMutex);

	I3ASSERT( hndl < MAX_IOTICKET );
	I3ASSERT( getLoadState( hndl) == LOADSTATE_LOADED);

	i3SceneGraphInfo *	pSgInfo = m_Queue[ hndl].m_pSgInfo;
	I3ASSERT( pSgInfo != nullptr );
	pSgInfo->SetResourcePath(m_Queue[ hndl].m_szPath);
	m_Queue[hndl].m_pSgInfo = nullptr;
	m_Queue[hndl].m_State = LOADSTATE_NA;
	m_Queue[hndl].m_Code = 0;
	m_Queue[hndl].m_szPath[0] = 0;

	return pSgInfo;
}

INT32 i3SceneIOQueue::_FindTicket( UINT128 code)
{
	INT32 i;

	for( i = 0; i < MAX_IOTICKET; i++)
	{
		if( m_Queue[ i].m_Code == code)
			return i;
	}

	return -1;
}

void i3SceneIOQueue::FlushTicket( void)
{
}

INT32 i3SceneIOQueue::PushIO( const char * pszPath, bool bLoadedDX, bool bCommonRes)
{
	i3Monitor2 mon( m_pMutex);
	INT32 idx = -1;

	if( m_Queue[ m_idxPush].m_State == LOADSTATE_LOADING)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Scene IO request exceeds the limit (%d)", MAX_IOTICKET);
		return idx;
	}

	UINT128 code;
	char conv[ MAX_PATH];
	i3::safe_string_copy( conv, pszPath, _countof( conv));
	i3::make_unix_path( conv);
	i3::to_upper( conv);

	i3MD5::Gen( conv, i3::generic_string_size( conv), (char *) &code);

	idx = _FindTicket( code);

	if( idx == -1)
	{
		i3::safe_string_copy( m_Queue[ m_idxPush].m_szPath, pszPath, sizeof(m_Queue[ m_idxPush].m_szPath));
		I3_SAFE_RELEASE( m_Queue[ m_idxPush].m_pSgInfo);
		m_Queue[ m_idxPush].m_State = LOADSTATE_LOADING;
		m_Queue[ m_idxPush].m_bLoadedDX = bLoadedDX;
		m_Queue[ m_idxPush].m_bCommonRes = bCommonRes;

		idx = m_idxPush;

		m_idxPush++;
		if( m_idxPush >= MAX_IOTICKET)
			m_idxPush = 0;

		m_pSignal_Kick->Raise();
	}
	else
	{
		// 아직 로딩중.. 기다리자
	}

	return idx;
}

// Background Thread : Queue에 새로운 IO가 들어올 때까지 대기한다.
INT32 i3SceneIOQueue::WaitIO(void)
{
	if( m_Queue[ m_idxLoad].m_State != LOADSTATE_LOADING)
	{
		m_pSignal_Kick->Reset();
		m_pSignal_Kick->WaitFor();
	}

	return m_idxLoad;
}

// Background Thread : Loading이 종료되었을 때 호출된다. pInfo == nullptr은 Error다.
void i3SceneIOQueue::EndLoad( INT32 idx, i3SceneGraphInfo * pInfo)
{
	I3ASSERT( idx == m_idxLoad);

	if( pInfo != nullptr)
	{
		I3_REF_CHANGE( m_Queue[idx].m_pSgInfo, pInfo);
		m_Queue[ idx].m_State = LOADSTATE_LOADED;
	}
	else
	{
		m_Queue[ idx].m_State = LOADSTATE_ERROR;
	}

	m_idxLoad ++;
	if( m_idxLoad >= MAX_IOTICKET)
		m_idxLoad = 0;
}

void i3SceneIOQueue::ReleaseSceneGraphInfo( i3SceneGraphInfo * pInfo)
{
	i3Monitor2 mon( m_pMutex);
	I3_SAFE_RELEASE( pInfo);
}

