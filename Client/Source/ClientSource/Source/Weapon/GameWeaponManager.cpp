#include "pch.h"
#include "GlobalVariables.h"
#include "GameWeaponManager.h"
#include "StageBattle.h"
#include "WeaponGlobal.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaMultiWeaponContext.h"
#include "WeaponM197.h"
#include "WeaponShotGun3.h"
#include "WeaponShotGunWithButt.h"
#include "WeaponShotGunMaleFemale.h"
#include "WeaponShotGunGenderWithDual.h"
#include "WeaponLRU_Manager.h"

#include "BGLoadThread.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#include "MainWeapon_Grenade.h"
#include "MainWeapon_C5.h"
#include "MainWeapon_GrenadeShell.h"
#include "MainWeapon_RLShell.h"
#include "GameMissionManager.h"
#include "Weapon/WeaponItemID.h"
#include "Weapon/WeaponInfo/MeleeWeaponInfo.h"

#include "UI/UIHudManager.h"
#include "UI/UIHudMultiWeapon.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "BattleHud.h"
#include "BattleHUD_Scope.h"

#include "GrenadeGunInfo.h"

#include "ClientStageInfoUtil.h"


#define DROPEDWEAPON_SENDTIME		0.064f				// 이 시간 만큼 드랍된 무기 정보를 보냅니다.	// 초당 10회
#define DROPEDWEAPON_MINSENDTIME	1.f					// 초당 1회 마다 최소 싱크를 맞춥니다.
#define DROPEDWEAPON_ASSIST_TIME	0.14f
#define DROPEDWEAPON_ASSIST_INVTIME 7.1428571428571428571428571428571f

#define DROPWEAPON_LIFETIME			11.f				// 드랍된 무기의 라이프 타임
#define DROPWEAPON_ALPHATIME		10.f				// 드랍된 무기가 사라지기 시작하는 시간
#define DROPWEAPON_ALPHASLOPE		1.f 
#define DROPWEAPON_ALPHASLOPEINV	1.f

#define MAKE_THROW_COUNT			32					// 수류탄 계열의 갯수
#define MAKE_GRENADESHELL_COUNT		16					// 유탄 갯수
#define MAKE_ROCKETLAUNCHERSHELL_COUNT	16				// 로켓런쳐(RPG-7) 갯수 (4vs4)
#define MAKE_ROCKETLAUNCHERSHELL_FOR_ALL_COUNT	48		// 로켓런쳐(RPG-7) 갯수 (ForAll)
#define MAKE_C5_COUNT				16					// C5 갯수
#define MAKE_THROWING_KNIFE_COUNT	16					// 던지는 근접 무기 갯수
#define MAKE_ARROW_COUNT			16					// Giran arrow count


REAL32	CGameWeaponManager::s_BrokenWeaponDecreaseDamage	= 5;		//	무기 데미지
REAL32	CGameWeaponManager::s_BrokenWeaponVertRecoil		= 5;			//	수직 반동
REAL32	CGameWeaponManager::s_BrokenWeaponHorzRecoil		= 5;			//	수평 반동



I3_CLASS_INSTANCE( CGameWeaponManager);//, i3ElementBase);

static bool _SetSoundInitProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( !i3::same_of<i3AIContext*>(pNode))
		return true;

	i3AIContext * pCtx = (i3AIContext *) pNode;
	i3AI * pAI = pCtx->getAI();
	INT32 i, j;

	for( i = 0; i < pAI->getAIStateCount(); i++)
	{
		i3AIState * pState = pAI->getAIState( i);
		i3TimeSequence * pTimeSeq = pState->getTimeSeq();

		if( pTimeSeq == nullptr)
			continue;

		for( j = 0; j < pTimeSeq->GetSequenceCount(); j++)
		{
			i3TimeSequenceInfo * pInfo = pTimeSeq->GetSequence( j);

			if( i3::same_of<i3SoundNode*>(pInfo->GetNode()))
			{
				i3SoundNode * pSnd = (i3SoundNode *) pInfo->GetNode();
				if( pSnd == nullptr) 
				{
					continue;
				}

				i3SoundPlayInfo * pPlayInfo = pSnd->getSoundPlayInfo();
				if( pPlayInfo == nullptr)
				{
					continue;
				}

				char szTemp[MAX_PATH];

				i3Sound * pSound = pPlayInfo->getSound();
				if( pSound != nullptr && (pSound->hasName()))
				{
				//	i3String::SplitFileName( pSound->GetName(), szTemp, true);
					i3::extract_filename( pSound->GetName(), szTemp);

					i3GameResSound * pRes = (i3GameResSound *) g_pFramework->QuaryResource( szTemp);
					if( pRes)
					{
						i3SoundPlayInfo * pSndResInfo = pRes->getSound();
						if( pSndResInfo != nullptr)
						{
							pPlayInfo->RemoveStyle( I3SND_PROP_TYPE_BGM | I3SND_PROP_TYPE_VOICE);
							pPlayInfo->AddStyle( I3SND_PROP_TYPE_EFFECT);

							pPlayInfo->SetDistanceInfo( pSndResInfo->getMaxDistance(),
														pSndResInfo->getMinDistance(), 
														pSndResInfo->getVolumeType());

							pPlayInfo->SetDefaultVolume( pSndResInfo->GetDefaultVolume());
							pPlayInfo->setListenerVolume( pSndResInfo->getListenerVolume());
						}
					}
//					else
//					{
//						I3PRINTLOG(I3LOG_DEFALUT,  "Weapon Sound 에서 [ %s ] 를 찾을 수 없다.", szTemp );
//					}
				}
			}
		}
	}

	return true;
}

WeaponDecoySndInterval::WeaponDecoySndInterval()
{
	m_interval = nullptr;
	m_num = 0;
	m_loop = true;
}

WeaponDecoySndInterval::~WeaponDecoySndInterval()
{
	if(m_interval)
		delete [] m_interval;

}



CGameWeaponManager::CGameWeaponManager( void) : m_rTrashTimer(0.f)
{
	m_pStage = nullptr;
	m_pThrowWeaponList = nullptr; 
	m_pThrowItemList = nullptr;
	m_GrenadeShellWeaponList = nullptr;

	m_nWeaponIDCount = 0;

	{
		m_decoySndInterval = new WeaponDecoySndInterval;

		m_decoySndInterval->m_num = 10;		
		m_decoySndInterval->m_interval = new REAL32[10];

		// 총쏘는간격 (1.0=1초)
		const static REAL32 tmpData[10] = { 0.1f, 0.3f, 0.1f, 0.7f, 0.3f, 0.8f, 0.2f, 0.5f, 0.1f, 0.8f };
		memcpy(m_decoySndInterval->m_interval, tmpData, sizeof(REAL32)*10);
	}

	{
		m_EventDecoySndInterval = new WeaponDecoySndInterval;
		m_EventDecoySndInterval->m_num = 1;

		m_EventDecoySndInterval->m_interval = new REAL32;
		m_EventDecoySndInterval->m_interval[0] = 0.0f;

		m_EventDecoySndInterval->m_loop = false;
	}
	
#ifdef ENABLE_TEST_COMMAND
	m_bUnlimitGun = false;
	m_bUnlimitGrenade = false;
	m_CmdGunBullet = -1;
#endif

	m_pLoadInfoPool = new i3FixedMemoryPool;
	MEMDUMP_NEW( m_pLoadInfoPool, sizeof(i3FixedMemoryPool));

	m_pLoadInfoPool->Create( "WeaponLoadInfoPool", sizeof( sWEAPONLOADINFO), 128);
	m_pLoadInfoPool->SetAutoCompactEnable( false);
}

CGameWeaponManager::~CGameWeaponManager( void)
{
	Destroy();

	if(m_decoySndInterval)
	{
		delete m_decoySndInterval;
		m_decoySndInterval = nullptr;
	}

	if(m_EventDecoySndInterval)
	{
		delete m_EventDecoySndInterval;
		m_EventDecoySndInterval = nullptr;
	}

	I3_SAFE_DELETE( m_pLoadInfoPool);

	i3::cu::for_each_SafeRelease(m_CharaWeaponList);
	i3::cu::for_each_SafeRelease(m_DropWeaponList);
}

// static
void	CGameWeaponManager::InitValue()
{
	s_BrokenWeaponDecreaseDamage	= g_pConfigEx->GetGame().BrokenWeaponDecreaseDamage;
	s_BrokenWeaponVertRecoil		= g_pConfigEx->GetGame().BrokenWeaponVertRecoil;
	s_BrokenWeaponHorzRecoil		= g_pConfigEx->GetGame().BrokenWeaponHorzRecoil;
}

void CGameWeaponManager::Create( i3Stage * pStage)
{
	m_pStage = pStage;
	
	// Common Res만 로드
	for( INT32 classType = 0; classType < WEAPON_CLASS_COUNT; classType++)
	{
		UINT32 cnt = WEAPON::getItemCount( (WEAPON_CLASS_TYPE) classType);
		m_WeaponResList[ classType].resize( cnt);

		for( UINT32 num = 1; num < cnt; num++)
		{	
			_LoadRes( (WEAPON_CLASS_TYPE) classType, num);
		}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		m_WeaponResCheckList[ classType].resize( cnt);
#endif
	}

	WeaponLRU_Manager::i()->PreLoad();
}

void CGameWeaponManager::_CreateThrow( i3Stage * pStage, WEAPON_CLASS_TYPE type, UINT32 maxCount)
{
	VEC3D vPos;
	WeaponBase * pWeapon;
	i3ClassMeta * pMeta = nullptr;
	i3InstanceManager * pManager = pStage->GetInstanceManager( WeaponBase::static_meta() );	
	i3ClassMeta * pOldMeta = pManager->getInstanceMeta();

	switch( type)
	{
	case WEAPON_CLASS_THROWING_GRENADE :
		if( m_pThrowWeaponList == nullptr)
		{
			m_pThrowWeaponList = new i3::vector<WeaponGrenade*>[ maxCount];
			MEMDUMP_NEW( m_pThrowWeaponList, sizeof(i3::vector<WeaponGrenade*>) * maxCount);
		}
		break;
	case WEAPON_CLASS_THROWING_CONTAIN :
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:
		{
			// 견제형 , 공격형, 회복형 아이템은 전부 한 리스트에 담습니다.
			if(m_pThrowItemList == nullptr)
			{	
				UINT32 i32ListCount = GetItemCountThrowItem();
				m_pThrowItemList = new i3::vector<WeaponGrenade*>[i32ListCount];	
				MEMDUMP_NEW( m_pThrowItemList, sizeof(i3::vector<WeaponGrenade*>) * i32ListCount);
			}	
		}
		break;
	case WEAPON_CLASS_GRENADESHELL :
		if( m_GrenadeShellWeaponList == nullptr)
		{
			m_GrenadeShellWeaponList = new i3::vector<WeaponGrenadeShell*>[ maxCount];
			MEMDUMP_NEW( m_GrenadeShellWeaponList, sizeof(i3::vector<WeaponGrenadeShell*>) * maxCount);
		}
		break;
	}

	i3::rc_wstring	wstrClassMeta;
	i3::string		strClassMeta;

	for( UINT32 weaponNumber = 1; weaponNumber  < maxCount ; weaponNumber++)
	{
		pMeta = nullptr;
		CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo( type, weaponNumber);
		
		if ( pInfo == nullptr)
			continue;
		
		if( type == WEAPON_CLASS_THROWING_GRENADE && weaponNumber == WEAPON::getItemIndex( WEAPON::DOMI_MEATBOMB))
			continue;	// 미트봄은 Domination에서만 사용한다.

		// 등록되지 않은 무기인 경우, 생성하지 않음.
		if( IsRegisteredWeapon(type, weaponNumber) == false )
			continue;

		// 실제 리소스(I3CHR)가 없는 무기는 생성하지 않는다.
		PreloadWeaponResource( type, weaponNumber);

		wstrClassMeta.clear();
		pInfo->GetClassMetaName(wstrClassMeta);

		if(i3::generic_string_size(wstrClassMeta) > 0)
		{
			i3::utf16_to_mb(wstrClassMeta, strClassMeta);
			pMeta =	i3ClassMetaSystem::i()->find_class_meta(strClassMeta);			//	i3ClassMeta::FindClassMetaByName(szClassMeta);
		}
		
		if(!pMeta && type == WEAPON_CLASS_GRENADESHELL)
		{	// GrenadeShell계열은 ClassMeta가 없을시 WeaponGrenadeShell을 사용한다.
			pMeta = WeaponGrenadeShell::static_meta();
		}

		if(!pMeta)
		{	
			i3::utf16_to_mb(wstrClassMeta, strClassMeta);
			I3PRINTLOG(I3LOG_FATAL,  "Invalid Throw weapon type. %d / %s", weaponNumber, strClassMeta );
			continue;
		}

		pManager->setInstanceMeta( pMeta);

		for(INT32 j = 0; j < MAKE_THROW_COUNT; j++)
		{
			pWeapon = (WeaponBase *) pManager->GetFreeInstance();
			I3ASSERT( pWeapon->GetRefCount() >= 1);

			ITEM_TYPE itemType = ITEM_TYPE_THROWING1;

			switch( type)
			{
			case WEAPON_CLASS_THROWING_GRENADE	:	itemType = ITEM_TYPE_THROWING1;		break;
			case WEAPON_CLASS_THROWING_CONTAIN	:	itemType = ITEM_TYPE_THROWING2;		break;
			case WEAPON_CLASS_THROWING_ATTACK	:	itemType = ITEM_TYPE_THROWING2;		break;
			case WEAPON_CLASS_THROWING_HEAL		:	itemType = ITEM_TYPE_THROWING2;		break;
			case WEAPON_CLASS_GRENADESHELL		:	itemType = ITEM_TYPE_TEMP;	break;
			}

			T_ItemID Itemid = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, itemType,	type, weaponNumber);

			pWeapon->Create( nullptr, Itemid, false);
			pWeapon->Reset();
			pWeapon->setIdx( m_nWeaponIDCount);
			m_nWeaponIDCount++;
			pWeapon->SetEnable(false);

			switch( type)
			{
			case WEAPON_CLASS_THROWING_GRENADE :
				{
					I3ASSERT_RETURN( weaponNumber < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE));
					m_pThrowWeaponList[weaponNumber].push_back( static_cast<WeaponGrenade*>(pWeapon) );
				}
				break;
			case WEAPON_CLASS_THROWING_CONTAIN :	
				{
					m_pThrowItemList[weaponNumber].push_back( static_cast<WeaponGrenade*>(pWeapon) );
				}
				break;
			case WEAPON_CLASS_THROWING_ATTACK :	
				{
					INT32 idx = WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN) + weaponNumber - 1;
					if( idx > -1)
						m_pThrowItemList[idx].push_back( static_cast<WeaponGrenade*>(pWeapon) );
				}
				break;
			case WEAPON_CLASS_THROWING_HEAL :	
				{
					INT32 idx = WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN)
						+ WEAPON::getItemCount(WEAPON_CLASS_THROWING_ATTACK) + weaponNumber - 2;
					if( idx > -1)
						m_pThrowItemList[idx].push_back( static_cast<WeaponGrenade*>(pWeapon) );
				}
				break;
			case WEAPON_CLASS_GRENADESHELL : 
				{
					m_GrenadeShellWeaponList[weaponNumber].push_back( static_cast<WeaponGrenadeShell*>(pWeapon));	
				}
				break;
			}
			

			// 생성 후 공중에 띄워줍니다.
			i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10) );
			MoveShapePosition( pWeapon, &vPos);
		}
	}

	pManager->setInstanceMeta( pOldMeta);
}

void CGameWeaponManager::CreateThrowWeapon(i3Stage * pStage)
{
	_CreateThrow( pStage, WEAPON_CLASS_THROWING_GRENADE,	WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE));
	_CreateThrow( pStage, WEAPON_CLASS_THROWING_CONTAIN,	WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN));
	_CreateThrow( pStage, WEAPON_CLASS_THROWING_ATTACK,		WEAPON::getItemCount(WEAPON_CLASS_THROWING_ATTACK));
	_CreateThrow( pStage, WEAPON_CLASS_THROWING_HEAL,		WEAPON::getItemCount(WEAPON_CLASS_THROWING_HEAL));
}

void CGameWeaponManager::DestroyThrowWeapon(i3Stage * pStage)
{
	if (m_pThrowWeaponList)
	{
		for( UINT32 i = 1; i < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE); i++)
		{
			for( UINT32 j = 0; j < m_pThrowWeaponList[i].size(); j++ )
			{
				WeaponBase * pWeapon = m_pThrowWeaponList[i][j];				
				pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
				pWeapon->ReturnInstance();
			}	
		}
		I3_SAFE_DELETE_ARRAY( m_pThrowWeaponList);
	}

	if( m_pThrowItemList != nullptr)
	{
		UINT32 count = GetItemCountThrowItem();
		for( UINT32 i = 1; i < count; i++)
		{
			for( UINT32 j = 0; j < m_pThrowItemList[ i].size(); j++)
			{
				WeaponBase * pWeapon = m_pThrowItemList[i][j];
				pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
				pWeapon->ReturnInstance();
			}
		}
		I3_SAFE_DELETE_ARRAY( m_pThrowItemList);
	}
}

namespace {
	bool	s_EnableThrowWeapon( WeaponBase * pWeapon, i3Stage * pStage ) {
		if( pWeapon != nullptr && pWeapon->isEnable() == false)
		{
			pWeapon->SetEnable(true);
			if( pStage->getWorld() != nullptr )
				pStage->getWorld()->getWorldRoot()->AddChild( pWeapon->getSceneObject());
			return true;
		}
		return false;
	}
};

WeaponBase * CGameWeaponManager::GetFreeThrowWeapon( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number)
{
	i3::vector<WeaponGrenade*>* pList = nullptr;

	switch(classType)
	{
	case WEAPON_CLASS_THROWING_GRENADE:
		{
			pList = m_pThrowWeaponList;
			if( number == 0 || number > WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE) )
			{
				I3TRACE( "invalid ThrowWeapon number.\n" );
				return nullptr;
			}
		}
		break;
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:
		{
			pList = m_pThrowItemList;

			if(number > 0)
			{
				if(classType == WEAPON_CLASS_THROWING_ATTACK)		
					number = WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN) + number - 1;
				else if(classType == WEAPON_CLASS_THROWING_HEAL)
				{
					number = WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN)
						+ WEAPON::getItemCount(WEAPON_CLASS_THROWING_ATTACK) + number - 2;
				}
			}

			if( number == 0 || number > GetItemCountThrowItem() )
			{
				I3TRACE( "invalid ThrowItem number.\n" );
				return nullptr;
			}
		}
		break;

	case WEAPON_CLASS_KNIFE:
		{
			i3::vector<WeaponThrowKnife*> * pKnifeList = _GetThrowKnifeList( number);

			for( UINT32 i = 0; i < pKnifeList->size(); i++)
			{
				WeaponThrowKnife * pWeapon = pKnifeList->at(i);
				if( s_EnableThrowWeapon( pWeapon, m_pStage))
					return pWeapon;
			}

			return nullptr;
		}

	case WEAPON_CLASS_GRENADESHELL :
		{
			if( m_GrenadeShellWeaponList == nullptr)
				return nullptr;

			if( number <= 0 || number >= WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL) )
			{
				I3TRACE( "invalid GrenadeShell number.\n" );
				return nullptr;
			}

			UINT32 cnt = m_GrenadeShellWeaponList[number].size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				WeaponGrenadeShell * pWeapon = m_GrenadeShellWeaponList[number][i];
				if( s_EnableThrowWeapon( pWeapon, m_pStage))
					return pWeapon;
			}

			return nullptr; 
		}

	case WEAPON_CLASS_ROCKET_LAUNCHER :
		{
			WeaponRocketLauncherShell * pWeapon = nullptr;	

			UINT32 cnt = m_RocketLauncherShellWeaponList.size();
			for( size_t i = 0; i < cnt; i++)
			{
				pWeapon = m_RocketLauncherShellWeaponList[i];
				if( s_EnableThrowWeapon( pWeapon, m_pStage) )
					return pWeapon;
			}

			return nullptr;
		}
	}

	if( pList == nullptr)
		return nullptr;

	WeaponGrenade * pWeapon = nullptr;
	UINT32 cnt = pList[number].size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		pWeapon = pList[number][i];
		if( s_EnableThrowWeapon( pWeapon, m_pStage))
			return pWeapon;
	}

	return nullptr; 
}

void CGameWeaponManager::PushThrowWeapon(WeaponBase	* pWeapon)
{
	
	// 공중에 띄워줍니다.
	VEC3D vPos;
	i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10));
	MoveShapePosition( pWeapon, &vPos);

	if( i3::kind_of<WeaponGrenade*>(pWeapon) || 
		i3::same_of<WeaponC5*, WeaponThrowKnife*, WeaponBow*, WeaponGrenadeShell*, WeaponRocketLauncherShell*, WeaponHandGrenade*>(pWeapon) ) 
	{
		MainWeapon_Throw* mainWeapon = static_cast<MainWeapon_Throw*>(pWeapon->GetMainWeapon());
		mainWeapon->removeBombState( 0xFFFFFFFF);
	}
		
	// Network buffer reset
	_ResetNetworkInfo( pWeapon);

	pWeapon->SetEnable(false);
	pWeapon->SetOwner( nullptr);
	pWeapon->ResetColideeGroup();
	
	if( m_pStage->getWorld() != nullptr )
		m_pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());

	return;
}

WeaponGrenade * CGameWeaponManager::FindThrowWeaponByNetIndex( WEAPON::RES_ID type, INT32 netIdx)
{
	INT32 i;
	INT32 count = getThrowWeaponCount( type);

	for( i = 0; i < count; i++)
	{
		WeaponGrenade * pWeapon = getThrowWeapon( type, i);
		if( pWeapon != nullptr)
		{
			if( pWeapon->getNetworkIdx() == netIdx)
				return pWeapon;
		}
	}

	return nullptr;
}

void CGameWeaponManager::_LoadRes( WEAPON_CLASS_TYPE classType, INT32 number)
{
	char szFileName[ MAX_PATH];

	if( number == 0 || classType == WEAPON_CLASS_UNKNOWN)	return;	//	유효한 무기가 아니다.
	
	i3GameResChara * pRes = nullptr;
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);

	if( pWeaponInfo != nullptr)
	{
		if( __GetWeaponFileName( szFileName, pWeaponInfo) == true)
		{
			pRes = (i3GameResChara *) g_pFramework->FindResourceByOnlyName( szFileName);
			if( pRes != nullptr)
				__RegisterWeaponResource( classType, number, pRes);
			else
			{	// Common Res가 아니면 nullptr로 설정
				__RegisterWeaponResource( classType, number, nullptr);
			}

			return;
		}
	}
	
	//I3TRACE( "무기를 찾을 수 없습니다. %d, %d \n", classType, number);
	__RegisterWeaponResource( classType, number, nullptr);

	return;
}

void CGameWeaponManager::CreateGrenadeShellWeapon(i3Stage * pStage)
{
	_CreateThrow( pStage, WEAPON_CLASS_GRENADESHELL, WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL));
}



void CGameWeaponManager::DestroyGrenadeShellWeapon(i3Stage * pStage)
{
	if(m_GrenadeShellWeaponList != nullptr)
	{
		UINT32 count = WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL);
		for( UINT32 i = 1; i <count ; i++)
		{
			for( UINT32 j = 0; j < m_GrenadeShellWeaponList[i].size(); j++ )
			{
				WeaponBase * pWeapon = m_GrenadeShellWeaponList[i][j];				
				pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
				pWeapon->ReturnInstance();
			}
		}
		I3_SAFE_DELETE_ARRAY( m_GrenadeShellWeaponList);
	}
	return; 
}


void CGameWeaponManager::PushGrenadeShellWeapon( WeaponGrenadeShell * pWeapon)
{
	// 공중에 띄워줍니다.
#if !defined( NO_PHYSIX_HEIGHT )
	MATRIX mat;
	
	i3Matrix::SetPos( &mat, (REAL32)(pWeapon->getWeaponInfo()->GetNumber()) * 10.f, g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getNetworkIdx()) * 10.f, 1.f );
	i3Body * pBody = pWeapon->getSceneObject()->GetBody();
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	for( INT32 k = 0; k < pSkel->getBoneCount(); k++ )
	{
		if( pSkel->isShapeSet( k ))
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( k );
			if( pShapeSet != nullptr )
			{
				pShapeSet->setShapeGlobalPose( &mat );
			}
		}
	}
#endif

	MainWeapon_GrenadeShell* mainWeapon_GS = static_cast<MainWeapon_GrenadeShell*>(pWeapon->GetMainWeapon());
	mainWeapon_GS->removeBombState( 0xFFFFFFFF);

	// Network buffer reset
	_ResetNetworkInfo( pWeapon);

	pWeapon->SetOwner( nullptr);		// 리셋하기 전에 이전에 사용된 캐릭터를 초기화해줘야 한다.
	pWeapon->Reset();
	pWeapon->SetEnable(false);
	
	if( m_pStage->getWorld() != nullptr )
		m_pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
}

// Rocket Launcher Shell
void CGameWeaponManager::CreateRocketLauncherShellWeapon( i3Stage * pStage)
{
	WeaponRocketLauncherShell	* pWeapon	= nullptr;
	i3ClassMeta * pMeta		= nullptr;
	i3InstanceManager * pManager = pStage->GetInstanceManager( WeaponBase::static_meta() );	

	VEC3D vPos;

	i3ClassMeta * pOldMeta = pManager->getInstanceMeta();
	{	
		pMeta = WeaponRocketLauncherShell::static_meta();
		pManager->setInstanceMeta( pMeta);

		INT32 maxCnt = MAKE_ROCKETLAUNCHERSHELL_COUNT;		//	default count

		// Enable_RPG7ForAll의 경우 모든 맵에서 8vs8 대전으로 한사람당 3발씩 가지므로 갯수를 최대한 늘린다.
		if( g_pConfigEx->GetGame().UnlimitedForRPG7)
			maxCnt = MAKE_ROCKETLAUNCHERSHELL_FOR_ALL_COUNT;	//	Entended count

		T_ItemID Itemid;
		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
		{// 공룡모드 사용 RPG7Shell (현재 없음)
			Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1,	WEAPON_CLASS_GRENADESHELL, GetItemIndex("GRENADESHELL_RPG7_FOR_DINO"));
		}
		else
		{
			UINT8 WeaponFlag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();
			if (TESTBIT( WeaponFlag, LOCK_RPG7))
			{// revision 53430 RPG전 전용
				Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1,	WEAPON_CLASS_GRENADESHELL, GetItemIndex("GRENADESHELL_RPG7_FOR_MODE") );
			}
			else
			{	// 배치된 RPG7 사용 RPG7Shell
				Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1,	WEAPON_CLASS_GRENADESHELL, GetItemIndex("GRENADESHELL_RPG7"));
			}
		}

		for(INT32 i = 0; i < maxCnt; i++)
		{
			pWeapon = (WeaponRocketLauncherShell *) pManager->GetFreeInstance();
			I3ASSERT( pWeapon->GetRefCount() >= 1);

			pWeapon->Create( nullptr, Itemid, false);
			pWeapon->Reset();
			pWeapon->setIdx( m_nWeaponIDCount );
			m_nWeaponIDCount++;
			pWeapon->SetEnable(false);
			m_RocketLauncherShellWeaponList.push_back( pWeapon);

			// 생성 후 공중에 띄워줍니다.
			i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10) );
			MoveShapePosition( pWeapon, &vPos);
		}
	}
	pManager->setInstanceMeta( pOldMeta);
}

void CGameWeaponManager::DestroyRocketLauncherWeapon( i3Stage * pStage)
{
	for( size_t i = 0; i < m_RocketLauncherShellWeaponList.size(); i++)
	{
		WeaponRocketLauncherShell * pWeapon = m_RocketLauncherShellWeaponList[i];
		pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
		pWeapon->ReturnInstance();
	}

	m_RocketLauncherShellWeaponList.clear();
}

void CGameWeaponManager::PushRocketLauncherWeapon( WeaponRocketLauncherShell * pWeapon)
{
	// 공중에 띄워줍니다.
#if !defined( NO_PHYSIX_HEIGHT )
	MATRIX mat;
	
	i3Matrix::SetPos( &mat, (REAL32)(pWeapon->getWeaponInfo()->GetNumber()) * 10.f, g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getNetworkIdx()) * 10.f, 1.f );
	i3Body * pBody = pWeapon->getSceneObject()->GetBody();
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	for( INT32 k = 0; k < pSkel->getBoneCount(); k++ )
	{
		if( pSkel->isShapeSet( k))
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( k );
			if( pShapeSet != nullptr )
			{
				pShapeSet->setShapeGlobalPose( &mat );
			}
		}
	}
#endif

	MainWeapon_RLShell* mainWeapon = static_cast<MainWeapon_RLShell*>(pWeapon->GetMainWeapon() );

	static_cast<MainWeapon_RLShell*>(mainWeapon)->removeBombState( 0xFFFFFFFF);

	// Network buffer reset
	_ResetNetworkInfo( pWeapon);

	pWeapon->SetOwner( nullptr);
	pWeapon->Reset();
	pWeapon->SetEnable(false);
	
	if( m_pStage->getWorld() != nullptr )
		m_pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
}

void CGameWeaponManager::Destroy( void )
{
}

void CGameWeaponManager::PreStartRound( void)
{
		
}

void CGameWeaponManager::EnterStage( i3Stage * pStage )		
{
	m_pStage = pStage;
}

void CGameWeaponManager::LeaveStage( bool bDestroy )
{
	// 순서 중요합니다.
	if( i3::same_of<CStageBattle*>( m_pStage))
	{
		DestroyThrowWeapon( m_pStage);
		DestroyGrenadeShellWeapon( m_pStage);
		DestroyRocketLauncherWeapon( m_pStage);
		DestroyThrowKnife( m_pStage);

		RemoveAllDropWeapon();

		// WeaponLRU를 계산하여 재사용 무기는 common res로 전환
		WeaponLRU_Manager::i()->BackingDataFromAddList();
		WeaponLRU_Manager::i()->SortWeaponByPriority();
		WeaponLRU_Manager::i()->AfterLoad();	// 30분 이상
	}

	_RemoveAllCharaWeapon();

	// CommonRes가 아닌 무기는 ResourceManager에서 메모리를 해제시킨다.
	// List를 정리해주자
	RemoveWeaponRes( false);

	m_pStage		= nullptr;
	m_nWeaponIDCount = 0;	// 스테이지에서 나올때 초기화
}

void CGameWeaponManager::RemoveWeaponRes( bool bRemoveCommonRes)
{	
	__RemoveWeaponBGLoadInfo();

	i3ResourceManager * pMng = g_pFramework->GetResourceManager();

	i3::vector<size_t> avecClassID[WEAPON_CLASS_COUNT];
	CGameWeaponManager::_CollectBackgroundClassIDList(avecClassID);

	for(INT32 i = 1; i < WEAPON_CLASS_COUNT; i++)
	{
		const i3::vector<size_t>& vecClassID = avecClassID[i];

		for(size_t j = 0; j < m_WeaponResList[ i].size(); j++)
		{
			i3GameResChara * pRes = m_WeaponResList[ i][j];
			if( pRes != nullptr )
			{
				// CommonRes를 남기는 경우
				if( (bRemoveCommonRes == false) && (pRes->isCommonRes() == true) )
				{
					continue;
				}

				if( pRes->GetRefCount() <= 2)
				{
					i3::vector<size_t>::const_iterator it = i3::find(vecClassID.begin(), vecClassID.end(), j);

					if (it == vecClassID.end() )
					{
						pMng->RemoveResource( pRes);
					}
					else
					{
						I3TRACE("can't delete resource because of background loading : %s !!!! \n", pRes->GetName());
					}
				}

				m_WeaponResList[i][j] = nullptr;
			}
		}
	}
}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
bool CGameWeaponManager::IsExistResource( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number)
{
	if( m_WeaponResCheckList[ classType][ number] == false)
	{
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);

		if( pWeaponInfo == nullptr)
			return false;

		char szName[ MAX_PATH];
		if( __GetWeaponFileName( szName, pWeaponInfo) == true)
		{
			const char * pszFullPath = __GetWeaponResourcePath( pWeaponInfo);
			if( pszFullPath == nullptr)
				return nullptr;
			m_WeaponResCheckList[ classType][ number] = i3FileStream::isFile( pszFullPath);
		}
	}

	return m_WeaponResCheckList[ classType][ number];
}
#endif

i3GameResChara * CGameWeaponManager::QueryResource( WEAPON_CLASS_TYPE classType, INT32 number, bool bDirectLoad)
{
	char szName[ MAX_PATH];

	if (classType >= WEAPON_CLASS_COUNT)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[CGameWeaponManager::QueryResource] Not Find Weapon - classType : %d, number : %d\n", classType, number);
		return NULL;
	}

	if( classType == WEAPON_CLASS_UNKNOWN || (INT32)m_WeaponResList[ classType].size() < number)	return NULL;

	i3GameResChara * pRes = (i3GameResChara *) m_WeaponResList[ classType][ number];

	if( (pRes == nullptr || pRes->getSceneGraphInfo() == nullptr) && bDirectLoad)
	{
		// 메모리에 없다?
		// 바로 로딩을 해야 한다.
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);

		if( pWeaponInfo == nullptr)
			return nullptr;

		if( __GetWeaponFileName( szName, pWeaponInfo) == true)
		{
			const char * pszFullPath = __GetWeaponResourcePath( pWeaponInfo);
			if( pszFullPath == nullptr)
				return nullptr;
			if( i3FileStream::isFile( pszFullPath))
			{
				pRes = (i3GameResChara *) g_pFramework->QuaryResource( szName);
				I3ASSERT( pRes != nullptr);

				if( pRes->getSceneGraphInfo() == nullptr)
				{
					i3::rc_wstring wstrName;
					//무기가 등록이 안된 경우 무기 이름 표시로 변경
					pWeaponInfo->GetWeaponName(wstrName);

					i3::string strName;
					i3::utf16_to_mb(wstrName, strName);
					I3PRINTLOG(I3LOG_FATAL, "i3GameResChara 가 nullptr임 - %s\n  \
								RSC 파일에 무기 I3CHR 파일을 등록했는지 확인하세요.", strName.c_str() );
				}

				__RegisterWeaponResource( classType, number, pRes);
			}
		}
	}

	return pRes;
}

i3GameResChara * CGameWeaponManager::QueryResource( T_ItemID ItemID, bool bDirectLoad)
{
	WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( ItemID );
	WEAPON::RES_ID number = WEAPON::ItemID2ResID( ItemID );

	return QueryResource( classType, number, bDirectLoad);
}

WeaponBase * CGameWeaponManager::QueryWeapon( CGameCharaBase * pOwner, T_ItemID Itemid, T_ItemID subItem, bool MustCreate, bool bCreateInWeapon)
{
	// 사용할 수 있는 여유 무기가 있는지 확인
	if ( MustCreate == false )
	{
		WeaponBase * pWeapon = _FindFreeWeapon(Itemid);
		if ( pWeapon )
		{
			if( pWeapon->getOwner() == nullptr)
				pWeapon->setOwner(pOwner);
			return pWeapon;
		}
	}

	// 여유 무기가 없으면 생성함.
	return _CreateNewWeapon(Itemid, subItem, pOwner, bCreateInWeapon);
}

INT32 CGameWeaponManager::_SearchCharaWeapon( WeaponBase * pWeapon)
{	//	실패시 -1 반환. 성공시 idx 반환
	//return m_CharaWeaponList.IndexOf( (void *) pWeapon);
	return i3::vu::int_index_of(m_CharaWeaponList, pWeapon);
}

INT32 CGameWeaponManager::_SearchDropWeapon( WeaponBase * pWeapon)
{	//	실패시 -1 반환. 성공시 idx 반환
	//return m_DropWeaponList.IndexOf( (void *) pWeapon);
	return i3::vu::int_index_of(m_DropWeaponList, pWeapon);
}

void CGameWeaponManager::_RemoveAllCharaWeapon( void)
{
	for( INT32 i = 0; i < (INT32)m_CharaWeaponList.size(); i++)
	{
		WeaponBase * pWeapon = m_CharaWeaponList[i];
		I3ASSERT( pWeapon != nullptr);

		if( ReleaseCharaWeapon( pWeapon) == true)
			i = -1;	//듀얼 무기가 동시에 삭제 될 수 있으므로 처음부터 다시 처리하도록 합니다.
	}

	I3ASSERT( m_CharaWeaponList.size() == 0);
}

void CGameWeaponManager::RemoveAllDropWeapon( void)
{
	for( size_t i = 0; i < m_DropWeaponList.size(); i++)
	{
		WeaponBase * pWeapon = m_DropWeaponList[i];
		I3ASSERT( pWeapon != nullptr);

		if( _ReleaseDropWeapon( pWeapon) == true)
			i--;
	}

	I3ASSERT( m_DropWeaponList.size() == 0);
}

bool CGameWeaponManager::ReleaseCharaWeapon( WeaponBase * pWeapon)
{
	if( pWeapon != nullptr)
	{
		I3ASSERT( m_pStage != nullptr);
		
		if( (pWeapon->getPairWeapon() == nullptr) )
		{
#if defined( I3_DEBUG)
			i3::rc_wstring wstrWeaponName;
			pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
			I3TRACE( "[ReleaseCharaWeapon] %s (cnt:%d)\n", wstrWeaponName, (INT32)m_CharaWeaponList.size());
#endif

			i3::vu::remove(m_CharaWeaponList, pWeapon);
			pWeapon->ReturnInstance();		//	인스턴스 반환

			BattleObjContext::i()->ResetEventHandler( pWeapon);

			return true;
		}
	}

	return false;
}

bool CGameWeaponManager::_ReleaseDropWeapon( WeaponBase * pWeapon)
{
	if( pWeapon != nullptr)
	{
		I3ASSERT( pWeapon->getPairWeapon() == nullptr);

#if defined( I3_DEBUG)
		i3::rc_wstring wstrWeaponName;
		pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
		I3TRACE( "[ReleaseDropWeapon] %s %d (cnt:%d)\n", wstrWeaponName, pWeapon->getNetworkIdx(), (INT32)m_DropWeaponList.size());
#endif

		i3::vu::remove(m_DropWeaponList, pWeapon);
		pWeapon->ReturnInstance();		//	인스턴스 반환

		BattleObjContext::i()->ResetEventHandler( pWeapon);

		return true;
	}

	return false;
}

bool CGameWeaponManager::_AddCharaWeapon( WeaponBase * pWeapon)
{
	if( _SearchCharaWeapon( pWeapon) == -1)
	{
		i3::rc_wstring wstrName;
		pWeapon->getWeaponInfo()->GetWeaponName(wstrName);
		I3TRACE("_AddCharaWeapon - %s\n", wstrName);

		m_CharaWeaponList.push_back( pWeapon);
		I3_SAFE_ADDREF(pWeapon);
		return true;
	}

	I3ASSERT_0;		//여기까지 오면 않됨 20080701 

	return false;
}

void CGameWeaponManager::_ResetNetworkInfo( WeaponBase * pWeapon)
{	
	// 데디케이션 모드(일반전) 일 때, 무기 소유주가 자기 자신 or AI 캐릭터인 경우에는 처리 안함.
	if ( BattleServerContext::i()->IsUseDedicationMode() )
	{
		CGameCharaBase * pOwner = pWeapon->getOwner();
		if ( pOwner != nullptr && ( pOwner->isLocal() || pOwner->isAI() ) )
		{
			return;
		}
	}
	else if(BattleServerContext::i()->IsP2PHost())
	{ //P2P Host의 경우 패킷 전체를 관리(사실상 무기 소유주가 방장)하기 때문에 처리 안함.
		return;
	}

	// 무기 인덱스가 있을 때, 네트워크 정보 리셋
	if ((pWeapon->getNetworkIdx() > -1) &&	(pWeapon->getNetworkIdx() < MAX_THROWWEAPON_COUNT))
	{
		GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(pWeapon->getNetworkIdx());
		pInfo->Reset();
		pWeapon->setNetworkIdx(-1);
	}
}

bool CGameWeaponManager::MoveWeaponCharaToDrop( WeaponBase * pWeapon)
{	
	//	CharaList에서 DropList로 무기 인스턴스 이동
	//	CharaList에 등록되어 있는지 일단 검사
	if( _SearchCharaWeapon( pWeapon) != -1)
	{
		m_DropWeaponList.push_back( pWeapon);			//	DropList로 이동

		// 다중무기 시스템 사용 시 주무기 계열은 무조건 GameCharaMultiWeaponContext에서 관리
		// 무기 버리는 것은 같은 프레임에 가능
		if ( pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
			&& MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			// 여기서 다중무기 슬롯을 삭제
			CGameCharaBase* pChara = pWeapon->getOwner();

			if ( pChara != nullptr)
			{
				I3TRACE("무기를 드랍한 캐릭터 Index : %d\n", pChara->getCharaNetIndex());
				pChara->getMultiWeaponContext()->DropedMultiWeapon();
			}
		}

		i3::vu::remove(m_CharaWeaponList, pWeapon);		//	CharaList에서 삭제

		I3TRACE( "Move Weapon Chara -> Drop %d\n", pWeapon->getNetworkIdx());

		return true;
	}

	return false;
}

bool CGameWeaponManager::MoveWeaponDropToChara( WeaponBase * pWeapon, bool bOnlyList, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList)
{	
	//	DropList에서 CharaList로 무기 인스턴스 이동	
	//	DropList에 등록되어 있는지 일단 검사
	if( _SearchDropWeapon( pWeapon) != -1 || bCheckDropList == FALSE)
	{
#if defined(I3_DEBUG)
		i3::rc_wstring wstrName;
		pWeapon->getWeaponInfo()->GetWeaponName(wstrName);
		I3TRACE("MoveWeaponDropToChara - %s\n", wstrName);
#endif

		if( bPushWeaponList)
			m_CharaWeaponList.push_back( pWeapon);		//	CharaList로 이동 - 왜 이렇게 했더라...?

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon()
			&& pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
			&& bOnlyList == FALSE ) // bOnlyList는 뭐지?
		{
			CGameCharaBase * pChara = pWeapon->getOwner();

			I3ASSERT( pChara != nullptr);

			// 무기를 다중무기 시스템에 넣는다 - 여기서 교체까지 하면 안됨!!! Only 추가만!!
			pChara->getMultiWeaponContext()->AddMultiWeapon( pWeapon, nSyncNumber);
		}

		i3::vu::remove(m_DropWeaponList, pWeapon);		//	DropList에서 삭제

		I3TRACE( "Move Weapon Drop -> Chara %d\n", pWeapon->getNetworkIdx());
		
		return true;
	}

	return false;
}

WeaponBase * CGameWeaponManager::FindDropWeapon( CGameCharaBase * pOwner, WEAPON_CLASS_TYPE type, INT32 nNumber)
{
	for( size_t i = 0; i < m_DropWeaponList.size(); i++)
	{
		WeaponBase * pCurWeapon = m_DropWeaponList[i];

		I3ASSERT( i3::kind_of<WeaponBase*>(pCurWeapon));

		CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
		I3ASSERT( pCurWeaponInfo != nullptr);
		
		if( pOwner != nullptr)
		{
			if( pCurWeapon->getOwner() != pOwner)		continue;
		}

		if( pCurWeaponInfo->isSame(type, nNumber))
			return pCurWeapon;
	}

	return nullptr;
}

WeaponBase * CGameWeaponManager::FindDropWeaponByIdx( INT32 nIdx )
{
	for( size_t i = 0; i < m_DropWeaponList.size(); i++ )
	{
		WeaponBase * pWeapon = m_DropWeaponList[i];
		if( pWeapon->getNetworkIdx() == nIdx )
			return pWeapon;
	}
	return nullptr;
}

WeaponBase * CGameWeaponManager::SetDropWeapon( INT32 idx, WEAPON_SLOT_TYPE nWeaponSlot, INT16 nDropIdx, bool bNextWeapon, bool bDirectPickup )
{
	VEC3D vLin, vDropPos;
	
	WeaponBase * pWeapon;
	WeaponBase * pWorldWeapon;

	I3ASSERT( idx < SLOT_MAX_COUNT );
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx );
	if( pChara == nullptr )
	{
		// 조금 더 생각해봅시다.
		I3TRACE( "CGameContext::SetDropWeapon() - 캐릭터가 없습니다. idx:%d 문의 구리사마\n", idx );
		return nullptr;
	}

	CGameCharaWeaponContext * pWeaponCtx = pChara->getWeaponContext();

	if( pWeaponCtx == nullptr )		return nullptr;

	pWeapon = pWeaponCtx->getWeapon( nWeaponSlot );
	if( pWeapon == nullptr )		return nullptr;

	if( pWeapon->getWeaponInfo()->isEnableDrop() == false)
	{
		// No drop weapon
		I3PRINTLOG(I3LOG_WARN,  "버리는 무기가 아닙니다. class : %d, num : %d", pWeapon->getWeaponInfo()->GetTypeClass(), pWeapon->getWeaponInfo()->GetNumber() );
		return nullptr;
	}

	bool bHost = pWeapon->IsDedicatedHost();

	// 이미 사용중인 인덱스에 총기 드랍 상태라면 그 총을 지워줍니다. 새로 덮어 씌운다.
	pWorldWeapon = FindDropWeaponByIdx( nDropIdx );
	if( pWorldWeapon != nullptr )
	{
		PushDropWeaponToChara( pWorldWeapon );
	}

	// 무기를 캐릭터에서 띄어낸다.
	pChara->Cmd_DetachWeapon( pWeapon);

	if( !bNextWeapon )
	{	// 다음 무기를 사용하지 않는다면 자동으로 버리기
		pChara->Cmd_DropWeapon( nWeaponSlot, i3Matrix::GetPos( pWeapon->getSceneObject()->GetCacheMatrix()), false);

		if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			pChara->getMultiWeaponContext()->ClearWeaponSlotList();
	}

	// 컬리젼 그룹을 Simulate용으로 전환
	if( bHost)
	{
		if( pWeapon->getContactHitBox() != nullptr )
			pWeapon->getContactHitBox()->modifyDynamicState( 0, I3_PHYSIX_STATE_NO_COLLISION);
	}

	pWeapon->setNetworkIdx( nDropIdx );
	pWeapon->OnSetBlendAlpha( 1.f );

	pWeapon->SetVisible(true);
	
	pChara->getCameraContext()->getVirtualCamPosition( &vDropPos);
	addY( &vDropPos, -0.2f);

	MATRIX mtx;

	i3Matrix::Copy( &mtx, pChara->GetMatrix());
	if(bNextWeapon)
		i3Matrix::SetPos( &mtx, &vDropPos);

	pWeapon->SetMatrix( &mtx );

	if( bHost || (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
	{	// 방장은 해당 무기를 Physix로 합니다.
		MATRIX mat;

		VEC3D * pAt = i3Matrix::GetAt( pChara->GetMatrix() );

		i3Matrix::SetRotateAxis( &mat, i3Matrix::GetRight( pChara->GetMatrix() ), -pChara->getBoneContext()->getRhoValue() );
		i3Vector::TransformCoord( &vLin, pAt, &mat );
	
		VEC3D vAddPos;
		i3Vector::Scale( &vAddPos, &vLin, 0.05f);
		i3Vector::Add( &vDropPos, &vDropPos, &vAddPos);
		
		if( bNextWeapon )
		{	// 무기를 직접 버리는 경우에는 앞으로 던지도록 합니다. (캐릭터가 죽은경우에는 힘을 주지 않음)
			i3Vector::Scale( &vLin, &vLin, 6.8f );
		}

		i3Skeleton * pSkel = pWeapon->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++)
		{
			if( pSkel->isShapeSet( i))
			{
				i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i);
				if(bNextWeapon)
					pShapeSet->setFBLinearVelocity( &vLin);
			}
		}

		pWeapon->setDynamicActiveState( true, false );
		pWeapon->SetDynamicState( I3GAME_DYNAMIC_STATE_DYNAMIC, true);
	}
	
	
	pWeapon->SetPos( &vDropPos);
		
	// 다음 무기를 들게 됩니다.
	if( bNextWeapon )
	{
		pChara->Cmd_DropWeapon( nWeaponSlot, &vDropPos, true, bDirectPickup);
	}
	else
	{// changing Knife & visiable off
		pChara->Cmd_ChangeWeapon( WEAPON_SLOT_MELEE, true);

		if( pWeaponCtx->getCurrent() != nullptr )
		{
			pWeaponCtx->getCurrent()->SetEnable( false);
			pWeaponCtx->getCurrent()->DetachNode();
		}
	}

	pWeapon->OnSetVisibleExtension( false );
	pWeapon->OnDrop();

	// xgkick_thai
	// 방장이 아닌 경우, 자신의 무기가 주워지지 않는 문제를 해결하기 위해 수정.
	pWeapon->setTeamTag( pChara->getCharaTeam());

	// revision 29616
	//GM 옵저버 모드 일 경우 데스 후 GM의 무기가 다른 유저에게 안보이도록 함.( GM 자신은 보입니다.. )
	if( BattleSlotContext::i()->IsGMObserverForClient( pChara->getCharaNetIndex())
		&& pChara != g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx()))
	{
		PushDropWeaponToChara( pWeapon);
	} 

	I3TRACE( "[DropWeapon] chara : %d, id : %d, idx : %d\n", idx, pWeapon->getNetworkIdx(), pWeapon->getNetworkIdx() );

	return pWeapon;
}

void CGameWeaponManager::PushDropWeaponToChara( WeaponBase * pWeapon )
{
	I3TRACE( "Remove DroppedWeapon %d %d\n", pWeapon->getNetworkIdx(), pWeapon->getNetworkIdx());

	MoveWeaponDropToChara( pWeapon, TRUE, 0, TRUE, TRUE);

	pWeapon->setNetworkIdx( -1);
	pWeapon->SetOwner( nullptr );			// 누구든지 사용할 수 있도록 합니다
	pWeapon->SetEnable( false);			// 노드에서 끕니다

	// 공중으로 보냅니다.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)(pWeapon->getIdx() / 10) );
	MoveShapePosition( pWeapon, &vec );
}

void CGameWeaponManager::MoveShapePosition( WeaponBase * pWeapon, VEC3D * pPos )
{
	if( pWeapon == nullptr )
		return;

	if( pWeapon->GetRefCount() < 2)
		return;

	i3Body * pBody = pWeapon->getSceneObject()->GetBody();

	if( pBody == nullptr)
		return;

	if( pBody->getFrameLOD() == nullptr)
		return;

	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	if( pSkel == nullptr)
		return;

	pWeapon->SetPos( pPos );

	MATRIX mat;
	i3Matrix::SetPos( &mat, pPos );
	for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
	{
		if( pSkel->isShapeSet( i) )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet != nullptr )
			{	
				pShapeSet->setShapeGlobalPose( &mat );
			}
		}
	}
}


void CGameWeaponManager::OnUpdate( REAL32 rDeltaTime )
{
	
	// EmptyCartridgeSoundPlay
	UINT32 cnt = m_CharaWeaponList.size();
	for( UINT32 i = 0; i < cnt; i++ )
	{
		WeaponBase* pWeapon = m_CharaWeaponList[i];
		pWeapon->ProcessSound( rDeltaTime );
	}	

	//throw Weapon 
	if( m_pThrowWeaponList != nullptr)
	{
		WeaponBase * pGrenade = nullptr;
		for( UINT32 i = 1; i  < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE) ; i++)
		{
			cnt = m_pThrowWeaponList[i].size();
			for( UINT32 j = 0; j < cnt; j++)
			{
				pGrenade = m_pThrowWeaponList[i][j];
				if( pGrenade->isEnable() )
				{	
					pGrenade->OnUpdate(rDeltaTime); 
				}
			}
		}		
	}

	if( m_pThrowItemList != nullptr)
	{
		WeaponBase * pGrenade = nullptr;
		UINT32 count = GetItemCountThrowItem();

		for( UINT32 i = 1; i  < count ; i++)
		{
			cnt = m_pThrowItemList[i].size();
			for( UINT32 j = 0; j < cnt; j++)
			{
				pGrenade = m_pThrowItemList[i][j];
				if( pGrenade->isEnable() )
					pGrenade->OnUpdate(rDeltaTime); 
			}
		}		
	}

	if( m_GrenadeShellWeaponList != nullptr)
	{
		WeaponBase * pGrenade = nullptr;
		UINT32 count = WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL);
		for( UINT32 i = 1; i < count ; i++)
		{
			cnt = m_GrenadeShellWeaponList[i].size();
			for( UINT32 j = 0; j < cnt; j++)
			{
				pGrenade = m_GrenadeShellWeaponList[i][j];
				if( pGrenade->isEnable() )
					pGrenade->OnUpdate(rDeltaTime); 
			}
		}		
	}
	
	// Rocket Launcher shell (RPG-7)
	cnt = m_RocketLauncherShellWeaponList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		WeaponRocketLauncherShell * pWeapon = m_RocketLauncherShellWeaponList[i];
		if( pWeapon->isEnable() )
			pWeapon->OnUpdate( rDeltaTime);
	}

	// Throw knife (miniaxe)
	tThrowKnifeCon::iterator it = m_ThrowKnifeList.begin();
	while( it != m_ThrowKnifeList.end())
	{
		i3::vector<WeaponThrowKnife*> & list = it->second;
		cnt = list.size();
		for( UINT32 i = 0; i < cnt; i++)
		{
			WeaponThrowKnife * pWeapon = list[i];
			if( pWeapon->isEnable() )
				pWeapon->OnUpdate( rDeltaTime);
		}
		it++;
	}

	//DropWeapon
	_ProcessDropedWeapon( rDeltaTime);

	// Background load
	__BackgroundLoading();

#if defined( I3_DEBUG)
	// 디버그 AI에서는 무기를 랜덤으로 생성하기에 메모리 부족이 생길 수 있음.
	// 오래된 리소스는 삭제하도록 한다.
	TrashWeaponRes( rDeltaTime);
#endif
}


void CGameWeaponManager::_ProcessDropedWeapon( REAL32 rDeltaTime)
{
	bool	bSend;
	bool	bHost;

	for( size_t i = 0; i < m_DropWeaponList.size(); i++ )
	{
		WeaponBase * pWeapon = m_DropWeaponList[i];
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		if( (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  == false)
			&& ( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION ))
			continue;

		if( pWeapon->getNetworkIdx() == -1 )
		{
			// 네트워크 싱크로 이곳에 들어 올수 있습니다.
			PushDropWeaponToChara( pWeapon );
			i--;	// 리스트에서 지웠기에 카운트를 이전부터 다시 체크합니다.
			continue;
		}

		GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( pWeapon->getNetworkIdx() );
		if( pInfo->_pWeaponRef == nullptr )	//
		{	// 알수 없는 무기
			I3TRACE( "invalid droped weapon _pWeaponRef == nullptr. %d - %d\n", pWeapon->getNetworkIdx(), pWeapon->getNetworkIdx() );
			I3TRACE( "[RemoveDropWeapon] idx : %d\n", pWeapon->getNetworkIdx() );
			// Ageia. 무기를 삭제하지 말고 다시 사용하기 위해 CharaList로 옮깁니다.
			PushDropWeaponToChara( pWeapon );
			pInfo->Reset();
			i--;	// 리스트에서 지웠기에 카운트를 이전부터 다시 체크합니다.
			continue;
		}

		REAL32 fLifeTime = pInfo->_fDropedWeaponLifeTime += rDeltaTime;
		
		bHost = pWeapon->IsDedicatedHost();

		if( bHost )
		{
			//////////////////////////////////////////////////////////////////////////
			//								( P2PServer )							//
			if( CStageInfoUtil::IsDeletableDroppedWeapon(*MyRoomInfoContext::i()->getStage()) )
			{// 일정 시간이 지나면 무기를 월드에서 삭제합니다
				if( fLifeTime > DROPWEAPON_ALPHATIME )
				{
					pWeapon->setDroped( false );
					pWeapon->OnSetBlendAlpha( ((DROPWEAPON_ALPHASLOPE) - (fLifeTime - DROPWEAPON_ALPHATIME)) * DROPWEAPON_ALPHASLOPEINV );
				}

				if( fLifeTime > DROPWEAPON_LIFETIME )
				{	
					pInfo->setState( NET_WEAPONDROPED_REMOVE);
					pInfo->_fDropedWeaponLifeTime = 0.f;
				}
			}

			// 잔탄량을 확인하여 0이면 삭제합니다.
			if( (pWeapon->getTotalBulletCount() == 0 && pWeapon->getLoadedBulletCount() == 0 && pWeapon->getLoadedBulletDualCount() == 0)
				&& pWeapon->IsDroped() )
			{	
				if( fLifeTime > 1.f )
					pWeapon->OnSetBlendAlpha( (1.f - (fLifeTime - 1.f)));
				if( fLifeTime > 2.f )
				{
					pInfo->setState( NET_WEAPONDROPED_REMOVE);
					pInfo->_fDropedWeaponLifeTime = 0.f;
				}
			}


			bSend = false;

			switch( pInfo->getState() )
			{
			case NET_WEAPONDROPED_SLEEP :
				// 1초에 2회마다 싱크를 맞춰줍니다.(난입하거나 패킷을 못 받았을 경우를 대상으로 합니다)
				pInfo->_fSendTime += rDeltaTime;
				if( pInfo->_fSendTime >= DROPEDWEAPON_MINSENDTIME )
				{
					bSend = true;
				}
				break;

			case NET_WEAPONDROPED_START :	// 최초 1회는 좀더 빨리 보내줍니다.
				bSend = true;
				pInfo->setState( NET_WEAPONDROPED_FLY);
				break;

			case NET_WEAPONDROPED_FLY :
				pInfo->_fSendTime += rDeltaTime;
				if( pInfo->_fSendTime >= DROPEDWEAPON_SENDTIME )
				{
					bSend = true;
				}
				break;

			case NET_WEAPONDROPED_NONE :
				break;

			case NET_WEAPONDROPED_GET :
				pInfo->setState( NET_WEAPONDROPED_REMOVE ); 
				bSend = true;
				break;

			default ://NET_WEAPONDROPED_BOUND//NET_WEAPONDROPED_REMOVE
				bSend = true;
				break;
			}

			if( bSend )
			{
				pInfo->_fSendTime = 0.f;

				// 회전값을 구합니다.
				MATRIX mat;
				
				i3Matrix::SetRight( &mat, 0.f, 0.f, 1.f, 0.f );
				i3Matrix::SetUp( &mat, 1.f, 0.f, 0.f , 0.f );
				i3Matrix::SetAt( &mat, 0.f, 1.f, 0.f, 0.f );
				i3Matrix::Mul( &mat, &mat, pWeapon->GetMatrix() );

				// broadcast packet
				GAMEEVENT::Write_DroppedWeapon( pWeapon->getNetworkIdx(), pWeapon->GetPos(), &mat);

				// process after send
				if( pInfo->getState() == NET_WEAPONDROPED_REMOVE)
				{
					I3TRACE( "[RemoveDropWeapon] idx : %d\n", pWeapon->getNetworkIdx() );
						
					// Ageia. 무기를 삭제하지 말고 다시 사용하기 위해 CharaList로 옮깁니다.
					PushDropWeaponToChara( pWeapon );
				}
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//									( Client )							//
			if( CStageInfoUtil::IsDeletableDroppedWeapon(*MyRoomInfoContext::i()->getStage()) )
			{	
				if( fLifeTime > DROPWEAPON_ALPHATIME )
				{
					pWeapon->setDroped( false );
					pWeapon->OnSetBlendAlpha( (DROPWEAPON_ALPHASLOPE - (fLifeTime - DROPWEAPON_ALPHATIME)) * DROPWEAPON_ALPHASLOPEINV );
				}

				if( fLifeTime > DROPWEAPON_LIFETIME + 1.f )
				{
					pInfo->setState( NET_WEAPONDROPED_REMOVE);
					pInfo->_fDropedWeaponLifeTime = 0.f;
					I3TRACE( "Remove Dropped weapon by lifetime.\n " );
				}
			}

			// 잔탄량을 확인하여 0이면 삭제합니다.
			if( (pWeapon->getTotalBulletCount() == 0 && pWeapon->getLoadedBulletCount() == 0 && pWeapon->getLoadedBulletDualCount() == 0 ) 
				&& pWeapon->IsDroped() )
			{	
				if( fLifeTime > 1.5f )
				{
					pWeapon->OnSetBlendAlpha( (1.f - (fLifeTime - 1.5f)));
				}
				if( fLifeTime > 3.f )
				{
					pInfo->setState( NET_WEAPONDROPED_REMOVE);
					pInfo->_fDropedWeaponLifeTime = 0.f;
				}
			}

			if( pInfo->isDroped() || fLifeTime > 3.f )
			{	// 무기 드랍상태 체크(클라이언트는 패킷을 못 받은 경우가 있을 수 있으므로 3초 후에는 드랍된 상태로 체크한다)
				pWeapon->setDroped( true);
			}

			switch( pInfo->getState() )
			{
			case NET_WEAPONDROPED_FLY:
			case NET_WEAPONDROPED_START:
				pInfo->_tAssistData._fAssistTime += rDeltaTime;
				if( pInfo->_tAssistData._fAssistTime > DROPEDWEAPON_ASSIST_TIME )
					pInfo->_tAssistData._fAssistTime = DROPEDWEAPON_ASSIST_TIME;
				
				break;
			case NET_WEAPONDROPED_SLEEP:
				pInfo->_tAssistData._fAssistTime = DROPEDWEAPON_ASSIST_TIME;
				i3Vector::Copy( &pInfo->_tAssistData._vPostPos, const_cast<VEC3D*>(pInfo->_tNetData.getPosition()) );
				pWeapon->setDroped( true );
				break;
			case NET_WEAPONDROPED_REMOVE :
				{
					PushDropWeaponToChara( pWeapon );

					// 정보 초기화
					pInfo->Reset();
					i--;	// 드랍리스트에서 삭제했기에 이전부터 다시 검사
					continue;
				}
				break;

			case NET_WEAPONDROPED_BOUND :
				{
					//VEC3D vUp;
					//i3Vector::Set( &vUp, 0.f, 1.f, 0.f );
					//g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_WEAPON_DOWN_COMMON, &pInfo->_vRecvPos, &vUp );
					pWeapon->setDroped( true );

					pInfo->_tAssistData._fAssistTime += rDeltaTime;
					if( pInfo->_tAssistData._fAssistTime > DROPEDWEAPON_ASSIST_TIME )
						pInfo->_tAssistData._fAssistTime = DROPEDWEAPON_ASSIST_TIME;
				}
				break;
			}

			VEC3D	vPos;
			REAL32 fSlerp = pInfo->_tAssistData._fAssistTime * DROPEDWEAPON_ASSIST_INVTIME;
			i3Vector::Lerp( &vPos, &pInfo->_tAssistData._vPostPos, pInfo->_tNetData.getPosition(), fSlerp );
			
			i3Matrix::SetRotateQuat( pWeapon->GetMatrix(), pInfo->_tNetData.getRotation() );	// 로테이션은 인터폴레이트를 하지 않습니다.
			i3Matrix::PostTranslate( pWeapon->GetMatrix(), &vPos );

			pInfo->_bRecv = false;
		}

	}
}

i3::vector<WeaponThrowKnife*> * CGameWeaponManager::_GetThrowKnifeList( WEAPON::RES_ID num)
{
	tThrowKnifeCon::iterator it = m_ThrowKnifeList.find( num);
	if( it != m_ThrowKnifeList.end())
		return &it->second;

	I3PRINTLOG(I3LOG_FATAL,  "insert code here for throw knife!");
	return nullptr;
}

void CGameWeaponManager::_CreateThrowKnifeInstance( i3Stage * pStage, WEAPON::RES_ID weaponNumber)
{	
	VEC3D vPos;
	I3ASSERT( pStage != nullptr);

	tThrowKnifeCon::iterator it = m_ThrowKnifeList.find( weaponNumber);
	if( it != m_ThrowKnifeList.end())
	{	// 이미 있다.
		return;
	}

	CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_KNIFE, weaponNumber);
	if( pInfo == nullptr)	return;

	// 등록되지 않은 무기인 경우, 생성하지 않음.
	if( IsRegisteredWeapon(WEAPON_CLASS_KNIFE, weaponNumber) == false )
		return;

	i3::vector<WeaponThrowKnife*> List;

	i3InstanceManager * pManager = pStage->GetInstanceManager( WeaponBase::static_meta() );

	i3ClassMeta * pOldMeta = pManager->getInstanceMeta();
	pManager->setInstanceMeta( WeaponThrowKnife::static_meta());

	PreloadWeaponResource( WEAPON_CLASS_KNIFE, weaponNumber);

	I3ASSERT( i3::same_of<CMeleeWeaponInfo*>(pInfo));
	INT32 cnt = ((CMeleeWeaponInfo*)pInfo)->GetInstanceCount();

	List.reserve( cnt);

	for(INT32 i = 0; i < cnt; i++)
	{
		WeaponThrowKnife * pWeapon = (WeaponThrowKnife *) pManager->GetFreeInstance();
		I3ASSERT( pWeapon->GetRefCount() >= 1);
		
		pWeapon->Create( nullptr, pInfo->GetItemID(), true );
		pWeapon->Reset();
		pWeapon->setIdx( m_nWeaponIDCount);
		m_nWeaponIDCount++;
		pWeapon->SetEnable(false);

		// 생성 후 공중에 띄워줍니다.
		i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10) );
		MoveShapePosition( pWeapon, &vPos);

		List.push_back( pWeapon);
	}

	pManager->setInstanceMeta( pOldMeta);

	m_ThrowKnifeList.insert(tThrowKnifeCon::value_type(weaponNumber, List));
}

void CGameWeaponManager::_CreateThrowKnife( i3Stage * pStage, CWeaponInfo * pWeaponInfo, bool bMode)
{
	I3ASSERT( pWeaponInfo != nullptr);

	for( UINT32 j = 0; j < MAX_WEAPON_EXTENSION; j++)
	{
		if( pWeaponInfo->getExtensionType( j) == WEAPON::EXT_THROW)
		{
			T_ItemID extensionID = pWeaponInfo->GetExtensionItemID( j);
			if( extensionID > 0)
			{
				CWeaponInfo * pExtensionInfo = g_pWeaponInfoDataBase->getWeaponInfo( extensionID, true);
				
				if( pExtensionInfo->IsThrowWeapon() &&
					(pWeaponInfo->isModeWeapon() == bMode))
				{
					_CreateThrowKnifeInstance( pStage, WEAPON::ItemID2ResID( extensionID) );
					break;
				}
			}
		}
	}
}

void CGameWeaponManager::_CreateThrowKnife( i3Stage * pStage, WEAPON_CLASS_TYPE mainClassType)
{
	UINT32 cnt = WEAPON::getItemCount( mainClassType);
	for( UINT32 i = 1; i < cnt; i++)
	{
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( mainClassType, i);
		if( pWeaponInfo == nullptr)
			continue;

		_CreateThrowKnife( pStage, pWeaponInfo);
	}
}

void CGameWeaponManager::CreateAllThrowKnife( i3Stage * pStage)
{
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  &&
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
	{
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::FRYINGPAN), true);
		_CreateThrowKnife( pStage, pWeaponInfo, true);

		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::HAIRDRYER), true);
		_CreateThrowKnife( pStage, pWeaponInfo, true);

		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::CHINESECLEAVER_SEIZED), true);
		_CreateThrowKnife( pStage, pWeaponInfo, true);
	}
	else
	{
		// 던지는 나이프류에 화살도 포함됩니다.
		_CreateThrowKnife( pStage, WEAPON_CLASS_KNIFE);
		_CreateThrowKnife( pStage, WEAPON_CLASS_DUALKNIFE);
		_CreateThrowKnife( pStage, WEAPON_SUBCLASS_BOW);
	}
}


void CGameWeaponManager::DestroyThrowKnife( i3Stage * pStage)
{
	I3ASSERT( pStage != nullptr);

	tThrowKnifeCon::iterator it = m_ThrowKnifeList.begin();
	while( it != m_ThrowKnifeList.end())
	{
		i3::vector<WeaponThrowKnife*> & list = it->second;
		UINT32 cnt = list.size();
		for( UINT32 i = 0; i < cnt; i++ )
		{
			WeaponBase * pWeapon = list[i];
			pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
			pWeapon->ReturnInstance();
		}
		list.clear();
		it++;
	}

	m_ThrowKnifeList.clear();
}


void CGameWeaponManager::PushThrowKnife( WeaponBase * pWeapon)
{	
	// 공중에 띄워줍니다.
	VEC3D vPos;
	I3ASSERT( pWeapon != nullptr);

	i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10));
	MoveShapePosition( pWeapon, &vPos);

	// Network buffer reset
	_ResetNetworkInfo( pWeapon);

	pWeapon->SetOwner( nullptr);
	pWeapon->SetEnable(false);
	
	if( m_pStage->getWorld() != nullptr )
		m_pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());

	return;
}



void CGameWeaponManager::StageRoundEnd( void)
{


	// 라운드가 종료되면 1.5초후에 사라지도록 합니다.
	for( UINT32 i = 0; i < m_DropWeaponList.size(); i++ )
	{
		WeaponBase * pWeapon = m_DropWeaponList[i];
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION )
			continue;

		if( pWeapon->getNetworkIdx() == -1 )	continue;

		GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( pWeapon->getNetworkIdx() );
		if( pInfo->_pWeaponRef == nullptr )	continue;

		pInfo->_fDropedWeaponLifeTime = DROPWEAPON_ALPHATIME - 0.5f;
	}

	// All grenade weapon is disable
	if(m_pThrowWeaponList != nullptr)
	{
		for( UINT32 i = 1;i < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE); i++)
		{
			for( UINT32 j = 0; j < m_pThrowWeaponList[i].size(); j++ )
			{
				WeaponBase * pWeapon = m_pThrowWeaponList[i][j];
				PushThrowWeapon( pWeapon );
			}
		}
	}

	if( m_pThrowItemList != nullptr)
	{
		UINT32 count = GetItemCountThrowItem();
		for( UINT32 i = 1; i < count; i++)
		{
			for( UINT32 j = 0; j < m_pThrowItemList[ i].size(); j++)
			{
				WeaponBase * pWeapon = m_pThrowItemList[i][j];
				PushThrowWeapon( pWeapon);
			}
		}
	}

	//RemoveAllDropWeapon();	//	스테이지 초기화이므로 월드에 떨어진 무기만 제거한다.

	// All grenade weapon is disable
	if(m_GrenadeShellWeaponList != nullptr)
	{
		UINT32 count = WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL);
		for(UINT32 i = 1; i < count; i++)
		{
			for( UINT32 j = 0; j < m_GrenadeShellWeaponList[i].size(); j++ )
			{
				WeaponBase * pWeapon = m_GrenadeShellWeaponList[i][j];
				PushThrowWeapon( pWeapon );
			}
		}
	}

	for(size_t i = 0; i < m_RocketLauncherShellWeaponList.size(); i++)
	{
		WeaponRocketLauncherShell * pWeapon = m_RocketLauncherShellWeaponList[i];
		PushRocketLauncherWeapon( pWeapon);
	}

}

void CGameWeaponManager::PreloadWeaponResource( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number)
{
	char pszRes[256] = "";

	CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(classType, number);

	if ( pInfo == nullptr)
		return;


	INT32 len = i3::generic_string_size( pInfo->GetName());
	if( len > 0)
	{
		I3ASSERT( len < 256);

		GlobalFunc::DecryptValue( pszRes, pInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);

		i3::generic_string_cat( pszRes, ".I3CHR");

		// 무기 I3CHR 리소스가 실제로 로딩되었는지 확인
		g_pFramework->QuaryResource( pszRes);
	}
}

bool CGameWeaponManager::IsRegisteredWeapon(WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number)
{
	// 등록된 무기인지 확인.	
	CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(classType, number);
	return (__GetWeaponResourcePath( pInfo) != nullptr);
}

WeaponBase * CGameWeaponManager::_CreateNewWeapon( T_ItemID Itemid, T_ItemID subItem, CGameCharaBase * pOwner, bool bCreateInWeapon )
{
//	char szClassMeta[256];

	// Instance로 무기 생성
	if( m_pStage == nullptr)
		return nullptr;

	i3InstanceManager * pManager	= m_pStage->GetInstanceManager( WeaponBase::static_meta());
	i3ClassMeta * pMeta				= WeaponBase::static_meta();

	if( subItem == 0)
		subItem = Itemid;

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid, true);
	if( pWeaponInfo == nullptr)
	{
		return nullptr;
	}

	
	//	Weapon Class Meta - komet
	//	새로운 무기를 쉽게 추가하기 위해 소스에서 코드를 추가했던 옛날 방식을 PEF 파일에서 문자열로 저정할수 있도록 수정.
	//	BC파일에 지정된 클래스 메타 이름을 받아온다.
	//	지정이 안되있으면 기본적으로 WeaponBase로 설정한다.
	
	i3::rc_wstring	wstrClassMeta;

	pWeaponInfo->GetClassMetaName(wstrClassMeta);

//	char * pszCreateClassMeta = szClassMeta;
	if( i3::generic_string_size( wstrClassMeta) > 0)
	{
		i3::string		strClassMeta;
		i3::utf16_to_mb(wstrClassMeta, strClassMeta);

		pMeta = i3ClassMetaSystem::i()->find_class_meta(strClassMeta);
			// i3ClassMeta::FindClassMetaByName( pszCreateClassMeta);
		I3ASSERT( pMeta != nullptr);

		if( pMeta == nullptr)
		{
			char message[MAX_STRING_COUNT] = "";
			i3::snprintf( message, MAX_STRING_COUNT, 
				"[%s] 해당 무기의 class meta를 찾을수 없거나 등록되지 않았습니다. 다시 확인해 주십시오.", strClassMeta);
			I3PRINTLOG(I3LOG_NOTICE,message); 

			return nullptr;
		}
	}

	i3ClassMeta * pOldMeta		= pManager->getInstanceMeta();
	pManager->setInstanceMeta( pMeta);

	WeaponBase * pQuaryWeapon = (WeaponBase *) pManager->GetFreeInstance();
	I3ASSERT( pQuaryWeapon->GetRefCount() == 2);

	pManager->setInstanceMeta( pOldMeta);

	//	반드시 Instance를 사용하기 위해서는 AttachStage()되어 있어야 한다. 주의!
	I3ASSERT( pQuaryWeapon != nullptr);

	// 캐릭터 애니메이션도 로딩하자
	if( pOwner != nullptr && pOwner->getCharaInfo()->GetType() != CHARA::TYPE_PROFILE)
	{
		//0.6ms
		pOwner->PreloadAnimationByWeapon( subItem);
	}

	// 8ms
	pQuaryWeapon->Create( pOwner, subItem, bCreateInWeapon);

	// 컨택트 히트 박스
	if( pOwner != nullptr && g_pEnvSet->m_nEffectLevel > 1)
		pQuaryWeapon->CreateContactHitBox( pOwner->getCharaNetIndex());

	//	캐릭터 무기 인스턴스를 리스트에 추가
	_AddCharaWeapon( pQuaryWeapon );		// 이 쪽에서 ADDREF되었으므로..
	pQuaryWeapon->Release();				// 여기서 Relase처리..(2015.04.27.수빈)


	// 무기에 고유 인덱스를 부여합니다.
	pQuaryWeapon->setIdx( m_nWeaponIDCount );
	m_nWeaponIDCount++;

	return pQuaryWeapon;
}

WeaponBase * CGameWeaponManager::_CreateDinoWeapon( T_ItemID Itemid, T_ItemID subItem, CGameCharaBase * pOwner )
{
	// Instance로 무기 생성
	if( m_pStage == nullptr)
		return nullptr;

	i3InstanceManager * pManager	= m_pStage->GetInstanceManager( WeaponBase::static_meta());
	i3ClassMeta * pMeta				= WeaponBase::static_meta();

	if( subItem == 0)
		subItem = Itemid;

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid, true);
	if (pWeaponInfo == nullptr)
	{
		return nullptr;
	}
	
	i3::rc_wstring	wstrClassMeta;
	pWeaponInfo->GetClassMetaName(wstrClassMeta);

	if( i3::generic_string_size( wstrClassMeta) > 0)
	{
		i3::string strClassMeta;
		i3::utf16_to_mb(wstrClassMeta, strClassMeta);

		pMeta = i3ClassMetaSystem::i()->find_class_meta(strClassMeta);
		I3ASSERT( pMeta != nullptr);

		if( pMeta == nullptr)
		{
			char message[MAX_STRING_COUNT] = "";
			i3::snprintf( message, MAX_STRING_COUNT, 
				"[%s] 해당 무기의 class meta를 찾을수 없거나 등록되지 않았습니다. 다시 확인해 주십시오.", 
				strClassMeta);
			I3PRINTLOG(I3LOG_NOTICE,message); 

			return nullptr;
		}
	}

	i3ClassMeta * pOldMeta		= pManager->getInstanceMeta();
	pManager->setInstanceMeta( pMeta);

	WeaponBase * pQuaryWeapon = (WeaponBase *) pManager->GetFreeInstance();
	I3ASSERT( pQuaryWeapon->GetRefCount() == 2);

	pManager->setInstanceMeta( pOldMeta);

	//	반드시 Instance를 사용하기 위해서는 AttachStage()되어 있어야 한다. 주의!
	I3ASSERT( pQuaryWeapon != nullptr);

	// 8ms
	pQuaryWeapon->Create( pOwner, subItem, false);

	return pQuaryWeapon;
}

WeaponBase * CGameWeaponManager::_FindFreeWeapon( T_ItemID Itemid )
{
	UINT32 cnt = m_CharaWeaponList.size();
	for( size_t i = 0; i < cnt; i++)
	{
		WeaponBase * pFindWeapon = m_CharaWeaponList[i];
		I3ASSERT( pFindWeapon != nullptr);

		if( pFindWeapon->getPairWeapon() != nullptr)
			continue;

		CWeaponInfo * pWeaponInfo = pFindWeapon->getWeaponInfo();

		if ( ! (pWeaponInfo->GetItemID() ==(T_ItemID)Itemid) )
			continue;

		if ( !pFindWeapon->isTakeChara() )
		{
		#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			if( pFindWeapon->GetRefCount() > 3)
			{
				I3PRINTLOG(I3LOG_FATAL,  "QueryWeapon RefCount invalid. itemid %d",  pWeaponInfo->GetItemID());
			}
		#endif
			return pFindWeapon;
		}

	}

	return nullptr;
}

void CGameWeaponManager::_ClearGarbageCharaWeaponList( void)
{	
	// 오래된 무기는 삭제
	for( size_t i = 0; i < m_CharaWeaponList.size(); i++)
	{
		WeaponBase * pFindWeapon = m_CharaWeaponList[i];
		I3ASSERT( pFindWeapon != nullptr);

		if( pFindWeapon->getPairWeapon() != nullptr)
			continue;

		bool bDeletable = false;

		if( pFindWeapon->isTakeChara() == false)
		{
			if( pFindWeapon->getCreater() != nullptr)
			{
				INT32 userRespawnCnt = pFindWeapon->getCreater()->getCharaInfo()->GetRespawnCnt();
				if( userRespawnCnt - pFindWeapon->getUserRespawnCnt() > 2)
				{	// 3회 리스폰 동안 사용되지 않은 무기
					bDeletable = true;
				}

				if ( pFindWeapon->getOwner() != nullptr && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				{
					// 내 캐릭터가 다중 무기로 사용 중인 무기는 중간에 지우면 안됨
					if ( pFindWeapon->getOwner()->getMultiWeaponContext()->IsUsingMultiWeapon(pFindWeapon))
						continue;
				}
			}
			else if( pFindWeapon->getOwner() == nullptr)
			{	// 유저가 나간 경우
				bDeletable = true;
			}	
		}

		if( bDeletable)
		{
			if( ReleaseCharaWeapon( pFindWeapon))
				i--;
		}
	}
}

void CGameWeaponManager::__RegisterWeaponResource( WEAPON_CLASS_TYPE classType, INT32 num, i3GameResChara * pRes)
{
	if( pRes != nullptr)
	{
		I3TRACE( "load res %d %d - %s\n", classType, num, pRes->GetName());
	}

	i3::vu::set_value_force(m_WeaponResList[ classType], num, pRes);	//	무기 번호대로 저장
}


void CGameWeaponManager::__RemoveWeaponBGLoadInfo(void)
{
	REAL32 tm = 0.0f;

	// 로딩중인 리소스가 있는 경우, 5초간 로딩 완료를 대기.
	if( isBGLoading() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "Queue is not empty\n");

		INT32 queue_size = m_BgLoadQueue.size() + m_BgPriorityLoadQueue.size();
		INT32 delete_count = 0;

		for( INT32 idx = 0 ; idx < (INT32)m_BgLoadQueue.size() ; ++idx)
		{
			sWEAPONLOADINFO * pInfo = m_BgLoadQueue[idx];

			if( pInfo->_state != eWEAPONLOAD_STATE_LOADING )
			{
				m_BgLoadQueue.erase(m_BgLoadQueue.begin() + idx);
				--idx;
				delete_count++;
			}
		}		

		for( INT32 idx = 0 ; idx < (INT32)m_BgPriorityLoadQueue.size() ; ++idx)
		{
			sWEAPONLOADINFO * pInfo = m_BgPriorityLoadQueue[idx];

			if( pInfo->_state != eWEAPONLOAD_STATE_LOADING )
			{
				m_BgPriorityLoadQueue.erase(m_BgPriorityLoadQueue.begin() + idx);
				--idx;
				delete_count++;
			}
		}	

		I3PRINTLOG(I3LOG_NOTICE, "Queue Total Count = %d, Delete Count = %d", queue_size, delete_count);

		i3Timer timer;
		while( isBGLoading() && tm < 3.0f )
		{
			tm += timer.GetDeltaSec();
			__BackgroundLoading();
		}

		I3PRINTLOG(I3LOG_NOTICE, "Finish wait. time = %f\n",tm );
	}
}

bool CGameWeaponManager::__GetWeaponFileName( char * pszOut, CWeaponInfo * pWeaponInfo)
{
	I3ASSERT( pWeaponInfo != nullptr);

	INT32 len = i3::generic_string_size( pWeaponInfo->GetName());

	if( len > 0)
	{
		I3ASSERT( len < MAX_PATH);
		GlobalFunc::DecryptValue( pszOut, pWeaponInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);

		i3::generic_string_cat( pszOut, ".I3CHR");
		return true;
	}

	return false;
}

const char * CGameWeaponManager::__GetWeaponResourcePath( CWeaponInfo * pWeaponInfo)
{
	char szResPath[ MAX_PATH];

	if( pWeaponInfo == nullptr)
		return nullptr;

	if( __GetWeaponFileName( szResPath, pWeaponInfo) == true)
	{
		return g_pFramework->FindResourceFullPathByOnlyName( szResPath);
	}

	return nullptr;
}

const char * CGameWeaponManager::__GetWeaponResourcePath( WEAPON_CLASS_TYPE classType, INT32 num)
{
	return __GetWeaponResourcePath( g_pWeaponInfoDataBase->getWeaponInfo( classType, num));
}

const char * CGameWeaponManager::__GetWeaponResourcePath( T_ItemID ItemID)
{
	return __GetWeaponResourcePath( g_pWeaponInfoDataBase->getWeaponInfo( ItemID));
}

bool CGameWeaponManager::__LoadWeapon( sWEAPONLOADINFO * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	switch( pInfo->_state)
	{
	case eWEAPONLOAD_STATE_PUSH :
		{
			BGLoadThread * pThread = g_pFramework->GetBGLoadThread();
			if( pThread != nullptr )
			{
				const char * pszPath = __GetWeaponResourcePath( pInfo->_ItemID);

				if( pszPath != nullptr)
				{
					pInfo->_pThread = pThread;
					pInfo->_state = eWEAPONLOAD_STATE_LOADING;
					pThread->SetLoadCmd( pszPath);
				}
				else
				{
					I3PRINTLOG(I3LOG_NOTICE, "Weapon Path is NULL, Weapon ItemID %d", pInfo->_ItemID );
					return true;
				}
			}
		}
		break;

	case eWEAPONLOAD_STATE_LOADING :
		{
			// 로드 완료를 확인
			i3GameResChara * pRes = static_cast<i3GameResChara*>(pInfo->_pThread->GetRes());

			if( pRes != nullptr)
			{
				WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( pInfo->_ItemID );
				WEAPON::RES_ID number = WEAPON::ItemID2ResID( pInfo->_ItemID );

				if( m_WeaponResList[ classType][ number] == nullptr)
				{	// 리소스 메니저에 등록해주자

					i3ResourceManager* pResMgr = g_pFramework->GetResourceManager();

					const char * pszPath = __GetWeaponResourcePath( pInfo->_ItemID);
					
					i3GameResChara* pResForCheck = static_cast<i3GameResChara*>(pResMgr->FindResourceByPath(pszPath));		// 메인쓰레드 로딩에서 선수쳐 로딩된 경우..
					
					if (pResForCheck == nullptr)
						g_pFramework->GetResourceManager()->AddResource( pRes);

					__RegisterWeaponResource( classType, number, pRes);
					I3TRACE( "PreLoad Weapon Complate %d %s\n", pInfo->_ItemID, pRes->GetName());
				}

				I3_MUST_RELEASE(pRes);
				m_pLoadInfoPool->Free( pInfo);
				return true;
			}
		}
		break;
	}

	return false;
}

void CGameWeaponManager::__BackgroundLoading( void)
{
	if( !m_BgPriorityLoadQueue.empty())
	{
		sWEAPONLOADINFO * pInfo = m_BgPriorityLoadQueue.front();
		if( __LoadWeapon( pInfo))
			m_BgPriorityLoadQueue.pop_front();
	}
	else
	{
		if( !m_BgLoadQueue.empty())
		{
			sWEAPONLOADINFO * pInfo = m_BgLoadQueue.front();
			if( __LoadWeapon( pInfo))
				m_BgLoadQueue.pop_front();
		}
	}
}

bool CGameWeaponManager::_IsLoadPushed( T_ItemID itemID, bool bPriority)
{
	i3::deque<sWEAPONLOADINFO*>* pque = &m_BgLoadQueue;

	if( bPriority)
	{
		pque = &m_BgPriorityLoadQueue;
	}

	i3::deque<sWEAPONLOADINFO*>& que = *pque;

	UINT32 cnt = que.size();

	for( UINT32 i = 0; i < cnt; i++)
	{
		sWEAPONLOADINFO * pInfo = que.front();

		que.pop_front();
		que.push_back( pInfo);

		if( pInfo->_ItemID == itemID)
			return true;
	}
	
	return false;
}

void CGameWeaponManager::PushLoadWeapon( T_ItemID itemID, bool bPriority)
{
	if( itemID > 0)
	{
		i3GameResChara * pRes = QueryResource( itemID, false);

		if( pRes == nullptr)
		{
			if( _IsLoadPushed( itemID, bPriority) == false)
			{
				I3ASSERT( m_pLoadInfoPool != nullptr);

				sWEAPONLOADINFO * pNewLoadCmd = reinterpret_cast<sWEAPONLOADINFO*>(m_pLoadInfoPool->Alloc( sizeof( sWEAPONLOADINFO), __FILE__, __LINE__));

				pNewLoadCmd->_state = eWEAPONLOAD_STATE_PUSH;
				pNewLoadCmd->_ItemID = itemID;

				if( bPriority)
					m_BgPriorityLoadQueue.push_back( pNewLoadCmd);
				else
					m_BgLoadQueue.push_back( pNewLoadCmd);

				I3TRACE( "PreLoad Weapon %d\n", itemID);
			}
		}
	}
}


void CGameWeaponManager::PreloadMyWeapons( void)
{
	// 무기만 골라서 로드
	CInvenList * pInven = CInvenList::i();

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	if( pChara != nullptr)
	{
		UINT32 size = pInven->GetItemCount( INVEN::DB_WEAPON);
		for(UINT32 i = 0; i < size; i++)
		{
			T_ItemID itemID = pInven->GetItemID( INVEN::DB_WEAPON, i);

			if( WeaponLRU_Manager::i()->IsPriorityWeapon( itemID))
			{
				PushLoadWeapon( itemID);
				pChara->PreloadAnimationByWeapon( itemID);
			}
		}
	}
}


void CGameWeaponManager::_CollectBackgroundClassIDList(i3::vector<size_t>(& inout)[WEAPON_CLASS_COUNT])
{
	const i3::deque<sWEAPONLOADINFO*>& vecLoadQueue1 = m_BgPriorityLoadQueue;
	const size_t num_Que1 = vecLoadQueue1.size();

	for (size_t i = 0 ; i < num_Que1 ; ++i)
	{
		sWEAPONLOADINFO* info = vecLoadQueue1[i];
		WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( info->_ItemID );
		WEAPON::RES_ID number = WEAPON::ItemID2ResID( info->_ItemID );		
		inout[classType].push_back(number);
	}
	
	const i3::deque<sWEAPONLOADINFO*>& vecLoadQueue2 = m_BgLoadQueue;
	const size_t num_Que2 = vecLoadQueue2.size();

	for (size_t i = 0 ; i < num_Que2 ; ++i)
	{
		sWEAPONLOADINFO* info = vecLoadQueue2[i];
		WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( info->_ItemID );
		WEAPON::RES_ID number = WEAPON::ItemID2ResID( info->_ItemID );		
		inout[classType].push_back(number);
	}
}

void CGameWeaponManager::TrashWeaponRes( REAL32 tm)
{
	if( isBGLoading() )
		return;
	
	m_rTrashTimer += tm;
	if( m_rTrashTimer > 30.f)
	{
		_ClearGarbageCharaWeaponList();	// 무기 삭제 여부 확인

		i3::vector<size_t> avecClassID[WEAPON_CLASS_COUNT];
		CGameWeaponManager::_CollectBackgroundClassIDList(avecClassID);

		i3ResourceManager * pMng = g_pFramework->GetResourceManager();
		for( UINT32 i = 0; i < WEAPON_CLASS_COUNT; i++)
		{
			const i3::vector<size_t>& vecClassID = avecClassID[i];

			for( UINT32 j = 0; j < m_WeaponResList[i].size(); j++)
			{
				i3GameResChara * pRes = m_WeaponResList[i].at(j);
				if( pRes != nullptr && pRes->isCommonRes() == false)
				{
					if( pRes->GetRefCount() == 2)
					{
						i3::vector<size_t>::const_iterator it = i3::find(vecClassID.begin(), vecClassID.end(), j);

						if (it == vecClassID.end() )
						{
							pMng->RemoveResource( pRes);
							m_WeaponResList[i][j] = nullptr;
						}
						else
						{
							I3TRACE("can't delete resource because of background loading : %s !!!! \n", pRes->GetName());
						}
					}
				}
			}
		}

		m_rTrashTimer = 0.f;
	}
}

const WeaponDecoySndInterval* CGameWeaponManager::GetDecoySndInterval(WEAPON_CLASS_TYPE weaponCT)
{
	return m_decoySndInterval;
}

const WeaponDecoySndInterval* CGameWeaponManager::GetEventDecoySndInverval(void)
{
	return m_EventDecoySndInterval;
}


void CGameWeaponManager::CreateAllSpecialDamageFilter()
{
	if( g_pFramework->IsBattleStage() )
	{
		WeaponBase * pWeapon;
		INT32 temp_index = -1;
		for( UINT32 i = 0; i < m_CharaWeaponList.size(); i++ )
		{
			pWeapon = m_CharaWeaponList[i];
			temp_index = pWeapon->getWeaponInfo()->GetWeaponSpecialHitHudIndex();
			if(temp_index > -1)
				CHud::i()->InsertSpecialDamageFilter(temp_index);
		}	
	}
}

void CGameWeaponManager::CreateAllSpecialDieFilter()
{
	if (g_pFramework->IsBattleStage())
	{
		WeaponBase * pWeapon;
		INT32 temp_index = -1;
		for (UINT32 i = 0; i < m_CharaWeaponList.size(); i++)
		{
			pWeapon = m_CharaWeaponList[i];
			temp_index = pWeapon->getWeaponInfo()->GetWeaponSpecialDieHudIndex();
			if (temp_index > -1)
			{
				CHud::i()->InsertSpecialDieFilter(temp_index);
			}
		}
	}
}

void CGameWeaponManager::CreateAllScopeHudTexture()
{
	if (g_pFramework->IsBattleStage())
	{
		WeaponBase * pWeapon;
		INT32 temp_index = -1;
		for (UINT32 i = 0; i < m_CharaWeaponList.size(); i++)
		{
			pWeapon = m_CharaWeaponList[i];
			temp_index = pWeapon->getWeaponInfo()->GetScopeType();
			if (temp_index > -1)
			{
				CHud::i()->InsertScopeHudTexture(temp_index);
			}
		}
	}
}

