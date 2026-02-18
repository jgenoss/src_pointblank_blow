#include "pch.h"
#include "MapMgr.h"
#include "MapData.h"


#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "LogFile.h"

void g_InitMapBounds();
CMapMgr* g_pMapMgr = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
//	
CMapMgr::CMapMgr(void)
{
	memset( m_pMaps,		0,	sizeof(m_pMaps));
	memset( m_pTabMapNames,	0,	sizeof(m_pTabMapNames));	
	memset( m_pTabMapIDs,	0,	sizeof(m_pTabMapIDs));
	m_ui32Count = 0;
}

CMapMgr::~CMapMgr(void)
{
	if(m_ui32Count > 0)
	{
		_Release();
	}

	NXU::releaseGlobalInstances();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// init()
//	map을 로드하고 name 테이블과 id 테이블을 초기화한다
BOOL CMapMgr::Init()
{
	// init map bounds (global)
	g_InitMapBounds();
	
	// load map names
	m_ui32Count = FillMapNames();
	
	I3TRACE("******************\n");
	I3TRACE("TOTAL STAGE: %d\n", m_ui32Count );
	
	// create map resource
	
	// 각 스테이지별로 컬리전 표시하기 위함
	//UINT32 i = STAGE_ORD_PORT_AKABA - 1;	// 1m
	//UINT32 i = STAGE_ORD_RED_ROCK - 1;	
	//UINT32 i = STAGE_ORD_LIBRARY - 1;
	//UINT32 i = STAGE_ORD_MSTATION - 1;
	//UINT32 i = STAGE_ORD_MIDNIGHT_ZONE - 1;
	//UINT32 i = STAGE_ORD_UPTOWN - 1;
	//UINT32 i = STAGE_ORD_BURNINGHALL - 1;
	//UINT32 i = STAGE_ORD_DSQUAD - 1;
	//UINT32 i = STAGE_ORD_CRACKDOWN - 1;
	//UINT32 i = STAGE_ORD_SAINTMANSION - 1; 
	//UINT32 i = STAGE_ORD_EASTERNROAD - 1;	
	//UINT32 i = STAGE_ORD_D_DOWNTOWN - 1;
	//UINT32 i = STAGE_ORD_D_LUXVILLE - 1;
	//UINT32 i = STAGE_ORD_D_BLOWCITY - 1;
	//UINT32 i = STAGE_ORD_D_STORMTUBE - 1;
	//UINT32 i = STAGE_ORD_D_GIRAN2 - 1;	
	//UINT32 i = STAGE_ORD_D_BREAKDOWN - 1;
	//UINT32 i = STAGE_ORD_TRAININGCAMP - 1;
	//UINT32 i = STAGE_ORD_D_SENTRYBASE - 1;
	//UINT32 i = STAGE_ORD_DESERTCAMP - 1;
	//UINT32 i = STAGE_ORD_KICKPOINT - 1;
	//UINT32 i = STAGE_ORD_FACEROCK - 1;
	//UINT32 i = STAGE_ORD_SUPPLYBASE - 1;
	//UINT32 i = STAGE_ORD_D_SANDSTORM - 1;
	//UINT32 i = STAGE_ORD_DOWNTOWN - 1;					
	//UINT32 i = STAGE_ORD_LUXVILLE - 1;
	//UINT32 i = STAGE_ORD_OUTPOST - 1;		// 999로 설정되어 있음
	//UINT32 i = STAGE_ORD_BLOWCITY - 1;
	//UINT32 i = STAGE_ORD_STORMTUBE - 1;
	//UINT32 i = STAGE_ORD_SENTRYBASE - 1;
	//UINT32 i = STAGE_ORD_HOSPITAL - 1;
	//UINT32 i = STAGE_ORD_DOWNTOWN2 - 1;
	//UINT32 i = STAGE_ORD_SHOPPINGCENTER - 1;
	//UINT32 i = STAGE_ORD_SANDSTORM - 1;
	//UINT32 i = STAGE_ORD_BREAKDOWN - 1;				
	//UINT32 i = STAGE_ORD_GIRAN - 1;
	//UINT32 i = STAGE_ORD_GIRAN2 - 1;
	//UINT32 i = STAGE_ORD_HELISPOT - 1;	// 999
	//UINT32 i = STAGE_ORD_BLACKPANTHER - 1;				
	//UINT32 i = STAGE_ORD_BREEDING_NEST - 1;			
	//UINT32 i = STAGE_ORD_D_UPTOWN - 1;
	//UINT32 i = STAGE_ORD_DINO_BREAKDOWN - 1;
	//UINT32 i = STAGE_ORD_DEADTOWN - 1;					// BB 데이터 없음(제압미션)
	//UINT32 i = STAGE_ORD_TUTORIAL - 1;					// BB 데이터 없음				
	//UINT32 i = STAGE_ORD_D_SHOPPINGCENTER - 1;			// BB OK. ccd 이상 데스 매치 - 쇼핑 센터 
	//UINT32 i = STAGE_ORD_D_SAFARI - 1;					// ccd 이상
	//UINT32 i = STAGE_ORD_DRAGONALLEY - 1;					// BB 데이터 없음
	//UINT32 i = STAGE_ORD_MACHUPICHU - 1;					// BB 데이터 없음
	//UINT32 i = STAGE_ORD_CARGOSHIP - 1;
	
	for( UINT32 i = 0; i < m_ui32Count; i++)
	{
		// map data alloc and initialize
		m_pMaps[i] = new CMapData ( GetMapID(i) );
		if( m_pMaps[i] == NULL ) 
		{
			DSERROR("[CMapMgr::Init] Memory\n");
			return FALSE;
		}
		
		// load map data
		if( m_pMaps[i]->Load( GetMapName( GetMapID(i) )) == FALSE )
		{
			// Load()가 로그 찍음
			SAFE_DELETE( m_pMaps[i] );
		}
	}

	// physics 실행한다.
	// Note. Load 직후에 실행하면 에러 발생
	for( UINT32 i = 0; i < m_ui32Count; i++) 
	{
		if( m_pMaps[i] != NULL ) m_pMaps[i]->Start();
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// _Release
//	destructor에서 호출 됨
void CMapMgr::_Release()
{
	for( UINT32 i = 0; i < m_ui32Count; i++)
	{
		SAFE_DELETE( m_pMaps[i] );
		SAFE_DELETE( m_pTabMapNames[i]);
	}

	m_ui32Count = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FillMapNames
//	맵의 코드명과 stage ord 배열을 셋업한다. 
//	코드명은 리소스의 경로명을 만들 때 사용된다.
UINT32 CMapMgr::FillMapNames()
{
	INT32		i32Len = 0;
	char		szStageCodeName[MAX_PATH] = { '\0', };
	
	InitStageInfo();
	
	m_ui32Count = 0;
	
	for( INT32 i = DS_MIN_MAP; i < DS_MAX_MAP; i++ )
	{
		getStageCodeName( szStageCodeName, i  );								// map name
		i32Len		= (INT32) strlen( szStageCodeName );						// alloc mem
		if( i32Len <= 0 ) continue;
		
		m_pTabMapNames[ m_ui32Count ] = new char[ i32Len + 1 ];					// ...
		if( m_pTabMapNames[ m_ui32Count ] == NULL )
		{
			DSERROR("[MapManager] Memory(%d)\n", GetLastError());
			return 0;
		}
		
		strcpy_s( m_pTabMapNames[ m_ui32Count ], i32Len + 1, szStageCodeName );	// copy map name
		m_pTabMapIDs[ m_ui32Count ] = (STAGEORD)i ;								// packed index: ord 0이 NA이기 때문에 0이 아닌 값이 저장된다
																				// - 마지막 0는 EOF로 사용가능
		I3TRACE("STAGE ORD NUM:%d, STAGE NAME: %s\n", i, m_pTabMapNames[ m_ui32Count ] );

		m_ui32Count++;
	}

	return m_ui32Count;
}


// step 함수
void CMapMgr::OnUpdate_Main(void)
{
	for( UINT32 i = 0; i < m_ui32Count; i++)
	{
		if( m_pMaps[i] != NULL )
		{
			m_pMaps[i]->step_T();
		}
		else
		{
			DSERROR("[CMapMgr] Check Map:%s\n",  m_pTabMapNames[i] );
		}
	}
}
