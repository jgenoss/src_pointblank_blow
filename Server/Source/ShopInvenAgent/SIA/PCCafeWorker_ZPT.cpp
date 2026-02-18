#include "pch.h"
#include "PCCafeWorker_ZPT.h"

#define PCCAFE_ZPT_UPDATETIME		7*60

CPCCafeWorker_ZPT::CPCCafeWorker_ZPT()
{
	m_ui32UpdateTime	= 0;
	m_i32PCCafeVer		= 0;
}

CPCCafeWorker_ZPT::~CPCCafeWorker_ZPT()
{
}

BOOL CPCCafeWorker_ZPT::_WorkProcess( PCCAFE_INPUT* pInput, PCCAFE_OUTPUT* pOutput )
{
	switch( pInput->m_eInputType )
	{
	case TYPE_PCCAFE_INPUT_FIRSTLOGIN		:
		{
			if( FALSE == IsPcCafe( pInput->m_ui32IP, pOutput->m_ui8PCCafe ) )
			{
				pOutput->m_ui8PCCafe = PC_NOT_CAFE;
			}
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CPCCafeWorker_ZPT::OnRunning()
{
	if( m_ui32UpdateTime > i3ThreadTimer::GetServerTime() )														return FALSE;
	m_ui32UpdateTime = i3ThreadTimer::GetServerTime() + PCCAFE_ZPT_UPDATETIME;

	wchar_t wstrQuery[ MAX_PATH ];
	i3String::Format(wstrQuery, MAX_PATH, L"usp_PCCafeVersion_sel" );

	if( FALSE == m_pDB->ExecuteSP(wstrQuery) )
	{
		ReConnect(wstrQuery);
		return TRUE;
	}
	if( 0 == m_pDB->GetRecordCount() )																	return TRUE;
	INT32 i32Ver	= m_pDB->GetINT32Data(0);
	if( i32Ver == m_i32PCCafeVer )																		return TRUE;

	// 로드
	INT32 i32NextIdx = m_i32UseIdx+1;
	if( PC_BUFFER_COUNT <= i32NextIdx ) i32NextIdx = 0;

	if( FALSE == _LoadPCCafeList( m_pIpList[ i32NextIdx ],  &m_iIpListCount[ i32NextIdx ]) )	return TRUE;
	
	m_i32PCCafeVer	= i32Ver;
	m_i32UseIdx		= i32NextIdx;

	return TRUE;
}

BOOL CPCCafeWorker_ZPT::_LoadPCCafeList( TYPE_IPLIST * pList, UINT32 * pui32Count )
{
	UINT32 ui32NewCount = 0;

	wchar_t wstrQuery[ MAX_PATH ];
	i3String::Format(wstrQuery, MAX_PATH, L"usp_PCCafe_scan" );
	if( FALSE == m_pDB->ExecuteSP(wstrQuery) )
	{
		ReConnect(wstrQuery);
		return FALSE;
	}
	
	//Set PCCafe Count 
	ui32NewCount = (UINT32)m_pDB->GetRecordCount(); 
	if( PCCAFE_MAX_COUNT < ui32NewCount )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[OnCreate-CDataBase::LoadPcCafeList_PcThread] NewCount > PCCAFE_MAX_COUNT" ); 
		return FALSE; 
	}

	*pui32Count = ui32NewCount;

	//Load Ip List 
	char	strPCIP[16]; 
	strPCIP[15] = 0;
	UINT32	ui32IP;
	UINT8	ui8Rank;
	UINT8	ui8CIDR;
	for( UINT32 i = 0 ; i < ui32NewCount ; i++ )
	{
		m_pDB->GetTextData(0, strPCIP, 16);
		ui32IP	= ::htonl( ::inet_addr(strPCIP) );
		ui8Rank	= (UINT8)m_pDB->GetUINT16Data( 1 );
		ui8CIDR = m_pDB->GetUINT8Data( 2 );

		// 순차 정렬을 합니다.
		INT32 i32InsertIdx = 0;
		for( UINT32 j = 0 ; j <= i ; j++ )
		{
			i32InsertIdx = j;

			if( pList[ j ].m_ui32IP == ui32IP )
			{
				i32InsertIdx = -1;
				break;
			}
			else if( pList[ j ].m_ui32IP == 0 || pList[ j ].m_ui32IP > ui32IP )
			{
				break;
			}
		}
		// -1 이면 이미 같은 IP 가 존재할 때입니다. 더이상 추가 할 필요가 없습니다.
		if( -1 != i32InsertIdx )
		{
			INT32 i32MoveCount = i - i32InsertIdx;
			if( 0 < i32MoveCount )
			{
				memmove( &pList[ i32InsertIdx+1 ], &pList[ i32InsertIdx ], sizeof( TYPE_IPLIST )*i32MoveCount );
			}
			
			// 값을 세팅해 줍니다.
			pList[ i32InsertIdx ].m_ui32IP		= ui32IP;
			pList[ i32InsertIdx ].m_ui8Rank		= ui8Rank;
			pList[ i32InsertIdx ].m_ui32CIDR	= 0;
			for( INT32 j = 0 ; j < ui8CIDR ; j++ )
			{
				// CIDR 을 설정합니다. 추후 이 값으로 & 연산해서 속해있는 IP 인지 확인 할 예정입니다.
				pList[ i32InsertIdx ].m_ui32CIDR |= 1 << (31-j);
			}
		}

		if( FALSE == m_pDB->MoveNext() )
		{
			return FALSE;
		}
	}
		
	return TRUE; 
}

BOOL CPCCafeWorker_ZPT::IsPcCafe( UINT32 ui32IP, UINT8& ui8Rank )
{
	UINT32			ui32Count	= m_iIpListCount[ m_i32UseIdx ];
	TYPE_IPLIST *	pList		= m_pIpList[ m_i32UseIdx ];

	if( 0 == ui32Count )	return FALSE;

	INT32 i32Start	= 0;
	INT32 i32End	= ui32Count;
	INT32 i32Center = 0;
	while( i32Start < i32End )
	{
		i32Center = (i32Start + i32End) / 2;

		if( pList[ i32Center ].m_ui32IP		> ui32IP )	
		{
			i32End = i32Center;
			i32Center = i32End;
		}
		else if( pList[ i32Center ].m_ui32IP	< ui32IP )
		{
			i32Start = i32Center;
			i32Center = i32Start;
		}
		else 
		{
			break;
		}
		if( 1 == (i32End - i32Start) )
		{
			i32Center = i32Start;
			break;
		}
	} 

	if( (ui32IP & pList[i32Center].m_ui32CIDR) == pList[i32Center].m_ui32IP )
	{
		ui8Rank = pList[i32Center].m_ui8Rank;
		return TRUE; 
	}

	return FALSE; 
}