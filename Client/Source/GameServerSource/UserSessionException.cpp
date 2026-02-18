#include "pch.h"
#include "UserSession.h"
#include "ModuleTrans.h"
#include "ModuleContextNC.h"

void CUserSession::_ExceptionWeapon( INT32 i32ThreadIdx )
{
	// Default Item Insert
	// Normal 지급 아이템은 묻지도 따지지도 않고 지급 합니다.
	DEFAULT_ITEM_SET* pDefaultSet;
	pDefaultSet = g_pContextMain->GetDefaultItemSet( DEFAULT_ITEM_TYPE_NORMAL );
	if( pDefaultSet )
	{
		for( INT32 i = 0 ; i < pDefaultSet->m_ui8Count ; i++ )
		{
			_ExceptionInsert( pDefaultSet->m_TItemID[ i ] );
		}
	}

	// PC방 유저일 경우 지급 아이템
	pDefaultSet = g_pContextMain->GetDefaultItemSet( DEFAULT_ITEM_TYPE_PCCAFE, m_UserInfoBasic._pcCafe );
	if( pDefaultSet )
	{
		for( INT32 i = 0 ; i < pDefaultSet->m_ui8Count ; i++ )
		{
			_ExceptionInsert( pDefaultSet->m_TItemID[ i ] );
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	//기본으로 있으야 하는 품목(영구적 무기로 DB에 등록)
	switch( m_ui8LocaleCode )
	{
	case LOCALE_CODE_THAILAND		:
		{
			_ExceptionWeaponInsert( 100003004 );				//K2
			_ExceptionWeaponInsert( 200004006 );				//K1
			_ExceptionWeaponInsert( 300005003 );				//SSG69
			_ExceptionWeaponInsert( 601002003 );				//K5
			_ExceptionWeaponInsert( 702001001 );				//M7
			_ExceptionWeaponInsert( 803007001 );				//K400(Grenade)
			_ExceptionWeaponInsert( 904007002 );				//SmokeGrenade

			if( PC_PREMIUM_CAFE == m_UserInfoBasic._pcCafe || PC_GARENA_GCA == m_UserInfoBasic._pcCafe )
			{
				_ExceptionWeaponInsert( 200004010, ITEM_ATTR_TYPE_PC_2 );		//P90 MC
			}
		}
		break;
	case LOCALE_CODE_INDONESIA		:
		{		
			// 인도네시아 와루넷 골드 이용자
			if( m_UserInfoBasic._pcCafe == PC_INDO_GOLD )	_ExceptionWeaponInsert( 601013006 );	// P99&HAK D CIC(GW)
		}
	case LOCALE_CODE_LATIN_AMERICA	:
	case LOCALE_CODE_MATCH			:
		{
			_ExceptionWeaponInsert( 100003004 );				//K2
			_ExceptionWeaponInsert( 200004006 );				//K1
			_ExceptionWeaponInsert( 300005003 );				//SSG69
			_ExceptionWeaponInsert( 601002003 );				//K5
			_ExceptionWeaponInsert( 702001001 );				//M7
			_ExceptionWeaponInsert( 803007001 );				//K400(Grenade)
			_ExceptionWeaponInsert( 904007002 );				//SmokeGrenade
		}
		break;
	case LOCALE_CODE_RUSSIA			:
		{
			_ExceptionWeaponInsert( 702001001 );				// M-7
			_ExceptionWeaponInsert( 803007001 );				// K-400
			_ExceptionWeaponInsert( 904007002 );				// Smoke
		}
		break;
	case LOCALE_CODE_TURKEY			:
	case LOCALE_CODE_BRAZIL			:
 	case LOCALE_CODE_NORTHAMERICA	:
	case LOCALE_CODE_ITALY			:
		{
			_ExceptionWeaponInsert( 601002003 );				//K5
			_ExceptionWeaponInsert( 702001001 );				//M7
			_ExceptionWeaponInsert( 803007001 );				//K400(Grenade)
			_ExceptionWeaponInsert( 904007002 );				//SmokeGrenade
		}
		break;
	case LOCALE_CODE_TAIWAN			:
		{
			_ExceptionWeaponInsert( 702001001 );				//M7
			_ExceptionWeaponInsert( 803007001 );				//K400(Grenade)
			_ExceptionWeaponInsert( 904007002 );				//SmokeGrenade
			
			switch ( m_UserInfoBasic._pcCafe )
			{
			case PC_NORMAL_CAFE :	
				_ExceptionWeaponInsert( 601002030,	ITEM_ATTR_TYPE_PC_1 );		// Desert Eagle EV
				_ExceptionWeaponInsert( 200004056,	ITEM_ATTR_TYPE_PC_1 );		// MP7 Non Ext
				break;
			}
		}
		break;
	case LOCALE_CODE_PHILIPPINES			:
		{
			_ExceptionWeaponInsert( 601002003 );				// K-5
			_ExceptionWeaponInsert( 702001001 );				// M-7
			_ExceptionWeaponInsert( 803007001 );				// K-400
			_ExceptionWeaponInsert( 904007002 );				// Smoke
		}
		break;
	case LOCALE_CODE_MIDDLE_EAST			:
		{
			_ExceptionWeaponInsert( 100003059 );				// AK-47
			_ExceptionWeaponInsert( 601002003 );				// K-5
			_ExceptionWeaponInsert( 702001001 );				// M-7
			_ExceptionWeaponInsert( 803007001 );				// K-400
			_ExceptionWeaponInsert( 904007002 );				// Smoke
		}
		break;
	case LOCALE_CODE_NONE			:
		{
			_ExceptionWeaponInsert( 100003004 );				//K2
			_ExceptionWeaponInsert( 200004006 );				//K1
			//_ExceptionWeaponInsert( 300005003 );				//SSG69
			_ExceptionWeaponInsert( 601002003 );				//K5
			_ExceptionWeaponInsert( 702001001 );				//M7
			_ExceptionWeaponInsert( 803007001 );				//K400(Grenade)
			_ExceptionWeaponInsert( 904007002 );				//SmokeGrenade		
		}
		break;
	}
	return; 
	*/
}

void CUserSession::_ExceptionCharacter( INT32 i32ThreadIdx )
{
	/*
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//기본으로 있으야 하는 품목
	_ExceptionCharaInsert( 1001001005 );					// 레드불스		MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,	CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,		CHARACTER_RES_ID_MALE_RED) );
	_ExceptionCharaInsert( 1001002006 );					// 엑시드풀		MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,	CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_MALE_BLUE) );
	_ExceptionCharaInsert( 1102003001 );					// 기본헬멧		MAKE_ITEM_FLAG(ITEM_CLASS_HEAD,		CHARACTER_CLASS_HEAD,		CHARACTER_TEAM_RB,		HEAD_RES_ID_DEFAULT) );

#if !defined( I3_DEBUG)
	switch (m_ui8LocaleCode)
	{
	case LOCALE_CODE_LATIN_AMERICA:
		{
			if(1 == g_pContextMain->m_i8CCModeOpen)
			{
				_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_Raptor) );		// CC랩터
				_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_STING) );		// CC스팅
				_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_ACID) );		// CC애시드
			}
			else
			{
				_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_Raptor) );		// CC랩터
			}			
		}
		break;
	case LOCALE_CODE_PHILIPPINES:	// 은 아직 예전 랩터
		{
			if(1 == g_pContextMain->m_i8CCModeOpen)
			{
				_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_Raptor) );		// CC랩터
				_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_STING) );		// CC스팅
				_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_ACID) );		// CC애시드
			}
			else
			{
				_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_Raptor) );		// 랩터
			}			
		}
		break;		
	default:
		{
			_ExceptionCharaInsert( 1006003041 );			// CC랩터	MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_Raptor) );
			_ExceptionCharaInsert( 1006003042 );			// CC스팅	MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_STING) );
			_ExceptionCharaInsert( 1006003043 );			// CC애시드	MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_ACID) );
		}
		break;
	}
#endif

	switch( m_ui8LocaleCode )
	{
	case LOCALE_CODE_INDONESIA		:		
		{
			if( PC_INDO_TELCOM_ITEM == m_UserInfoBasic._pcCafe )
			{	// 인니 텔콤 이벤트 반다나 줌. Mask - Bandana
				_ExceptionCharaInsert( 1105003003, ITEM_ATTR_TYPE_SPECIAL );
			}
			else if( m_UserInfoBasic._pcCafe == PC_INDO_GOLD )
			{
				// 인도네시아 와루넷 골드유저에게 반다나 줌
				_ExceptionCharaInsert( 1105003005 );	// Bandana (GW)

				// 인도네시아 와루넷 유저에게 데스마스크 줌
				//_ExceptionCharaInsert( 1104003120 );	// Death Mask (GW)
			}
		}
		break;
	case LOCALE_CODE_TAIWAN			:
		{
			switch ( m_UserInfoBasic._pcCafe )
			{
			case PC_NORMAL_CAFE :	
				_ExceptionCharaInsert( 1104003112,	ITEM_ATTR_TYPE_PC_1 );		// Mask White EV
				break;
			}
		}
		break;
	case LOCALE_CODE_THAILAND		:		break;
	case LOCALE_CODE_RUSSIA			:		break;
	case LOCALE_CODE_TURKEY			:		break;
	case LOCALE_CODE_BRAZIL			:		break;
	case LOCALE_CODE_LATIN_AMERICA	:		break;
	case LOCALE_CODE_ITALY			:		break;
	}
	return; 
	*/
}

void CUserSession::_ExceptionSpecial( INT32 i32ThreadIdx )
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//베레모 예외처리 입니다. 
	if( GET_USERTITLE( m_ui64GainUserTitle, DESIGNATION_TYPE_13 ) )
	{
		_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_ASSAULT) );
	}

	if( GET_USERTITLE( m_ui64GainUserTitle, DESIGNATION_TYPE_25 ) )
	{
		_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_SMG) );
	}

	if( GET_USERTITLE( m_ui64GainUserTitle, DESIGNATION_TYPE_19 ) )
	{
		_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_RIFLE) );
	}

	if( GET_USERTITLE( m_ui64GainUserTitle, DESIGNATION_TYPE_39 ) )
	{
		_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_SHOTGUN) );
	}

	if( GET_USERTITLE( m_ui64GainUserTitle, DESIGNATION_TYPE_34 ) )
	{
		_ExceptionCharaInsert( MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_HANDGUN) );		
	}
}

void CUserSession::_ExceptionTitleItem( INT32 i32ThreadIdx, UINT8 ui8TitleId )
{
	UINT32 ui32ItemID = 0;
	UINT32 ui32ItemArg = 0;
	UINT8  ui8GetType = 5;

	switch( m_ui8LocaleCode )
	{		
	//case LOCALE_CODE_MIDDLE_EAST	:	
	//case LOCALE_CODE_PHILIPPINES	:		// 필리핀은 지급하지 않도록 - 서동권
	//case LOCALE_CODE_TAIWAN		:
	case LOCALE_CODE_INDONESIA		:
	case LOCALE_CODE_BRAZIL			:
	case LOCALE_CODE_TURKEY			:
	case LOCALE_CODE_LATIN_AMERICA	:
	case LOCALE_CODE_KOREA			:
	case LOCALE_CODE_NORTHAMERICA	:	//북미 지급되지 않도록 수정 - 박희찬	//북미측 요청으로 다시 지급되도록 수정 - 서동권 ( 내구도제 도입시 다시 빼기로 협의. 2012-08-30 )
	case LOCALE_CODE_ITALY			:
	case LOCALE_CODE_MIDDLE_EAST	:

	case LOCALE_CODE_MATCH			:
		switch( ui8TitleId )
		{
		case 8:		// 일반 돌격 - SG 550 S.
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 100003014, 10 );
			}
			break;
		case 10:	// 수석 돌격 - G36C Ext.
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 100003013, 10 );
			}
			break;
		case 12:	// 돌격 지휘관 - AK SOPMAD, AUG A3
			{
				if( m_ui8LocaleCode != LOCALE_CODE_PHILIPPINES )
				{
					g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 100003015, 10 );
					g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 100003036, 10 );	// AUG A3
				}
			}
			break;
		case 26:	// 일반 침투 - Dual Knife
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 702015001, 10 );
			}
			break;
		case 28:	// 특등 침투장교 - Amok Kukri
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 702001004, 10 );
			}
			break;
		case 29:	// 침투 지휘관 - Mini Axe
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 702001007, 10 );
			}
			break;
		case 14:	// 일반 저격 - Dragunov G.
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 300005006, 10 );
			}
			break;
		case 16:	// 수석 저격 - PSG-1 S.
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 300005007, 10 );
			}
			break;
		case 18:	// 저격 지휘관 - L115A1
			{
				if( m_ui8LocaleCode != LOCALE_CODE_PHILIPPINES )
				{
					g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 300005005, 10 );
				}
			}
			break;
		case 30:	// 일반 사수 - P99&HAK
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 601013001, 10 );
			}
			break;
		case 32:	// 특등 사수장교 - Dual Handgun
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 601014001, 10 );
			}
			break;
		case 33:	// 사격 지휘관 - Dual D-Eagle
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 601014002, 10 );
			}
			break;
		case 20:	// 일반 습격 - MP5K G./ Spectre W.
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 200004007, 10 );
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 200004009, 10 );
			}
			break;
		case 22:	// 수석 습격 - P90 Ext.
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 200004011, 10 );
			}
			break;
		case 24:	// 습격 지휘관 - Kriss Super V
			{
				switch(m_ui8LocaleCode)
				{
				case LOCALE_CODE_PHILIPPINES:
					break;
				case LOCALE_CODE_INDONESIA:
					{
						g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 200004052, 10 );		// Kriss S.V Batik
					}
					break;				
				default:
					{
						g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 200004013, 10 );		// Kriss S.V
					}
					break;
				}
			}
			break;
		case 40:	// 일반 폭파 - C-5
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 803007004, 10 );
			}
			break;
		case 42:	// 수석 폭파 - WP Smoke
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 904007005, 10 );
			}
			break;
		case 35:	// 일반 타격 - 870MCS W.
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 400006004, 10 );
			}
			break;	break;	
		case 37:	// 특등 타격장교 - SPAS - 15
			{
				g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, 400006003, 10 );
			}
			break;
		case 1:		// 일반
		case 2:		// 선임
		case 3:		// 특등
		case 4:		// 특수
		case 5:		// 돌격
		case 6:		// 저격
		case 7:		// 습격
		case 9:		// 선임 돌격
		case 11:	// 특등 돌격
		case 13:	// 광폭한 맹수
		case 15:	// 선임 저격
		case 17:	// 특등 저격
		case 19:	// 은밀한 암살자
		case 21:	// 선임 습격
		case 23:	// 특등 습격
		case 25:	// 교활한 침입자
		case 27:	// 수석 침투
		case 31:	// 수석 사수
		case 34:	// 예리한 명사수
		case 38:	// 타격 지휘관
		case 39:	// 난폭한 파괴자
		case 41:	// 선임 폭파
		case 43:	// 특등 폭파
		case 44:	// 폭파 지휘관
			break;
		}
		break;		
	}
}