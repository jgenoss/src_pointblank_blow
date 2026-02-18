#include "pch.h"
#include "UICharaView.h"

#include "UIMainFrame.h"
#include "../GameCharaActionContext.h"
#include "../GameCharaAnimContext.h"
#include "../GameCharaWeaponContext.h"
#include "CommunityContext.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameContextUtil.h"
#include "UICharaInfoMgr.h"
#include "UIUtil.h"
#include "../Weapon/WeaponItemID.h"

/************************************************************************/
/*							UICharaView Class							*/
/************************************************************************/

I3_CLASS_INSTANCE( UICharaView);

UICharaView::UICharaView() : m_pCharacter(nullptr), m_bFirstUpdate(true)
	, m_ui8RecreateFlag(0), m_bChangeAction(false), m_ChangingWeaponSlot(WEAPON_SLOT_UNKNOWN)
	, m_ui32CurViewItemID(0), m_bIsSameWeapon(false), m_bMyCharaView(true)
	, m_iIndex(0)
{
	m_viewCharaInfo._charaInfo.SetType( CHARA::TYPE_PLAYER_NET);
	m_viewCharaInfo._charaInfo.SetHP(100.f);
}

UICharaView::~UICharaView()
{
	RemoveViewChara();
}

bool UICharaView::_RecreateSlotChara( const CHARA_PARTS_INFO * pPartsInfo, TEAM_TYPE team)
{
	bool bChanged = false;
	I3ASSERT( pPartsInfo != nullptr);

	m_viewCharaInfo._charaInfo.SetType( CHARA::TYPE_PLAYER_NET);
	m_viewCharaInfo._charaInfo.SetHP(100.f);

	//	리소스와 팀을 설정
	if( getTeam() != team)
	{
		setTeam( team);
		bChanged = true;
	}
	
	if( m_viewCharaInfo._charaInfo.GetParts()->IsSameBody( pPartsInfo) == false )
	{
		pPartsInfo->CopyTo( m_viewCharaInfo._charaInfo.SetParts());
		bChanged = true;		
	}

	return bChanged;
}

bool UICharaView::_CheckEquip( const CHARA_PARTS_INFO * pParts)
{	
	// vv3 - check
	if( pParts->GetPartsItemID(CHAR_EQUIPMENT_PARTS_SKIN) > 0)
	{
		return false;
	}

	bool bRecreate = false;

	// vv3 - check
	for( INT32 idx = 0 ; idx < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx )
	{
		switch( idx )
		{
		case CHAR_EQUIPMENT_PARTS_HEAD		:
		case CHAR_EQUIPMENT_PARTS_FACE		:
		case CHAR_EQUIPMENT_PARTS_BELT		:
		case CHAR_EQUIPMENT_PARTS_HOLSTER	:
		case CHAR_EQUIPMENT_PARTS_BERET		:
			{
				T_ItemID cur_item_id = pParts->GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx) );
				T_ItemID old_item_id = m_viewCharaInfo._charaInfo.GetParts()->GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx) );

				if( g_pEnvSet->IsV2Version() )
				{
					// V2 버전은 베레모를 제외한 파츠에 값이 있어야한다
					if( idx != CHAR_EQUIPMENT_PARTS_BERET && cur_item_id == 0 )
						continue;
				}
				else
				{
					// V1 버전은 베레모/헤드기어를 제외한 파츠에 값이 있어야한다.
					if( idx != CHAR_EQUIPMENT_PARTS_HEAD && idx != CHAR_EQUIPMENT_PARTS_BERET && cur_item_id == 0)
						continue;
				}

				if( cur_item_id != old_item_id )
				{
					bRecreate = true;
					m_viewCharaInfo._charaInfo.SetParts()->m_Info[idx] = pParts->m_Info[idx];
				}
			}
			break;
		}
	}
	
	return bRecreate;
}

bool UICharaView::_CheckWeapon( const UI_CHARA_INFO * pUICharaInfo, WEAPON_SLOT_TYPE mustUpdateSlot, TEAM_TYPE team)
{
 	bool bRecreate = false;

	ITEM_INFO weapons[ WEAPON_SLOT_COUNT];

	for(INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		weapons[i].m_TItemID = pUICharaInfo->getWeaponItemID( (WEAPON_SLOT_TYPE)i);
	}

	if( m_bMyCharaView )
		GameContextUtil::CheckWeaponList( weapons );

	//	weapons
	for(INT32 i = 0 ; i <= MAX_WEAPON_CHECK_SLOT_COUNT ; ++i)
	{
		if( MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
		{
			if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) )
			{
				if( i == WEAPON_SLOT_MELEE )
				{
					WEAPON_CLASS_TYPE classType = WEAPON_CLASS_KNIFE;
					WEAPON::RES_ID iWeaponNumber = WEAPON::ItemID2ResID( weapons[i].m_TItemID);

					if( iWeaponNumber != WEAPON::getItemIndex(WEAPON::CHINESECLEAVER))
					{
						if( team ==  TEAM_RED)
							iWeaponNumber = WEAPON::getItemIndex(WEAPON::HAIRDRYER);
						else
							iWeaponNumber = WEAPON::getItemIndex(WEAPON::FRYINGPAN);
					}

					weapons[i].m_TItemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE,	classType, iWeaponNumber);
				}
				else
					continue;
			}
			else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY))	//겨울 탈출 모드
			{
				WEAPON::RES_ID iWeaponNumber = WEAPON::ItemID2ResID( weapons[i].m_TItemID);

				if( i == WEAPON_SLOT_PRIMARY )
				{
					if( iWeaponNumber != WEAPON::getItemIndex(WEAPON::SNOW_BALL_BLASTER) )
					{
						iWeaponNumber = WEAPON::getItemIndex(WEAPON::SNOW_BALL_BLASTER);
					}

					weapons[i].m_TItemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY,	WEAPON_CLASS_ASSAULT, iWeaponNumber);
				}
				else if( i == WEAPON_SLOT_SECONDARY )
				{
					if( iWeaponNumber != WEAPON::getItemIndex(WEAPON::HANDGUN_MGL32) )
					{
						iWeaponNumber = WEAPON::getItemIndex(WEAPON::HANDGUN_MGL32);
					}

					weapons[i].m_TItemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY,	WEAPON_CLASS_HANDGUN, iWeaponNumber);
				}
				else if( i == WEAPON_SLOT_MELEE )
				{
					if( iWeaponNumber != WEAPON::getItemIndex(WEAPON::KNIFE_ICICLE)
						&& iWeaponNumber != WEAPON::getItemIndex(WEAPON::KNIFE_ICE_FORK))
					{
						iWeaponNumber = WEAPON::getItemIndex(WEAPON::KNIFE_ICICLE);
					}

					weapons[i].m_TItemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE,	WEAPON_CLASS_KNIFE, iWeaponNumber);
				}
				else
					continue;
			}
		}
		else if (MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_KNUCKLE_ONLY))
		{
			if (i == WEAPON_SLOT_MELEE)
			{
				if (!CInvenList::i()->HaveItemBySubtype(INVEN::DB_WEAPON, WEAPON_CLASS_KNUCKLE) )
				{
					WEAPON::RES_ID iWeaponNumber = WEAPON::getItemIndex(WEAPON::BAREFIST_IN_MODE);

					weapons[i].m_TItemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_KNUCKLE, iWeaponNumber);
				}
				else
				{
					INT32 nKnuckleFlag = g_pFramework->GetAvailableWeaponFlagInMyInven(WEAPON_CLASS_KNUCKLE, true, true);
					if (nKnuckleFlag == 0)
					{
						WEAPON::RES_ID iWeaponNumber = WEAPON::getItemIndex(WEAPON::BAREFIST_IN_MODE);

						weapons[i].m_TItemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_KNUCKLE, iWeaponNumber);
					}
				}
			}
		}

		//	네트워크 무기 정보와 배경 캐릭터 무기 정보가 다를 경우 싱크를 맞춘다.
		if( (weapons[i].m_TItemID != 0) && (weapons[i].m_TItemID != m_viewCharaInfo._weapon[i]))
		{
			bRecreate = true;
			m_viewCharaInfo._weapon[i] = weapons[i].m_TItemID;

			m_ChangingWeaponSlot = static_cast<WEAPON_SLOT_TYPE>(i);
		}
	}
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if( pFrame->GetCurrentSubPhaseType() == USP_INVENTORY)
	{
		CharaInfoMgr::CurrentState * state = (CharaInfoMgr::CurrentState *)CharaViewMgr::i()->GetParam();
		
		if(state != nullptr)
		{
			bRecreate = true;
			m_ChangingWeaponSlot = static_cast<WEAPON_SLOT_TYPE>(state->slot_idx);
			CharaViewMgr::i()->SetParam(0);
		}
	}
	
	// 현재 CharaView에 보여지는 무기가 이전 무기와 다른경우에 Flag를 false로 변경
	m_bIsSameWeapon = true;

	if( m_ChangingWeaponSlot > -1 && m_ui32CurViewItemID != m_viewCharaInfo._weapon[m_ChangingWeaponSlot])
	{
		m_ui32CurViewItemID = m_viewCharaInfo._weapon[m_ChangingWeaponSlot];
		m_bIsSameWeapon = false;
	}

	// 하지만 무기 제한이 있다면 제한시키자
	if( g_pFramework->GetUIMainframe()->GetCurrentPhaseType() == UIPHASE_READY_ROOM &&
		g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() == USP_NONE)
	{
		ROOM_INFO_BASIC Info;

		m_ChangingWeaponSlot = WEAPON_SLOT_PRIMARY;
		
		MyRoomInfoContext::i()->getMyRoomInfo(&Info);

		if( TESTBIT( Info._WeaponFlag, LOCK_PRIMARY) != LOCK_PRIMARY )
		{
			bRecreate = true;
			m_viewCharaInfo._weapon[ WEAPON_SLOT_PRIMARY] = 0;
			mustUpdateSlot = m_ChangingWeaponSlot = WEAPON_SLOT_SECONDARY;	// 보조 무기로 변경을 시도
		}

		if( TESTBIT(Info._WeaponFlag, LOCK_SECONDARY) != LOCK_SECONDARY )
		{
			bRecreate = true;
			m_viewCharaInfo._weapon[ WEAPON_SLOT_SECONDARY] = 0;

			if( m_viewCharaInfo._weapon[ WEAPON_SLOT_PRIMARY] == 0)	// 주무기 확인
				mustUpdateSlot = m_ChangingWeaponSlot = WEAPON_SLOT_MELEE;	// 없으면 근접무기로 변경
			else
				mustUpdateSlot = m_ChangingWeaponSlot = WEAPON_SLOT_PRIMARY;
		}

		if( TESTBIT(Info._WeaponFlag, LOCK_THROW) != LOCK_THROW )
		{
			bRecreate = true;
			m_viewCharaInfo._weapon[ WEAPON_SLOT_THROWING1] = 0;
		}	
	}

	// 반드시 설정하는 슬롯
	if( mustUpdateSlot != WEAPON_SLOT_UNKNOWN)
	{
		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) 
			&& MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
		{
			if( m_ChangingWeaponSlot != WEAPON_SLOT_MELEE)
			{
				m_ChangingWeaponSlot = WEAPON_SLOT_MELEE;
				bRecreate = true;
			}
		}
		else if( m_ChangingWeaponSlot != mustUpdateSlot)
		{
			m_ChangingWeaponSlot = mustUpdateSlot;
			bRecreate = true;
		}
	}

	return bRecreate;
}

INT32 UICharaView::_CheckLimitWeapon( void)
{
	INT32 i;

	ROOM_INFO_BASIC roomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

	INT32 selectUsage = m_viewCharaInfo._weaponSlot;

	{
		for( i = 0; i < 4; i++)
		{
			if (TESTBIT(roomInfo._WeaponFlag, LOCK_PRIMARY >> i))
			{
				selectUsage = i;
				break;
			}
		}

		//	레디룸에서 무기 제한이 걸렸다면 무기 슬롯을 변경
		{
			UIPHASE phase = g_pFramework->GetUIMainframe()->GetCurrentPhaseType();
			UISUBPHASE sub_phase = g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType();

			if( phase == UIPHASE_READY_ROOM && sub_phase == USP_NONE )
			{
				m_pCharacter->getWeaponContext()->CreateWeaponByGameMode( roomInfo._WeaponFlag);
			}
		}

		{
			// 무기를 변경하고 변경된 무기 슬롯을 받는다.
			if( m_pCharacter->getWeaponContext() != nullptr)
			{
				selectUsage = m_pCharacter->getWeaponContext()->ChangeToEquipedWeapon( m_iIndex, roomInfo._WeaponFlag);	
			}

			if( selectUsage >= 0)
				m_bChangeAction = true;
		}	
	}

	return selectUsage;
}

void UICharaView::_RecreateWeapon( void)
{
	if( m_pCharacter->getWeaponContext() != nullptr)
	{
		if( m_pCharacter->getWeaponContext()->getCurrentSlotNum() != m_ChangingWeaponSlot && m_ChangingWeaponSlot != -1)
		{
			_addRecreateFlag(CHARAVIEW_RECREATE_WEAPON);
		}
	}

	if( _isRecreateFlag(CHARAVIEW_RECREATE_WEAPON)  && (m_ChangingWeaponSlot != -1)
		&& !_isRecreateFlag(CHARAVIEW_RECREATE_CHARA) )
	{
		i3GameResChara * pRes = g_pWeaponManager->QueryResource(m_viewCharaInfo._weapon[ m_ChangingWeaponSlot]);

		if((pRes != nullptr) && (m_viewCharaInfo._weapon[m_ChangingWeaponSlot] != 0))
		{
			((Avatar*)m_pCharacter)->SetWeapon( m_ChangingWeaponSlot, m_viewCharaInfo._weapon[ m_ChangingWeaponSlot], m_bIsSameWeapon);
		}

		m_ChangingWeaponSlot = WEAPON_SLOT_UNKNOWN;
	}
}

void UICharaView::_Reaction( void)
{
	if( m_bChangeAction )
	{
		WeaponBase * pWeapon = m_pCharacter->GetCurrentCharaWeapon();
		if( pWeapon != nullptr)
		{
			if( i3::same_of<WeaponKrissSuperV*>( pWeapon))
			{
				((WeaponKrissSuperV*)pWeapon)->SetEnableDualWeapon( true, false);
			}
			else if( i3::same_of<WeaponShotGunGenderWithDual*>(pWeapon))
			{
				((WeaponShotGunGenderWithDual*)pWeapon)->SetEnableDualWeapon( true, false);
			}
		}

		m_pCharacter->Cmd_Profile_Idle_Stand();
		//m_pCharacter->Cmd_Profile_Idle_Change(false);
	}
}

bool UICharaView::_RecreateOtherWeapon(VIEW_CHARA_INFO * pNewCharaInfo)
{
	bool bRecreate = false;

	//	weapons
	for(INT32 i=0; i<WEAPON_SLOT_COUNT; i++)
	{
		if( pNewCharaInfo->_weapon[i] != 0 && pNewCharaInfo->_weapon[i] != m_viewCharaInfo._weapon[i] )
		{
			bRecreate = true;

			m_viewCharaInfo._weapon[i] = pNewCharaInfo->_weapon[i];
		}
	}

	return bRecreate;
}

void UICharaView::RemoveViewChara( bool bRemoveInstance)
{
	if( m_pCharacter != nullptr)
	{
		g_pFramework->GetUIMainframe()->RemoveChild(m_pCharacter);
		I3TRACE( "UI Chara return instance %d\n", m_pCharacter->GetRefCount());


		i3AttrSet* p3DRoot = CharaViewMgr::i()->Get3DRoot();

		if (p3DRoot)
		{
			p3DRoot->RemoveChild(m_pCharacter->getSceneObject());
		}
		
		m_pCharacter = nullptr;
	}

	if( bRemoveInstance)
	{
		g_pCharaManager->SetLobbyAvatar(nullptr);
	}
}


void UICharaView::ChangeCharaClanMark( void)
{
	I3ASSERT(m_pCharacter != nullptr);

	if( m_viewCharaInfo._charaInfo.GetNetIdx() != BattleSlotContext::i()->getMySlotIdx())
	{
		m_pCharacter->CharaChangeClanMark(BattleSlotContext::i()->getSlotInfo( m_viewCharaInfo._charaInfo.GetNetIdx() )->_clanMark);
	}
	else
	{
		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		m_pCharacter->CharaChangeClanMark(MyUserInfoBasic.m_ui32ClanMark);
	}	
}

void UICharaView::PlayCharaAnim_Ceremony( void)
{

}

bool UICharaView::UpdateCharacter( INT32 idx, bool bFirstUpdate, WEAPON_SLOT_TYPE mustUpdateSlot, TEAM_TYPE team )
{
	//김대영 후에 작업해야됨.
	// 로비에서 다른 유저는 -1로 판단합니다.
	bool bLobby = false;
	if( idx == -1 )
	{
		bLobby = true;
		m_iIndex = idx = 0;
	}
	m_iIndex = idx;
	m_bFirstUpdate = bFirstUpdate;

	// 초도보급 전이라면, 업데이트 하지 않는다
	if( UserInfoContext::i()->HaveNickName() == false)
		return false;
	
	_removeRecreateFlag(CHARAVIEW_RECREATE_ALL);
	m_bChangeAction = false;
	
	const UI_CHARA_INFO  * pUIInfo = nullptr;
	m_viewCharaInfo._charaInfo.SetNetIdx( m_iIndex);
	
	if( BattleSlotContext::i()->GetMySlotIdxEx() == m_iIndex && !bLobby)
	{
		// 무기가 설정되어있지 않다면, 기본지급무기로 설정한다.
		m_bMyCharaView = true;
		pUIInfo = (UserInfoContext::i()->GetMyUICharaInfo());
	}
	else
	{
		m_bMyCharaView = false;
		pUIInfo = (CCommunityContext::i()->GetAnotherUserInfo(idx));
	}

	// 예외 처리
	// 모든 무기가 설정이 안되어 있으면 기본 무기로 설정
	
	if( 0 == pUIInfo->getWeaponItemID( WEAPON_SLOT_PRIMARY) && 0 == pUIInfo->getWeaponItemID( WEAPON_SLOT_SECONDARY) &&
		0 == pUIInfo->getWeaponItemID( WEAPON_SLOT_MELEE) &&	0 == pUIInfo->getWeaponItemID( WEAPON_SLOT_THROWING1) &&
		0 == pUIInfo->getWeaponItemID( WEAPON_SLOT_THROWING2) )
	{
		UI_CHARA_INFO * pSetInfo = const_cast<UI_CHARA_INFO*>(pUIInfo);
		pSetInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_ASSAULT);
		pSetInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, DEFAULT_WEAPON_HANDGUN);
		pSetInfo->setWeaponItemID( WEAPON_SLOT_MELEE, DEFAULT_WEAPON_KNIFE);
		pSetInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, DEFAULT_WEAPON_THROWING1);
		pSetInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, DEFAULT_WEAPON_THROWING2);
	}

	// 캐릭터 정보 설정. 기존 캐릭터랑 같으면, 새로 생성하지 않는다.
	// 자동설정이면 기본 장비된 캐릭터를 보여준다.
	const CHARA_PARTS_INFO * pPartsInfo = nullptr;
	
	pPartsInfo = pUIInfo->getCharaParts(team);

	if( _CheckEquip(pPartsInfo) )
		_addRecreateFlag(CHARAVIEW_RECREATE_PARTS);

	if( _RecreateSlotChara( pPartsInfo, team) )
		_addRecreateFlag(CHARAVIEW_RECREATE_CHARA);

	if( _isRecreateFlag(CHARAVIEW_RECREATE_CHARA) && mustUpdateSlot == -1)
	{
		mustUpdateSlot = (WEAPON_SLOT_TYPE) 0;
	}

	if( _CheckWeapon( pUIInfo, mustUpdateSlot, team) )
		_addRecreateFlag(CHARAVIEW_RECREATE_WEAPON);

	m_viewCharaInfo._ChangeFlag = 0;
	m_viewCharaInfo._charaInfo.SetNetIdx( m_iIndex);

	if(m_bMyCharaView == true)
	{
		BattleSlotContext::i()->setMySlotIdxEx( m_iIndex);
		m_viewCharaInfo._charaInfo.SetTeam( ( getTeam() == TEAM_RED )?CHARACTER_TEAM_RED:CHARACTER_TEAM_BLUE);
	}
	else
	{
		m_viewCharaInfo._charaInfo.SetTeam( (team == TEAM_RED)?CHARACTER_TEAM_RED:CHARACTER_TEAM_BLUE);
	}
	
	if( _isRecreateFlag(CHARAVIEW_RECREATE_CHARA) || m_pCharacter == nullptr)
	{
		// vv3 - check
		CHARA_RES_ID resID = CHARA::ItemID2ResID( m_viewCharaInfo._charaInfo.GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));
	
		g_pCharaManager->CreateAvatarForView( (AvatarLoadedCallback) sAfterLoadChara, m_iIndex, &m_viewCharaInfo._charaInfo,
			resID, CHARA::TYPE_PROFILE);

		return true;
	}
	else
	{
		if( m_pCharacter != nullptr)
		{
			// 장비 설정
			if( _isRecreateFlag(CHARAVIEW_RECREATE_PARTS) )
			{
				// vv3 - check
				const CHARA_PARTS_INFO * pPartsInfo2 = m_viewCharaInfo._charaInfo.GetParts();
				CHARA_RES_ID charaID = CHARA::ItemID2ResID( pPartsInfo2->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));

				for( INT32 idx2 = 0 ; idx2 < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx2 )
				{
					switch( idx2 )
					{
					case CHAR_EQUIPMENT_PARTS_HEAD		:
						{
							T_ItemID item_id = pPartsInfo2->GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx2) ); 
							T_ItemID sub_item_id = pPartsInfo2->GetPartsItemID(CHAR_EQUIPMENT_PARTS_BERET);

							if( item_id )
								m_pCharacter->Cmd_CreateEquip( charaID, item_id , sub_item_id);
							else
								m_pCharacter->Cmd_DeleteEquip(EQUIP::ePART_HEADGEAR);
						}
						break;
					case CHAR_EQUIPMENT_PARTS_BELT		:
					case CHAR_EQUIPMENT_PARTS_HOLSTER	:
						{
							T_ItemID item_id = pPartsInfo2->GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx2) ); 
							I3ASSERT( item_id > 0);
							m_pCharacter->Cmd_CreateEquip( charaID, item_id );
						}
						break;
					case CHAR_EQUIPMENT_PARTS_FACE		:
						{
							T_ItemID item_id = pPartsInfo2->GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx2) ); 
							if( item_id > 0 )
								m_pCharacter->Cmd_CreateEquip( charaID, item_id );
							else
								m_pCharacter->Cmd_DeleteEquip( EQUIP::ePART_FACEGEAR);
						}
						break;
					// equip check
					//case CHAR_EQUIPMENT_PARTS_BERET		: 
					//	{
					//		T_ItemID item_id = pPartsInfo2->GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx2) ); 
					//		if( item_id > 0 )
					//			m_pCharacter->Cmd_CreateEquip( charaID, item_id );
					//	}
					//	break;
					}
				}

				m_bChangeAction = true;
				m_pCharacter->SetViewerChara();

				// 장비에 맞게 머리를 교체한다
				((Avatar*)m_pCharacter)->SetCharaHead( true);
			}

			// 모드에 따라 무기를 설정해야 하는 경우가 있다. 처리!!!
			INT32 selectUsage = _CheckLimitWeapon();

			//	뷰 캐릭터의 현재 무기 슬롯이 변경됐다면 보여지는 무기 슬롯 변경
			if(m_viewCharaInfo._weaponSlot != selectUsage)
			{
				m_bChangeAction = true;
			}

			// 무기 설정
			_RecreateWeapon();

			// 애니메이션을 다시 설정
			_Reaction();
		}
	}

	return _isRecreateFlag(CHARAVIEW_RECREATE_CHARA);
}

void UICharaView::SetLoadedChara( INT32 idx, Avatar * pLoadedAvatar)
{
	INT32 selectUsage = m_viewCharaInfo._weaponSlot;
	I3ASSERT_RETURN( pLoadedAvatar != nullptr);

	pLoadedAvatar->ApplyLoadedRes();
	m_pCharacter = pLoadedAvatar;

	I3_GAMENODE_ADDCHILD( g_pFramework->GetUIMainframe(), m_pCharacter );
	m_pCharacter->SetEnable( true);
	m_pCharacter->SetPos( &m_CharacterPosition);
	
	g_pCharaManager->CreateViewChara( &m_viewCharaInfo);

#if defined( I3_DEBUG)
	m_pCharacter->getSceneObject()->SetName( "3D Chara");
#endif
	
	CharaViewMgr::i()->AddChildByChara(m_pCharacter);
	
	WeaponBase * pWeapon = m_pCharacter->GetCurrentCharaWeapon();
	if( pWeapon != nullptr)
	{
		if( i3::same_of<WeaponKrissSuperV*>(pWeapon))
		{
			((WeaponKrissSuperV*)pWeapon)->SetEnableDualWeapon( true, false);
			m_bChangeAction = true;
		}
		else if( i3::same_of<WeaponShotGunGenderWithDual*>(pWeapon))
		{
			((WeaponShotGunGenderWithDual*)pWeapon)->SetEnableDualWeapon( true, false);
			m_bChangeAction = true;
		}
	}

	if( selectUsage == -1)
		selectUsage = 0;

	ChangeCharaClanMark();

	//	무기 구성. 기존 무기랑 같으면, 새로 생성하지 않는다
	_RecreateWeapon();

	//	장비 구성. 기존 장비랑 같으면, 새로 생성하지 않는다
	if( _isRecreateFlag(CHARAVIEW_RECREATE_PARTS) && !_isRecreateFlag(CHARAVIEW_RECREATE_CHARA) )
	{
		g_pCharaManager->SetCharaEquipSet(m_pCharacter, &m_viewCharaInfo._charaInfo);
	}

	//// 머리 붙이기
	// 장비 구성후에 호출해야 한다.
	pLoadedAvatar->SetCharaHead( true);

	selectUsage = _CheckLimitWeapon();

	//	뷰 캐릭터의 현재 무기 슬롯이 변경됐다면 보여지는 무기 슬롯 변경
	if(m_viewCharaInfo._weaponSlot != selectUsage)
	{
		m_bChangeAction = true;
	}

	if( _isRecreateFlag(CHARAVIEW_RECREATE_CHARA_WEAPON) )
	{
		m_bChangeAction = true;
	}

	_removeRecreateFlag(CHARAVIEW_RECREATE_CHARA_WEAPON);

	// 무기가 없는 경우..
	// 선택 가능한 무기를 찾는다.
	if( m_pCharacter->GetCurrentCharaWeapon() == nullptr)
	{	
		for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			if( m_pCharacter->Cmd_ChangeWeapon( static_cast<WEAPON_SLOT_TYPE>(i)) )
			{
				break;
			}
		}
	}

	// 애니메이션 다시 설정
	_Reaction();
}

void UICharaView::sAfterLoadChara( void * pAvatar, INT32 idx, AI_BIND_DATA * pAIBind)
{
	CharaViewMgr::i()->LoadCompleteProcess( idx, static_cast<Avatar*>(pAvatar) );
}

void UICharaView::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate( rDeltaSeconds);
}

void UICharaView::SetVisibleChara( bool bVisible)
{
	SetVisible( bVisible);
}

void UICharaView::SetCharaPosition( VEC3D * pPos)
{
	if( m_pCharacter != nullptr)
		m_pCharacter->SetPos( pPos);

	i3Vector::Copy( &m_CharacterPosition, pPos);
}

/************************************************************************/
/*							Chara View Shared Inst						*/
/************************************************************************/

CharaViewMgr::CharaViewMgr(void) : m_pCharaView(nullptr), m_pLoadWaitIcon(nullptr)
								, m_p3DRoot(nullptr), m_pUI3DViewBox(nullptr), m_pUI3DViewLight(nullptr)
								, m_r32CharaRotTheta(0.0f), m_i32MouseOldX(0), m_i32MouseOldY(0)
								, m_i32Param(0), m_pfFinishLoadCallback( nullptr)
{
}

CharaViewMgr::~CharaViewMgr(void)
{
	ReleaseCharaView();
}

void CharaViewMgr::_AddLight( void)
{
	if( m_pUI3DViewLight == nullptr)
	{	
		m_pUI3DViewLight = i3Light::new_object();
		m_pUI3DViewLight->SetLightType( I3G_LIGHT_DIRECTIONAL);

		i3RegKey * pRoot = Config::_GetRegRoot("Stage/Stage");
		I3ASSERT( pRoot != nullptr);

		COLOR col;
		COLORREAL d_diffuse, d_specular, ambient;
		VEC4D vd_diffuse, vd_specular;
		VEC3D d_rot;

		i3RegKey * pKey = Config::FindKey( "Lobby", pRoot);

		if( pKey != nullptr)
		{
			FIND_REG_DATA( pKey, "Ambient", &col);
			i3Color::Set( &ambient, i3Color::GetR(&col),i3Color::GetG(&col),i3Color::GetB(&col),i3Color::GetA(&col) );

			//directional light
			FIND_REG_DATA( pKey, "D_Light_Diffuse", &vd_diffuse);
			i3Color::Set( &d_diffuse, getX( &vd_diffuse), getY( &vd_diffuse), getZ( &vd_diffuse),getW( &vd_diffuse));

			FIND_REG_DATA( pKey, "D_Light_Specular", &vd_specular);
			i3Color::Set( &d_specular, getX( &vd_specular), getY( &vd_specular), getZ( &vd_specular),getW( &vd_specular));

			FIND_REG_DATA( pKey, "D_Light_Rotate",	&d_rot);
		}
		else
		{
			i3Color::Set( &ambient, 0.4f, 0.4f, 0.4f, 1.0f);
			i3Color::Set( &d_diffuse, 1.0f, 0.0f, 0.0f, 1.0f);
			i3Color::Set( &d_specular, 1.0f, 1.0f, 1.0f, 1.0f);

			i3Vector::Set( &d_rot, -12.0f, -34.0f, 4.0f);
		}

		m_pUI3DViewLight->SetDiffuse( &d_diffuse);
		m_pUI3DViewLight->SetSpecular( &d_specular);

		g_pFramework->getSgContext()->setAmbient( &ambient);

		MATRIX mtx;

		i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, getZ( &d_rot));
		i3Matrix::PostRotateX( &mtx, I3_DEG2RAD( getY( &d_rot)));
		i3Matrix::PostRotateY( &mtx, I3_DEG2RAD( getX( &d_rot)));

		m_pUI3DViewLight->SetWorldSpaceTransform( &mtx);

		g_pFramework->addLight( m_pUI3DViewLight);
	}
}

void CharaViewMgr::_RemoveLight()
{
	if( m_pUI3DViewLight )
	{
		g_pFramework->removeLight( m_pUI3DViewLight);
		I3_SAFE_RELEASE( m_pUI3DViewLight );
	}
}

i3RenderAttr * CharaViewMgr::_GetReflectAttr(void)
{
	i3ImageFile file;
	i3TextureCube * pTexture;

	i3::rc_string strFileName;
	if ( g_pEnvSet->GetUIVersion() == UFV_1_5 || g_pEnvSet->GetUIVersion() == UFV_1_2)
	{
		i3::sprintf( strFileName, "Gui/Showroom/UI_V15/Gunshowcaseroom_Ref.dds");
	}
	else
	{
		i3::sprintf( strFileName, "Gui/Showroom/UI_V10/Gunshowcaseroom_Ref.dds");
	}

	pTexture = (i3TextureCube*) file.Load( strFileName.c_str());

	I3ASSERT( pTexture != nullptr);

	i3ReflectMapBindAttr * pReflectAttr = i3ReflectMapBindAttr::new_object_ref();
	pReflectAttr->SetTexture( pTexture);
	pTexture->Release();

	return pReflectAttr;
}

void CharaViewMgr::CreateCharaView(void)
{
	if( m_pCharaView == nullptr )
		m_pCharaView = UICharaView::new_object();
}

void CharaViewMgr::ReleaseCharaView(void)
{
	if( m_pCharaView )
		I3_SAFE_RELEASE(m_pCharaView);
}

void CharaViewMgr::SetCharaViewInfo(i3GameNode * pParent, i3AttrSet * p3DRoot)
{
	if (m_p3DRoot == nullptr)
	{
		I3_GAMENODE_ADDCHILD(pParent, m_pCharaView);
		m_p3DRoot = p3DRoot;
	}
}

void CharaViewMgr::SetCharaViewInfo(i3GameNode * pParent, i3UIFrameWnd * pFrameWnd)
{
	if (m_pUI3DViewBox == nullptr)
	{
		I3_GAMENODE_ADDCHILD(pParent, m_pCharaView);

		m_pUI3DViewBox = i3UI3DViewBox::new_object();
		I3ASSERT(m_pUI3DViewBox);

		I3ASSERT(pFrameWnd);
		pFrameWnd->AddChild(m_pUI3DViewBox);

		m_pUI3DViewBox->setSize(pFrameWnd->getWidth() - 2, pFrameWnd->getHeight() - 2);
		m_pUI3DViewBox->setPos(1, 1);
		_AddLight();
	}
}

void CharaViewMgr::ResetCharaViewInfo(i3GameNode * pParent)
{
	if( m_p3DRoot || m_pUI3DViewBox)
		pParent->RemoveChild(m_pCharaView);

	m_p3DRoot = nullptr;

	if( m_pUI3DViewBox )
	{
		m_pUI3DViewBox->getParent()->RemoveChild( m_pUI3DViewBox);
		I3_SAFE_RELEASE( m_pUI3DViewBox);

		_RemoveLight();
	}

	SetMouseOldPos(0, 0);
	m_r32CharaRotTheta = 0.0f;

	m_i32Param = 0;

	m_pfFinishLoadCallback = nullptr;
}

void CharaViewMgr::UpdateCharacter(INT32 idx, bool bFirstUpdate, WEAPON_SLOT_TYPE slot, TEAM_TYPE team, i3UIFrameWnd * pFrameWnd)
{
	if( m_pCharaView->UpdateCharacter(idx, bFirstUpdate, slot, team) )
	{
		if( m_pLoadWaitIcon == nullptr)
		{
			UIBase * pUIBase = i3::kind_cast<UIBase*>( m_pCharaView->getParent() );
			if (nullptr != pUIBase)
				m_pLoadWaitIcon = GameUI::EnableLoadingIcon( pUIBase, pFrameWnd);
			m_bIsLoadedChara = false;
		}
	}
	else
	{
		if( m_pLoadWaitIcon != nullptr)
		{
			GameUI::DisableLoadingIcon( m_pLoadWaitIcon);
			m_pLoadWaitIcon = nullptr;
			m_bIsLoadedChara = true;
		}
	}
}

void CharaViewMgr::LoadCompleteProcess(INT32 idx, Avatar * pAvatar)
{
	SetMouseOldPos(0, 0);
	m_r32CharaRotTheta = 0.0f;

	m_pCharaView->SetLoadedChara( idx, pAvatar);

	if( m_pLoadWaitIcon != nullptr)
	{
		GameUI::DisableLoadingIcon( m_pLoadWaitIcon);
		m_pLoadWaitIcon = nullptr;
		m_bIsLoadedChara = true;
	}

	if( m_pfFinishLoadCallback )
		m_pfFinishLoadCallback( m_pCharaView->getParent() );
}

void CharaViewMgr::AddChildByChara(CGameCharaBase * pChara)
{
	if( m_p3DRoot )
	{
		m_p3DRoot->AddChild( pChara->getSceneObject() );
		return;
	}

	if( m_pUI3DViewBox )
	{
		m_pUI3DViewBox->Create3DView();
		m_pUI3DViewBox->Set3DView( pChara->getSceneObject() );
		m_pUI3DViewBox->OnVisibleScene();

		// Set Camera Info
		i3Camera * pCamera = m_pUI3DViewBox->get3DCamera();
		pCamera->SetFOV( I3_DEG2RAD( 41.5f));

		MATRIX * pCamMtx = pCamera->GetMatrix();
		i3Matrix::Identity( pCamMtx);
		i3Matrix::SetPos( pCamMtx, 0.0f, 1.00f, 3.25f, 1.0f);

		// Init Chara Position.
		VEC3D vCharaPos = VEC3D(0.0f, 0.0f, 0.0f);
		m_pCharaView->SetCharaPosition( &vCharaPos );
	
		// Add Reflect Attr
		m_pUI3DViewBox->getAttrSetNode()->AddAttr( _GetReflectAttr() );

		return;
	}

}

void CharaViewMgr::RotateCharacter(INT32 x, INT32 y)
{
	Avatar * pAvatar = i3::same_cast<Avatar*>( m_pCharaView->GetCharacter() );

	if( pAvatar != nullptr)
	{
		INT32 dragX = x - m_i32MouseOldX;

		MATRIX * pMtx = pAvatar->getSceneObject()->GetMatrix();

		VEC3D vPos;
		i3Vector::Copy( &vPos, i3Matrix::GetPos( pMtx));

		if( dragX != 0)
		{
			m_r32CharaRotTheta += (REAL32)dragX * 0.01f;
			i3Matrix::SetRotateY( pMtx, m_r32CharaRotTheta);
			i3Matrix::SetPos( pMtx, &vPos);
		}
	}

	SetMouseOldPos(x, y);
}