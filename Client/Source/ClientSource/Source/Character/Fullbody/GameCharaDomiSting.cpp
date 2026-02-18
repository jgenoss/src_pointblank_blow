#include "pch.h"

#include "GameCharaDomiSting.h"
#include "FullbodyStateDomiSting.h"
#include "FullbodyBiStateMapFactory.h"
#include "../../Weapon/WeaponBase.h"
#include "../../GameCharaCameraContext.h"
#include "../../GameCharaMoveContext.h"
#include "DomiDinoGrenades.h"
#include "BattleObjContext.h"
#include "MainWeapon_GrenadeShell.h"
#include "../../Weapon/WeaponItemID.h"

// 기획 문서 참조
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaDomiSting);

CGameCharaDomiSting::CGameCharaDomiSting()
{
	m_wpSpit = WeaponBase::new_object();
}

CGameCharaDomiSting::~CGameCharaDomiSting()
{
	I3_SAFE_RELEASE(m_wpSpit);
}

void CGameCharaDomiSting::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaFullbodyDinosaur::Create(global, db);

	T_ItemID spitID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_DINO, WEAPON::getItemIndex( WEAPON::DINO_STING_SPIT));
	m_wpSpit->Create(this, spitID, false);

	i3Node* node = i3Scene::FindNodeByName(getSceneObject()->GetBody(), "FXDummy");
	if (node)
	{	
		if (i3::kind_of<i3BoneRef*>(node))
			m_wpReleasePoint = (i3Transform*)i3Scene::FindNodeByExactType(node, i3Transform::static_meta());
		else if (i3::kind_of<i3Transform*>(node))
			m_wpReleasePoint = static_cast<i3Transform*>(node);
	}
}

void CGameCharaDomiSting::ResetChara()
{
	CGameCharaFullbodyDinosaur::ResetChara();
}

void CGameCharaDomiSting::OnBindResource()
{
	CGameCharaFullbodyDinosaur::OnBindResource();

	INT32 count = fb_fsm::domi_sting::GetAnimationCount();
	m_smgr.ReserveMemory(count);
	for (INT32 i=0; i<count; i++)
	{
		INT32 id = m_ai->FindAIStateByName(s_fullbodyName[i]);
		if (id >= 0)
			m_smgr.AddState(fb_fsm::domi_sting::CreateAnimation(id, this));
		else
		{
			I3ASSERT_0;
		}
	}
	m_smgr.SetBiStateMap(fb_fsm::BiStateMapFactory::CreateMapDomiSting());
	m_smgr.SetName("DomiSting");
	m_smgr.SetID(getCharaNetIndex());
	m_smgr.Transition(m_smgr.GetState(fb_fsm::domi_sting::NONE));
}

void CGameCharaDomiSting::OnUpdate(REAL32 tm)
{
	CGameCharaFullbodyDinosaur::OnUpdate(tm);
}

void CGameCharaDomiSting::Spit(GAMEINFO_CHARA_NETDATA* info)
{
	VEC3D releasePoint, dir;
	getCameraContext()->getVirtualCamPosition(&releasePoint);

	i3Vector::Sub(&releasePoint, &releasePoint, GetPos());
	i3Vector::Add(&releasePoint, &releasePoint, getMoveContext()->getRecvPosition());
	i3Vector::Sub(&dir, info->_tFireData.getTargetPosition(), &releasePoint);

	m_wpSpit->FireCollision( &releasePoint, const_cast<VEC3D*>(info->_tFireData.getTargetPosition()) );
	m_wpSpit->_FireEffect(GetMatWeaponReleasePoint(), g_pEffectManager->FindEffect("DTown_Dino_Sting_Fir_A"), true);
}

void CGameCharaDomiSting::ThrowPoisonGrenade(GAMEINFO_CHARA_NETDATA* data)
{
#ifdef DOMI_STING_POISON_GRENADE
	GAMEINFO_DOMI_DINO_GRENADE* grenadeNetInfo = &data->_tDomiDinoGrenadeData;
	if (!grenadeNetInfo) return;

	WeaponBase* grenade = g_pWeaponManager->GetFreeThrowWeapon( WEAPON_CLASS_GRENADESHELL, WEAPON::getItemIndex( WEAPON::GSHELL_POISON));
	if (!grenade) return;

	grenade->SetOwner(this);
	grenade->Reset();
	INT16 id = static_cast<INT16>(getCharaNetIndex()+MAX_THROWWEAPON_COUNT);
	grenade->setNetworkIdx(id);

	VEC3D target, dir;

	i3Vector::Copy( &target, const_cast<VEC3D*>(grenadeNetInfo->getTargetPos()));
	VEC3D* start = i3Matrix::GetPos(GetMatWeaponReleasePoint());
	i3Vector::Sub(&dir, &target, start);
	i3Vector::Normalize(&dir, &dir);

	MainWeapon_GrenadeShell* mainWeapon = static_cast<MainWeapon_GrenadeShell*>(grenade->GetMainWeapon());
	mainWeapon->Throw(start, &dir, grenade->GetThrowSpeed(), false);

	GAMEINFO_THROW_WEAPON_DOMI_DINO* gameInfo = new GAMEINFO_THROW_WEAPON_DOMI_DINO;
	MEMDUMP_NEW( gameInfo, sizeof(GAMEINFO_THROW_WEAPON_DOMI_DINO));

	gameInfo->Bind(grenade, 0.5f, 0.06f, start, grenadeNetInfo->getCalculatorIndex(), grenade->getNetworkIdx());
	BattleObjContext::i()->setDomiDinoGrenades()->Add(gameInfo);
	grenade->SetVisible(true);

	GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo( grenade->getWeaponInfo()->GetItemID(), grenade->GetPos(), NET_GRENADE_STATE_FLY);

	GAMEEVENT::Write_DomiDinoThrowWeapon(grenade->getNetworkIdx(), &throwInfo);
#endif
}

WeaponBase* CGameCharaDomiSting::GetCurrentCharaWeapon() const
{
	return m_wpSpit;
}

MATRIX*	CGameCharaDomiSting::GetMatWeaponReleasePoint() const
{
	return m_wpReleasePoint->GetCacheMatrix();
}

