#include "pch.h"
#include "MapManager.h"
#include "MapData.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

CMapManager* g_pMapManager = NULL;


///////////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
//	
CMapManager::CMapManager(void)
{
	i3mem::FillZero( m_aMaps,			sizeof(m_aMaps));
	i3mem::FillZero( m_pTabMapNames,	sizeof(m_pTabMapNames));

	m_ui32Count = 0;
	
}

CMapManager::~CMapManager(void)
{
	_Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// init()
//	map을 로드하고 name 테이블과 id 테이블을 초기화한다
BOOL CMapManager::UnnecessaryMaps(INT8 i8MapIndex)
{
	if ( STAGE_UID_TUTORIAL == i8MapIndex )
		return FALSE;
	
	return g_pConfig->m_aMapLoad[i8MapIndex].m_bIsOpen;
}

BOOL CMapManager::Init()
{
	//Cast서버에서 맵로딩 데이터를 받기전까지 대기합니다.
	while (!g_pConfig->m_bMapDataReceived)
	{
		Sleep(1000);
	}

	// load map names
	if( FALSE == FillMapNames() )		return FALSE;
	I3TRACE("Stage Count = %d\n", m_ui32Count );
	
	

	// create map resource	
	UINT16 ui16MapCount = 1;
	for( UINT32 i = 0 ; i < STAGE_UID_MAX ; i++ ) 
	{

		if ( FALSE == UnnecessaryMaps( i ) )						continue;
				
		if( !m_aMaps[ i ] )
		{
			I3TRACE("TRACE: internal map %d initialized\n", i );
			
			// map data alloc and initialize
			m_aMaps[ i ] = new CMapData ( i, m_pTabMapNames[i] );
			if( ! m_aMaps[ i ] ) 
			{
				DSERROR("[CMapManager::Init] new CMapData Failed: stage ord=%d\n", i );
				return FALSE;
			}
			
			// load map data
			if( ! m_aMaps[i]->Load( m_pTabMapNames[i], i))
			{
				DSERROR("[CMapManager::Init] stage uid:%d - %s load error...\n", i, m_pTabMapNames[i] );
#ifdef _DEBUG
				g_pConfig->m_aMapLoad[i].m_bIsOpen = false;
				SAFE_DELETE(m_aMaps[i]);
				continue;
#else
				return FALSE;
#endif
			}

			DSERROR("[CMapManager::Init] MapLoad - Idx : %3d\t Name : %s\n", ui16MapCount, m_pTabMapNames[i]);
			++ui16MapCount;
		}

	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// _Release
//	destructor에서 호출 됨
void CMapManager::_Release()
{
	
	for( UINT32 i = 0; i < STAGE_UID_MAX ; i++)
	{
		SAFE_DELETE( m_aMaps[i] );
		SAFE_DELETE( m_pTabMapNames[i]);
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FillMapNames
//	맵의 코드명과 stage ord 배열을 셋업한다. 
//	코드명은 리소스의 경로명을 만들 때 사용된다.
BOOL CMapManager::FillMapNames()
{
	INT32		i32Len = 0;
	
	for( INT32 i = 1; i < STAGE_UID_MAX ; i++)
	{		

		if (FALSE == g_pConfig->m_aMapLoad[i].m_bIsOpen) continue;
				
		i32Len		= (INT32) strlen( g_pConfig->m_aMapLoad[i].m_strCodeName );		// alloc mem
		m_pTabMapNames[i] = new char[ i32Len + 1 ];						// ...
		I3ASSERT( m_pTabMapNames[ i ] );
		strcpy_s( m_pTabMapNames[ i ], i32Len + 1, g_pConfig->m_aMapLoad[i].m_strCodeName );	// copy map name
		
		m_ui32Count++;
	}

	return TRUE;
}

// step 함수
void CMapManager::OnUpdate_Main(void)
{
	for (UINT32 ui32StageUID = 1 ; ui32StageUID < STAGE_UID_MAX; ui32StageUID++)
	{				
		if( m_aMaps[ui32StageUID] != NULL )
		{
				m_aMaps[ui32StageUID]->Step_M();
		}
	}
}
