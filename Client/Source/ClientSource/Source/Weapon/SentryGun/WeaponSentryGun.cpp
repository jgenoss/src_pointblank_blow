#include "pch.h"

#include "WeaponSentryGun.h"
#include "SentrygunBehavior.h"
#include "../../BattleHUD.h"
#include "../UI/HUD/UIHudDomi_Tag.h"
#include "../../Character/InputCtrlAI_ForHuman.h"
#include "BattleSlotContext.h"
#include "WeaponFunction.h"


// 기획 문서 참조 
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3150

REAL32 FireArmAngle::LERP_TIME = 0.3f;
REAL32 FireArmAngle::interpolate_theta()
{
	interpolate_time_theta += /*g_pViewer->getDeltaTime()*/0.016f;
	if( interpolate_time_theta >= LERP_TIME)
	{
		interpolate_time_theta = static_cast<REAL32>(LERP_TIME);
		src_theta = theta = recv_theta;
	}

	REAL32 ratio = 1.0f - ( (interpolate_time_theta - LERP_TIME) / (float)(0.f - LERP_TIME));
	theta = (src_theta*(1.f - ratio) + recv_theta*ratio);

	return theta;
}

void FireArmAngle::ReceiveTheta( REAL32 recv)
{//가까운 회전 찾음.
	//theta (-PI ~ PI) 값으로 변환.

	REAL32 temp = theta;
	REAL32 dir = (temp >= 0.f) ? (-1.f) : (1.f);

	while( temp < -D3DX_PI || D3DX_PI < temp)
		temp += (D3DX_PI*2.f*dir);


	REAL32 dist = recv - temp;
	if( abs(dist) <= D3DX_PI)
	{
		recv_theta = theta + dist;	
	}
	else
	{
		dir = (dist >= 0.f) ? (-1.f) : (1.f);
		recv_theta = theta + ((D3DX_PI*2.f - abs(dist))*dir);
	}

	src_theta = theta;
	interpolate_time_theta = 0.f;

	//I3TRACE(">> recv (%.3f)   dest_theta(%.3f)\n", recv, recv_theta);
}

I3_CLASS_INSTANCE(WeaponSentryGun)

WeaponSentryGun::WeaponSentryGun() 
	: m_installer(0), m_team(0), m_hp(0), m_max_hp(0), m_tagHud(0)
							 
{
	::memset(&m_installPos, 0, sizeof(VEC3D));
}

WeaponSentryGun::~WeaponSentryGun()
{
	I3_SAFE_RELEASE(m_tagHud);
}

void WeaponSentryGun::Reset()
{
	WeaponBase::Reset();

	m_installer = 0;
	m_hp = 0;
	m_max_hp = 0;
}

void WeaponSentryGun::OnBindResource()
{
	i3Chara::OnBindResource();

	m_pAICtx = this->FindAI("Default");
}

void WeaponSentryGun::Create(CGameCharaBase* owner, T_ItemID Itemid, bool bCreateInWeapon )
{
	WeaponBase::Create(owner, Itemid, bCreateInWeapon);
}

void WeaponSentryGun::OnUpdate(REAL32 tm)
{
	WeaponBase::OnUpdate(tm);
}

void WeaponSentryGun::OnNone(i3AIContext* ctx, REAL32 tm)
{
	(*GetSentrygunBehavior(SG_NONE))(*this);
}

void WeaponSentryGun::OnUnderSurveillance(i3AIContext* ctx, REAL32 tm)
{
	(*GetSentrygunBehavior(SG_UNDER_SURVEILLANCE))(*this);
}

void WeaponSentryGun::OnAttack(i3AIContext* ctx, REAL32 tm)
{
	(*GetSentrygunBehavior(SG_ATTACK))(*this);

	if (ctx->getLocalTime()+tm > ctx->getCurrentAIState()->getActualDuration())
		WF::PlayWeaponAnim(this, SG_UNDER_SURVEILLANCE);
}

void WeaponSentryGun::OnSetup(i3AIContext* ctx, REAL32 tm)
{
	(*GetSentrygunBehavior(SG_SETUP))(*this);

	if (ctx->getLocalTime()+tm > ctx->getCurrentAIState()->getActualDuration())
		WF::PlayWeaponAnim(this, SG_UNDER_SURVEILLANCE);
}

void WeaponSentryGun::OnDestroy(i3AIContext* ctx, REAL32 tm)
{
	(*GetSentrygunBehavior(SG_DESTROY))(*this);

	if (ctx->getLocalTime()+tm > ctx->getCurrentAIState()->getActualDuration())
		WF::PlayWeaponAnim(this, SG_NONE);
}

void WeaponSentryGun::Uninstall()
{
	(*GetSentrygunBehavior(SG_DESTROY))(*this);
	WF::PlayWeaponAnim(this, SG_NONE);
	Reset();
}

void WeaponSentryGun::InstallToWorld(i3Node* newSceneNode, i3GameNode* newOwner, VEC3D* pos, bool toWorld, MATRIX* mat, REAL32 theta)
{
	I3ASSERT( getOwner() != nullptr);

	m_installer		= getOwner();
	m_installPos.x	= pos->x;
	m_installPos.y	= pos->y;
	m_installPos.z	= pos->z;

	WeaponBase::Transmission(newSceneNode, newOwner, pos, true);

	if( SetTagToWeapon( m_tagHud, WEP_TAG_SENTYGUN))
		getAttachNode()->AddChild( m_tagHud);

	WF::PlayWeaponAnim(this, SG_SETUP);
}

void WeaponSentryGun::ProcessPacket(VEC3D* target)
{ 
	// 발사와 fire arm 움직임은 별개로 처리. 
	// 움직임에 delay가 있으나 서버에서 괜찮다고 함.
	ProcessFirePacket(target);
	WF::PlayWeaponAnim(this, SG_ATTACK, 2.0f);
}

void WeaponSentryGun::ProcessFirePacket(VEC3D* target)
{
	// 총구 이펙트
	_FireEffect(getFXMatrix(), g_pEffectManager->FindEffect("Fir_Ar_SentryGun"));

	{ // 총알 이펙트
		//m_CollideeLine.SetStart(i3Matrix::GetPos(getFXMatrix()));
		m_CollideeLine.SetStart(i3Matrix::GetPos(GetMatrix()));
		m_CollideeLine.SetEnd(target);
		i3Vector::Copy(m_TargetPos, target);

		INT32 collGroup = CGRP_MASKGROUP_ATTACK;
		CGameCharaBase* myChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		if (myChara->getCharaTeam() & CHARACTER_TEAM_RED)
			collGroup = CGRP_MASKGROUP_CHARA_BLUE;
		else
			collGroup = CGRP_MASKGROUP_CHARA_RED;

		bool warningOwner = false;
		I3_PHYSIX_HIT_RESULT* hitResult = DoLineCollision2(&m_CollideeLine, collGroup, &warningOwner);
		AddBulletTrace(hitResult);

		i3SoundContext* sound = g_pViewer->GetSoundContext();
		
		if( sound != nullptr )
		{
			i3SoundPlayInfo* playInfo = m_pFireSoundPlayInfo[WEAPON::FIRE_SOUND_NORMAL];
			playInfo->set3DMode(I3SND_RT_3DMODE_WORLD_SPACE);
			playInfo->SetDefaultVolume(10);
			playInfo->setListenerVolume(10);
			playInfo->SetDistanceInfo(75.0f, 5.0f, 0);
			sound->Play(playInfo, getFXMatrix());	
		}
	}

	{ // 탄피 이펙트
		i3Body* body = getSceneObject()->GetBody();
		INT32 idx = body->FindBoneByName("EmptyCartridgeFXDummy");
		MATRIX* mat = body->getCacheMatrix(idx);

		VEC3D norm = VEC3D(1.0f, 0.0f, 0.0f);
		i3Vector::Scale(&norm, i3Matrix::GetUp(mat), -2.0f);

		g_pEffectManager->AddEffect(g_pEffectManager->FindEffect("Bul_Ar_M4A1"), i3Matrix::GetPos(mat), &norm, (MATRIX*)nullptr);
	}
}

void WeaponSentryGun::ReturnInstance()
{
	Reset();
	WeaponBase::ReturnInstance();
}

void WeaponSentryGun::SetHP(INT16 hp)
{
	//max hp 설정.
	if( m_max_hp < hp)
		m_max_hp = hp;

	//current hp 설정.
	m_hp = hp;
}

REAL32 WeaponSentryGun::GetHPRatio() const
{
	return (m_max_hp > 0) ? (m_hp / (REAL32)m_max_hp) : (0.f);
}

void WeaponSentryGun::_InitCollideeGroup( void)
{
//	InitColliderSet(CGRP_ITEM_WHOLE, CGRP_MASK_ITEM);
//	i3Scene::ModifyDynamicState(getSceneObject(), 0, I3_PHYSIX_STATE_NO_COLLISION);
	WF::SetCollideeGroup_SentryGun(this);
}


