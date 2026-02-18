#include	"pch.h"
#include	"CharaInfoContext.h"
#include	"UserInfoContext.h"

//I3_CLASS_INSTANCE( CCharaInfoContext);//, i3ElementBase );

CCharaInfoContext::CCharaInfoContext()
{
	m_vecChara.reserve( S2MO_MULTI_SLOT_COUNT);
	
	setMyLastRecord(false,0,0,0,0,0,0,0,0,0);
}

CCharaInfoContext::~CCharaInfoContext()
{
	i3::vector<STCLevel>::iterator itr;
	ClearAll();
	for(itr = m_Checklevel.begin(); itr != m_Checklevel.end(); ++itr)
	{
		for(INT32 i=0; i < 3; i++)
			itr->SlotLevel[i].clear();
	}
	m_Checklevel.clear();
}


//---------------------------------------------------------------------------------------------------------
//private
INT32 CCharaInfoContext::find_slot_idx(UINT8 idx) const
{
	const size_t size = m_vecChara.size();
	for(size_t i =0; i<size;++i)
	{
		const tagChara* pTagChara = m_vecChara[i];

		if(pTagChara->info.m_CharaBasicInfo.m_ui8CharaSlotIdx == idx )
		{
			return i;
		}
	}

	return -1;
}

UINT8 CCharaInfoContext::find_slot_idx_by_DBIdx(T_ItemDBIdx chara_db_idx)
{
	const size_t size = m_vecChara.size();
	for(size_t i =0; i<size;++i)
	{
		T_ItemDBIdx chara_db = GetCharaPartsInfo(i)->GetPartsIdx(CHAR_EQUIPMENT_PARTS_CHARA);
		if( chara_db == chara_db_idx )
		{
			const tagChara* pTagChara = m_vecChara[i];

			return pTagChara->info.m_CharaBasicInfo.m_ui8CharaSlotIdx;
		}
	}

	return 0xFF;
}

void CCharaInfoContext::SetMainCharaIdx( TEAM_TYPE team, INT32 idx)
{
	if( g_pEnvSet->IsV2Version() )
	{
		if( team == TEAM_RED )
			m_nMainChara[TEAM_RED] = m_nMainChara[TEAM_BLUE] = idx;
	}
	else
	{
		m_nMainChara[team] = idx;
	}
}

void CCharaInfoContext::SetMainCharaIdx( INT32 idx)
{
	if( !g_pEnvSet->IsV2Version() )
	{
		const CHARA_PARTS_INFO * pPartsInfo = GetCharaPartsInfo(idx);	
		if( pPartsInfo )
		{
			SetTeamSlotByCharaID( pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA) );
		}
	}

	SetMainCharaIdx(m_eTeamSlot, idx);
}

CCharaInfoContext::tagChara*	CCharaInfoContext::SetChara( int nSlot )
{
	if( nSlot < 0 || GetHasCharacterCount() <= nSlot )
	{
		return nullptr;
	}

	return m_vecChara[ nSlot ];
}

const CCharaInfoContext::tagChara* CCharaInfoContext::GetChara( int nSlot ) const
{
	if( nSlot < 0 || GetHasCharacterCount() <= nSlot )
	{
		return nullptr;
	}

	return m_vecChara[ nSlot ];
}





//---------------------------------------------------------------------------------------------------------
//public
INT32	CCharaInfoContext::GetEmptyCharacterIdx() const
{
	for(size_t i =0; i<S2MO_MULTI_SLOT_COUNT;++i)
	{
		if( find_slot_idx( (UINT8)i ) == -1)
			return i;
	}

	return -1;
}

void CCharaInfoContext::SetMainCharaBySlotIdx(UINT8 slot_idx)
{
	INT32 list_idx = find_slot_idx( slot_idx );

	if( list_idx == -1 )
	{
		I3PRINTLOG(I3LOG_FATAL, "캐릭터 Slot 인덱스가 잘못됨");
		return;
	}

	SetMainCharaIdx(list_idx);
}

UINT8 CCharaInfoContext::GetMainCharaSlotIdx( TEAM_TYPE team /*= TEAM_NONE*/ ) const
{
	if( team == TEAM_NONE )
		team = m_eTeamSlot;

	if( m_vecChara.size() > 0)
	{
		I3_BOUNDCHK( m_nMainChara[team], (INT32)m_vecChara.size());
		if(GetChara( m_nMainChara[team]) == nullptr)
			return GetChara(static_cast<int>(team))->info.m_CharaBasicInfo.m_ui8CharaSlotIdx;
		else
			return GetChara(m_nMainChara[team])->info.m_CharaBasicInfo.m_ui8CharaSlotIdx;
	}
	else
		return 0;
}

T_ItemDBIdx CCharaInfoContext::GetMainCharaDBIdx(TEAM_TYPE team /*= TEAM_NONE*/) const
{
	if (team == TEAM_NONE)
		team = m_eTeamSlot;

	if (m_vecChara.size() > 0)
	{
		I3_BOUNDCHK(m_nMainChara[team], (INT32)m_vecChara.size());
		if (GetChara(m_nMainChara[team]) == nullptr)
			return GetChara(static_cast<int>(team))->info.m_CharaEquipInfo.m_CharPartsInfo.GetPartsInfo(CHAR_EQUIPMENT_PARTS_CHARA)->m_TItemDBIdx;
		else
			return GetChara(m_nMainChara[team])->info.m_CharaEquipInfo.m_CharPartsInfo.GetPartsInfo(CHAR_EQUIPMENT_PARTS_CHARA)->m_TItemDBIdx;
	}
	else
		return 0;
}

INT32 CCharaInfoContext::FindCharaSlotIdxByItemID(T_ItemID chara_id)
{
	const size_t num_vecChara = m_vecChara.size();

	for (size_t i = 0; i < num_vecChara; i++)
	{
		const CHARA_INFO& chara_info = m_vecChara[i]->info;
		// vv3 - check
		const CHARA_PARTS_INFO& parts_info = chara_info.m_CharaEquipInfo.m_CharPartsInfo;
		T_ItemID TCharaItemID = parts_info.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

		if (TCharaItemID == chara_id)
		{
			return (INT32)chara_info.m_CharaBasicInfo.m_ui8CharaSlotIdx;
		}
	}

	return -1;
}

UINT32 CCharaInfoContext::FindCharaSlotIdxByItemInfo(::ITEM_INFO chara_item_info) const
{
	const size_t num_vecChara = m_vecChara.size();

	for(size_t i=0; i< num_vecChara;++i)
	{
		const CHARA_INFO& chara_info = m_vecChara[i]->info;
		// vv3 - check
		const CHARA_PARTS_INFO& parts_info = chara_info.m_CharaEquipInfo.m_CharPartsInfo;
		T_ItemID TCharaItemID = parts_info.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

		//if( item_info == chara_item_info)
		if( (T_ItemDBIdx)i == chara_item_info.m_TItemDBIdx && TCharaItemID == chara_item_info.m_TItemID)
			return i;
	}

	return UINT_MAX;
}

void CCharaInfoContext::SetTeamSlotByCharaID(T_ItemID chara_id)
{
	if( !g_pEnvSet->IsV2Version() )
	{
		m_eTeamSlot = static_cast<TEAM_TYPE>( GET_ITEM_SUBTYPE(chara_id) - 1 );
	}
}

void CCharaInfoContext::SetTeamSlotBySlotIdx(INT32 slot_idx)
{
	if( !g_pEnvSet->IsV2Version() )
	{
		m_eTeamSlot = static_cast<TEAM_TYPE>( slot_idx % TEAM_COUNT );
	}
}

//CHARA_INFO* CCharaInfoContext::GetCharaInfo( int nSlot )
//{
//	tagChara* chara = GetChara( nSlot );
//
//	if( nullptr != chara )
//		return &chara->info;
//
//	return nullptr;
//}

void CCharaInfoContext::SetCharaInfo( int nSlot, CHARA_INFO * pInfo)
{
	tagChara * chara = SetChara( nSlot);
	if( chara != nullptr)
	{
		i3mem::Copy( &chara->info, pInfo, sizeof( CHARA_INFO));
	}
	m_bOneCheckLevel = true;
}

const CHARA_INFO*	CCharaInfoContext::GetCharaInfo( int nSlot ) const
{
	const tagChara* chara = GetChara( nSlot );

	if( nullptr != chara )
		return &chara->info;

	return nullptr;
}

CHARA_INFO * CCharaInfoContext::SetCharaInfo( INT32 nSlot)
{
	tagChara* chara = SetChara( nSlot );

	if( nullptr != chara )
		return &chara->info;

	return nullptr;
}

const CHARA_INFO*	CCharaInfoContext::GetCharaInfoByList( int idx ) const
{
	if(idx < 0 || m_vecChara.size() <= (UINT32)idx)
		return nullptr;

	const tagChara* chara = m_vecChara[idx];
	if(nullptr != chara)
		return &chara->info;

	return nullptr;
}

const CHARA_INFO*	CCharaInfoContext::GetCharaInfoBySlotIdx( UINT8 nSlot) const
{
	const size_t num_vecChara = m_vecChara.size();

	for( UINT32 i=0; i < num_vecChara; ++i)
	{
		const tagChara * pChara = m_vecChara[i];
		if( pChara->info.m_CharaBasicInfo.m_ui8CharaSlotIdx == nSlot)
			return &pChara->info;
	}
	return nullptr;
}




const CHARA_PARTS_INFO*		CCharaInfoContext::GetCharaPartsInfo( int nSlot ) const
{
	const CHARA_INFO* info = GetCharaInfo( nSlot );

	if( info )
		return &(info->m_CharaEquipInfo.m_CharPartsInfo);

	return nullptr;
}

const CHARA_PARTS_INFO*		CCharaInfoContext::GetNetCharaPartsInfo(int nNetSlot) const
{
	tagChara* chara = nullptr;

	for (UINT32 i = 0; i < m_vecChara.size(); i++)
	{
		tagChara * itr = m_vecChara[i];
		if( itr->info.m_CharaBasicInfo.m_ui8CharaSlotIdx == nNetSlot)
			chara = m_vecChara[i];
	}

	if (chara != nullptr )
	{
		const CHARA_INFO* info = &chara->info;

		if (info)
			return &(info->m_CharaEquipInfo.m_CharPartsInfo);
	}
		
	return nullptr;
}

const CHARA_PARTS_INFO * CCharaInfoContext::GetCharaPartsInfoBySlotIdx( UINT32 idx) const
{
	const CHARA_INFO *pInfo = GetCharaInfoBySlotIdx( static_cast<UINT8>(idx));
	if( pInfo != nullptr)
		return &pInfo->m_CharaEquipInfo.m_CharPartsInfo;
	return nullptr;
}

void CCharaInfoContext::SetCharaPartsInfo( INT32 nSlot, CHARA_PARTS_INFO * pPartsInfo)
{
	tagChara * chara = SetChara( nSlot);
	if( chara != nullptr)
	{
		pPartsInfo->CopyTo( &chara->info.getEquipInfo()->m_CharPartsInfo );
	}
}

const CHARA_WEAPON_INFO*	CCharaInfoContext::GetCharaWeaponInfo( int nSlot ) const
{
	const CHARA_INFO* info = GetCharaInfoBySlotIdx(static_cast<UINT8>(nSlot) );

	if( info )
		return &(info->m_CharaEquipInfo.m_CharWeaponInfo);

	return nullptr;
}

const CHARA_WEAPON_INFO*	CCharaInfoContext::GetNetCharaWeaponInfo(int nNetSlot) const
{
	tagChara* chara = nullptr;

	for (UINT32 i = 0; i < m_vecChara.size(); i++)
	{
		tagChara * itr = m_vecChara[i];
		if (itr->info.m_CharaBasicInfo.m_ui8CharaSlotIdx == nNetSlot)
			chara = m_vecChara[i];
	}

	if (chara != nullptr)
	{
		const CHARA_INFO* info = &chara->info;

		if (info)
			return &(info->m_CharaEquipInfo.m_CharWeaponInfo);
	}

	return nullptr;
}

const CHARA_WEAPON_INFO* CCharaInfoContext::GetCharaWeaponInfoByIdx( UINT32 idx) const
{
	const CHARA_INFO * pInfo = GetCharaInfoBySlotIdx( static_cast<UINT8>(idx) );
	if( pInfo != nullptr)
		return &pInfo->m_CharaEquipInfo.m_CharWeaponInfo;
	return nullptr;
}

void CCharaInfoContext::SetCharaWeaponInfo( INT32 nSlot, CHARA_WEAPON_INFO * pWeaponInfo)
{
	tagChara * chara = SetChara( nSlot);
	if( chara != nullptr)
	{
		pWeaponInfo->CopyTo( &chara->info.getEquipInfo()->m_CharWeaponInfo);
	}
}

void CCharaInfoContext::SetCharaEquipInfo( INT32 nSlot, ITEM_INFO* pCharEquipment )
{
	//tagChara * chara = SetChara( nSlot);
	tagChara * chara = nullptr;
	for (UINT32 i = 0; i < m_vecChara.size(); i++)
	{
		tagChara * itr = m_vecChara[i];
		if (itr->info.m_CharaBasicInfo.m_ui8CharaSlotIdx == nSlot)
			chara = m_vecChara[i];
	}
	if( chara == nullptr)	return;

	CHARA_WEAPON_INFO* pCharWeaponInfo	= &chara->info.getEquipInfo()->m_CharWeaponInfo;
	CHARA_PARTS_INFO* pCharPartsInfo	= &chara->info.getEquipInfo()->m_CharPartsInfo;

	i3mem::Copy( pCharWeaponInfo->m_Info, pCharEquipment, sizeof(ITEM_INFO) * CHAR_EQUIPMENT_WEAPON_COUNT );

	// vv3 - check
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++i )
	{
		pCharPartsInfo->SetItemInfo( (CHAR_EQUIPMENT_PARTS)i,		pCharEquipment[ i + 5 ] );
	} 
}


bool	CCharaInfoContext::AddCharacterInfo( const CHARA_INFO& info )
{
	tagChara* pTagChara = new tagChara(info);

	m_vecChara.push_back(pTagChara);
	return true;
}

INT32 CCharaInfoContext::FindCharaInfoIndexByItemID( T_ItemID itemID)
{
	INT32 i = 0;

	for each( const tagChara* pChara in m_vecChara )
	{
		// vv3 - check
		const CHARA_INFO& info = pChara->info;
		T_ItemID chara_id = info.m_CharaEquipInfo.m_CharPartsInfo.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
		//2014.07.17 ENG팀 김대영
		//아이템 아이디로 삭제할 캐릭터를 고르기 때문에 동일한 캐릭터 아이템이 2개 이상일때 착용 중인 아이템을
		//선택해서 주기 때문에 조건 추가.
		if( chara_id == itemID 
			&& info.m_CharaBasicInfo.m_ui8CharaSlotIdx != GetMainCharaSlotIdx(TEAM_RED) 
			&& info.m_CharaBasicInfo.m_ui8CharaSlotIdx != GetMainCharaSlotIdx(TEAM_BLUE)	)
		{
			return i;
		}

		i++;
	}

	return -1;
}


void CCharaInfoContext::DelCharacterInfo(size_t idx)
{
	INT32 list_idx = find_slot_idx( (UINT8)idx );

	if( list_idx == -1 )
	{
		I3PRINTLOG(I3LOG_FATAL, "캐릭터 삭제 인덱스가 잘못됨");
		return;
	}

	// 메인캐릭터의 슬롯을 다시 설정해야 한다.
	UINT8 main_chara_slot[TEAM_COUNT];
	
	for( INT32 i = 0 ; i < TEAM_COUNT ; ++i)
		main_chara_slot[i] = GetMainCharaSlotIdx( static_cast<TEAM_TYPE>(i) );

	delete m_vecChara[list_idx];
	m_vecChara.erase( m_vecChara.begin() + list_idx);

	for( INT32 i = 0 ; i < TEAM_COUNT ; ++i)
		SetMainCharaBySlotIdx( main_chara_slot[i]);
}

void CCharaInfoContext::SortCharacterInfo()
{
	if (m_vecChara.size() < TEAM_COUNT) return;

	UINT8 main_chara_slot[TEAM_COUNT] = { 0, };
	for (INT32 i = 0; i < TEAM_COUNT; ++i)
		main_chara_slot[i] = GetMainCharaSlotIdx(static_cast<TEAM_TYPE>(i));

	struct slotsort
	{
		bool operator()(const tagChara* lhs, const tagChara* rhs) const
		{
			UINT8 lhsSlotIdx = const_cast<tagChara*>(lhs)->info.getBasicInfo()->m_ui8CharaSlotIdx;
			UINT8 rhsSlotIdx = const_cast<tagChara*>(rhs)->info.getBasicInfo()->m_ui8CharaSlotIdx;
			return (lhsSlotIdx < rhsSlotIdx);
		}
	};
	i3::sort(m_vecChara.begin(), m_vecChara.end(), slotsort());
	
	for (INT32 i = 0; i < TEAM_COUNT; ++i)
		SetMainCharaBySlotIdx(main_chara_slot[i]);
}


const CHARA_SLOT_SKILL*	CCharaInfoContext::GetCharaSkill( int nSlot) const
{
	CHARA_INFO* info = (CHARA_INFO*)GetCharaInfo( nSlot );

	if( info )
		return info->getSkillInfo();

	return nullptr;
}
























INT32 CCharaInfoContext::GetEmptyPlayType() const
{
	for( int i = 0; i < S2MO_MULTI_SLOT_COUNT; ++i )
	{	
		if( m_CharaSlotState[i] == CHARA_STATE_NONE )
		{
			return i;
		}
	}

	return -1;
}

const TTCHAR*		CCharaInfoContext::GetNickName( int nSlot ) const
{
	const tagChara* chara = GetChara( nSlot );

	if( chara )
	{
		if( chara->changed )
			return chara->changedNick;
		else
			return chara->info.m_CharaBasicInfo.m_strCharaName;
	}

	return _TT("");
}

const TTCHAR* CCharaInfoContext::GetNickName( ::ITEM_INFO chara_item_info ) const
{
	UINT32 idx  = FindCharaSlotIdxByItemInfo(chara_item_info);
	if( idx == UINT_MAX )
	{
		return _TT("multislot no name");
	}

	return GetNickName(idx);
}

UINT32	CCharaInfoContext::GetCreateDate( int nSlot ) const
{
	const tagChara* chara = GetChara( nSlot );

	if( chara )
		return chara->info.m_CharaBasicInfo.m_ui32CreateDate;

	return 0;
}

int		CCharaInfoContext::GetMultiSlotID( int nMultiSlot ) const
{
	if( nMultiSlot < 0 || S2MO_MULTI_SLOT_COUNT <= nMultiSlot )
	{
		return -1;
	}

	return m_CharaSlotState[ nMultiSlot ];
}

bool	CCharaInfoContext::IsChangedNickName() const
{
	for each( const tagChara* chara in m_vecChara )
	{
		if( chara->changed )
		{
			return true;
		}
	}

	return false;
}

bool	CCharaInfoContext::SetNickName( int nSlot, const TTCHAR* name )
{
	tagChara* chara = SetChara( nSlot );

	if( chara )
	{
		const TTCHAR* prevNick = chara->info.m_CharaBasicInfo.m_strCharaName;

		if(  i3::generic_is_iequal( prevNick, name ) )
		{
			chara->changed = false;
		}
		else
		{
			chara->changed = true;
			i3::safe_string_copy( chara->changedNick, name, NET_CHARA_NICK_NAME_SIZE );
		}

		return true;
	}

	return false;
}


void	CCharaInfoContext::SetChangeCharaNickName( int arg )
{
	if( false == IsChangedNickName() )
	{
		return;
	}

	for( UINT32 i = 0; i < m_vecChara.size(); i++)
	{
		tagChara * itr = m_vecChara[i];
		if( itr->changed )
		{
			if( EV_SUCCESSED( arg ) )
				i3::safe_string_copy( itr->info.m_CharaBasicInfo.m_strCharaName, itr->changedNick, NET_CHARA_NICK_NAME_SIZE );

			itr->changed = false;
			::memset( itr->changedNick, 0, sizeof(itr->changedNick) );
		}
	}
}

void	CCharaInfoContext::SetChangeMultiSlotIdx()
{
	const int nCnt = GetHasCharacterCount();

	for( int nSlot = 0; nSlot < nCnt; ++nSlot )
	{
		tagChara * chara = SetChara( nSlot);
		if( chara == nullptr) continue;

		chara->info.m_CharaBasicInfo.m_ui8CharaState = CHARA_STATE_CREATED;
	}
}

void	CCharaInfoContext::GetChangeCharaNickNameData( PACKET_H_NEW_CHANGE_CHARA_NICK_GROUP_REQ* header, PACKET_D_NEW_CHANGE_CHARA_NICK_GROUP* data ) const
{
	int nCnt = 0;
	for each( const tagChara* chara in m_vecChara )
	{
		if( chara->changed )
		{
			if( data )
			{
				data[ nCnt ].m_ChangeCharaNick.m_ui8CharaSlotIdx = (UINT8) chara->info.m_CharaBasicInfo.m_ui8CharaSlotIdx;
				i3::safe_string_copy( data[ nCnt ].m_ChangeCharaNick.m_strNewCharaName, chara->changedNick, NET_CHARA_NICK_NAME_SIZE );
			}

			++nCnt;
		}
	}

	if( header )
		header->m_ui8CharaSlotCount = (UINT8) nCnt;
}

void	CCharaInfoContext::GetCurrentSlotWeaponInfo( ITEM_INFO * pWeaponList)
{
	const CHARA_WEAPON_INFO * pWeaponInfo = GetCharaWeaponInfo(0);
	if(pWeaponInfo != nullptr)
		::memcpy(&pWeaponList[0], pWeaponInfo, sizeof(ITEM_INFO) * 5);
}

void CCharaInfoContext::GetNetCharaInfo( NET_CHARA_INFO * pOut )
{
	// My Team
	const CHARA_INFO * pCharaInfo = GetMainCharaInfo();
	if( pCharaInfo != nullptr)
	{
		pOut->setCharaParts( &pCharaInfo->m_CharaEquipInfo.m_CharPartsInfo);
		pOut->SetWeaponList( &pCharaInfo->m_CharaEquipInfo.m_CharWeaponInfo);
		pOut->setCommonParts( &m_common.parts );
	}
}

void CCharaInfoContext::GetUICharaInfo( UI_CHARA_INFO * pOut)
{
	const CHARA_INFO * pRedCharaInfo = GetMainCharaInfo(TEAM_RED);
	const CHARA_INFO * pBlueCharaInfo = GetMainCharaInfo(TEAM_BLUE);

	if( pRedCharaInfo && pBlueCharaInfo)
	{
		pOut->setCharaParts( &pRedCharaInfo->m_CharaEquipInfo.m_CharPartsInfo, TEAM_RED);
		pOut->setCharaParts( &pBlueCharaInfo->m_CharaEquipInfo.m_CharPartsInfo, TEAM_BLUE);
		pOut->SetWeaponList( &pRedCharaInfo->m_CharaEquipInfo.m_CharWeaponInfo);
		pOut->setCommonParts( &m_common.parts );
	}
}

void	CCharaInfoContext::SetNextChara( bool bRight )
{
	const int nCnt = GetHasCharacterCount();	

	if( nCnt <= 1 )
		return;

	if( bRight )
	{
		eCharaState nFirst	= m_CharaSlotState[ 0 ];

		for( int i = 0; i < (nCnt -1); ++i )
		{
			m_CharaSlotState[ i ] = m_CharaSlotState[ i + 1 ];
		}

		m_CharaSlotState[ nCnt -1 ] = nFirst;
	}
	else
	{
		eCharaState nLast = m_CharaSlotState[ nCnt - 1 ];

		for( int i = (nCnt -1); 0 < i ; --i )
		{
			m_CharaSlotState[ i ] = m_CharaSlotState[ i -1 ];
		}
		m_CharaSlotState[ 0 ] = nLast;
	}

	SetChangeMultiSlotIdx();
}

void	CCharaInfoContext::ClearAll()
{
	i3::cu::for_each_delete_clear(m_vecChara);
}

bool	CCharaInfoContext::CalculateMultiSlotIdx()
{
	for( int i = 0; i < S2MO_MULTI_SLOT_COUNT; ++i )
	{
		m_CharaSlotState[ i ] = CHARA_STATE_NONE;
	}

	if (m_vecChara.empty())
	{
#if defined(I3_DEBUG)
		I3PRINTLOG(I3LOG_NOTICE, "계정 생성 단계 / 아직 기본 캐릭터 배정이 안된 상태 m_vecChara.empty()");
#endif
		return false;
	}
		
	for( INT32 team = 0 ; team < TEAM_COUNT ; ++team)
	{
		INT32 idx = 0;

		for each( const tagChara* chara in m_vecChara )
		{
			const CHARA_INFO & info = chara->info;
			m_CharaSlotState[ idx ] = (eCharaState)info.m_CharaBasicInfo.m_ui8CharaState;

			const CHARA_BASIC_INFO& basic = info.m_CharaBasicInfo;

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			I3PRINTLOG(I3LOG_NOTICE, "CCharaInfoContext::CalculateMultiSlotIdx() --- Team : %d , m_vecChara[%d], basic.m_uiCharaSlotIdx = %d, m_mMainChara[team] = %d", team, idx, basic.m_ui8CharaSlotIdx, m_nMainChara[team]);
#endif

			if( basic.m_ui8CharaSlotIdx == m_nMainChara[team] )
				SetMainCharaIdx(static_cast<TEAM_TYPE>(team), idx);

			idx++;

			if (idx >= S2MO_MULTI_SLOT_COUNT )		// m_CharaSlotState 배열크기를 넘어서면 곤란하기 때문에 일단 이렇게 임시로 처리..(2013.03.05.수빈)
				break;
		}

		// 아직 캐릭터를 생성하지 않은 경우
	//	if( m_nMainChara[team] == 255)
	//		SetMainCharaIdx(static_cast<TEAM_TYPE>(team), team);
		


		if ((INT32)m_vecChara.size() <= m_nMainChara[team])
			//SetMainCharaIdx(static_cast<TEAM_TYPE>(team), team);
			return false;
	}

	I3TRACE("<<<<<<<<<<<<<<<< MultiSlot Character Dump <<<<<<<<<<<<<<<<\n");
	const size_t Count = m_vecChara.size();
	for(size_t i = 0 ; i < Count ; i++)
	{
		const CHARA_INFO& chara_info = m_vecChara[i]->info;
		const CHARA_BASIC_INFO& basic = chara_info.m_CharaBasicInfo;
	
		// vv3 - check
		const CHARA_PARTS_INFO& parts = chara_info.m_CharaEquipInfo.m_CharPartsInfo;
		T_ItemID item_id = parts.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
		T_ItemDBIdx db_idx = parts.GetPartsIdx(CHAR_EQUIPMENT_PARTS_CHARA);

		char str[256] = "";
		i3::snprintf(str, 256, "Idx = %03d, SlotNum = %03d, ItemId = %03d, Idx = %03d \n", i, 
			basic.m_ui8CharaSlotIdx, item_id, db_idx);

		I3TRACE( str);
	}
	I3TRACE(">>>>>>>>>>>>>>>> MultiSlot Character Dump >>>>>>>>>>>>>>>>\n");
	I3TRACE("\n");

	return true;
}

void	CCharaInfoContext::SetAllCharaData(void)
{
	INT32				iskillsize[3];
	NCHARA_SKILL *		pSkillSet = nullptr;
	INT32				iCharaCount,iRank;
	CHARA_SLOT_SKILL	Classtype;

	//유저 정보 계급등
	USER_INFO_BASIC infoBasic;							
	UserInfoContext::i()->GetMyUserInfoBasic( &infoBasic );

	//캐릭터 카운터
	iCharaCount = CCharaInfoContext::i()->GetHasCharacterCount();	
	//계급
	iRank = infoBasic.m_ui32Rank;													

	iskillsize[0] = MAX_COMMON_SKILL;
	iskillsize[1] = MAX_MAIN_SKILL;
	iskillsize[2] = MAX_ASSIST_SKILL;

	//캐릭터 기본 정보 저장합니다. 
	CCharaInfoContext::i()->ResetCheckSkillLevel();
	for(INT32 h = 0; h < iCharaCount; h ++)
	{
		CCharaInfoContext::STCLevel TempLevel;
		//스킬정보
		const CHARA_INFO*  charainfo = CCharaInfoContext::i()->GetCharaInfo(h);

		//데이터 기본 정보 저장합니다, 		//스킬정보
		TempLevel.rPlayTime = charainfo->m_ui32SkillRelTime / (REAL32)3600;			//플레이시간
		Classtype = charainfo->m_CharaSkillInfo;	//스킬 정보 받아오기


		for(INT i = 0; i < CATEGORYLIST_MAX; i++)
		{
			for( INT32 j = 0; j < iskillsize[i]; j++)
			{
				INT32	_level;
				pSkillSet = g_pSkillSystemValueSet->getSkillsystemSet( (SKILL_CATEGORYLIST)i, j);

				if(pSkillSet == nullptr)
					continue;

				//상승조건
				if( pSkillSet->getApplyCategory() == CATEGORYLIST_UNKNOWN)
					continue;

				if(i == CATEGORYLIST_MAIN)
				{
					if(pSkillSet->isWeaponType( Classtype.m_PrimarySkill) == false)
						continue;
				}

				if( pSkillSet->getApplyRequisiteType() == REQUISTE_CLASS)
				{
					_level =pSkillSet->getApplyRequisiteLevel(iRank);
				}
				else
				{
					_level =pSkillSet->getApplyRequisiteLevel(static_cast<INT32>(TempLevel.rPlayTime));
				}
				TempLevel.SlotLevel[i].push_back(_level);
			}
		}
		AddCheckSkillLevel(&TempLevel);
	}
}

bool	CCharaInfoContext::CheckAllCharaData(void)
{
	INT32				iskillsize[3];
	NCHARA_SKILL *		pSkillSet = nullptr;
	INT32				iCharaCount,iRank;
	CHARA_SLOT_SKILL	Classtype;

	//유저 정보 계급등
	USER_INFO_BASIC infoBasic;							
	UserInfoContext::i()->GetMyUserInfoBasic( &infoBasic );

	//캐릭터 카운터
	iCharaCount = CCharaInfoContext::i()->GetHasCharacterCount();	
	//계급
	iRank = infoBasic.m_ui32Rank;		
	m_bOneCheckLevel = false;

	iskillsize[0] = MAX_COMMON_SKILL;
	iskillsize[1] = MAX_MAIN_SKILL;
	iskillsize[2] = MAX_ASSIST_SKILL;

	//캐릭터 기본 정보 저장합니다. 
	for(INT32 h = 0; h < iCharaCount; h ++)
	{
		CCharaInfoContext::STCLevel TempLevel;
		//스킬정보
		const CHARA_INFO*  charainfo = CCharaInfoContext::i()->GetCharaInfo(h);

		//데이터 기본 정보 저장합니다, 		//스킬정보
		TempLevel.rPlayTime = charainfo->m_ui32SkillRelTime / (REAL32)3600;			//플레이시간
		Classtype = charainfo->m_CharaSkillInfo;	//스킬 정보 받아오기


		for(INT i = 0; i < CATEGORYLIST_MAX; i++)
		{
			INT32 icount = 0;
			for( INT32 j = 0; j < iskillsize[i]; j++)
			{
				INT32	_level;
				pSkillSet = g_pSkillSystemValueSet->getSkillsystemSet( (SKILL_CATEGORYLIST)i, j);

				if(pSkillSet == nullptr)
					continue;

				//상승조건
				if( pSkillSet->getApplyCategory() == CATEGORYLIST_UNKNOWN)
					continue;

				if(i == CATEGORYLIST_MAIN)
				{
					if(pSkillSet->isWeaponType( Classtype.m_PrimarySkill) == false)
						continue;
				}

				if( pSkillSet->getApplyRequisiteType() == REQUISTE_CLASS)
				{
					_level =pSkillSet->getApplyRequisiteLevel(iRank);
				}
				else
				{
					_level =pSkillSet->getApplyRequisiteLevel(static_cast<INT32>(TempLevel.rPlayTime));
				}

				INT32 tlevel = m_Checklevel.at(h).SlotLevel[i].at(icount);
				if( _level > tlevel )
				{
					return true;
				}
				icount++;
			}
		}
	}
	return false;
}

bool	CCharaInfoContext::isUsedCharacter(INT32 flag)
{
	//<캐릭터> 구조가 상이하게 다릅니다. 일단, 자신이 메인케릭터로 설정한 캐릭터에 대해서 검사하는걸로 상정하겠습니다.
	for( INT32 team = 0 ; team < TEAM_COUNT ; ++team)
	{
		const CHARA_PARTS_INFO* info = GetMainCharaPartsInfo( static_cast<TEAM_TYPE>(team) );

		for( int parts_type = 0; parts_type < CHAR_EQUIPMENT_PARTS_COUNT; parts_type++)
		{
			if( (info->GetPartsItemID((CHAR_EQUIPMENT_PARTS)parts_type)) == (T_ItemID)flag )
				return true;
		}
	}

	return false;
}