#include "pch.h"
#include "ObjectBattleUseItem.h"

#include "BattleObjContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaMultiWeaponContext.h"
#include "UserInfoContext.h"
#include "GameCharaMoveContext.h"

#include "UI/UIHudManager.h"

I3_CLASS_INSTANCE(CGameObjectBattleUseItem);

CGameObjectBattleUseItem::CGameObjectBattleUseItem() : m_iItemCount(5)
					, m_rStartTime(0.f), m_rRespawnTime(0.f), m_iEffectidx(-1), m_bOnOff(OBJECT_USE_ITEM_STATE_OFF)
{
	memset(m_ItemID, 0, sizeof(INT32) * ITEMBOX_ITEM_MAXCOUNT);
	memset(m_ItemProbability, 0, sizeof(INT32) * ITEMBOX_ITEM_MAXCOUNT);
	//m_ItemID.reserve(ITEMBOX_ITEM_MAXCOUNT);
	//m_ItemProbability.reserve(ITEMBOX_ITEM_MAXCOUNT);
}

CGameObjectBattleUseItem::~CGameObjectBattleUseItem()
{
	//m_ItemID.clear();
	//m_ItemProbability.clear();
}

typedef struct __tagPersist_ObjItemBox
{
	UINT32			nVer;
	INT32			nItemCount;
	INT32			nItemID[ITEMBOX_ITEM_MAXCOUNT];
	INT32			nItemProbability[ITEMBOX_ITEM_MAXCOUNT];
	REAL32			rStartTime;
	REAL32			rRespawnTime;

} PERSIST_OBJ_ITEM_BOX;

void CGameObjectBattleUseItem::CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Object::CopyTo(pDest, method);

	if (i3::same_of<CGameObjectBattleUseItem*>(pDest) == true)
	{
		CGameObjectBattleUseItem * pObject = (CGameObjectBattleUseItem*)pDest;

		pObject->SetItemCount(GetItemCount());
		pObject->SetStartTime(GetStartTime());
		pObject->SetRespawnTime(GetRespawnTime());

		for (INT32 i = 0; i < ITEMBOX_ITEM_MAXCOUNT; i++)
		{
			pObject->SetItemID(i, GetItemId(i));
			pObject->SetItemProbability(i, GetItemProbability(i));
		}
	}
}

UINT32 CGameObjectBattleUseItem::OnQueryGameData(i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void ** ppData)
{
	(*ppData) = i3MemAlloc(sizeof(PERSIST_OBJ_ITEM_BOX));
	i3mem::FillZero(*ppData, sizeof(PERSIST_OBJ_ITEM_BOX));

	PERSIST_OBJ_ITEM_BOX * pData = (PERSIST_OBJ_ITEM_BOX*)(*ppData);
	
	pData->nVer = 101;
	pData->nItemCount = m_iItemCount;
	for (INT32 i = 0; i < ITEMBOX_ITEM_MAXCOUNT; i++)
	{
		pData->nItemID[i] = m_ItemID[i];
		pData->nItemProbability[i] = m_ItemProbability[i];
	}
	pData->rStartTime = m_rStartTime;
	pData->rRespawnTime = m_rRespawnTime;

	return sizeof(PERSIST_OBJ_ITEM_BOX);
}

UINT32 CGameObjectBattleUseItem::OnLoadGameData(i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	UINT32 Rc = 0;
	i3Stream * pStream = pInfo->getStream();

	PERSIST_OBJ_ITEM_BOX info;

	Rc = pStream->Read(&info, sizeof(PERSIST_OBJ_ITEM_BOX));
	I3_CHKIO(Rc);

	m_iItemCount = info.nItemCount;
	for (INT32 i = 0; i < ITEMBOX_ITEM_MAXCOUNT; i++)
	{
		m_ItemID[i] = info.nItemID[i];
		m_ItemProbability[i] = info.nItemProbability[i];
	}
	m_rStartTime = info.rStartTime;
	m_rRespawnTime = info.rRespawnTime;

	return Rc;
}

UINT32 CGameObjectBattleUseItem::OnSave(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	result = i3Object::OnSave(pResFile);
	I3_CHKIO(result);

	PERSIST_OBJ_ITEM_BOX info;
	info.nItemCount = m_iItemCount;
	for (INT32 i = 0; i < ITEMBOX_ITEM_MAXCOUNT; i++)
	{
		info.nItemID[i] = m_ItemID[i];
		info.nItemProbability[i] = m_ItemProbability[i];
	}
	info.rStartTime = m_rStartTime;
	info.rRespawnTime = m_rRespawnTime;

	rc = pResFile->Write(&info, sizeof(info));
	I3_CHKIO(rc);
	result += rc;

	return result;
}

UINT32 CGameObjectBattleUseItem::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	PERSIST_OBJ_ITEM_BOX info;

	result = i3Object::OnLoad(pResFile);
	I3_CHKIO(result);

	rc = pResFile->Read(&info, sizeof(info));
	I3_CHKIO(rc);
	result += rc;

	m_iItemCount = info.nItemCount;
	for (INT32 i = 0; i < ITEMBOX_ITEM_MAXCOUNT; i++)
	{
		m_ItemID[i] = info.nItemID[i];
		m_ItemProbability[i] = info.nItemProbability[i];
	}
	m_rStartTime = info.rStartTime;
	m_rRespawnTime = info.rRespawnTime;

	return result;
}

#if defined(I3_DEBUG)
bool CGameObjectBattleUseItem::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3XMLElement * pChild = nullptr;

	pChild = new i3XMLElement("BattleUseItemBox");
	pXML->LinkEndChild(pChild);

	pChild->addAttr("ItemCount", m_iItemCount);

	i3::string temp_string;
	for (INT32 i = 0; i < ITEMBOX_ITEM_MAXCOUNT; i++)
	{
		i3::sprintf(temp_string, "BoxItemID%d", i);
		pChild->addAttr(temp_string.c_str(), m_ItemID[i]);

		i3::sprintf(temp_string, "BoxItemProbability%d", i);
		pChild->addAttr(temp_string.c_str(), m_ItemProbability[i]);
	}
	pChild->addAttr("StartTime", m_rStartTime);
	pChild->addAttr("RespawnTime", m_rRespawnTime);
	
	return true;
}

bool CGameObjectBattleUseItem::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3XMLElement * pChild = nullptr;

	pChild = (i3XMLElement*)pXML->FirstChild("BattleUseItemBox");
	if (pChild == NULL)
	{
		I3PRINTLOG(I3LOG_FATAL, "Load XML Error");
		return false;
	}

	pChild->getAttr("ItemCount", &m_iItemCount);
	
	i3::string temp_string;
	for (INT32 i = 0; i < ITEMBOX_ITEM_MAXCOUNT; i++)
	{
		i3::sprintf(temp_string, "BoxItemID%d", i);
		pChild->getAttr(temp_string.c_str(), &m_ItemID[i]);

		i3::sprintf(temp_string, "BoxItemProbability%d", i);
		pChild->getAttr(temp_string.c_str(), &m_ItemProbability[i]);
	}
	pChild->getAttr("StartTime", &m_rStartTime);
	pChild->getAttr("RespawnTime", &m_rRespawnTime);

	return true;
}
#endif


void CGameObjectBattleUseItem::InitObject()
{
	// 이펙트 불러오는 위치를 다르게 합니다.
	m_iEffectidx = g_pEffectManager->FindEffect("buff");

	// 무기 로드						
	for (INT32 i = 0; i < m_iItemCount; i++)
	{
		if (_GetUseItemType(i) == OBJECT_USE_ITEM_WEAPON)
			g_pWeaponManager->QueryWeapon(nullptr, m_ItemID[i]);
	}

	
	ResetObject();
}

void CGameObjectBattleUseItem::ResetObject()
{
	m_rStartTime = 0.f;
	m_rRespawnTime = 0.f;
	m_bOnOff = OBJECT_USE_ITEM_STATE_OFF;

	// Enable도 꺼줍니다.
	SetEnable(FALSE);
}

void CGameObjectBattleUseItem::EnableObject()
{

}

// Sync
void CGameObjectBattleUseItem::ProcessObject(REAL32 rDeltaSeconds)
{
	GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object(getNetIndex());

	// 업데이트 시에는 On Off만 체크합니다.
	if (m_bOnOff != pObjectInfo->_tNetData.getState())
	{
		m_bOnOff = pObjectInfo->_tNetData.getState();

		if (OBJECT_USE_ITEM_STATE_ON == m_bOnOff)
		{
			if (isEnable() == FALSE)
				SetEnable(TRUE);
		}
		else
		{
			if (isEnable() == TRUE)
				SetEnable(FALSE);
		}
	}
}

void CGameObjectBattleUseItem::SetControledObjectState(INT32 state)
{
}

// Item 처리
void CGameObjectBattleUseItem::UseItem(const UINT8 ItemNumber, CGameCharaBase * pChara, UINT8 nSyncNumber)
{
	//I3TRACE("Battle Use Item [UseItem] Start Item Number %d\n", ItemNumber);

	OBJECT_USE_ITEM_TYPE type = _GetUseItemType(ItemNumber);

	// 여기서 아이템 처리를 한다.
	switch (type)
	{
		case OBJECT_USE_ITEM_WEAPON:
		{
			// Cmd_DropWeapon 현재 무기를 버립니다.
			_ChangeWeapon(m_ItemID[ItemNumber], pChara, nSyncNumber);
		}break;
		case OBJECT_USE_ITEM_HPUP:
		{
			// 이펙트를 실행합니다.
				g_pEffectManager->AddEffect(m_iEffectidx, pChara->GetPos(), &I3_YAXIS);
		}break;
		
		// 중력 복구.. 우선 저중력이 없는 소스이기 때문에 패스
		case OBJECT_USE_ITEM_RECOVERY_GRAVITY:
		{
			_AnotherTeamGravityRecovery(pChara->getTeam());
		}break;
		// 무적 아이템.
		case OBJECT_USE_ITEM_UNBEATABLE:
		{
			// 무적이 아닐 경우 무적처리
			if( pChara->isInvincible() == false)
				pChara->Cmd_Invincible(true, 4.0f);
		}break;
		default :
		{
			I3PRINTLOG(I3LOG_NOTICE, "Not Object Use Item : %d", ItemNumber);
		}break;
	}

	// 여기서 시스템 메세지를 뿌리면 됩니다.
	_SystemMessage(pChara->getCharaNetIndex(), m_ItemID[ItemNumber]);
}

CGameObjectBattleUseItem::OBJECT_USE_ITEM_TYPE CGameObjectBattleUseItem::_GetUseItemType(INT32 UseItemidx)
{
	// 실제 사용하는 아이템의 type을 우선 체크
	ITEM_TYPE itemtype = (ITEM_TYPE)GET_ITEM_TYPE(m_ItemID[UseItemidx]);

	if (ITEM_TYPE_WRAP_COUNT == itemtype)
	{
		switch (m_ItemID[UseItemidx])
		{
			// 아이템 아이디를 박는 부분
			// HP
			case CASHITEM_ITEMID_HP_RECOVERY:
			{
				return OBJECT_USE_ITEM_HPUP;
			}break;
			// Gravity
			case CASHITEM_ITEMID_GRAVITY_RESOTRE:
			{
				return OBJECT_USE_ITEM_RECOVERY_GRAVITY;
			}break;
			// Unbeatable
			case CASHITEM_ITEMID_CHARA_UNBEATABLE:
			{
				return OBJECT_USE_ITEM_UNBEATABLE;
			}break;
		}
	}
	// 무기 인지 체크
	else if (ITEM_TYPE_UNKNOWN < itemtype && itemtype < ITEM_TYPE_CHARA)
	{
		return OBJECT_USE_ITEM_WEAPON;
	}

	I3PRINTLOG(I3LOG_NOTICE, "[_GetUseItemType] Not Item Type\n");
	return OBJECT_USE_ITEM_NONE;
}

void CGameObjectBattleUseItem::_AnotherTeamGravityRecovery(TEAM_TYPE myteam)
{
	// 저중력 모드에만 작동합니다.
	if (MyRoomInfoContext::i()->getStage()->IsSpace())
	{
		for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(i);
			if (pChara == nullptr)
				continue;

			// 다른 팀일 경우에만 체크합니다.
			if (myteam != pChara->getTeam())
			{
				// 살아있거나 점프상태일때만
				if (pChara->isCharaStateMask(CHARA_STATE_DEATH) == false &&
					pChara->isCharaStateMask(CHARA_STATE_JUMP) == true &&
					pChara->isCharaStateMask(CHARA_STATE_FLOOR) == false
					)
				{
					// Gravity 적용
					pChara->SetGravityRecovery(true);

					// 점프 초기화
					pChara->getMoveContext()->ResetJumping();
					
					// Jump 시작위치 변경
					VEC3D pos;
					pChara->getMoveContext()->GetPosition(&pos);
					pChara->getMoveContext()->setTakeOffPos(&pos);
				}
			}
		}
	}
}

void CGameObjectBattleUseItem::_SystemMessage(INT32 SlotIdx, const T_ItemID ItemID)
{
	// 메세지를 보내 켜도록 합니다.
	HUD::instance()->SetParameter(HUD::BATTLEUSEITEM, (INT32)SlotIdx, (INT32)ItemID);
}

void CGameObjectBattleUseItem::SetItemCount(INT32 count)
{
	m_iItemCount = count;
}

void CGameObjectBattleUseItem::SetItemID(INT32 idx, INT32 itemid)
{
	if (idx < 0 || ITEMBOX_ITEM_MAXCOUNT <= idx) return;

	m_ItemID[idx] = itemid;
}

void CGameObjectBattleUseItem::SetItemProbability(INT32 idx, INT32 prob)
{
	if (idx < 0 || ITEMBOX_ITEM_MAXCOUNT <= idx) return;

	m_ItemProbability[idx] = prob;
}

void CGameObjectBattleUseItem::SetStartTime(REAL32 starttime)
{
	m_rStartTime = starttime;
}

void CGameObjectBattleUseItem::SetRespawnTime(REAL32 resTime)
{
	m_rRespawnTime = resTime;
}

void CGameObjectBattleUseItem::_ChangeWeapon(T_ItemID itemID, CGameCharaBase * pChara, UINT8 nSyncNumber)
{
	WEAPON_SLOT_TYPE slot = (WEAPON_SLOT_TYPE)(GET_ITEM_TYPE(itemID) - 1);
	WeaponBase * pWeapon = pChara->GetCharaWeapon(slot);

	// 들고 있는 무기가 있을 경우에는 DirectPickUp을 사용
	if ( ( pWeapon != nullptr && !MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		|| ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && slot != WEAPON_SLOT_PRIMARY)
		|| ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && slot == WEAPON_SLOT_PRIMARY && pChara->getMultiWeaponContext()->GetMultiWeaponCount() >= UserInfoContext::i()->GetMaxMultiWeapon()))
	{
		// 이벤트 보내기.
		if (pChara->isLocal() == true)
		{
			GAMEEVENT::Write_Chara_DirectPickupWeapon(pChara, itemID);
		}
	}
	else
	{
		// 즉시 무기를 든다.
		WeaponBase * pPickUpWeapon = g_pWeaponManager->QueryWeapon(pChara, itemID, 0, TRUE);

		I3ASSERT(pPickUpWeapon != nullptr);
		pPickUpWeapon->Reset();
		pPickUpWeapon->SetEndurance(100);	// revision 61642 방어미션 미리 드랍되어 있는 무기를 들 시, 내구도를 100으로 세팅해줌(hansoft. 5832)

		pChara->Cmd_PickupWeapon( pPickUpWeapon, nSyncNumber, FALSE, FALSE);
	}
}
