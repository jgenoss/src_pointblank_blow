#include "pch.h"
#include "GlobalVariables.h"
#include "AllStage.h"
#include "InGameQACommandSet.h"
#include "PBLog.h"
#include "GameCharaBoneContext.h"
#include "GameCharaMoveContext.h"

#include "GameMap.h"
#include "UtilBattle.h"
#include "BattleHud.h"
#include "GameMode.h"
#include "ObjExplosion.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/atoi.h"

#include "UI/UIBattleFrame.h"
#include "UI/UIHUDBase.h"
#include "UI/UIHudManager.h"
#include "UI/UIHudDefine.h"
#include "UI/UIHUDNoticeShop.h"
#include "UI/HUD/UIHUD_Domination_Manager.h"
#include "BattleSlotContext.h"
#include "GameContextUtil.h"
#include "GameMissionManager.h"
#include "UserInfoContext.h"

#include "GameEventHandler.h"
#include "CQAWeaponCMgr.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "Designation/DesigUtil.h"

namespace CommandSet
{
	TYPE parsing( const i3::wliteral_range& wCmdRng)
	{
		if( Domination_Util::parsing( wCmdRng) == CommandSet::domination)
		{
			_type = CommandSet::domination;
			return CommandSet::domination;
		}

		const wchar_t* pos = wCmdRng.begin(); // (char*)pCommandLine;
		while( *pos == ' ') pos++;

		i3::wstring wstr;
		while( *pos != 0 && (::IsCharAlphaW(*pos) || *pos == L' ' || *pos == L'_' || *pos == L'/'))
		{
			wchar_t temp = *pos;
			if( *pos == ' ') temp = '_';
			wstr += temp;

			pos++;
		}
		
		i3::wstring::iterator itr = wstr.end();
		while( *(--itr) == L'_') itr = wstr.erase( itr);
	
		for( size_t _e = none; _e < max; ++_e)
		{
			if( g_wstr_cmd[_e] == 0) continue;

			i3::wstring wstr_cmd = L"/";
			wstr_cmd += g_wstr_cmd[_e];

			if( wstr.find( wstr_cmd ) == 0 && (wstr.length() == wstr_cmd.length()))
			{
				for( size_t i = 0; i < VALUE_COUNT; ++i)
				{
					wchar_t wstrTemp[256];
					wstrTemp[0] = 0;
					INT32 idx = 0;

					while( *pos != 0 && *pos != ' ')
					{
						wstrTemp[idx] = *pos;

						idx++;
						pos++;
					}
					pos++;
					wstrTemp[idx] = 0;

					if(i3::contain_string(wstrTemp, L".") == -1) 
						g_value[i] = (REAL32)i3::atoi( wstrTemp);
					else 
						g_value[i] = (REAL32)::_wtof(wstrTemp);
				}
				_type = (TYPE)_e;
				return (TYPE)_e;
			}
		}
		_type = none;
		return none;
	}
};

namespace Domination_Util
{
	i3::wstring cmd_msg;
	bool enable_domi_nick = false;

	char g_domi_rank_web[MAX_RANKING_URL_SIZE] = {0,};

	INT32 g_current_score = 0;
	INT32 g_best_score = 0;
	bool g_renewal_best = false;

	CommandSet::TYPE parsing( const i3::wliteral_range& wCmdRng )
	{
		{
			i3::wstring cmd = L"//";
			
			i3::wstring wstr(wCmdRng.begin(), wCmdRng.end()); 
			i3::wstring::iterator itr = wstr.begin();
			while( *itr == L' ') itr = wstr.erase( itr);

			if( wstr.find( cmd) == 0)
			{
				// '/'제거...
				itr = wstr.begin();
				while( *itr == L'/') itr = wstr.erase( itr);

				cmd_msg = wstr;
				return CommandSet::domination;
			}
		}
		cmd_msg.clear();
	
		return CommandSet::none;
	}
};

I3_CLASS_INSTANCE( CGameCommandSet);//, i3ElementBase);

CGameCommandSet::CGameCommandSet(void)
{
	m_pSkillCmdText = i3TextNodeDX2::new_object();
	m_pSkillCmdText->Create( GetDefaultFontName(), FONT_COUNT_1024, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL, 1.0f, true);
	m_pSkillCmdText->SetText(L"");
	m_pSkillCmdText->setSize(1024, 768);
	m_pSkillCmdText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pSkillCmdText->SetColor(200, 200, 200, 255);
	m_pSkillCmdText->setPos(824,20);
	m_pSkillCmdText->SetEnable(false);

	g_pFramework->getMessagBoxLayer()->AddChild(m_pSkillCmdText);
}

CGameCommandSet::~CGameCommandSet(void)
{
	I3_SAFE_NODE_RELEASE( m_pSkillCmdText);
}

void CGameCommandSet::ProcessCommand()
{
	// Ability 체크와 관계 없이 커멘드 실행 가능하도록 하기 위에 맨 위로 올림.
	//switch( CommandSet::_type)
	//{
	//case CommandSet::wc:
	//	{
	//		CStageBattle* pStage = g_pFramework->GetStageBattle();
	//		if( pStage == nullptr)
	//			return;
	//		
	//		if(CQAWeaponCMgr::i()->getOnWeaponCollision())
	//		{
	//			CQAWeaponCMgr::i()->ReleasePhysXBox();
	//			CQAWeaponCMgr::i()->setOnWeaponCollision(false);
	//		}
	//		else
	//		{
	//			// 크기 정보부터 들어가야 한다.
	//			CQAWeaponCMgr::i()->setOnWeaponCollision(true);
	//			CQAWeaponCMgr::i()->setValue(CommandSet::g_value[0], CommandSet::g_value[1], 
	//						(INT32)CommandSet::g_value[2], (INT32)CommandSet::g_value[3]);
	//			CQAWeaponCMgr::i()->CreatePhysXBox(pStage->getWorld());

	//			if(CHud::i())
	//				CHud::i()->SetNoticeText( CQAWeaponCMgr::i()->getOutCollisionBox() );
	//		}
	//	}
	//	break;
	//}
	if( !UserInfoContext::i()->CheckSpecialAbility(SA_CHEAT) ) return;

	i3::stack_wstring	wstrTemp;

	CStageBattle* pStage = g_pFramework->GetStageBattle();
	if( pStage == nullptr)
		return;

	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());


	// Command list 출력
	wstrTemp = L"Command : ";
	i3::wliteral_range wstrCmd = CommandSet::g_wstr_cmd[CommandSet::_type];
	
	wstrTemp.append(wstrCmd.begin(), wstrCmd.end());

	CHud::i()->CreateInGameMessage(wstrTemp);

	//value
	for(INT32 i = 0; i < CommandSet::VALUE_COUNT; ++i)	{
		m_fValue[i] = CommandSet::g_value[i];
	}

	// 각각의 Command list에 맞추어 동작!
	switch( CommandSet::_type)
	{
	/*case CommandSet::mission_win:		
		GameContextUtil::QA_SetRound((UINT8) m_fValue[0] , (UINT8) m_fValue[1]); 
		break;

	case CommandSet::time_limit:		
		GameContextUtil::QA_SetEndlessBattle(); 			
		break;*/

	/*case CommandSet::pos:
		if( pMyChara != nullptr)
		{
			i3::stack_wstring wstrPos;
			VEC3D *pos = pMyChara->GetPos();
			i3::sprintf( wstrPos, L"Player position: X(%.4f), Y(%.4f), Z(%.4f)", 
				i3Vector::GetX(pos), i3Vector::GetY(pos), i3Vector::GetZ(pos));

			CHud::i()->SetNoticeText( wstrPos );

			m_bAlwaysShowPos = !m_bAlwaysShowPos;
		}
		break;*/

	/*case CommandSet::move_pos:
		if( pMyChara != nullptr)
		{
			VEC3D pos;
			i3Vector::Set( &pos, m_fValue[0], m_fValue[1], m_fValue[2] );
			pMyChara->forceSetPos( &pos );
		}
		break;*/

	case CommandSet::cash:				
		GameContextUtil::QA_SetCash((UINT32) m_fValue[0] ); 
		break;

	/*case CommandSet::notice:			
		break;*/

	/*case CommandSet::change_grade:			
		GameContextUtil::QA_SetRank((UINT32)m_fValue[0] ); 
		break;*/

	/*case CommandSet::set_kill:			
		GameContextUtil::QA_SetKillCount((UINT16) m_fValue[0] , (UINT16) m_fValue[1]);	
		break;*/

	/*case CommandSet::respawn_time:
		CGameMissionManager::i()->setRespawnTime(m_fValue[0]);
		CGameMissionManager::i()->setRespawnTime4vs4(m_fValue[0]);
		break;*/

	/*case CommandSet::item_period:		
		break;*/

	/*case CommandSet::mastery_point:		
		break;*/

	//case CommandSet::object:		
	//	{
	//		// Effect를 초기화 합니다.
	//		g_pEffectManager->Reset();

	//		g_pTracerBulletManager->ResetAll();

	//		// Object를 초기화 합니다.
	//		gmode::i()->ResetObjectStatus();

	//		// Object 폭파 여부 리셋
	//		obj_explosion::i()->Process_ResetObjectExploded(g_pFramework->GetStageBattle());
	//	}
	//	break;

	/*case CommandSet::exp: 
		GameContextUtil::QA_SetExp((UINT32) m_fValue[0] );		
		break; */

	case CommandSet::pino: 
		GameContextUtil::QA_SetPino((UINT32) m_fValue[0] );		
		break; 

	case CommandSet::challenge: 
		GameContextUtil::QA_SetChallenge();					
		break;

	/*case CommandSet::clan_rank: 
		GameContextUtil::QA_SetClanRank((UINT32) m_fValue[0]);	
		break;

	case CommandSet::clan_exp: 
		GameContextUtil::QA_SetClanExp((UINT32) m_fValue[0]);	
		break;*/

	/*case CommandSet::medal_count: 
		GameContextUtil::QA_SetMedalCount((UINT8) m_fValue[0], (UINT8) m_fValue[1], (UINT8) m_fValue[2]);	
		break;*/

	/*case CommandSet::log: 
		break;*/

	case CommandSet::move_offset:
		if( pMyChara != nullptr)
			pMyChara->SetMoveSpeedAddOffset(m_fValue[0]);
		break;

	case CommandSet::acceleration_offset:
		if( pMyChara != nullptr)
			pMyChara->SetAccelerationTimeOffset(m_fValue[0]);
		break;

	case CommandSet::jump_height_offset:
		if( pMyChara != nullptr)
			pMyChara->SetJumpHeightOffset(m_fValue[0]/100.f);
		break;
	case CommandSet::change_designation:
		{
			INT32 iDesigSlot = (INT32)m_fValue[0];
			INT32 iDesigSetType_New = (INT32) m_fValue[1];
//			bool  bEnable = (m_fValue[2] > 0 );

			if (-1 < iDesigSlot && iDesigSlot < MAX_EQUIP_USER_TITLE)
			{
				INT32 numDesigSet = DesigUtil::GetDesigSetCountByType();
				
				if (0 <= iDesigSetType_New && iDesigSetType_New < numDesigSet )
				{
					NET_CHARA_INFO* myInfo = BattleSlotContext::i()->setNetCharaInfo( BattleSlotContext::i()->getMySlotIdx());
				
					myInfo->ApplyEquipUserTitle(iDesigSlot, (UINT8)iDesigSetType_New);
				}
			}
			
		}
		break;
	/*case CommandSet::jump_damage_offset:
		if( pMyChara != nullptr)
			pMyChara->SetFallDamageMinOffset(m_fValue[0]/10.f);
		break;*/

	/*case CommandSet::clan_match_point:
		GameContextUtil::QA_SetClanMatchPoint( (UINT32) m_fValue[0] );									
		break;

	case CommandSet::clan_match_win_lose:
		GameContextUtil::QA_SetClanMatchWinLose( (UINT32) m_fValue[0], (UINT32) m_fValue[1], (UINT32) m_fValue[2] );		
		break;

	case CommandSet::clan_match_room_create:
		GameContextUtil::QA_SetClanMatchRoomInsert( (UINT32) m_fValue[0] );					
		break;

	case CommandSet::death_all:
		for( i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara( i);
			if( pChara == nullptr )	continue;
			if( pChara != pMyChara)
			{
				VEC3D vDir;
				i3Vector::Sub( &vDir, pChara->GetPos(), pMyChara->GetPos());
				i3Vector::Normalize( &vDir, &vDir);

				g_pCharaManager->DeathChara( pChara, pChara->getCharaNetIndex(), &vDir, CHARA_DEATH_A, CHARA_HIT_HEAD);
			}
		}
		break;

	case CommandSet::change_character:
		{	
			CHARA_RES_ID charaType = (CHARA_RES_ID) ((UINT32)m_fValue[0]);
			if( (charaType > CHARA_RES_ID_HUMAN_S) && (charaType < CHARA_RES_ID_HUMAN_E) ||
				(charaType > CHARA_RES_ID_DINO_S) && (charaType < CHARA_RES_ID_DINO_E) )
			{
				UINT32	nCharaUseMap[ SLOT_MAX_COUNT];
				INT32	nUseCharaCount = 0;

				i3mem::FillZero( nCharaUseMap, sizeof( nCharaUseMap));

				for( i = 0; i < g_pCharaManager->getCharaCount(); i++)
				{
					CGameCharaBase * pChara = g_pCharaManager->getChara( i);
					if( pChara == nullptr )	continue;
					nCharaUseMap[ nUseCharaCount] = pChara->getCharaNetIndex();
					nUseCharaCount++;
				}

				if( nUseCharaCount > 0)
				{
					g_DebugChara[ 0] = charaType;
					g_DebugChara[ 1] = charaType;

					g_pCharaManager->RemoveAllChara();

					for( i = 0; i < nUseCharaCount; i++)
					{
						CGameCharaBase * pChara = g_pCharaManager->AddChara( nCharaUseMap[ i], g_DebugChara[ nCharaUseMap[ i] %2]);
						if( pChara != nullptr)
						{
							g_pCharaManager->RespawnChara( pChara->getCharaNetIndex());
							pChara->addCharaStateMask( CHARA_STATE_FIRSTRESPAWN);
						}
					}
				}
			}
		}
		break;*/
	/*case CommandSet::change_designation:
		break;*/
	



	/*case CommandSet::kill_text:
		if( pMyChara != nullptr)
		{
			if( pMyChara->GetCurrentCharaWeapon() != nullptr)
			{
				CHud* pHud = CHud::i();

				CWeaponInfo * pMyWeaponInfo = pMyChara->GetCurrentCharaWeapon()->getWeaponInfo();

				pHud->SetKillText( 0, 1, pMyWeaponInfo, false);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, false, true);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, true, true);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, false);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, false, true);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, true, true);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, false, true);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, false);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, true, true);
				pHud->SetKillText( 0, 1, pMyWeaponInfo, false, true);
			}
		}
		break;*/

	/*case CommandSet::view_bone:
#if defined( I3_DEBUG)
		if( pMyChara != nullptr)
		{
			pMyChara->ToggleRenderBone();
		}
#endif
		break;*/

	/*case CommandSet::invincible:
#if defined( I3_DEBUG)
		if( pMyChara != nullptr)
		{
			pMyChara->setInvincibleTime(FLT_MAX);
		}
#endif
		break;*/

	/*case CommandSet::skill:
		{
			INT32 iSlot = BattleSlotContext::i()->getMySlotIdx();
			m_fValue[0]--;
			m_fValue[1]--;
			m_fValue[2]--;
			NET_CHARA_INFO * pInfo = BattleSlotContext::i()->setNetCharaInfo( iSlot);
			SKILLSYSTEM_APPLY_INFO * pSkillsystem = pInfo->setSkillsystem((INT32)m_fValue[0]);
			pSkillsystem->setSkillData((INT32)m_fValue[0], (INT32)m_fValue[1], (INT32)m_fValue[2]);
			util_battle::ApplySkillsystem(iSlot);
		}
		break;*/

//#ifdef UITEST_MINIMAP_MOVE
//	case CommandSet::move_minimap:
//		{
//			GameContextUtil::UITest_SetMinimapMove(true);
//		}
//		break;
//#endif
	/*case CommandSet::killcount:
		{
			INT32 ivalue = (INT32)m_fValue[0];
			GameEventSender::i()->SetEvent( EVENT_QA_COMMAND_INCREASE_KILL_COUNT, &ivalue );
		}
		break;*/
	//case CommandSet::ts:
	//	{
	//		// 커맨드 뒤의 숫자는 '증가시킬 TS시간(분)'이다. 디폴트값은 1분
	//		INT32 i32DecTSMinute = (INT32)m_fValue[0];
	//		if (i32DecTSMinute < 1 ) i32DecTSMinute = 1;

	//		GameEventSender::i()->SetEvent( EVENT_QA_COMMAND_INCREASE_PLAY_TIME, &i32DecTSMinute );
	//	}
	//	break;
	//case CommandSet::solo:
	//	{
	//		GameEventSender::i()->SetEvent( EVENT_QA_COMMAND_ENABLE_SOLO_PLAY );	
	//	}
	//	break;
	//case CommandSet::tp:
	//	{
	//		QA_COMMAND_TELEPORT_DATA StructData;

	//		StructData._i32SlotIdx = (INT32)m_fValue[0];
	//		if ( StructData._i32SlotIdx < 0 || StructData._i32SlotIdx >= SLOT_MAX_COUNT )
	//			StructData._i32SlotIdx = BattleSlotContext::i()->getMySlotIdx();

	//		StructData._ar32Pos[0] = m_fValue[1];
	//		StructData._ar32Pos[1] = m_fValue[2];
	//		StructData._ar32Pos[2] = m_fValue[3];

	//		GameEventSender::i()->SetEvent( EVENT_QA_COMMAND_TELEPORT_SOMEONE, &StructData );
	//	}
	//	break;
	//case CommandSet::obdmg:
	//	{
	//		// 커맨드 뒤의 숫자는 '오브젝트에 줄 데미지'이다. 디폴트값은 100데미지
	//		// 현재는 맵에 파괴 오브젝트가 2개밖에 없어서, 오브젝트 번호를 하드코딩으로 제한해둠.
	//		// 
	//		INT32 i32Object = (INT32)m_fValue[0];
	//		INT32 i32Damage = (INT32)m_fValue[1];

	//		if(g_pFramework->IsStage(STAGE_ID_BB_OUTPOST))
	//		{
	//			if(i32Object != 2)
	//				break;	
	//		}
	//		else
	//		{
	//			if(i32Object < 0 || i32Object >= 2)
	//				break;	
	//		}

	//		if (i32Damage == 0 ) 
	//			i32Damage = 100;

	//		GameEventSender::i()->SetEvent( EVENT_QA_COMMAND_DAMAGE_TO_OBJECT, &i32Object, &i32Damage );
	//	}
	//	break;
	/*case CommandSet::hackoff:
		{
			GameEventSender::i()->SetEvent( EVENT_QA_COMMAND_DETECT_HACK_OFF );
		}
		break;*/
	/*case CommandSet::timergmpause:
		{
			UINT32 ui32Second = (UINT32)m_fValue[0];
			GameEventSender::i()->SetEvent( EVENT_QA_COMMAND_TIMER_GMPAUSE, &ui32Second );
		}
		break;*/
	case CommandSet::teamband:
		{
			INT32 TeamOff = static_cast<INT32>(m_fValue[0]);
			//INT32 Position = static_cast<INT32>(m_fValue[1]);	// 왼팔은 기본, 오른팔(1), 왼다리(2), 오른팔 + 왼다리(3)

			switch( TeamOff)
			{
			case 0 :	// All Off
				{
					wstrTemp = L"TeamBand Off";

					for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
					{
						Avatar * pAvatar = static_cast<Avatar*>(g_pCharaManager->getChara( i));
						if( pAvatar != nullptr)
							pAvatar->DisableTeamBand();
					}
				}
				break;
			case 1 :	// Alliance Off Enemy On
			case 2 :
				{
					wstrTemp = L"TeamBand On Enemy";

					for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
					{
						Avatar * pAvatar = static_cast<Avatar*>(g_pCharaManager->getChara( i));
						if( pAvatar != nullptr)
						{
							if( pAvatar->getCharaTeam() == BattleSlotContext::i()->getMyTeam())
							{
								pAvatar->DisableTeamBand();
							}
							else
							{
								pAvatar->SetTeamBand( (TeamOff==2));
							}
						}
					}

					if( TeamOff == 1)
					{
						i3::generic_string_cat( wstrTemp, L" with Red Color");
					}
					else
					{
						i3::generic_string_cat( wstrTemp, L" with Team(R/B) Color");
					}
				}
				break;
			case 3 :	// Enemy Off Alliance On
			case 4 :
				{
					wstrTemp = L"TeamBand On Alliance";

					for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
					{
						Avatar * pAvatar = static_cast<Avatar*>(g_pCharaManager->getChara( i));
						if( pAvatar != nullptr)
						{
							if( pAvatar->getCharaTeam() != BattleSlotContext::i()->getMyTeam())
							{
								pAvatar->DisableTeamBand();
							}
							else
							{
								pAvatar->SetTeamBand( (TeamOff==4));
							}
						}
					}

					if( TeamOff == 3)
					{
						i3::generic_string_cat( wstrTemp, L" with Red Color");
					}
					else
					{
						i3::generic_string_cat( wstrTemp, L" with Team(R/B) Color");
					}
				}
				break;
			case 5 :	// All On
				{
					wstrTemp = L"TeamBand On All";

					for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
					{
						Avatar * pAvatar = static_cast<Avatar*>(g_pCharaManager->getChara( i));
						if( pAvatar != nullptr)
						{
							pAvatar->SetTeamBand( true);
						}
					}
				}
				break;
			}


		}
		break;

	//case CommandSet::domination:
	//	{
	//		PACKET_DOMINATION_CHEAT_MESSAGE_REQ m;
	//		// TODO : 서버 협업 후 교체 예정 (2014.07.21.수빈)
	//		i3::string str_cmd_msg;		i3::utf16_to_mb( Domination_Util::cmd_msg, str_cmd_msg);

	//		i3::safe_string_copy( m._szMessage, str_cmd_msg, MAX_STRING_COUNT); 

	//		GameEventSender::i()->SetEvent( EVENT_DOMI_CHEAT_MESSAGE,
	//			static_cast<void*>(&m));

	//		i3::sprintf( wstrTemp, L"등장!  %s !!", Domination_Util::cmd_msg );
	//	}
	//	break;
	}
}

void CGameCommandSet::SkillDebugView(void)
{
	char strtemp[MAX_STRING_COUNT] = "";
	char strtemp2[MAX_STRING_COUNT] = "";
	char texttemp[FONT_COUNT_1024] = "";
	INT32 icategory[3];
	INT32 iindex[3];
	INT32 iLevel[3];
	INT32 iSlot = BattleSlotContext::i()->getMySlotIdx();

	const NET_CHARA_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo( iSlot);
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( iSlot);
	const CCharaInfo * pDBInfo = pChara->getDBInfo();

	if (pInfo->getSkillsystem(0) != nullptr)
	{
		for(INT32 i = 0; i < 3; i++)
		{
			icategory[i]	= pInfo->getSkillsystem(i)->getCategory();
			iindex[i]		= pInfo->getSkillsystem(i)->getIndex();
			iLevel[i]		= pInfo->getSkillsystem(i)->getLevel();

			i3::snprintf( strtemp, MAX_STRING_COUNT, "Skill Slot %d (index: %d, Level: %d) \n", icategory[i]+1, iindex[i], iLevel[i]);
			i3::generic_string_cat(texttemp, strtemp );

			//무기타입
			pInfo->getSkillsystem(i)->getWeaponTypeStr(strtemp2);
			i3::snprintf( strtemp, MAX_STRING_COUNT, "WeaponType : %s \n", strtemp2);
			i3::generic_string_cat(texttemp, strtemp );

			//스킬타입
			pInfo->getSkillsystem(i)->getSkillTypeStr(strtemp2);
			i3::snprintf( strtemp, MAX_STRING_COUNT, "Skilltype : %s \n", strtemp2);
			i3::generic_string_cat(texttemp, strtemp );

			//스킬 값
			i3::snprintf( strtemp, MAX_STRING_COUNT, "SkillValue : %f \n", pInfo->getSkillsystem(i)->getSkillValue());
			i3::generic_string_cat(texttemp, strtemp );
			i3::snprintf( strtemp, MAX_STRING_COUNT, "---------------------------------------\n\n");
			i3::generic_string_cat(texttemp, strtemp );
		}
	}

	i3::snprintf( strtemp, MAX_STRING_COUNT, "---------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp );

	//이동속도
	i3::snprintf( strtemp, MAX_STRING_COUNT, "RunSpeed : %f \n", pChara->GetMoveSpeedAddOffset(MST_RUN, MDT_FRONT));
	i3::generic_string_cat(texttemp, strtemp );

	i3::snprintf( strtemp, MAX_STRING_COUNT, "---------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp );

	//걷기속도
	i3::snprintf( strtemp, MAX_STRING_COUNT, "WalkSpeed : %f \n", pChara->GetMoveSpeedAddOffset(MST_WALK, MDT_FRONT));
	i3::generic_string_cat(texttemp, strtemp );

	i3::snprintf( strtemp, MAX_STRING_COUNT, "---------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp );

	//점프 높이
	i3::snprintf( strtemp, MAX_STRING_COUNT, "JumpHeight : %f \n", pDBInfo->GetJumpHeight()+(pDBInfo->GetJumpHeight()*pChara->getAbility( CHARA::eABILITY_JUMPHEIGHT)));
	i3::generic_string_cat(texttemp, strtemp );

	i3::snprintf( strtemp, MAX_STRING_COUNT, "---------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp );

	//방어력
	i3::snprintf( strtemp, MAX_STRING_COUNT, "Defense : %f \n", pChara->GetDamageRateByDefence());
	i3::generic_string_cat(texttemp, strtemp );

	i3::snprintf( strtemp, MAX_STRING_COUNT, "---------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp );

	//헬멧 방어력
	i3::snprintf( strtemp, MAX_STRING_COUNT, "Helmet Defense : %f \n", pChara->GetHelmetProtectionRate());
	i3::generic_string_cat(texttemp, strtemp );

	i3::snprintf( strtemp, MAX_STRING_COUNT, "---------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp );

	i3::snprintf( strtemp, MAX_STRING_COUNT, "F8 key Enable-------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp );
	i3::snprintf( strtemp, MAX_STRING_COUNT, "Test Cmd-------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp);
	i3::snprintf( strtemp, MAX_STRING_COUNT, "ex)/test (type1~3) (index) (Level1~10)-------------------------------------\n");
	i3::generic_string_cat(texttemp, strtemp);
}