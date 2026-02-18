#include "pch.h"
#include "BGLoadThread.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/algorithm/to_upper.h"

I3_CLASS_INSTANCE( BGLoadThread);

BGLoadThread::BGLoadThread() : m_writeIdx(0)
{
	i3mem::FillZero( m_Que, sizeof( m_Que));

	m_Que[0]._OwnList.reserve( 128);
	m_Que[1]._OwnList.reserve( 128);
}

void BGLoadThread::Run( void * pData)
{
	char szTemp[ MAX_PATH];

	if( m_Que[ m_writeIdx].m_State == LT_LOADING)
	{
		i3ResourceManager * pResManager = g_pFramework->GetResourceManager();
		i3::extract_filename(m_Que[ m_writeIdx].m_szPath, szTemp);

		i3GameRes * pRes = pResManager->LoadBackgroundRes( szTemp, false/*, true*/);
		if( pRes == nullptr)
		{	// RSC에 등록하지 않은 Resource
			I3PRINTLOG(I3LOG_WARN,  "RSC에 등록되지 않은 Resource입니다. %s", szTemp);
			pRes = pResManager->LoadResource( m_Que[ m_writeIdx].m_szPath, 0, false, true);
		}
		
		I3_REF_CHANGE( m_Que[ m_writeIdx].m_pLoadedRes, pRes);
		m_Que[ m_writeIdx].m_State = LT_LOADED;

		::InterlockedExchange( (volatile LONG *)&m_writeIdx, m_writeIdx?0:1);
	}
}

void BGLoadThread::SetLoadCmd( const char * pszPath)
{
	I3ASSERT( pszPath != nullptr);
	UINT32 idx = m_writeIdx;

	I3ASSERT( m_Que[ idx].m_State == LT_NONE);

	i3::safe_string_copy( m_Que[ idx].m_szPath, pszPath, sizeof( m_Que[ idx].m_szPath));
	m_Que[ idx].m_State = LT_LOADING;

	Wakeup();
}

bool BGLoadThread::IsFree( void)
{
	bool bFree = (m_Que[ m_writeIdx].m_State == LT_NONE)?true:false;
	
	return bFree;
}

i3GameRes * BGLoadThread::GetRes( void)
{
	INT32 readIdx = m_writeIdx?0:1;

	i3GameRes * pRes = m_Que[ readIdx].m_pLoadedRes;
	if( pRes != nullptr)
	{
		m_Que[ readIdx].m_pLoadedRes = nullptr;
		m_Que[ readIdx].m_State = LT_NONE;
	}

	return pRes;
}