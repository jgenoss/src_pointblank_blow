#include "pch.h"
#include "CharaInfoDataBase.h"

#include "DinoInfo.h"
#include "EquipInfo.h"
#include "i3Base/ColorString.h"

I3_CLASS_INSTANCE( CharaInfoDataBase);//, i3GameNode);

CharaInfoDataBase::CharaInfoDataBase( void)
{
	m_pCharaShift = nullptr;
	m_pGlobalInfo = nullptr;

	m_GlobalShift = 0;
	for(INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		m_pEquipmentShift[i] = nullptr;
	}
}

CharaInfoDataBase::~CharaInfoDataBase( void)
{
	I3MEM_SAFE_FREE( m_pCharaShift);
	I3_SAFE_RELEASE( m_pGlobalInfo);
	I3_SAFE_RELEASE( m_pGlobalDinoInfo);

	for(INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		i3::cu::for_each_safe_release_clear( m_EquipmentInfoList[i], &CEquipInfo::Release );
		I3MEM_SAFE_FREE( m_pEquipmentShift[i]	);
	}

	I3MEM_SAFE_FREE( m_pCharaShift);

	i3::cu::for_each_safe_release_clear(m_UITemplatePartsInfos, &i3NamedElement::Release);
	i3::cu::for_each_safe_release_clear(m_UITemplateCharaInfos, &i3NamedElement::Release);
}

void CharaInfoDataBase::__ReadAICharacterInfo( void)
{
	INT32 i, j, cnt;
	// AI Character valid check
	// 사용 가능 무기 확인에 사용할 테이블 결정	

	// 바뀐 아이템 아이디로 인해 잠시 주석을 합니다.	-- 20090611 현우
	// DB 에 등록되어있는 아이템 ID가 예전꺼이기 떄문에 새로운 아이템 ID 로 변경해주는 작업이 꼭 필요합니다. -- 20090611 현우
//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	{
		i3RegKey * pRoot = Config::_GetRegRoot( "AICharacter/AICharacter");
		I3ASSERT( pRoot != nullptr);

		char conv[MAX_PATH];
		char szNick[ MAX_PATH];

		for( cnt = 1; cnt <= 10; cnt++)
		{
			sprintf_s( conv, "Level_%d", cnt);

			i3RegKey * pLevelRoot = Config::FindKey( conv, pRoot);
			I3ASSERT( pLevelRoot != nullptr);

			bool bCheck = false;

			// 등록된 NPC 안에서 선택해야한다.
			for( i = 0; i < SLOT_MAX_COUNT / 2 ; i++)
			{
				I3ASSERT( i < pLevelRoot->getChildCount());	
				i3RegKey * pCharaKey = (i3RegKey*)pLevelRoot->getChild( i);
				I3ASSERT( pCharaKey != nullptr);

				// 출력 정보 (닉네임, 성별, 계급, 사용무기, 사용장비)
				i3::safe_string_copy( szNick, pCharaKey->GetName(), MAX_PATH );	
				INT32 rank = -1;
				FIND_REG_DATA(pCharaKey, "Info_Rank", &rank);

				//////
				bCheck = false;
				INT32 iItem = -1;
				FIND_REG_DATA(pCharaKey, LS_STR("Weapon_Primary"), &iItem);
				for( j = 0; j < ::GetAiUsablePrimaryWeaponCount(); j++)
				{	
					if( g_vecAI_UsablePrimaryWeaponFlag[ j] == iItem )
					{
						bCheck = true;
						break;
					}
				}
				if( bCheck == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "invalid ai weaponset (%s) Primary %d", szNick, iItem );
				}

				//////
				bCheck = false;
				iItem = -1;
				FIND_REG_DATA(pCharaKey, LS_STR("Weapon_Secondary"), &iItem);
				for( j = 0; j < ::GetAiUsableSecondaryWeaponCount(); j++)
				{	
					if( g_vecAI_UsableSecondaryWeaponFlag[ j] == iItem )
					{
						bCheck = true;
						break;
					}
				}
				if( bCheck == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "invalid ai weaponset (%s) Secondary %d", szNick, iItem );
				}

				//////
				bCheck = false;
				iItem = -1;
				FIND_REG_DATA(pCharaKey, LS_STR("Weapon_Melee"), &iItem);
				for( j = 0; j < ::GetAiUsableMeleeWeaponCount(); j++)
				{	
					INT32 iTemp = g_vecAI_UsableMeleeWeaponFlag[ j];
					if( iTemp == iItem )
					{
						bCheck = true;
						break;
					}
				}
				if( bCheck == false)
				{
					//I3PRINTLOG(I3LOG_FATAL,  "invalid ai weaponset (%s) Melee %d, %d", szNick, ((iItem >> 8) & 0xFF), (iItem & 0xFF) );
				}

				//////
				bCheck = false;
				iItem = -1;
				FIND_REG_DATA(pCharaKey, "Weapon_Throw", &iItem);
				for( j = 0; j < ::GetAiUsableThrowingWeaponCount(); j++)
				{	
					if( g_vecAI_UsableThrowingWeaponFlag[ j] == iItem )
					{
						bCheck = true;
						break;
					}
				}
				if( bCheck == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "invalid ai weaponset (%s) Throw %d", szNick, iItem );
				}

				/////
				bCheck = false;
				iItem = -1;
				FIND_REG_DATA(pCharaKey, "Weapon_Item", &iItem);
				for( j = 0; j < ::GetAiUsableItemWeaponCount(); j++)
				{	
					if( g_vecAI_UsableItemWeaponFlag[ j] == iItem )
					{
						bCheck = true;
						break;
					}
				}
				if( bCheck == false)
				{
//					I3PRINTLOG(I3LOG_FATAL,  "invalid ai weaponset (%s) item %d", szNick, iItem );
				}
			}
		}
	}
//#endif
}

bool CharaInfoDataBase::Create( void)
{
//	CEquipInfo::RegisterMeta();
//	CCharaInfo::RegisterMeta();

	I3ASSERT( m_pGlobalInfo == nullptr);

	INT32 charaCnt, dinoCnt;
	INT32 i =0;

#if defined( I3_DEBUG)
	m_GlobalShift = 0;
#else
	m_GlobalShift = (UINT8)((i3Math::Rand() % 7) + 1);
#endif

	// 캐릭터 정보를 읽습니다.

	i3RegKey * pDinoRoot = Config::_GetRegRoot("Character/Dino");
	I3ASSERT( pDinoRoot != nullptr);

	dinoCnt = pDinoRoot->getChildCount();

	{	
		i3RegKey * pRoot = Config::_GetRegRoot("Character/Character");
		I3ASSERT( pRoot != nullptr);

		m_pGlobalInfo = CCharaGlobal::new_object();
		m_pGlobalInfo->ReadInfo( pRoot);
		m_pGlobalInfo->OnEncryptInfo( m_GlobalShift);

		{
			i3RegKey * pModeKey = Config::_GetRegRoot("Character/Mode");
			i3RegKey * pRoot2 = Config::FindKey("HeadHunter_killer", pModeKey);
			I3ASSERT( pRoot2 != nullptr);
			m_pGlobalInfo->ReadDamageByModeInfo( pRoot2 );
		}
		
		charaCnt = pRoot->getChildCount();

		I3MEM_SAFE_FREE( m_pCharaShift);
		m_pCharaShift = (UINT8*) i3MemAlloc( sizeof(UINT8) * (charaCnt + dinoCnt) );

		m_EquipmentInfoList[ EQUIP::ePART_CHARA].clear();
		m_EquipmentInfoList[ EQUIP::ePART_CHARA].resize( charaCnt + dinoCnt);

		for( i = 0; i < charaCnt; ++i)
		{
			i3RegKey * pKey = ( i3RegKey*)pRoot->getChild( i);
			I3ASSERT( pKey != nullptr);

			CCharaInfo * pInfo = CCharaInfo::new_object();
			pInfo->ReadInfo( pKey);

#if defined( I3_DEBUG)
			m_pCharaShift[i] = 0;
#else
			m_pCharaShift[i] = (UINT8)(i3Math::Rand()%7 + 1);
#endif

			pInfo->OnEncryptInfo( m_pCharaShift[i]);
			m_EquipmentInfoList[ EQUIP::ePART_CHARA][ i] = pInfo;
		}
	}

	//장신구아이템 생성
	EquipmentCreate();
	__ReadUITemplateInfo_Parts();
	__ReadUITemplateInfo_Chara();

	// 공룡 정보 읽기
	{
		m_pGlobalDinoInfo = CCharaGlobal::new_object();
		m_pGlobalDinoInfo->ReadInfo( pDinoRoot);
		m_pGlobalDinoInfo->OnEncryptInfo( m_GlobalShift);

		for( i = 0; i < dinoCnt; ++i)
		{	
			i3RegKey * pKey = ( i3RegKey*)pDinoRoot->getChild( i);
			I3ASSERT( pKey != nullptr);

			CDinoInfo * pInfo = CDinoInfo::new_object();

			pInfo->ReadInfo( pKey);

			INT32 idx = charaCnt + i;

#if defined( I3_DEBUG)
			m_pCharaShift[idx] = 0;
#else
			m_pCharaShift[idx] = (UINT8)(i3Math::Rand()%7 + 1);
#endif

			pInfo->OnEncryptInfo( m_pCharaShift[idx]);
			m_EquipmentInfoList[ EQUIP::ePART_CHARA][ idx] = pInfo;
		}
	}

//	__ReadAICharacterInfo();

	return true;
}

INT32 CharaInfoDataBase::__ReadPartsInfo( EQUIP::ePART part, INT32 startIdx, i3RegKey * pRoot)
{
	if( pRoot != nullptr)
	{
		INT32 cnt = pRoot->getChildCount();
		for( INT32 j = 0; j < cnt; j++)
		{
			INT32 curIdx = startIdx + j;
			i3RegKey * pKey = (i3RegKey*) pRoot->getChild( j);

			if (pKey == nullptr)
			{
				I3ASSERT(pKey != nullptr);	continue;
			}

			CEquipInfo * pInfo = nullptr;

			switch( part)
			{
			case EQUIP::ePART_HEADGEAR:		pInfo = CHeadInfo::new_object();		break;
			case EQUIP::ePART_FACEGEAR:		pInfo = CFacegearInfo::new_object();	break;
			case EQUIP::ePART_BELT:			pInfo = CBeltInfo::new_object();		break;
			case EQUIP::ePART_GLOVE:		pInfo = CGloveInfo::new_object();		break;
			case EQUIP::ePART_HOLSTER:		pInfo = CHolsterInfo::new_object();		break;
			case EQUIP::ePART_VEST:			pInfo = CJacketInfo::new_object();		break;
			case EQUIP::ePART_PANT:			pInfo = CPoketInfo::new_object();		break;
			case EQUIP::ePART_SKIN:			pInfo = CSkinInfo::new_object();		break;
			case EQUIP::ePART_BERET:		pInfo = CBeretInfo::new_object();		break;
			default :
				I3PRINTLOG(I3LOG_FATAL,  "invalid part type!!!");
				break;
			}

			if (pInfo == nullptr)
				continue;

			pInfo->ReadInfo( pKey);

		#if defined( I3_DEBUG)
			m_pEquipmentShift[part][ curIdx] = 0;
		#else
			m_pEquipmentShift[part][ curIdx] = (UINT8)(i3Math::Rand()%7+1);
		#endif

			pInfo->OnEncryptInfo( m_pEquipmentShift[part][ curIdx]);
			m_EquipmentInfoList[part][ curIdx] = pInfo;
		}

		return cnt;
	}

	return 0;
}

bool CharaInfoDataBase::EquipmentCreate( void)
{
	//Equipment 를 읽습니다.

	i3RegKey * pEquipmentRoot = nullptr;

	if( i3::contain_string( ::GetCommandLine(), "OLD_EQUIPMENT") >= 0)
		pEquipmentRoot = Config::_GetRegRoot("Equipment_Backup/Equipment");
	else
		pEquipmentRoot = Config::_GetRegRoot("Equipment/Equipment");

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		if( i == EQUIP::ePART_CHARA)
			continue;	// Character는 이전에 이미 로딩했삼.

		i3RegKey * pRoot = Config::FindKey( AVATAR::getDataBaseName( (EQUIP::ePART) i).c_str(), pEquipmentRoot);

		if( pRoot != nullptr)
		{
			I3MEM_SAFE_FREE( m_pEquipmentShift[ i]);
			m_EquipmentInfoList[ i].clear();

			/*if( i == EQUIP::ePART_SKIN)
			{
				INT32 totalCnt = 0;
				i3RegKey * pCommon = Config::FindKey( "Common", pRoot);
				if( pCommon != nullptr)
					totalCnt += pCommon->getChildCount();

				i3RegKey * pMan = Config::FindKey( "Man", pRoot);
				if( pMan != nullptr)
					totalCnt += pMan->getChildCount();

				i3RegKey * pWoman = Config::FindKey( "Woman", pRoot);
				if( pWoman != nullptr)
					totalCnt += pWoman->getChildCount();

				m_pEquipmentShift[ i] = (UINT8*) i3MemAlloc( sizeof(UINT8) * totalCnt);
				m_EquipmentInfoList[ i].resize( totalCnt);

				INT32 startIdx = 0;
				startIdx += __ReadPartsInfo( (EQUIP::ePART) i, startIdx, pCommon);
				startIdx += __ReadPartsInfo( (EQUIP::ePART) i, startIdx, pMan);
				startIdx += __ReadPartsInfo( (EQUIP::ePART) i, startIdx, pWoman);
			}
			else*/
			{
				INT32 cnt = pRoot->getChildCount();

				m_pEquipmentShift[ i] = (UINT8*) i3MemAlloc( sizeof(UINT8) * cnt);
				m_EquipmentInfoList[ i].resize( cnt);

				__ReadPartsInfo( (EQUIP::ePART) i, 0, pRoot);
			}
		}
	}

	return true;
}

#if !defined( I3_NO_PROFILE)
void CharaInfoDataBase::OnReloadProperty()
{
	i3RegKey * pRoot = Config::_GetRegRoot("Character/Character");
	I3ASSERT( pRoot != nullptr);

	if( m_pGlobalInfo != nullptr)
	{
		m_pGlobalInfo->ReadInfo( pRoot);
	}

	INT32 i;
	INT32 cnt = pRoot->getChildCount();
	for( i = 0; i < cnt; ++i)
	{
		i3RegKey * pKey = (i3RegKey*)pRoot->getChild( i);

		CCharaInfo * pInfo = (CCharaInfo*) m_EquipmentInfoList[ EQUIP::ePART_CHARA][i];
		
		pInfo->ReadInfo( pKey);
	}

	pRoot = Config::_GetRegRoot("Character/UITemplate");
	I3ASSERT( pRoot != nullptr);
	cnt = pRoot->getChildCount();
	for ( i = 0; i< cnt; ++i)
	{
		i3RegKey * pKey = (i3RegKey*)pRoot->getChild( i);
		CUITemplateInfo_Chara * pInfo = (CUITemplateInfo_Chara*)m_UITemplateCharaInfos[i];
		pInfo->ReadInfo(pKey);
	}

	if( i3::contain_string( ::GetCommandLine(), "OLD_EQUIPMENT") >= 0 )
		pRoot = Config::_GetRegRoot("Equipment_Backup/UITemplate");
	else
		pRoot = Config::_GetRegRoot("Equipment/UITemplate");

	I3ASSERT( pRoot != nullptr);
	cnt = pRoot->getChildCount();
	for ( i = 0; i< cnt; ++i)
	{
		i3RegKey * pKey = (i3RegKey*)pRoot->getChild( i);
		CUITemplateInfo_Parts * pInfo = (CUITemplateInfo_Parts*)m_UITemplatePartsInfos[i];
		pInfo->ReadInfo(pKey);
	}
}
#endif

bool CharaInfoDataBase::GetCharacterName( T_ItemID ItemID, i3::rc_wstring& out) const
{

	const CEquipInfo * pInfo = GetInfo(ItemID);
	if ( ! pInfo )
	{
	//	i3::snprintf(pszBuffer, size, "%s (%d)", INVALID_TEXT, ItemID);
		i3::wstring wstr;
		i3::sprintf(wstr, L"%s (%d)", INVALID_TEXTW, ItemID); 
		::ColorString(wstr, 255, 0, 0);
		out = wstr;
		return false;
	}

	pInfo->getName(out);

	if( out.empty() )
	{
		i3::wstring wstr;
		i3::sprintf(wstr, L"No Name (%d)", ItemID);
		::ColorString(wstr, 255, 0, 0);
		out = wstr;
	}

	return true;
}

bool	CharaInfoDataBase::GetDescription( T_ItemID ItemID, i3::rc_wstring& out)
{

	const CEquipInfo * pInfo = GetInfo(ItemID);
	if ( ! pInfo )
	{
		i3::sprintf(out, L"%s (%d)", INVALID_TEXTW, ItemID);
		return false;
	}

	pInfo->getDescription( out);

	if( out.empty() )
		i3::sprintf(out, L"No Description (%d)", ItemID);

	return true;
}

const CCharaInfo * CharaInfoDataBase::GetCharaDBInfo( T_ItemID ItemID)
{
	EQUIP::ePART part = EQUIP::ItemID2PartsType( ItemID);

	if( part == EQUIP::ePART_CHARA)
	{
		CHARA_RES_ID ResID = CHARA::ItemID2ResID( ItemID);
		CEquipInfo * pInfo = (CEquipInfo*) GetInfo( part, ResID);

	#if defined( I3_DEBUG)
		if( pInfo != nullptr)
		{
			I3ASSERT( i3::same_of<CCharaInfo*>(pInfo) ||
					  i3::same_of<CDinoInfo*>(pInfo) );
		}
	#endif

		return (const CCharaInfo*) pInfo;
	}

	return nullptr;
}

const CEquipInfo *	CharaInfoDataBase::GetInfo( T_ItemID ItemID) const
{
	EQUIP::ePART part = EQUIP::ItemID2PartsType( ItemID);

	if( part == EQUIP::ePART_UNKNOWN)
	{
		I3ASSERT( part != EQUIP::ePART_UNKNOWN);
		return nullptr;
	}

	for( UINT32 i = 0; i < m_EquipmentInfoList[ part].size(); i++)
	{
		const CEquipInfo * pInfo = (const CEquipInfo *)m_EquipmentInfoList[ part][ i];
		if( pInfo != nullptr && pInfo->GetITEMID() == ItemID)
			return pInfo;
	}

	return nullptr;
}

const CEquipInfo * CharaInfoDataBase::GetInfo( EQUIP::ePART part, EQUIP::RES_ID ResID) const
{
	if( part == EQUIP::ePART_UNKNOWN)
	{
		I3ASSERT( part != EQUIP::ePART_UNKNOWN);
		return nullptr;
	}

	for( UINT32 i = 0; i < m_EquipmentInfoList[ part].size(); i++)
	{
		const CEquipInfo * pInfo = (const CEquipInfo *)m_EquipmentInfoList[ part][ i];
		if( pInfo != nullptr && pInfo->GetResID() == ResID)
			return pInfo;
	}

	return nullptr;
}


const CEquipInfo * CharaInfoDataBase::GetInfoByIndex( EQUIP::ePART part, INT32 idx)
{
	if( part == EQUIP::ePART_UNKNOWN)
	{
		I3ASSERT( part != EQUIP::ePART_UNKNOWN);
		return nullptr;
	}

	I3ASSERT(idx < (INT32)m_EquipmentInfoList[part].size());
	return (const CEquipInfo *)m_EquipmentInfoList[ part][ idx];
}

INT32 CharaInfoDataBase::GetInfoCount( EQUIP::ePART part)
{
	return (INT32) m_EquipmentInfoList[ part].size();
}

void CharaInfoDataBase::__ReadUITemplateInfo_Parts()
{
	i3RegKey* rootKey = nullptr;
	
	if( i3::contain_string( ::GetCommandLine(), "OLD_EQUIPMENT") >= 0 )
		rootKey = Config::_GetRegRoot("Equipment_Backup/UITemplate");
	else
		rootKey = Config::_GetRegRoot("Equipment/UITemplate");

	I3ASSERT(rootKey != nullptr);

	if (rootKey)
	{
		INT32 count = rootKey->getChildCount();
		for (INT32 i=0; i<count; i++)
		{
			i3RegKey* subKey = static_cast<i3RegKey*>(rootKey->getChild(i));

			CUITemplateInfo_Parts* info = CUITemplateInfo_Parts::new_object();
			info->ReadInfo(subKey);

			m_UITemplatePartsInfos.push_back(info);
		}
	}
}

size_t CharaInfoDataBase::GetUITemplateInfoCount_Parts() const
{
	return m_UITemplatePartsInfos.size();
}

const CUITemplateInfo_Parts* CharaInfoDataBase::GetUITemplateInfo_Parts(const size_t i) const
{
	return m_UITemplatePartsInfos.at(i);
}

void CharaInfoDataBase::__ReadUITemplateInfo_Chara()
{
	i3RegKey* rootKey = Config::_GetRegRoot("Character/UITemplate");
	I3ASSERT(rootKey != nullptr);

	if (rootKey)
	{
		INT32 count = rootKey->getChildCount();
		for (INT32 i=0; i<count; i++)
		{
			i3RegKey* subKey = static_cast<i3RegKey*>(rootKey->getChild(i));

			CUITemplateInfo_Chara* info = CUITemplateInfo_Chara::new_object();
			info->ReadInfo(subKey);

			m_UITemplateCharaInfos.push_back(info);
		}
	}
}

size_t CharaInfoDataBase::GetUITemplateInfoCount_Chara() const
{
	return m_UITemplateCharaInfos.size();
}

const CUITemplateInfo_Chara* CharaInfoDataBase::GetUITemplateInfo_Chara(const size_t i) const
{
	return m_UITemplateCharaInfos.at(i);
}