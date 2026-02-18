#include "pch.h"
#include "EquipShapeManager.h"
#include "StageDef.h"
#include "GlobalVariables.h"
#include "Weapon/WeaponItemID.h"
#include "Weapon/WeaponInfo/WeaponDinoInfo.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "i3Framework/i3UI/i3UIFilePathMgr.h"

#include "ClientStageInfoUtil.h"
#include "CSI_MAP.h"

I3_CLASS_INSTANCE(CEquipShapeManager);//, i3ElementBase);


#define MAX_EXPENDABLE_ITEM_TEX_COUNT	2		//	소모성 캐시 아이템 이미지 (현재 1장)
#define MAX_MAINTENANCE_ITEM_TEX_COUNT	2		//	기간제 캐시 아이템 이미지 (현재 2장)
#define MAX_SET_ITEM_TEX_COUNT	2		//	세트 아이템 이미지 (현재 2장)
#define MAX_MASK_ITEM_TEX_COUNT	7	//마스크 아이템 이미지 (현재 4장) 1부터 시작한다.

#define MAX_WEAPON_KILLMARK_TEX			6		//	킬마크 이미지 (현재 2장) 
#define MAX_WEAPON_SELECT_TEX			3		//	무기 선택 이미지 (현재 2장)

#define MAX_CHARACTER_ITEM_TEXT			3


//	한 텍스처당 이미지 등록 갯수
#define WEAPON_KILLMARK_SHAPE_COLUMN		8
#define WEAPON_KILLMARK_SHAPE_ROW			11

//	한 텍스처당 이미지 등록 갯수
#define WEAPON_SELECT_SHAPE_COLUMN			5
#define WEAPON_SELECT_SHAPE_ROW				18

//	한 페이지당 무기 이미지 등록 가능
#define WEAPON_KILLMARK_SHAPE_PER_PAGE		(WEAPON_KILLMARK_SHAPE_COLUMN * WEAPON_KILLMARK_SHAPE_ROW)	
#define WEAPON_SELECT_SHAPE_PER_PAGE		(WEAPON_SELECT_SHAPE_COLUMN * WEAPON_SELECT_SHAPE_ROW)


#define CHARACTER_SHAPE_COLUMN		3
#define CHARACTER_SHAPE_ROW			6

enum {CHARACTER_SHAPE_PER_PAGE = CHARACTER_SHAPE_COLUMN * CHARACTER_SHAPE_ROW};


#define		TEX_CHARA_WIDTH				135
#define		TEX_CHARA_HEIGHT			78
#define		TEX_CHARA_HORZ_COUNT		3

#define		TEX_HELMET_WIDTH			135
#define		TEX_HELMET_HEIGHT			78
#define		TEX_HELMET_HORZ_COUNT		3

#define		TEX_BERET_WIDTH				135
#define		TEX_BERET_HEIGHT			78
#define		TEX_BERET_HORZ_COUNT		3

#define		TEX_MASK_WIDTH				135
#define		TEX_MASK_HEIGHT				78
#define		TEX_MASK_HORZ_COUNT			3

#define		TEX_ITEM_WIDTH				90
#define		TEX_ITEM_HEIGHT				65
#define		TEX_ITEM_HORZ_COUNT			5

#define		TEX_MAP_WIDTH				166
#define		TEX_MAP_HEIGHT				88
#define		TEX_MAP_HORZ_COUNT			3

#define		TEX_RIBBON_U				166
#define		TEX_RIBBON_V				460
#define		TEX_RIBBON_WIDTH			46
#define		TEX_RIBBON_HEIGHT			15

#define		TEX_ENSIGN_U				214
#define		TEX_ENSIGN_V				447
#define		TEX_ENSIGN_WIDTH			52
#define		TEX_ENSIGN_HEIGHT			47

#define		TEX_MEDAL_U					258
#define		TEX_MEDAL_V					447
#define		TEX_MEDAL_WIDTH				52
#define		TEX_MEDAL_HEIGHT			47

#define		TEX_MASTER_U				306
#define		TEX_MASTER_V				447
#define		TEX_MASTER_WIDTH			52
#define		TEX_MASTER_HEIGHT			47

POINT32		CalcuTextureUV(INT32 Index, INT32 TexWidth, INT32 TexHeight, INT32 HorzCount)
{
	POINT32 UV;
	UV.x = (1 + (Index % HorzCount) * (TexWidth + 1));
	UV.y = (1 + (Index / HorzCount) * (TexHeight + 1));
	return UV;
}


CEquipShapeManager::CEquipShapeManager()
{
	InitTextureList();

	// 임무카드	
	m_pCardTexture = nullptr;

	// 약장
	m_BadgeTexture = nullptr;
	m_pEventCardSet = nullptr;
	m_pWeaponDescImage = nullptr;
	m_pBossStartTexture = nullptr;
	m_iMaxWeaponKillSize = 0;
}

CEquipShapeManager::~CEquipShapeManager()
{
	I3_SAFE_RELEASE(m_pEventCardSet);
	I3_SAFE_RELEASE(m_BadgeTexture);	
	I3_SAFE_RELEASE(m_pCardTexture);
	I3_SAFE_RELEASE(m_pBossStartTexture);

	TextureFileList::iterator It = m_TextureList.begin();
	TextureFileList::iterator End = m_TextureList.end();

	for(; It != End; ++It)
	{
		i3::cu::for_each_safe_release_clear(It->second, &i3Texture::Release);
	}
	
	m_TextureList.clear();
}

void CEquipShapeManager::InitTextureList()
{
#if !defined(USE_LOCALE_UI)
	m_TextureList.insert( TextureFileList::value_type("Chara", i3::vector<i3Texture*>() ) );
	m_TextureList.insert( TextureFileList::value_type("Weapon", i3::vector<i3Texture*>() ) );
	m_TextureList.insert( TextureFileList::value_type("Head", i3::vector<i3Texture*>() ) );
	m_TextureList.insert( TextureFileList::value_type("Mask", i3::vector<i3Texture*>() ) );
	m_TextureList.insert( TextureFileList::value_type("Beret", i3::vector<i3Texture*>()) );

	m_TextureList.insert( TextureFileList::value_type("Count", i3::vector<i3Texture*>()) );
	m_TextureList.insert( TextureFileList::value_type("Perido", i3::vector<i3Texture*>()) );
	m_TextureList.insert( TextureFileList::value_type("Set", i3::vector<i3Texture*>()) );

	m_TextureList.insert( TextureFileList::value_type("Map", i3::vector<i3Texture*>()) );

	m_TextureList.insert( TextureFileList::value_type("WeaponSelect", i3::vector<i3Texture*>()) );
#endif

	m_TextureList.insert( TextureFileList::value_type("WeaponKill", i3::vector<i3Texture*>()) );
}

#if !defined(USE_LOCALE_UI)
void CEquipShapeManager::CreateGlobalImageRes(void)
{	
	char filename[MAX_PATH] = "";

	i3::vector<i3Texture*>& List = GetTextureList("WeaponSelect");

	List.resize(MAX_WEAPON_SELECT_TEX);

	for(INT32 i=0; i<MAX_WEAPON_SELECT_TEX; i++)
	{
		i3::snprintf( filename, sizeof(filename), "Gui/WeaponShape/Weapon_Select%d.i3i", i);
		i3Texture * pTexture = g_pFramework->GetResourceManager()->LoadTexture( filename);
		pTexture->setCommonRes(true);
		List[i] = pTexture;
	}
}

void CEquipShapeManager::DestroyGlobalImageRes(void) 	
{
	i3::cu::for_each_safe_release_clear(GetTextureList("WeaponSelect"), &i3Texture::Release); 
}

void CEquipShapeManager::CreateReadyStageImageRes(void)
{	
	char temp[256] = "";

	i3Texture * pTexture = nullptr;
	i3ResourceManager * pResManager = g_pFramework->GetResourceManager();
	I3ASSERT(pResManager != nullptr);

	const char * pCharacterFileName[MAX_CHARACTER_ITEM_TEXT] =
	{
		"Gui/Item/item_cha.i3i",
		"Gui/Item/item_cha_01.i3i",
		"Gui/Item/item_cha_02.i3i",
	};

	
	{
		i3::vector<i3Texture*>& List = GetTextureList("Chara");

		List.resize(MAX_CHARACTER_ITEM_TEXT);
		for ( INT32 i = 0 ; i < MAX_CHARACTER_ITEM_TEXT ; ++i )
		{
			pTexture = pResManager->LoadTexture(pCharacterFileName[i]);
			List[i] = pTexture;
		}
	}
	{
		i3::vector<i3Texture*>& List = GetTextureList("Map");
		pTexture = pResManager->LoadTexture("Gui/Interface/UI_Source_MapImage.i3i");
		List.push_back(pTexture);
	}
	
	{
		i3::vector<i3Texture*>& List = GetTextureList("Head");
		pTexture = pResManager->LoadTexture("Gui/Item/item_head.i3i");
		List.push_back(pTexture);
	}

	{
		// ENABLE_DESIGNATION_INTEGRATION
		i3::vector<i3Texture*>& List = GetTextureList("Beret");
		pTexture = pResManager->LoadTexture("Gui/Item/item_Beret_New.i3i");
		List.push_back(pTexture);
	}

	{
		//마스크 아이템 이미지 로딩
		i3::vector<i3Texture*>& List = GetTextureList("Mask");
		for(INT32 i=1; i<MAX_MASK_ITEM_TEX_COUNT; i++)
		{
			sprintf_s(temp, "Gui/Item/item_Mask_%d.i3i", i);

			i3Texture * pMaskItemTex = pResManager->LoadTexture(temp);
			if ( pMaskItemTex != nullptr)
				List.push_back( pMaskItemTex);
		}
	}
	// 임무카드	
	I3ASSERT( m_pCardTexture == nullptr); 	
	m_pCardTexture = pResManager->LoadTexture("Gui/Card/item_card.i3i");

	// 약장
	//ENABLE_DESIGNATION_INTEGRATION
	I3ASSERT( m_BadgeTexture == nullptr);
	m_BadgeTexture = pResManager->LoadTexture("Gui/Main/UI_Source02.i3i");


	// 소모성 캐시 아이템 이미지 로딩
	{
		i3::vector<i3Texture*>& List = GetTextureList("Count");
		for(INT32 i=0; i<MAX_EXPENDABLE_ITEM_TEX_COUNT; i++)
		{
			sprintf_s(temp, "Gui/Item/item_cash_A%d.i3i", i);

			i3Texture * pExpendableItemTex = pResManager->LoadTexture(temp);
			if ( pExpendableItemTex != nullptr)
				List.push_back( pExpendableItemTex);
		}
	}

	{
		//	기간제 캐시 아이템 이미지 로딩
		i3::vector<i3Texture*>& List = GetTextureList("Perido");
		for(INT32 i=0; i<MAX_MAINTENANCE_ITEM_TEX_COUNT; i++)
		{
			sprintf_s(temp, "Gui/Item/item_cash_B%d.i3i", i);

			i3Texture * pMaintenanceItemTex = pResManager->LoadTexture(temp);
			if ( pMaintenanceItemTex != nullptr)
				List.push_back( pMaintenanceItemTex);
		}
	}

	I3ASSERT( m_pEventCardSet == nullptr); 
	m_pEventCardSet = pResManager->LoadTexture("Gui/Card/event_mission.i3i");

	// 세트상품
	{
		i3::vector<i3Texture*>& List = GetTextureList("Set");
		i3Texture * pPackageGoodsTex = pResManager->LoadTexture("Gui/Item/item_cash_set.i3i");
		if( pPackageGoodsTex )
			List.push_back( pPackageGoodsTex);
		else
		{
			//	세트 아이템 이미지 로딩
			for(INT32 i=0; i<MAX_MAINTENANCE_ITEM_TEX_COUNT; i++)
			{
				sprintf_s(temp, "Gui/Item/item_cash_set%d.i3i", i);

				i3Texture * pSetItemTex = pResManager->LoadTexture(temp);
				if ( pSetItemTex != nullptr)
					List.push_back( pSetItemTex);
			}
		}
	}

	// Boss Stage 시작 이미지 - 
	//이 함수는 자주 호출된다.	DestroyReadyStageImageRes 이 함수도 자주 호출된다.
	//그래서 값이 잇는건 또 다시 생성을 하지 않는다.
	if( !m_pBossStartTexture )
	{
		m_pBossStartTexture = pResManager->LoadTexture("Gui/Interface/LW_MapImage.i3i");

		if( !m_pBossStartTexture )
		{
			I3PRINTLOG(I3LOG_WARN, "Boss 스테이지 시작 이미지 [Gui/Interface/LW_MapImage.i3i] 파일를 찾지 못함");
		}
	}

	return; 
}

void CEquipShapeManager::CreateWeaponDescImageRes(INT32 nWeaponFlag, i3GuiStatic* pDescStatic1, i3GuiStatic* pDescStatic2 )
{
	WEAPON_CLASS_TYPE nClassType	= WEAPON::ItemID2Class( nWeaponFlag);
	WEAPON::RES_ID nNumber		= WEAPON::ItemID2ResID( nWeaponFlag);

	I3ASSERT( m_pWeaponDescImage == nullptr); 

	//Load Res
	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( nClassType, nNumber);
	if(pInfo && pInfo->isValidUiPath())
	{
		i3::rc_wstring wstrUIPath;
		pInfo->GetUiPath( wstrUIPath);
		i3::string strUIPath;		i3::utf16_to_mb(wstrUIPath, strUIPath);
		m_pWeaponDescImage = g_pFramework->GetResourceManager()->LoadTexture( strUIPath.c_str() );
	}

	if(m_pWeaponDescImage == nullptr)return; 

	//Description       
	pDescStatic1->SetEnable(true);
	pDescStatic1->ChangeFirstShape(m_pWeaponDescImage, 0, 232, 512, 280);

	//Extension 
	pDescStatic2->SetEnable(true);
	pDescStatic2->ChangeFirstShape(m_pWeaponDescImage, 1, 58, 348, 132);

	return; 
}

void CEquipShapeManager::DestroyWeaponDescImageRes(void)
{
	I3_SAFE_RELEASE(m_pWeaponDescImage);
}

// 신규 UI에서는 CreateReadyStageImageRes()를 호출하지 않기때문에 m_pBossStartTexture가 nullptr임.
// 신규 UI로 만들기 전까지 void UIPhaseReadyRoom::OnCreate()에서 아래 메소드를 호출해준다.
void CEquipShapeManager::CreateBossStageStartImageRes()
{
	i3ResourceManager * pResManager = g_pFramework->GetResourceManager();
	I3ASSERT(pResManager != nullptr);

	if( !m_pBossStartTexture )
	{
		m_pBossStartTexture = pResManager->LoadTexture("Gui/Interface/LW_MapImage.i3i");

		if( !m_pBossStartTexture )
		{
			I3PRINTLOG(I3LOG_WARN, "Boss 스테이지 시작 이미지 [Gui/Interface/LW_MapImage.i3i] 파일를 찾지 못함");
		}
	}
}

void CEquipShapeManager::DestroyReadyStageImageRes(void) 
{
	I3_SAFE_RELEASE(m_pEventCardSet);
	I3_SAFE_RELEASE(m_BadgeTexture);	
	I3_SAFE_RELEASE(m_pCardTexture);

	i3::cu::for_each_safe_release_clear(GetTextureList("Map"), &i3Texture::Release);
	i3::cu::for_each_safe_release_clear(GetTextureList("Chara"), &i3Texture::Release);
	i3::cu::for_each_safe_release_clear(GetTextureList("Head"), &i3Texture::Release);
	i3::cu::for_each_safe_release_clear(GetTextureList("Mask"), &i3Texture::Release);
	i3::cu::for_each_safe_release_clear(GetTextureList("Beret"), &i3Texture::Release);
	i3::cu::for_each_safe_release_clear(GetTextureList("Perido"), &i3Texture::Release);
	i3::cu::for_each_safe_release_clear(GetTextureList("Count"), &i3Texture::Release);
	i3::cu::for_each_safe_release_clear(GetTextureList("Set"), &i3Texture::Release);
}

#endif

void CEquipShapeManager::CreateBattleStageImageRes(void)
{


	i3::vector<i3Texture*>& List = GetTextureList("WeaponKill");

	List.resize(m_iMaxWeaponKillSize);

	i3::rc_string strCommonUIFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);
	i3::stack_string strFileName;

	for(UINT8 i=0; i<m_iMaxWeaponKillSize; i++)
	{
		
		i3::sprintf(strFileName, "%s/Image/Weapon_Kill/Weapon_Kill%d.i3i", strCommonUIFolder, i);

		i3Texture * pTexture = g_pFramework->GetResourceManager()->LoadTexture(strFileName.c_str());
		if ( pTexture != nullptr)
			List[i] = pTexture;
	}
}

void CEquipShapeManager::DestroyBattleStageImageRes(void)
{
	i3::cu::for_each_safe_release_clear(GetTextureList("WeaponKill"), &i3Texture::Release);
}


//어시스트킬 킬마크 처리
bool CEquipShapeManager::SetWeaponKillMark(i3Sprite2D* pSprite, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum, bool bHP_Up, bool bHeadShot )
{
	if( pSprite == nullptr) return false;

	INT32 iWeaponOrdIdx = _GetWeaponOrdIdx(nWeaponClass, nWeaponNum); 

	if( bHP_Up && !bHeadShot)
	{
		if( nWeaponClass == WEAPON_CLASS_DINO) {
			// 공룡 어시스트 킬 마크
			CWeaponDinoInfo * pWeaponInfo = (CWeaponDinoInfo*) g_pWeaponInfoDataBase->getWeaponInfo( nWeaponClass, nWeaponNum, true);
			if( pWeaponInfo->GetAssistKillImageIndex() > -1)
				iWeaponOrdIdx = pWeaponInfo->GetAssistKillImageIndex();
		}
		else if( nWeaponClass == WEAPON_CLASS_GRENADESHELL && nWeaponNum == WEAPON::getItemIndex(WEAPON::GSHELL_POISON)) {
			iWeaponOrdIdx = 166;
		}
	}

	if(iWeaponOrdIdx == -1)
	{
		pSprite->SetEnable(false);
		return false; 
	}

	INT32 nWeaponU=0, nWeaponV=0, nWeaponWidth=0, nWeaponHeight=0; 
	nWeaponWidth	= 127; 	
	nWeaponHeight	= 39; 

	INT32 nIndex = iWeaponOrdIdx / WEAPON_KILLMARK_SHAPE_PER_PAGE;
	INT32 nTexOrd = iWeaponOrdIdx % WEAPON_KILLMARK_SHAPE_PER_PAGE;

	I3_BOUNDCHK( nIndex, m_iMaxWeaponKillSize);
	I3_BOUNDCHK( nTexOrd, WEAPON_KILLMARK_SHAPE_PER_PAGE);

	nWeaponU = nWeaponWidth * (nTexOrd % WEAPON_KILLMARK_SHAPE_COLUMN); 
	nWeaponV = nWeaponHeight * (INT32)(nTexOrd / WEAPON_KILLMARK_SHAPE_COLUMN); 


	i3Texture * pTexture = GetShapeTexture("WeaponKill", nIndex);		// 단순한 타이핑 오류로 보여지는 커버러티 처리. ( 11.11.07.수빈)

	if( pTexture == nullptr)
		return false;

	pSprite->SetTexture( pTexture );
	pSprite->SetTextureCoord(0, (REAL32)nWeaponU, (REAL32)nWeaponV, (REAL32)(nWeaponU + nWeaponWidth), (REAL32)(nWeaponV + nWeaponHeight));

	return true; 
}

bool CEquipShapeManager::SetWeaponSelectMark(i3GuiStatic* pStatic, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum)
{
	if( pStatic == nullptr) return false;

	INT32 iWeaponOrdIdx = _GetWeaponOrdIdx(nWeaponClass, nWeaponNum); 
	if(iWeaponOrdIdx == -1)
	{
		pStatic->SetEnable(false);
		return false; 
	} 

	INT32 nWeaponWidth	= 204;
	INT32 nWeaponHeight	= 57;

	INT32 nIndex = iWeaponOrdIdx / WEAPON_KILLMARK_SHAPE_PER_PAGE;	//	<-- 주의!! killmark 단위로 계산한다.
	INT32 nTexOrd = iWeaponOrdIdx % WEAPON_KILLMARK_SHAPE_PER_PAGE;	//	<-- 주의!! killmark 단위로 계산한다.

	I3_BOUNDCHK( nIndex, MAX_WEAPON_SELECT_TEX);
	I3_BOUNDCHK( nTexOrd, WEAPON_KILLMARK_SHAPE_PER_PAGE);

	INT32 nWeaponU		= nWeaponWidth * (nTexOrd % WEAPON_SELECT_SHAPE_COLUMN); 
	INT32 nWeaponV		= nWeaponHeight * (INT32)(nTexOrd / WEAPON_SELECT_SHAPE_COLUMN); 

	pStatic->SetEnable(true);

	i3Texture * pTexture = GetShapeTexture("WeaponSelect", nIndex);

	return pStatic->ChangeFirstShape(pTexture, nWeaponU, nWeaponV, nWeaponWidth, nWeaponHeight);
}

INT32 CEquipShapeManager::GetCharacterShape( T_ItemID ItemID)
{
	const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( ItemID);
	if( pInfo != nullptr)
	{
		return pInfo->GetUIShapeIndex();
	}

	return -1;
}

bool CEquipShapeManager::SetCharacterShape(i3GuiStatic* pStatic, T_ItemID ItemID)
{
	I3ASSERT(pStatic);

	i3Texture * pTexture = nullptr;
	INT32 texWidth = 0, texHeight = 0;
	POINT32 TexUV = {};

	const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( ItemID);
	if( pInfo != nullptr)
	{
		pStatic->SetEnable(true);

		// 아래 대입되는 숫자상수는 Media/Gui/Item/Item_cha.i3i 파일에 이미지 인덱스입니다.
		INT32 ShapeIndex = pInfo->GetUIShapeIndex();
		INT32 TexIndex = ShapeIndex / CHARACTER_SHAPE_PER_PAGE;
		ShapeIndex %= CHARACTER_SHAPE_PER_PAGE;

		switch( CHARA::ItemID2Type(ItemID) )
		{
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_DINO:
			{	
				pTexture	= GetShapeTexture("Chara", TexIndex); 
				TexUV		= CalcuTextureUV(ShapeIndex, TEX_CHARA_WIDTH, TEX_CHARA_HEIGHT, TEX_CHARA_HORZ_COUNT);
				texWidth	= TEX_CHARA_WIDTH;
				texHeight	= TEX_CHARA_HEIGHT;
			}
			break;
		case ITEM_TYPE_FACEGEAR:
			{
				pTexture = GetShapeTexture("Facegear", TexIndex);
				TexUV = CalcuTextureUV(ShapeIndex, TEX_HELMET_WIDTH, TEX_HELMET_HEIGHT, TEX_HELMET_HORZ_COUNT);
				texWidth = TEX_HELMET_WIDTH;
				texHeight = TEX_HELMET_HEIGHT;
			}
			break;
		case ITEM_TYPE_HEADGEAR:
			{
				pTexture = GetShapeTexture("Head", TexIndex);
				TexUV = CalcuTextureUV(ShapeIndex, TEX_HELMET_WIDTH, TEX_HELMET_HEIGHT, TEX_HELMET_HORZ_COUNT);
				texWidth = TEX_HELMET_WIDTH;
				texHeight = TEX_HELMET_HEIGHT;
			}
			break;

		case ITEM_TYPE_BERET:
			{
				pTexture = GetShapeTexture("Beret", TexIndex);
				TexUV = CalcuTextureUV(ShapeIndex, TEX_HELMET_WIDTH, TEX_HELMET_HEIGHT, TEX_HELMET_HORZ_COUNT);
				texWidth = TEX_HELMET_WIDTH;
				texHeight = TEX_HELMET_HEIGHT;
			}
			break;
		
		default:
			pStatic->SetEnable(false);
			return false;
		}

		return pStatic->ChangeFirstShape( pTexture, TexUV.x, TexUV.y, texWidth, texHeight);
	}
	else
	{
		pStatic->SetEnable( false);
		return false;
	}
}

INT32 CEquipShapeManager::GetMapIdx(STAGE_ID id)
{
	INT32 idxImage = -1;

	CSI_MAP* map_data = CStageInfoUtil::GetMapData((UINT8)id);
	i3::string StageName; 
	i3::utf16_to_mb( map_data->getCodeName(), StageName);
	StageImageMap::iterator It = m_StageImageMap.find( StageName );

	if( It != m_StageImageMap.end() )
		idxImage = It->second;
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "아직 등록 되지 않는 Stage 이름 [ %s ] 입니다. (Stage.pef 참조)", StageName.c_str() );
	}

	return idxImage;
}

INT32 CEquipShapeManager::GetMapIdx( STAGE_UID uid)
{
	INT32 idxImage = -1;

	CSI_MAP* map_data = CStageInfoUtil::GetMapData(uid);
	std::string StageName;
	i3::utf16_to_mb(map_data->getCodeName(), StageName);
	StageImageMap::iterator It = m_StageImageMap.find(StageName);

	if (It != m_StageImageMap.end())
		idxImage = It->second;
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "아직 등록 되지 않는 Stage 이름 [ %s ] 입니다. (Stage.pef 참조)", StageName.c_str());
	}

	return idxImage;
}

INT32 CEquipShapeManager::GetEscapeMapIdx(STAGE_ID id)
{
	INT32 idxImage = -1;

	CSI_MAP* map_data = CStageInfoUtil::GetMapData((UINT32)id);
	i3::string StageName;
	i3::utf16_to_mb( map_data->getCodeName(), StageName);
	StageImageMap::iterator It = m_StageImageMap.find( StageName );

	if (It != m_EscapeImageMap.end())
		idxImage = It->second;
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "아직 등록 되지 않는 Stage 이름 [ %s ] 입니다. (Stage.pef 참조)", StageName.c_str());
	}

	return idxImage;
}

INT32 CEquipShapeManager::GetEscapeMapIdx(STAGE_UID uid)
{
	INT32 idxImage = -1;

	CSI_MAP* map_data = CStageInfoUtil::GetMapData((UINT32)uid);
	i3::string StageName;
	i3::utf16_to_mb(map_data->getCodeName(), StageName);
	StageImageMap::iterator It = m_EscapeImageMap.find(StageName);

	if (It != m_EscapeImageMap.end())
		idxImage = It->second;
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "아직 등록 되지 않는 Stage 이름 [ %s ] 입니다. (Stage.pef 참조)", StageName.c_str());
	}

	return idxImage;
}

INT32 CEquipShapeManager::GetRandomMapIdx( void)
{
	return 26;
}

bool CEquipShapeManager::SetRandomMapShape(i3GuiStatic* pStatic)
{
	I3ASSERT(pStatic);
	
	i3Texture * pTexture = GetShapeTexture("Map", 0);
	POINT32 MapUV = CalcuTextureUV(26, TEX_MAP_WIDTH, TEX_MAP_HEIGHT, TEX_MAP_HORZ_COUNT);
	return pStatic->ChangeFirstShape(pTexture, MapUV.x, MapUV.y, TEX_MAP_WIDTH, TEX_MAP_HEIGHT);
}
bool CEquipShapeManager::SetCashItemShape(i3GuiStatic* pStatic, T_ItemID ItemID)
{
	I3ASSERT(pStatic);

	INT32 itemUsage = GET_ITEM_TYPE(ItemID);
	I3ASSERT(itemUsage >= 0 && (itemUsage >= ITEM_TYPE_MAINTENANCE && itemUsage <= ITEM_TYPE_WRAP_COUNT));

	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(ItemID);
	if ( ! pInfo )
	{
		pStatic->SetEnable(false);
		return nullptr;
	}

	if ( GET_ITEM_TYPE(ItemID) == ITEM_TYPE_POINT )
	{
		itemUsage = pInfo->GetType();
	}

	INT32 nTexIndex = pInfo->GetTexIndex();
	i3Texture * pTex = nullptr;
	switch(itemUsage)
	{
	case ITEM_TYPE_MAINTENANCE:	pTex = GetShapeTexture("Perido", nTexIndex);	break;
	case ITEM_TYPE_WRAP_PERIOD:	pTex = GetShapeTexture("Count", nTexIndex);	break;
	case ITEM_TYPE_WRAP_COUNT:	pTex = GetShapeTexture("Count", nTexIndex);	break;
	default:
		pStatic->SetEnable(false);
		return nullptr;
	}

	INT32 shapeIdx = pInfo->GetShapeIndex();

	POINT32 ItemUV = CalcuTextureUV(shapeIdx, TEX_ITEM_WIDTH, TEX_ITEM_HEIGHT, TEX_ITEM_HORZ_COUNT);

	if( pTex != nullptr)	
	{	//	컨트롤 강제 업데이트하기 위해
		pStatic->ChangeFirstShape(pTex, ItemUV.x, ItemUV.y, TEX_ITEM_WIDTH, TEX_ITEM_HEIGHT);

        pStatic->SetEnable(true);
		pStatic->UpdateRenderObjects();

		return true;
	}
	else
	{
		pStatic->SetEnable(false);

		return false;
	}
}

bool CEquipShapeManager::SetPackageGoodsShape(i3GuiStatic* pStatic, INT32 GoodsID)
{
	I3ASSERT(pStatic);

	ShopPackageGoodsInfo *ItemInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
	if ( ! ItemInfo )
	{
		pStatic->SetEnable(false);
		return false;
	}

	//임시코드 - 하동익
	i3::rc_wstring wstrTexType = ItemInfo->GetType();
	i3::rc_string  strTexType;		i3::utf16_to_mb(wstrTexType, strTexType);

	INT32 TexIndex = ItemInfo->GetTexIndex();
	INT32 shapeIdx = ItemInfo->GetImageIndex();

	i3Texture* texture = GetShapeTexture(strTexType, TexIndex);

	INT32 TexWidth = 0, TexHeight = 0;
	POINT32 TexUV = {};
	if ( i3::generic_is_equal( wstrTexType, "Chara" ) || i3::generic_is_equal( wstrTexType, L"Set" ) )
	{
		TexWidth = TEX_CHARA_WIDTH;
		TexHeight = TEX_CHARA_HEIGHT;
		TexUV = CalcuTextureUV(shapeIdx, TEX_CHARA_WIDTH, TEX_CHARA_HEIGHT, TEX_CHARA_HORZ_COUNT);

		pStatic->setPosition(12.0f, 19.0f);
	}
	else
	{
		TexWidth = TEX_ITEM_WIDTH;
		TexHeight = TEX_ITEM_HEIGHT;
		TexUV = CalcuTextureUV(shapeIdx, TEX_ITEM_WIDTH, TEX_ITEM_HEIGHT, TEX_ITEM_HORZ_COUNT);

		pStatic->setPosition(35.0f, 25.0f);
	}

	REAL32 widthRate = i3GuiRoot::getGuiResolutionWidth();
	REAL32 heightRate = i3GuiRoot::getGuiResolutionHeight();
	pStatic->SetControlSize( (INT32)(TexWidth * widthRate), (INT32)(TexHeight * heightRate));

	if (texture == nullptr)
	{
		pStatic->SetEnable(false);
		return false;
	}

	pStatic->ChangeFirstShape(texture, TexUV.x, TexUV.y, TexWidth, TexHeight);
	pStatic->SetEnable(true);
	pStatic->UpdateRenderObjects();
	return true;
}

bool CEquipShapeManager::SetCardShape(i3GuiStatic* pStatic,INT32 cardSetId,INT32 cardIdx)
{
	I3ASSERT(pStatic);
	I3ASSERT(cardSetId != QUEST_CARDSET_TYPE_NONE);

	i3Texture* pTexture = nullptr;

	INT32 U = 0, V = 0;
	INT32 W = 34, H = 55;
	switch(cardSetId)
	{
	case QUEST_CARDSET_TYPE_EVENT:
		U = 3 + (cardIdx % 10) * (W + 5);
		V = 3 + (cardIdx / 10) * (H + 5);
		pTexture = m_pEventCardSet;
		break;
	default:
		{
			INT32 idx = (cardSetId - 1) * 10 + cardIdx;	//1 = (none)
			U = 3 + (idx % 10) * (W + 5);
			V = 3 + (idx / 10) * (H + 5);
			pTexture = m_pCardTexture;
		}
		break;
	};

	return pStatic->ChangeFirstShape(pTexture, U, V, W, H);
}

bool CEquipShapeManager::SetCardSetShape(i3GuiStatic* pStatic,INT32 cardSetId)
{
	I3ASSERT(pStatic);	
	I3ASSERT(cardSetId != QUEST_CARDSET_TYPE_NONE);

	i3Texture* pCardTexture = nullptr;
	INT32 U = 0, V = 0;
	INT32 W = 56, H = 59;
	switch(cardSetId)
	{
	case QUEST_CARDSET_TYPE_EVENT:
		U = 392;
		V = 1;

		pCardTexture = m_pEventCardSet;
		break;
	default:
		{
			// cardSetId는 1-base idx
			INT32 idx = cardSetId - 1;	// 2 = (none, tutorial)
			U = 392;
			V = 1 + idx * (H + 1);
			pCardTexture = m_pCardTexture;
		}
		break;
	};

	return pStatic->ChangeFirstShape(pCardTexture, U, V, W, H);
}

bool CEquipShapeManager::SetRibbonShape(i3GuiStatic* pStatic,INT32 ribbonId)
{
	I3ASSERT(pStatic);
	if ( ! pStatic )	return false;
	return pStatic->ChangeFirstShape(m_BadgeTexture, TEX_RIBBON_U, TEX_RIBBON_V, TEX_RIBBON_WIDTH, TEX_RIBBON_HEIGHT);
}

bool CEquipShapeManager::SetEnsignShape(i3GuiStatic* pStatic,INT32 ensignId)
{
	I3ASSERT(pStatic);
	if ( ! pStatic )	return false;
	return pStatic->ChangeFirstShape( m_BadgeTexture, TEX_ENSIGN_U, TEX_ENSIGN_V, TEX_ENSIGN_WIDTH, TEX_ENSIGN_HEIGHT);
}

bool CEquipShapeManager::SetMedalShape(i3GuiStatic* pStatic,INT32 medalId)
{
	I3ASSERT(pStatic);

	if( medalId == -1)
	{	//	-1일 경우 이미지 셋팅 안하도록
		pStatic->SetEnable( false);
		return false;
	}
	else
	{
		pStatic->SetEnable( true);
	}

	i3Texture* pTexture = m_BadgeTexture;
	INT32 idx = medalId - 1;
	INT32 U = 0, V = 0;
	INT32 Width = 0, Height = 0;
	if (idx < 16)
	{	
		U = TEX_MEDAL_U;
		V = TEX_MEDAL_V;
		Width	= TEX_MEDAL_WIDTH;
		Height	= TEX_MEDAL_HEIGHT;
	}
	else
	{
		U = TEX_MASTER_U;
		V = TEX_MASTER_V;
		Width	= TEX_MASTER_WIDTH;
		Height	= TEX_MASTER_HEIGHT;
	}

	return pStatic->ChangeFirstShape( pTexture, U, V, Width, Height);
}


void CEquipShapeManager::_AddWeaponImageIndex( WEAPON_CLASS_TYPE Type, WEAPON::RES_ID Number, INT32 Index)
{
	if( Type == 0 || Number == 0 )
		return;

	WeaponImageMap::iterator It = mWeaponImageMap.find( Type );

	if( It == mWeaponImageMap.end() )
	{
		ImageIndexMap Map;

		mWeaponImageMap.insert( 
			WeaponImageMap::value_type(Type, ImageIndexMap() )
			);

		It = mWeaponImageMap.find( Type );
	}

	ImageIndexMap::iterator IndexIt = It->second.find( Number );

	if( IndexIt != It->second.end() )
	{
		I3PRINTLOG(I3LOG_FATAL, "The WeaponType[%d] Number[ %d ] UI Index[ %d ] duplicated!",
			Type, Number, Index);
	}

	It->second.insert( ImageIndexMap::value_type( Number, Index ) );
}

void CEquipShapeManager::_InitWeaponImageIndex(i3RegKey* pWeaponListKey)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pWeaponListKey, "_UIShapeIndex" );
	
	if( pData )
	{
		INT32 Index = 0;

		GET_REG_DATA(pData, &Index);

		T_ItemID ItemID;
		FIND_REG_DATA( pWeaponListKey, "ITEMID", (INT32*) &ItemID);

		WEAPON_CLASS_TYPE Type = WEAPON::ItemID2Class( ItemID);
		WEAPON::RES_ID Number = WEAPON::ItemID2ResID( ItemID);

		_AddWeaponImageIndex( Type, Number, Index);
		INT32 page = (Index / WEAPON_KILLMARK_SHAPE_PER_PAGE) +1;
		if( m_iMaxWeaponKillSize < page )
			 m_iMaxWeaponKillSize = (INT8)page;
	}

	for( int i = 0; i < pWeaponListKey->getChildCount(); i++)
	{
		i3RegKey * pChild = (i3RegKey*)pWeaponListKey->getChild( i);
		_InitWeaponImageIndex( pChild );
	}
}

INT32 CEquipShapeManager::_GetWeaponOrdIdx( WEAPON_CLASS_TYPE iWeaponClass, WEAPON::RES_ID iWeaponNum)
{
	INT32 idx = -1;

	if( iWeaponClass == 100)
	{
		// 전동차 예외 처리. -_-;;;
		static INT32 s_ExceptionTable[ 3] = 
		{
			-1,
			52,
			53
		};
		static UINT32 s_ExceptionTableCount = GET_ARRAY_COUNT( s_ExceptionTable);

		I3ASSERT( iWeaponNum < s_ExceptionTableCount);

		if ( iWeaponNum < s_ExceptionTableCount )
			idx = s_ExceptionTable[ iWeaponNum];
	}
	else
	{
		WeaponImageMap::iterator WeaponIt = mWeaponImageMap.find( iWeaponClass );
		if( WeaponIt == mWeaponImageMap.end() )
		{
			return -1;
		}

		ImageIndexMap::iterator IndexIt = WeaponIt->second.find( iWeaponNum );
		if( IndexIt == WeaponIt->second.end() )
		{
			return -1;
		}

		idx = IndexIt->second;
	}

	if( idx == -1)
	{
		I3TRACE("_GetWeaponOrdIdx UnKnown Weapon Class %d Number %d\n ", iWeaponClass, iWeaponNum); 
	}

	return idx; 
}

void CEquipShapeManager::_AddStageImageIndex(const char* IndexBuf, INT32 ShapeIndex)
{
	StageImageMap::iterator It = m_StageImageMap.find( IndexBuf );

	if( It == m_StageImageMap.end() )
	{
		m_StageImageMap.insert( 
			StageImageMap::value_type( IndexBuf, ShapeIndex )
			);
	}
	else
	{
		char ErrorMsg[256] = "";
		sprintf_s(ErrorMsg, 255, "Stage에 이미지 Name [ %s ] 가 중복 (Stage.pef 참조)", IndexBuf );
		I3PRINTLOG(I3LOG_NOTICE, ErrorMsg );
	}
}

void CEquipShapeManager::_AddEscapeImageIndex(const char* IndexBuf, INT32 ShapeIndex)
{
	StageImageMap::iterator It = m_EscapeImageMap.find(IndexBuf);

	if (It == m_EscapeImageMap.end())
	{
		m_EscapeImageMap.insert(
			StageImageMap::value_type(IndexBuf, ShapeIndex)
			);
	}
	else
	{
		char ErrorMsg[256] = "";
		sprintf_s(ErrorMsg, 255, "Stage에 이미지 Name [ %s ] 가 중복 (Stage.pef 참조)", IndexBuf);
		I3PRINTLOG(I3LOG_NOTICE, ErrorMsg);
	}
}

void CEquipShapeManager::_InitStageImageIndex(i3RegKey* pStageListKey)
{
	INT32 count = pStageListKey->getChildCount();

	for (INT32 i = 0; i < count; ++i)
	{
		i3RegKey * pChild = (i3RegKey*)pStageListKey->getChild(i);

		INT32 ShapeIndex = -2;		//반드시 있어야 하는 값
		INT32 _EscapeUIShapeIndex = -1;		//없어도 상관없는 값

		FIND_REG_DATA(pChild, "_UIShapeIndex", &ShapeIndex);
		I3ASSERT(ShapeIndex != -2 && "Stage에 _UIShapeIndex 값이 없음.(Stage.pef 참조)");

		FIND_REG_DATA(pChild, "_BossUIShapeIndex", &_EscapeUIShapeIndex);

		const char *IndexBuf = pChild->GetName();
		_AddStageImageIndex(IndexBuf, ShapeIndex);
		_AddEscapeImageIndex(IndexBuf, _EscapeUIShapeIndex);
	}
}

namespace
{
	i3::vector<i3Texture*>	unknown_error_tex_list;
}

i3::vector<i3Texture*>& CEquipShapeManager::GetTextureList(const i3::rc_string& TextureName)
{
	TextureFileList::iterator It = m_TextureList.find( TextureName );

	if( It == m_TextureList.end() )
	{
		
		I3PRINTLOG(I3LOG_NOTICE, "등록 되지 않는 TextureName[ %s ] 입니다. InitTextureList() 함수 참조", TextureName );
		return unknown_error_tex_list;
	}

	return It->second;
}

i3Texture* CEquipShapeManager::GetShapeTexture(const i3::rc_string& TextureName, INT32 Index)
{
	TextureFileList::iterator It = m_TextureList.find( TextureName );

	if( It == m_TextureList.end() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "등록 되지 않는 TextureName[ %s ] 입니다. InitTextureList() 함수 참조", TextureName );
		return nullptr;
	}

	if( (INT32)(It->second).size() < Index+1)
	{
		I3PRINTLOG(I3LOG_NOTICE, "등록 되지 않는 TextureName[ %s ] 입니다. Index : %d", TextureName, Index );
		return nullptr;
	}
	return (It->second)[Index];
}

