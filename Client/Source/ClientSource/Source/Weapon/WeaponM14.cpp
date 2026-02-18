#include "pch.h"
#include "WeaponM14.h"
#include "MainWeapon_M14.h"

#include "GameCharaBoneContext.h"
#include "WeaponFunction.h"

I3_CLASS_INSTANCE(WeaponM14);

MainWeapon * WeaponM14::CreateMainWeapon()
{
	MainWeapon_M14 * p = new MainWeapon_M14(this);
	MEMDUMP_NEW(p, sizeof(MainWeapon_M14));
	return p;
}

WeaponM14::WeaponM14()
{
}

WeaponM14::~WeaponM14()
{
}

/*virtual*/ void WeaponM14::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NormalWeapon(this);
	i3Scene::ModifyDynamicState(getSceneObject(), I3_PHYSIX_STATE_DISABLE, 0);
}

bool WeaponM14::_OnCheckCharaCollisionByExplosion(CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange)
{
	bool					bValidCol = false;
	i3CollideeLine			ColLine;
	MATRIX *				pMtx;
	bool					bCharaAlive = !pChara->isCharaStateMask(CHARA_STATE_DEATH);
	INT32					nCurHP = pChara->getCurHP();

	ColLine.SetStart(GetPos());

	if (bCharaAlive)
	{
		if (pChara->IsDino()) {
			pMtx = pChara->getBoneContext()->getIKMatrix();
		}
		else {
			pMtx = pChara->getBoneContext()->getBoneMatrix(BONE_LFOOT);
		}

		ColLine.SetEnd(i3Matrix::GetPos(pMtx));

		//World에 대해서 한번 테스트
		// 월드 컬리젼 거리보다 작으면.
		if ((_CheckHit(ColLine, rRange, CGRP_MASKGROUP_OBJECT) == false) &&
			(_CheckHit(ColLine, rRange, CGRP_MASKGROUP_ONLYWORLDHIT) == false))
		{
			_OnHitCharaByExplosion(pChara, CHARA_HIT_LFOOT, ColLine.GetDir(), ColLine.GetStart());
			bValidCol = true;
		}
	}

	if (bValidCol && pChara->isLocal() && (nCurHP > pChara->getCurHP()))
	{	// Hit only Host
		g_pSndMng->StartWeaponStateSnd(GTBWS_ON_HIT_THROW, 0, GetPos(), true);
	}

	return bValidCol;
}

/*virtual*/ void WeaponM14::SetHitCollideeGroup(void)
{
	WF::SetCollideeGroup_HitWeapon(this);
}