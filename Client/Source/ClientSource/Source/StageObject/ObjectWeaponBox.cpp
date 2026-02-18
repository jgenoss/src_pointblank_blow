#include "pch.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "ObjectWeaponBox.h"
#include "GameCharaWeaponContext.h"

#include "./StageBattle/ScoreContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"
#include "../Weapon/WeaponItemID.h"
#include "BattleObjContext.h"
I3_CLASS_INSTANCE( CGameObjectWeaponBox);//, CGameControledObject);

CGameObjectWeaponBox::CGameObjectWeaponBox()
{
	m_pWeaponPointDummy		= nullptr;
	m_pWeaponInBox			= nullptr;
	m_iShellCount			= 0;
	m_iMaxShellCount		= 0;
	m_rShellRespawnTimer	= 0.f;
	m_rShellRespawnTime		= 0.f;
	m_iGroupIdx				= 0;
	m_iUniqNumber			= 0;
	m_eWeaponType			= WT_RPG7_OUTPOST;
	m_bCleaverKnifeUsed		= false;
}

CGameObjectWeaponBox::~CGameObjectWeaponBox()
{
	if( m_pWeaponInBox != nullptr)
	{
		m_pWeaponInBox->ReturnInstance();
		m_pWeaponInBox = nullptr;
	}
}

struct PERSIST_OBJ_WEAPON_BOX
{
	UINT32			nVer;
	REAL32			rRespawnTime;
	INT32			nGroupIdx;
	INT32			nWeaponType;
	UINT32			pad[4];
} ;

UINT32 CGameObjectWeaponBox::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{	
	(*ppData) = i3MemAlloc( sizeof( PERSIST_OBJ_WEAPON_BOX));

	i3mem::FillZero( *ppData, sizeof(PERSIST_OBJ_WEAPON_BOX));

	PERSIST_OBJ_WEAPON_BOX * pData = (PERSIST_OBJ_WEAPON_BOX*) (*ppData);

	pData->nVer			= 101;
	pData->rRespawnTime = m_rShellRespawnTime;
	pData->nGroupIdx	= m_iGroupIdx;
	pData->nWeaponType  = m_eWeaponType;

	return sizeof( PERSIST_OBJ_WEAPON_BOX);
}

UINT32 CGameObjectWeaponBox::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	UINT32 Rc;
	i3Stream * pStream = pInfo->getStream();

	PERSIST_OBJ_WEAPON_BOX info;
	
	Rc = pStream->Read( &info, sizeof( PERSIST_OBJ_WEAPON_BOX));
	I3_CHKIO( Rc);

	m_rShellRespawnTime = info.rRespawnTime;
	m_iGroupIdx			= info.nGroupIdx;
	m_eWeaponType		= (EWEAPON_TYPE)info.nWeaponType;
	return Rc;
}

struct ALIGN4 I3_PERSIST_WEAPONBOX
{
	INT32		iGroupIdx;
	REAL32		rShellRespawnTime;
	INT32		eWeaponType;
	UINT32		pad[4];
} ;

UINT32 CGameObjectWeaponBox::OnSave(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	result = CGameControledObject::OnSave(pResFile);
	I3_CHKIO(result);

	I3_PERSIST_WEAPONBOX data;
	data.iGroupIdx = m_iGroupIdx;
	data.rShellRespawnTime = m_rShellRespawnTime;
	data.eWeaponType = m_eWeaponType;

	rc = pResFile->Write(&data, sizeof(data));
	I3_CHKIO(rc);
	result += rc;

	return result;
}

UINT32 CGameObjectWeaponBox::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	I3_PERSIST_WEAPONBOX data;

	result = CGameControledObject::OnLoad(pResFile);
	I3_CHKIO(result);

	rc = pResFile->Read(&data, sizeof(data));
	I3_CHKIO(rc);
	result += rc;

	m_iGroupIdx = data.iGroupIdx;
	m_rShellRespawnTime = data.rShellRespawnTime;
	m_eWeaponType = (EWEAPON_TYPE)data.eWeaponType;

	return result;
}

#if defined(I3_DEBUG)//XML
bool	CGameObjectWeaponBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{	
	i3XMLElement * pChild;

	pChild = new i3XMLElement( "RPG7Box");
	pXML->LinkEndChild( pChild);

	pChild->addAttr( "GroupIdx",	m_iGroupIdx);
	pChild->addAttr( "RespawnTime",	m_rShellRespawnTime);
	pChild->addAttr( "WeaponType",	m_eWeaponType);

	return true;
}

bool	CGameObjectWeaponBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{	
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( "RPG7Box");
	if( pChild == nullptr)
	{
		I3ERRLOG( i3Error::_INVALIDFORMAT);
		return false;
	}

	pChild->getAttr( "GroupIdx", &m_iGroupIdx);
	pChild->getAttr( "RespawnTime", &m_rShellRespawnTime);
	INT32 iWeaponType = 0;
	pChild->getAttr( "WeaponType", &iWeaponType);
	m_eWeaponType = (EWEAPON_TYPE)iWeaponType;

	return true;
}
#endif

WeaponBase * CGameObjectWeaponBox::_MakeWeapon( CGameCharaBase * pOwner)
{
	WEAPON_CLASS_TYPE classType = WEAPON_CLASS_ROCKET_LAUNCHER;
	INT32 number = 0;
	ITEM_TYPE itemptype = ITEM_TYPE_PRIMARY;
	switch( m_eWeaponType)
	{
	case WT_RPG7_OUTPOST :	number = WEAPON::getItemIndex(WEAPON::RPG7_OUTPOST);	break;
	case WT_RPG7_BOSS :		number = WEAPON::getItemIndex(WEAPON::RPG7_OUTPOST);	break;
	case WT_K201 :
		classType = WEAPON_CLASS_ASSAULT;
		number = WEAPON::getItemIndex(WEAPON::K201GL);
		break;
	case WT_CHINESE_CLEAVER :
		classType = WEAPON_CLASS_KNIFE;
		number = WEAPON::getItemIndex(WEAPON::CHINESECLEAVER_SEIZED);
		itemptype = ITEM_TYPE_MELEE;
		break;
	case WT_CROWNWATERMELON :
		classType = WEAPON_CLASS_MISSION;
		number = WEAPON::getItemIndex(WEAPON::CROWNWATERMELON);
		itemptype = ITEM_TYPE_MISSION;
		break;
	default :
		I3PRINTLOG(I3LOG_FATAL,  "invalid eWeaponType");
		return nullptr;
	}

	T_ItemID Itemid = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, itemptype, classType, number);

	if( pOwner == nullptr)
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		i3InstanceManager * pManager = pStage->GetInstanceManager( WeaponBase::static_meta() );	
		i3ClassMeta * pOldMeta = pManager->getInstanceMeta();

		if( classType == WEAPON_CLASS_ROCKET_LAUNCHER)
			pManager->setInstanceMeta( WeaponRocketLauncher::static_meta());
		else if( classType == WEAPON_CLASS_ASSAULT)
			pManager->setInstanceMeta( WeaponGrenadeLauncher::static_meta());
		else if( classType == WEAPON_CLASS_KNIFE)
			pManager->setInstanceMeta( WeaponKnife::static_meta());
		else if( classType == WEAPON_CLASS_MISSION)
			pManager->setInstanceMeta( WeaponWatermelon::static_meta());		
		else
		{
			I3ASSERT_0;
		}

		WeaponBase * pWeapon = (WeaponBase *) pManager->GetFreeInstance();
		I3ASSERT( pWeapon->GetRefCount() >= 1);
		pManager->setInstanceMeta( pOldMeta);

		pWeapon->Create( nullptr, Itemid, false);

		return pWeapon;
	}
	else
	{
		WeaponBase * pWeapon = g_pWeaponManager->QueryWeapon( pOwner, Itemid);
		I3_SAFE_ADDREF(pWeapon);
		return pWeapon;
	}
}

void CGameObjectWeaponBox::CreateWeaponInBox( void)
{
	if( m_pWeaponInBox == nullptr)
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		
		m_pWeaponInBox = _MakeWeapon( nullptr);
		I3ASSERT( m_pWeaponInBox != nullptr);
		// m_pWeaponInBox->AddRef();  이것이 릭 유발의 원인으로 강력히 추정된다..(GetFreeInstance는 내부카운트를 증가시키고, 쿼리웨폰은 증가시키지 않는 차이가 있다.) (2013.11.11.수빈)

		
		m_pWeaponInBox->Reset();
		m_pWeaponInBox->SetEnable( false);

		m_pWeaponInBox->ChangeGunDummy( WEAPON_ATTACH_GRIP);	//	기본 Root로 변경
		m_pWeaponInBox->AttachNode( m_pWeaponPointDummy, pStage);
		m_pWeaponInBox->ReturnToReady();
		i3Scene::ModifyDynamicState( m_pWeaponInBox->getSceneObject(), I3_PHYSIX_STATE_DISABLE, 0);
	}
}

void CGameObjectWeaponBox::InitObject( void)
{
	

	if( m_pWeaponPointDummy == nullptr)
	{
		m_pWeaponPointDummy = i3Scene::FindNodeByName( getSceneObject(), "WeaponPointDummy");
		I3ASSERT( m_pWeaponPointDummy != nullptr);
	}

	CreateWeaponInBox();
	
	i3Stage * pStage = g_pFramework->GetCurrentStage();
	StageObjectListPtr pObjList = pStage->GetStageObjectList();

	for( size_t i = 0; i < pObjList->size(); i++)
	{
		i3Object * pObj = (*pObjList)[i];
		if( i3::same_of<CGameObjectWeaponBox*>(pObj))
		{
			if( pObj == this)
				m_iUniqNumber = (INT32)m_SameGroupRPG7BoxList.size();

			if( ((CGameObjectWeaponBox*)pObj)->getRPG7GroupIdx() == m_iGroupIdx)
				m_SameGroupRPG7BoxList.push_back(static_cast<CGameObjectWeaponBox*>(pObj));
		}
	}

	ResetObject();
}

void CGameObjectWeaponBox::ResetObject()
{
	m_bCleaverKnifeUsed = false;
	if( m_pWeaponInBox != nullptr)
	{
		m_rShellRespawnTimer = 0.f;
		m_iShellCount		= 0;
		switch( MyRoomInfoContext::i()->getStage()->GetStageUID())
		{
		case STAGE_UID_OUTPOST :	
			if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB))
				m_iMaxShellCount	= 0;	
			break;
		case 1:
		default :					m_iMaxShellCount	= 1000;	break;
		}
		m_pWeaponInBox->SetEnable( false);
	}

	INT32 roundCount	= ScoreContext::i()->getTotalRoundCount();
	INT32 nR = g_pRand->GetRand( roundCount + m_iGroupIdx);
	I3ASSERT( m_SameGroupRPG7BoxList.size() > 0);
	nR = (nR % (INT32)m_SameGroupRPG7BoxList.size());
	if( nR == m_iUniqNumber)
		EnableObject();
}

void CGameObjectWeaponBox::EnableObjectInBox()
{
	if( m_pWeaponInBox != nullptr)
	{
		m_rShellRespawnTimer = 0.f;
		m_iShellCount		= 1;
		m_pWeaponInBox->SetEnable( true);
		//GlobalFunc::PB_TRACE("EnableObject m_iUniqNumber %d", m_iUniqNumber);
	}
}

void CGameObjectWeaponBox::DisableObjectInBox()
{
	if( m_pWeaponInBox != nullptr)
	{
		m_rShellRespawnTimer = 0.f;
		m_iShellCount		= 0;
		m_pWeaponInBox->SetEnable( false);
		// GlobalFunc::PB_TRACE("DisableObject m_iUniqNumber %d", m_iUniqNumber);
	}
}


void CGameObjectWeaponBox::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);
	bool bCheck = true;

	I3ASSERT( m_SameGroupRPG7BoxList.size() > 0);
	for(size_t i = 0; i < m_SameGroupRPG7BoxList.size(); i++)
	{
		CGameObjectWeaponBox * pGroupBox = m_SameGroupRPG7BoxList[i];
		if( pGroupBox->getShellCount() > 0 )
		{
			bCheck = false;
			break;
		}
	}

	if( bCheck)
	{
		if( m_iMaxShellCount > 0 )
		{
			m_rShellRespawnTimer += rDeltaSeconds;
			if( m_rShellRespawnTimer > m_rShellRespawnTime)
			{
				INT32 roundCount	= ScoreContext::i()->getTotalRoundCount();
				INT32 nR = g_pRand->GetRand( roundCount + m_iGroupIdx + m_iMaxShellCount);
				nR = (nR % (INT32)m_SameGroupRPG7BoxList.size());

				if( nR == m_iUniqNumber )
				{
					EnableObject();
					for( size_t i = 0; i < m_SameGroupRPG7BoxList.size(); i++)
					{
						CGameObjectWeaponBox * pAnotherObj = m_SameGroupRPG7BoxList[i];
						pAnotherObj->setRespawnTimer( 0.f);
						INT32 iMaxShell = pAnotherObj->getMaxShellCount();
						iMaxShell--;
						pAnotherObj->setMaxShellCount( iMaxShell);
					}
				}
			}
		}
	}
}

void CGameObjectWeaponBox::SetControledObjectState( INT32 state)
{	
}

bool CGameObjectWeaponBox::HoldToChara( CGameCharaBase * pChara)
{
	/*if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  == true)
		return false;*/

	if( m_eWeaponType >= WT_CHINESE_CLEAVER)
		return HoldToCharaUserpation(pChara);
	else
		return HoldToCharaNomal(pChara);
	
}

bool CGameObjectWeaponBox::HoldToCharaNomal( CGameCharaBase * pChara)
{
	I3ASSERT( pChara != nullptr);

	//if( m_iShellCount > 0)
	{
		CGameCharaWeaponContext * pWeaponCtx = pChara->getWeaponContext();
		WeaponBase * pWeapon = pWeaponCtx->getWeapon( WEAPON_SLOT_PRIMARY);
		if( pWeapon != nullptr)
		{
			if( !i3::same_of<WeaponRocketLauncher*>(pWeapon))
			{
				if ( !MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				{
					pChara->Cmd_DeleteWeapon(WEAPON_SLOT_PRIMARY);
				}
			}
			else
			{
				DisableObjectInBox();
				return false;
			}
		}


		{
			WeaponBase * pPickUpWeapon = _MakeWeapon( pChara);

			if( m_eWeaponType == WT_K201)
			{
				NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( pChara->getCharaNetIndex());
				pNetCharaInfo->setEndurance(WEAPON_SLOT_PRIMARY, 100);
			}

			I3ASSERT( pPickUpWeapon != nullptr);
			pPickUpWeapon->Reset();
			pPickUpWeapon->SetEndurance(100);	// revision 61642 방어미션 미리 드랍되어 있는 무기를 들 시, 내구도를 100으로 세팅해줌(hansoft. 5832)

			pChara->Cmd_PickupWeapon( pPickUpWeapon);

			// _MakeWeapon()은 이제 레퍼런스카운터증가가 되며, Cmd_PickupWeapon()함수 역시 대입에 의한 증가가 있기 때문에
			// 여기서 한번은 릴리즈 처리해줘야 한다..(13.11.13.수빈)

			I3_MUST_RELEASE(pPickUpWeapon);

			DisableObjectInBox();
		
		}

		return true;
	}
//	return false;
}

bool CGameObjectWeaponBox::HoldToCharaUserpation( CGameCharaBase * pChara)
{
	I3ASSERT( pChara != nullptr);

	if( m_iShellCount > 0)
	{
		CGameCharaWeaponContext * pWeaponCtx = pChara->getWeaponContext();
		WeaponBase * pWeapon = nullptr;

		if( m_eWeaponType == WT_CHINESE_CLEAVER)
		{
			m_bCleaverKnifeUsed  =true;
			pWeapon = pWeaponCtx->getWeapon( WEAPON_SLOT_MELEE);
		}
		else if( m_eWeaponType == WT_CROWNWATERMELON)
		{
			pWeapon = pWeaponCtx->getWeapon( WEAPON_SLOT_MISSION);
		}
		else
			m_bCleaverKnifeUsed = false;

		if( pWeapon != nullptr)
		{
			pChara->Cmd_DeleteWeapon( WEAPON_SLOT_MELEE);
		}

		WeaponBase * pPickUpWeapon = _MakeWeapon( pChara);

		I3ASSERT( pPickUpWeapon != nullptr);
		pPickUpWeapon->Reset();
		pPickUpWeapon->SetEndurance(100);	// revision 61642 방어미션 미리 드랍되어 있는 무기를 들 시, 내구도를 100으로 세팅해줌(hansoft. 5832)

		pChara->Cmd_PickupWeapon( pPickUpWeapon);

		I3_MUST_RELEASE(pPickUpWeapon);

		DisableObjectInBox();
		return true;

	}
	return false;
}