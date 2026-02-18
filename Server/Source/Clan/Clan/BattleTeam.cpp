#include "pch.h"
#include "BattleTeam.h"

I3_CLASS_INSTANCE(CBattleTeam, i3ElementBase);

static INT32 _CompareProcBattle(void* p1,void* p2)
{
	return *((UINT32*)p1) - *((UINT32*)p2);
}

CBattleTeam::CBattleTeam()
{
}

CBattleTeam::~CBattleTeam()
{
	OnDestroy();
}

BOOL	CBattleTeam::Create()
{
	i3mem::Fill( m_ai32Naming, -1, sizeof(INT32)*MAX_MEMBER_COUNT );

	m_pListChannel	= i3BinList::NewObject();
	m_pListChannel->SetCompareProc( _CompareProcBattle );
	m_pListClan		= i3BinList::NewObject();
	m_pListClan->SetCompareProc( _CompareProcBattle );

	return TRUE;
}

void	CBattleTeam::OnDestroy()
{
	if( m_pListClan )
	{
		INT32 i32Count = m_pListClan->GetCount();
		for( INT32 i = 0 ; i < i32Count ; i++ )
		{
			BATTLE_CLAN_INFO* pstClanInfo = (BATTLE_CLAN_INFO*)m_pListClan->GetItem( i );
			I3_SAFE_RELEASE( pstClanInfo->_pClanList );	// pstClanInfo->_pClanList 에 들어가는 내용은 m_pListChannel 과 동일하기때문에 delete 해줄 필요는 없습니다.
			I3_SAFE_DELETE( pstClanInfo );
		}
		I3_SAFE_RELEASE( m_pListClan );
	}
	if( m_pListChannel )
	{
		INT32 i32Count = m_pListChannel->GetCount();
		for( INT32 i = 0 ; i < i32Count ; i++ )
		{
			BATTLE_INFO* pstBattleInfo = (BATTLE_INFO*)m_pListChannel->GetItem( i );
			I3_SAFE_DELETE( pstBattleInfo );
		}
		I3_SAFE_RELEASE( m_pListChannel );
	}
}

INT32	CBattleTeam::_GetNaming( i3BinList* pClanList )
{
	INT32 i32Naming = -1;
	if( NULL == pClanList )
		return i32Naming;

	for( INT32 i = 0 ; i < pClanList->GetCount(); i++ )
	{
		BATTLE_INFO* pstBattleTeam = (BATTLE_INFO*)pClanList->GetItem( i );
		m_ai32Naming[ pstBattleTeam->_Team._Naming ] = 0;
	}

	for( INT32 i = 0 ; i < MAX_MEMBER_COUNT ; i++ )
	{
		if( 0 == m_ai32Naming[i] )
			continue;

		i32Naming = i;
		break;
	}

	for( INT32 i = 0 ; i < pClanList->GetCount() ; i++ )
	{
		BATTLE_INFO* pstBattleTeam = (BATTLE_INFO*)pClanList->GetItem( i );
		m_ai32Naming[ pstBattleTeam->_Team._Naming ] = -1;
	}

	return i32Naming;
}

INT32 CBattleTeam::_GetChannelNaming( BATTLE_CHANNE_INFO* pstChannelInfo )
{
	for( INT32 i = 0 ; i < BATTLE_CHANNEL_NAMING ; i++ )
	{
		if( 0 != pstChannelInfo->_ui8Naming[i] )
			continue;

		pstChannelInfo->_ui8Naming[i] = 1;

		return i;
	}

	return INVALID_IDX;
}

BATTLE_INFO* CBattleTeam::_GetEmptyBattleInfo()
{
	BATTLE_INFO* pMatchInfo = new BATTLE_INFO;
	if( !pMatchInfo )
		return NULL;

	// 우선 여기서 초기화하고 구조 변경하면서 각 클래스에서 초기화 하도록 수정
	pMatchInfo->_Team._Channel = 0;
	pMatchInfo->_Team._PerMax = 0;
	pMatchInfo->_Team._PerCount = 0;
	pMatchInfo->_ui32Idx = 0;
	for( INT32 i = 0 ; i < 8 ; i++ )
	{
		pMatchInfo->_Member[i]._State = CLAN_ROOM_EMPTY;
	}

	return pMatchInfo;
}

BATTLE_CLAN_INFO*	CBattleTeam::_GetEmptyClanInfo( UINT32 ui32CID )
{
	BATTLE_CLAN_INFO* pstClanInfo = NULL;

	INT32 i32ClanIdx = m_pListClan->FindItem( &ui32CID );
	if( -1 < i32ClanIdx )
	{
		pstClanInfo = (BATTLE_CLAN_INFO*)m_pListClan->GetItem( i32ClanIdx );
	}
	if( NULL == pstClanInfo )
	{
		pstClanInfo = new BATTLE_CLAN_INFO;
		if( NULL == pstClanInfo )							return NULL;
	
		pstClanInfo->_ui32CID	= ui32CID;
		pstClanInfo->_pClanList	= i3BinList::NewObject();
		if( NULL == pstClanInfo->_pClanList )
		{
			I3_SAFE_DELETE( pstClanInfo );
			return NULL;
		}
		pstClanInfo->_pClanList->SetCompareProc( _CompareProcBattle );

		pstClanInfo->_stClanContext._ui8Totalcount			= 0;
		pstClanInfo->_stClanContext._ui8SectionCount		= 0;

		if( 0 > m_pListClan->Add( pstClanInfo ) )
		{
			I3_SAFE_RELEASE( pstClanInfo->_pClanList );
			I3_SAFE_DELETE( pstClanInfo );
			return NULL;
		}
	}

	return pstClanInfo;
}

BATTLE_CHANNE_INFO*	CBattleTeam::_GetEmptyChannelInfo( UINT32 ui32Channel )
{
	BATTLE_CHANNE_INFO* pstChannelInfo = NULL;

	INT32 i32ChannelIdx = m_pListChannel->FindItem( &ui32Channel );
	if( -1 < i32ChannelIdx )
	{
		pstChannelInfo = (BATTLE_CHANNE_INFO*)m_pListChannel->GetItem( i32ChannelIdx );
	}
	if( NULL == pstChannelInfo )
	{
		pstChannelInfo = new BATTLE_CHANNE_INFO;
		if( NULL == pstChannelInfo )						return NULL;

		pstChannelInfo->_ui32Channel	= ui32Channel;
		i3mem::FillZero( pstChannelInfo->_ui8Naming, sizeof(UINT8) * BATTLE_CHANNEL_NAMING );
		pstChannelInfo->_pChannelList	= i3BinList::NewObject();
		if( NULL == pstChannelInfo->_pChannelList )
		{
			I3_SAFE_DELETE( pstChannelInfo );
			return NULL;
		}
		pstChannelInfo->_pChannelList->SetCompareProc( _CompareProcBattle );

		pstChannelInfo->_stChannelContext._ui16Totalcount	= 0;
		pstChannelInfo->_stChannelContext._ui16SectionCount	= 0;

		if( 0 > m_pListChannel->Add( pstChannelInfo ) )
		{
			I3_SAFE_RELEASE( pstChannelInfo->_pChannelList );
			I3_SAFE_DELETE( pstChannelInfo );
			return NULL;
		}
	}

	return pstChannelInfo;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
BATTLE_INFO* CBattleTeam::CreateTeam( UINT32 ui32CID, UINT32 ui32Channel, INT32* pi32Naming )
{
	BATTLE_INFO* pstBattleInfo = _GetEmptyBattleInfo();
	if( NULL == pstBattleInfo )										goto exit_create;
	// ClanList Setting
	BATTLE_CLAN_INFO* pstClanInfo = _GetEmptyClanInfo( ui32CID );
	if( NULL == pstClanInfo )										goto exit_create;

	// 해당 클랜의 매치팀 최대 갯수 체크.
	if( MAX_CLAN_MATCH_TEAM_COUNT <= pstClanInfo->_stClanContext._ui8Totalcount ) 
	{
		goto exit_create;
	}

	// ChannelList Setting
	BATTLE_CHANNE_INFO* pstChannelInfo = _GetEmptyChannelInfo( ui32Channel );
	if( !pstChannelInfo )
		goto exit_create;

	// BattleInfo Setting
	// Naming 설정
	INT32 i32ChannelNaming = _GetChannelNaming( pstChannelInfo );
	pstBattleInfo->_ui32Idx	= SET_TEAM_CLAN_IDX( ui32Channel, i32ChannelNaming );

	// ClanList Insert
	if( 0 > pstClanInfo->_pClanList->Add( pstBattleInfo ) )			goto exit_create;
	// ChannelList Insert
	if( 0 > pstChannelInfo->_pChannelList->Add( pstBattleInfo ) )
	{	// Channel Insert 실패 했을 경우 _pClanList 도 지워줘야 합니다.
		INT32 i32ClBattleIdx = pstClanInfo->_pClanList->FindItem( &pstBattleInfo->_ui32Idx );
		if( -1 < i32ClBattleIdx )
		{
			pstClanInfo->_pClanList->Delete( i32ClBattleIdx );
		}
		goto exit_create;
	}

	*pi32Naming = _GetNaming( pstClanInfo->_pClanList );

	// ClanContext Setting
	pstClanInfo->_stClanContext._ui8Totalcount++;
	SetClanContext( &pstClanInfo->_stClanContext );

	// ChannelContext Setting
	pstChannelInfo->_stChannelContext._ui16Totalcount++;
	SetChannelContext( &pstChannelInfo->_stChannelContext );

	return pstBattleInfo;

exit_create:

	// 메모리 삭제코드 만듭니다.
	I3_SAFE_DELETE( pstBattleInfo );

	return NULL;
}

void	CBattleTeam::DestroyTeam( BATTLE_INFO* pstBattleInfo )
{
	INT32 i32Channel = GET_TEAM_CLAN_CHANNEL( pstBattleInfo->_ui32Idx );

	INT32 i32ChannelIdx = m_pListChannel->FindItem( &i32Channel );
	if( i32ChannelIdx < 0 )	
		return;

	BATTLE_CHANNE_INFO* pstChannelInfo = (BATTLE_CHANNE_INFO*)m_pListChannel->GetItem( i32ChannelIdx );
	if( !pstChannelInfo )
		return;

	// ChannelIdx
	INT32 i32ChBattleIdx = pstChannelInfo->_pChannelList->FindItem( &pstBattleInfo->_ui32Idx );
	if( 0 > i32ChBattleIdx )			return;

	// ClanList
	INT32 i32ClanIdx = m_pListClan->FindItem( &pstBattleInfo->_Clan._ClanIdx );
	if( 0 > i32ClanIdx )			return;
	BATTLE_CLAN_INFO* pstClanInfo = (BATTLE_CLAN_INFO*)m_pListClan->GetItem( i32ClanIdx );
	if( NULL == pstClanInfo )		return;

	// ClanelIdx
	INT32 i32ClBattleIdx = pstClanInfo->_pClanList->FindItem( &pstBattleInfo->_ui32Idx );
	if( 0 > i32ClBattleIdx )		return;
	
	// Delete
	pstClanInfo->_pClanList->Delete( i32ClBattleIdx );
	pstChannelInfo->_pChannelList->Delete( i32ChBattleIdx );
	pstChannelInfo->_ui8Naming[ GET_TEAM_CLAN_NAMING( pstBattleInfo->_ui32Idx ) ] = 0;

	I3_SAFE_DELETE( pstBattleInfo );

	pstClanInfo->_stClanContext._ui8Totalcount--;
	SetClanContext( &pstClanInfo->_stClanContext );

	pstChannelInfo->_stChannelContext._ui16Totalcount--;
	SetChannelContext( &pstChannelInfo->_stChannelContext );
}

BATTLE_INFO* CBattleTeam::FindTeam(UINT32 ui32Idx)
{
	UINT32 ui32Channel = GET_TEAM_CLAN_CHANNEL(ui32Idx);
	INT32 i32FindIdx = m_pListChannel->FindItem( &ui32Channel );
	if( i32FindIdx < 0 )
		return NULL;

	BATTLE_CHANNE_INFO* pstChannelInfo = (BATTLE_CHANNE_INFO*)m_pListChannel->GetItem( i32FindIdx );
	if( NULL == pstChannelInfo )
		return NULL;

	INT32 i32ChannelIdx = pstChannelInfo->_pChannelList->FindItem( &ui32Idx );
	if( i32ChannelIdx < 0 )
		return NULL;

	return (BATTLE_INFO*)pstChannelInfo->_pChannelList->GetItem( i32ChannelIdx );
}

BATTLE_CHANNE_INFO*	CBattleTeam::FindChannelTeam( UINT32 ui32Channel )
{
	INT32 i32FindIdx = m_pListChannel->FindItem( &ui32Channel );
	if( 0 > i32FindIdx ) return NULL;

	return (BATTLE_CHANNE_INFO*)m_pListChannel->GetItem( i32FindIdx );
}

BATTLE_CLAN_INFO* CBattleTeam::FindClanTeam( UINT32 ui32CID )
{
	INT32 i32FindIdx = m_pListClan->FindItem(&ui32CID);
	if( i32FindIdx < 0)
		return NULL;

	return static_cast<BATTLE_CLAN_INFO*>( m_pListClan->GetItem(i32FindIdx) );
}

void	CBattleTeam::SetClanContext( BATTLE_CLAN_CONTEXT* pClanContext )
{
	pClanContext->_ui8SectionCount = pClanContext->_ui8Totalcount / BATTLE_TEAM_SECTION_SIZE;
	if ( pClanContext->_ui8Totalcount % BATTLE_TEAM_SECTION_SIZE )
	{
		pClanContext->_ui8SectionCount++;
	}
}

void	CBattleTeam::SetChannelContext( BATTLE_CHANNEL_CONTEXT* pClanContext )
{
	pClanContext->_ui16SectionCount = pClanContext->_ui16Totalcount / BATTLE_ALL_TEAM_SECTION_SIZE;
	if ( pClanContext->_ui16Totalcount % BATTLE_ALL_TEAM_SECTION_SIZE )
	{
		pClanContext->_ui16SectionCount++;
	}
}
