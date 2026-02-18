#include "pch.h"
#include "MainWeapon_M14.h"

#include "BattleObjContext.h"
#include "BattleSlotContext.h"
#include "BattleServerContext.h"

#include "GameCharaBoneContext.h"
#include "WeaponFunction.h"

MainWeapon_M14::MainWeapon_M14(WeaponBase * p) : MainWeapon_MedicalKit(p)
{

}

namespace
{
	void DisableM14(const INT32 idx, CGameCharaBase* owner, WeaponBase* M14)
	{
		GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(idx);
		if (owner->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
		{
			INT32 GrenadeIdx = idx%MAX_THROWWEAPON_SLOP;
			if (GrenadeIdx > -1 && GrenadeIdx <= MAX_THROWWEAPON_SLOP)
				pInfo->Reset();
				//g_pDu->GTypeManager(GrenadeIdx)->Reset();
		}
		pInfo->Reset();

		g_pWeaponManager->PushThrowWeapon(M14);
	}
}

void MainWeapon_M14::Explosion()
{
	/*if (bShot)
	{
		m_pWeaponBase->_OnCheckCollisionByExplosion();
	}*/

	g_pEffectManager->AddEffect(m_pWeaponBase->getFireEffectIndex(), m_pWeaponBase->GetPos(), &I3_YAXIS);
	g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
}

bool MainWeapon_M14::OnHitChara(CGameCharaBase * pChara)
{
	if (!IsPossibleHit(pChara))
		return false;

	UINT32 nColGroup = CGRP_MASKGROUP_ATTACK;

	if (m_pWeaponBase->getOwner()->getCharaTeam() & CHARACTER_TEAM_RED)
	{
		nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
	}
	else
	{
		nColGroup = CGRP_MASKGROUP_CHARA_RED;
	}

	VEC3D	vDir;

	// 첫번째 Check
	i3Vector::Sub(&vDir, m_pWeaponBase->GetPos(), pChara->GetPos());
	REAL32 rLength = i3Vector::LengthSq(&vDir);

	// 두번째 Check	
	vDir.y = 0.0f;
	REAL32 rLength2 = i3Vector::LengthSq(&vDir);
	bool bPickofWall = false;

	{
		// 캐릭터 최대(Head Y Value), 최소(Original Y Value) 높이를 구함
		REAL32 rCharMinHeight = i3Vector::GetY(pChara->GetPos());
		REAL32 rCharMaxHeight = i3Vector::GetY(i3Matrix::GetPos(pChara->getBoneContext()->getBoneMatrix(BONE_LFOOT)));

		// 메디컬킷의 높이
		REAL32 rObjHeight = i3Vector::GetY(m_pWeaponBase->GetPos());

		// 메디컬킷의 Y값이 캐릭터의 높이값 범위내 없으면 획득하지 못함
		if (rObjHeight <= rCharMaxHeight && rObjHeight >= rCharMinHeight)
			bPickofWall = true;
	}

	REAL32 rRange = m_pWeaponBase->GetExplosionRange();
	bool bValidCol = false;

	// 범위조건에 해당할 시 컬리전 테스트 후 폭발처리

	if ((rLength < rRange) ||
		((rLength2 < rRange) && (bPickofWall == true)))
	{
		bValidCol = static_cast<WeaponM14*>(m_pWeaponBase)->_OnCheckCharaCollisionByExplosion(pChara, nColGroup, rRange);
	}

	if (bValidCol)
	{
		if ((g_pCamera != nullptr) && pChara->is1PV() && !pChara->isCharaStateMask(CHARA_STATE_DEATH))
		{
			REAL32 MaxLength = rRange * 2.0f;
			if (rLength < MaxLength)
			{
				REAL32 TrembAmpRate = 1.0f - rLength / MaxLength;
				g_pCamera->SetTremble(15.0f * TrembAmpRate * TrembAmpRate, 10.0f * TrembAmpRate, 0.2f, 1.0f *	TrembAmpRate);
			}
		}
	}

	return bValidCol;
}

bool MainWeapon_M14::IsPossibleHit(CGameCharaBase * pChara)
{
	if (pChara->getCurHP() == 0)
		return false;
	else if (m_pWeaponBase->getOwner()->getCharaTeam() == pChara->getCharaTeam())
		return false;
	else
		return true;
}

void MainWeapon_M14::UpdateNetworkData(REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp;

	I3ASSERT(m_pWeaponBase->IsDedicatedHost() == false);

	if (m_pWeaponBase->getNetworkIdx() < 0 || m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(m_pWeaponBase->getNetworkIdx());
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pInfo->_tNetData;

	pAssistInfo->_fInterpolateTime += rDeltatime;
	flerp = pAssistInfo->_fInterpolateTime;

	if (isBombState(WEAPON_BOMBSTATE_SETTLED) == false)
	{
		if (flerp > MINE_POS_ASSISTTIME)
		{
			if (pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
			{
				VEC3D vDir;
				VEC3D vPos = pNetInfo->GetPos();
				i3Vector::Sub(&vDir, &vPos, &pAssistInfo->_vPostPos);
				i3Vector::Copy(&pAssistInfo->_vPostPos, m_pWeaponBase->GetPos());

				i3Vector::Add(&vTemp, &pAssistInfo->_vPostPos, &vDir);
				pNetInfo->SetPos(vTemp);


				flerp = rDeltatime;
			}
			else
				flerp = MINE_POS_ASSISTTIME;
		}

		flerp *= MINE_POS_ASSISTTIMEINV;

		// 클라이언트 상에서 임의로 회전시켜줍니다.
		m_rRotateY -= I3_DEG2RAD(rDeltatime * 360.f);

		VEC3D vPos = pNetInfo->GetPos();
		i3Vector::Lerp(m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vPos, flerp);
		i3Vector::Sub(&vTemp, &pAssistInfo->_vPostPos, &vPos);

		if (i3Vector::LengthSq(&vTemp) > 0.0001f)
			OnGrenadeAniRotate(rDeltatime);
	}

	if (pInfo->_bRecv)
	{
		if (pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
			m_pWeaponBase->SetVisible(true);

		if ((isBombState(WEAPON_BOMBSTATE_SETTLED) == false))
		{
			// 월드에 고정됩니다.
			if ((pNetInfo->GetState() & NET_GRENADE_STATE_SETTLED))
			{
				m_pWeaponBase->SetVisible(true);
				addBombState(WEAPON_BOMBSTATE_SETTLED);

				// 축을 맞춥니다.
				VEC3D vDir = pNetInfo->GetDir();
				i3Vector::Set(&vDir, getX(&vDir), getY(&vDir), getZ(&vDir));
				i3Matrix::MakeWithAt(m_pWeaponBase->GetMatrix(), &vDir, &I3_YAXIS);

				i3Matrix::Identity(m_pWeaponBase->GetMatrix());
				i3Matrix::PreRotateY(m_pWeaponBase->GetMatrix(), m_rRotateY);
				i3Matrix::PreRotateX(m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WEAPON_FLOOR_X));

				// 좌표를 맞춥니다.
				VEC3D vPos = pNetInfo->GetPos();
				m_pWeaponBase->SetPos(&vPos);
			}
		}
		else
		{
			if (isBombState(WEAPON_BOMBSTATE_ATTACHOBJECT) &&
				!(pNetInfo->GetState() & NET_GRENADE_STATE_TAKEOBJECT))
			{
				removeBombState(WEAPON_BOMBSTATE_SETTLED);
			}
		}

		if (pNetInfo->GetState() & NET_GRENADE_STATE_BOUND)
		{// 바운딩 사운드
			g_pSndMng->StartWeaponStateSnd(GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);
			// 바운드 후 플라잉 모드로 바꿔줘야 합니다.
			UINT8 tState = pNetInfo->GetState();
			tState &= ~NET_GRENADE_STATE_BOUND;
			pNetInfo->SetState(tState);
		}

		if (pNetInfo->GetState() & NET_GRENADE_STATE_EXPLOSION)
		{
			VEC3D vPos = pNetInfo->GetPos();
			m_pWeaponBase->SetPos(&vPos);
			Explosion();
		}

		pInfo->_bRecv = false;
	}
}

void MainWeapon_M14::CheckWorldCollision(REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult = nullptr;
	bool	bCharaCollision = false;

	INT32	nBoundType = 0;

	VEC3D	vNew;
	i3Vector::Scale(&vNew, &m_ThrowData.m_vDirection, m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add(m_pWeaponBase->GetPos(), &m_ThrowData.m_vStartPos, &vNew);
	REAL32	rGravity = -5.0f * m_pWeaponBase->getWeaponInfo()->GetWeight() * m_ThrowData.m_rElapsedThrowTime * m_ThrowData.m_rElapsedThrowTime;
	addY(m_pWeaponBase->GetPos(), rGravity);

	m_rRotateY -= I3_DEG2RAD(rDeltaSeconds * 360.f);
	i3Matrix::PreRotateY(m_pWeaponBase->GetMatrix(), -I3_DEG2RAD(rDeltaSeconds * 360.f));


	if (!m_bFirstBound)
	{
		i3Matrix::PreRotateX(m_pWeaponBase->GetMatrix(), -I3_DEG2RAD(rDeltaSeconds * 360.f));
	}

	ColLine.SetStart(&m_ThrowData.m_vOldPos);
	i3Vector::Sub(&vTemp, m_pWeaponBase->GetPos(), &m_ThrowData.m_vOldPos);
	rLength = i3Vector::Normalize(&vTemp, &vTemp);

	if (rLength <= 0.f)
	{
		_ProcessBoundObjectForce();
		return;
	}

	m_rThrowLen += rLength;

	ColLine.SetEnd(m_pWeaponBase->GetPos());
	ColLine.SetDir(&vTemp);

	REAL32 rMinLen = 10000.f;
	// World collision
	UINT32 nWorldCount = g_pFramework->raycast(&ColLine, CGRP_MASKGROUP_ATTACK);
	if (nWorldCount >0)
	{
		I3_PHYSIX_HIT_RESULT * pWorldResult = g_pFramework->getRaycastResult();
		g_pFramework->reserveRaycastResult(nWorldCount);

		for (UINT32 i = 0; i < nWorldCount; i++)
		{
			if (pWorldResult[i].m_pDestShapeSet->getShapeGroup() != CGRP_ITEM_HIT)
			{
				if (pWorldResult[i].m_T < rMinLen)
				{
					pResult = &pWorldResult[i];
					rMinLen = pResult->m_T;
				}
			}
		}
	}

	if (pResult == nullptr)
	{
		_ProcessBoundObjectForce();
		return;
	}

	// 첫번째 raycast는 Chara들의 Rough Collision을 검사합니다.
	UINT32 nRoughCount = g_pFramework->raycast(&ColLine, CGRP_MASKGROUP_GRENADE);
	if (nRoughCount > 0)
	{
		I3_PHYSIX_HIT_RESULT * pRoughResult = g_pFramework->getRaycastResult();

		g_pFramework->reserveRaycastResult(nRoughCount + nWorldCount);

		for (UINT32 i = 0; i < nRoughCount; i++)
		{
			i3EventReceiver * pRecv = pRoughResult[i].m_pDestShapeSet->getEventReceiver();

			if (!i3::kind_of<CGameCharaBase*>(pRecv))
				continue;

			UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
			CGameCharaBase * pTarget = (CGameCharaBase *)pRecv;

			if (pTarget == m_pWeaponBase->getOwner())
				continue;

			if (pTarget->getCharaTeam() & CHARACTER_TEAM_RED)
			{
				nColGroup = CGRP_MASKGROUP_CHARA_RED;
			}

			UINT32 nCount = g_pFramework->raycast(&ColLine, nColGroup);
			if (nCount <= 0)
				continue;

			I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult(nCount + nRoughCount + nWorldCount);

			for (UINT32 j = 0; j < nCount; j++)
			{
				pRecv = pHitResult[j].m_pDestShapeSet->getEventReceiver();
				if (!i3::kind_of<CGameCharaBase*>(pRecv))
					continue;

				pTarget = (CGameCharaBase *)pRecv;
				if (pTarget == m_pWeaponBase->getOwner())
					continue;

				if (!pTarget->isCharaStateMask(CHARA_STATE_DEATH) &&
					(rMinLen > pHitResult[j].m_T))
				{
					pResult = &pHitResult[j];
					rMinLen = pHitResult[j].m_T;
					bCharaCollision = true;

					//I3TRACE( "CharaCol %d %.4f\n", pTarget->getCharaNetIndex(), rMinLen);
				}
			}
		}
	}

	VEC3D	vecNew, vecNewPos, vecNormal;
	REAL32	rDot1 = 0.0f;

	//이동 방향 벡터 와 이동거리
	i3Vector::Sub(&vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos);
	REAL32 rTestLen = i3Vector::Normalize(&vecNew, &vecNew);

	// 움직이는 오브젝트라면 힘을 계산해서 반사량을 더해줘야합니다. by dfly79 080117
	m_rBoundObjectLen = _BoundObject(pResult, ColLine.GetDir());

	if ((rLength < rTestLen - 0.03f + m_rBoundObjectLen) || (rTestLen == 0.f))
	{
		//유효한 거리내의 충돌이 없심..
		_ProcessBoundObjectForce();
		return;
	}

	// 들어간 만큼 뒤로 무른다.
	// Step1. 수류탄의 위치를 충돌이 일어난 위치로 이동( 수류탄의 Radius를 포함)
	VEC3D vecRevert;
	i3Vector::Scale(&vecRevert, &vecNew, -0.03f);

	if (getX(&m_vObjectForce) != 0.f || getY(&m_vObjectForce) != 0.f || getZ(&m_vObjectForce) != 0.f)
	{
		VEC3D vAddForce;
		REAL32 rHeightGravity = -rGravity * getY(&m_vObjectForce);
		i3Vector::Scale(&vAddForce, &m_vObjectForce, 4.f);
		i3Vector::Add(&vecRevert, &vecRevert, &vAddForce);
		i3Vector::AddY(&vecRevert, rHeightGravity);
	}

	REAL32 rDot = 0.f;

	UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();
	if ((shapeGroup == CGRP_WORLD_ATTACK) || OBJECT_COLLISION_BY_WEAPON_HIT(shapeGroup))
	{
		rDot = i3Vector::Dot(&pResult->m_Normal, &I3_YAXIS);
	}

	if (1.0f + I3_EPS > rDot && 1.0f - I3_EPS < rDot)
	{
		i3Vector::Scale(&vecNew, &pResult->m_Normal, 0.03f);
		i3Vector::Add(&vecNew, &pResult->m_Intersect, &vecNew);

		VEC3D vDir;
		i3Vector::Set(&vDir, getX(&pResult->m_Normal), getY(&pResult->m_Normal), getZ(&pResult->m_Normal));
		i3Matrix::MakeWithAt(m_pWeaponBase->GetMatrix(), &vDir, &I3_YAXIS);

		i3Matrix::Identity(m_pWeaponBase->GetMatrix());
		i3Matrix::PreRotateY(m_pWeaponBase->GetMatrix(), m_rRotateY);
		i3Matrix::PreRotateX(m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WEAPON_FLOOR_X));


		m_pWeaponBase->SetPos(&vecNew);
		m_ThrowData.m_rSpeed = 0.f;

		i3Vector::Copy(&m_vAttachNormal, &pResult->m_Normal);
		g_pSndMng->StartWeaponStateSnd(GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);

		addBombState(WEAPON_BOMBSTATE_ATTACHWALL);

		m_pAttachObject = nullptr;
		_TakeObject(pResult);
	}
	else
	{
		m_bFirstBound = true;

		VEC3D vGrenadeSize;
		i3Vector::Scale(&vGrenadeSize, &pResult->m_Normal, 0.03f);

		i3Vector::Add(&vecRevert, &vecRevert, &vGrenadeSize);
		i3Vector::Add(m_pWeaponBase->GetPos(), &pResult->m_Intersect, &vecRevert);	//	들어온 반대 방향으로 무른다.
		i3Vector::Add(&m_ThrowData.m_vOldPos, &m_ThrowData.m_vOldPos, &vecRevert);

		i3Vector::Add(&vecNewPos, &pResult->m_Intersect, &vecNew);	//NewPosition
		rDot1 = i3Vector::Dot(&vecNew, &pResult->m_Normal);

		i3Vector::Scale(&vecNormal, &pResult->m_Normal, -2.0f * rDot1);
		i3Vector::Add(&vecNewPos, &vecNewPos, &vecNormal);

		i3Vector::Sub(&vecNew, &vecNewPos, &pResult->m_Intersect);
		i3Vector::Normalize(&vecNew, &vecNew);


		VEC3D vPos;
		i3Vector::Copy(&vPos, m_pWeaponBase->GetPos());
		i3Matrix::Identity(m_pWeaponBase->GetMatrix());
		i3Matrix::SetRotateX(m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WEAPON_FLOOR_X)); // KEYWORD : ROTATE_GRENADE
		i3Matrix::PreRotateY(m_pWeaponBase->GetMatrix(), m_rRotateY);

		m_pWeaponBase->SetPos(&vPos);

		//반사 속도 계산
		REAL32	Speed = m_ThrowData.m_rSpeed;

		rDot1 = i3Vector::Dot(&vecNew, &m_ThrowData.m_vDirection);
		if (i3Vector::Dot(&I3_YAXIS, &pResult->m_Normal) > 0.5f)
		{
			Speed = ((rDot1 + 1.0f) * Speed * 0.1f) + (Speed * 0.165f);
		}
		else
		{
			Speed = ((rDot1 + 1.0f) * Speed * 0.11f) + (Speed * 0.165f);
		}

		if (pResult->m_pTerrain != nullptr)
			nBoundType = pResult->m_pTerrain->m_Terrain;

		//재 투척
		NET_Bound(m_pWeaponBase->GetPos(), &vecNew, Speed, nBoundType);

		i3Vector::Set(&m_vAttachNormal, 0.f, 1.f, 0.f);
	}

	g_pFramework->resetRaycastResult();
}

void MainWeapon_M14::OnUpdate_Grenade(REAL32 rDeltaSeconds)
{
	if (m_pWeaponBase->getOwner() && m_pWeaponBase->getOwner()->isCharaStateMask(CHARA_STATE_DEATH) && m_pWeaponBase->getNetworkIdx() != -1 && m_pWeaponBase->isEnable())
	{
		DisableM14(m_pWeaponBase->getNetworkIdx(), m_pWeaponBase->getOwner(), m_pWeaponBase);

		return;
	}

	if (isThrowing())
	{
		if (_CheckProcessUserLeave() == false)
			return;

		WF::PlayWeaponAnim(m_pWeaponBase, m_pWeaponBase->getAIIndex(WEAPON::AI_LOADBULLET));

		bool	bSettled = false;
		bool	bExplosion = false;
		bool	bHost = m_pWeaponBase->IsDedicatedHost();


		if (bHost)
		{
			if (m_ThrowData.m_rSpeed < 0.2f && !isBombState(WEAPON_BOMBSTATE_SETTLED))
			{
				m_bCalcEllipsedTime = true;
				bSettled = true;
				addBombState(WEAPON_BOMBSTATE_SETTLED);
			}
		}

		REAL32 rExplosionRange = m_pWeaponBase->GetExplosionTime();
		if (isSettled() || m_bCalcEllipsedTime)
		{
			m_rElapsedExplosionTime += rDeltaSeconds;

			//owner가 죽은 경우 삭제. Indonesia Hansoft.264
			if (m_pWeaponBase->getOwner()->isCharaStateMask(CHARA_STATE_DEATH))
				m_rElapsedExplosionTime += rExplosionRange;
		}

		UpdateFlying(rDeltaSeconds, rExplosionRange);

		m_pWeaponBase->_TransformWorldSpace(rDeltaSeconds);

		i3Vector::Copy(&m_ThrowData.m_vOldPos, m_pWeaponBase->GetPos());

		if (m_pWeaponBase->getNetworkIdx() != -1)
		{
			if (m_rElapsedExplosionTime >= rExplosionRange)
			{
				DisableM14(m_pWeaponBase->getNetworkIdx(), m_pWeaponBase->getOwner(), m_pWeaponBase);
			}
			else
			{
				for (INT32 i = 0; i < g_pCharaManager->getCharaCount(); ++i)
				{
					CGameCharaBase * pChara = g_pCharaManager->getChara(i);

					if (pChara != nullptr)
					{
						// 투척 후 자신은 일정시간은 획득 불가능 
						//(사용자가 던지는 순간 획득할 수 있으므로 시간제어)
						if (pChara == m_pWeaponBase->getOwner() && m_rElapsedPickTime < 0.5f)
						{
							m_rElapsedPickTime += rDeltaSeconds;
							continue;
						}

						bExplosion = OnHitChara(pChara);

						if (bExplosion)
							break;
					}
				}
			}

			if (bHost)
			{
				// 네트워크로 보내줍니다.
				GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, NET_GRENADE_STATE_FLY);
				throwInfo._pNormal = GetThrowInfo_Normal();
				if (bExplosion)	throwInfo._nAddStateFlag = NET_GRENADE_STATE_EXPLOSION;
				else if (isSettled())		throwInfo._nAddStateFlag = NET_GRENADE_STATE_SETTLED;

				if (this->isBombState(WEAPON_BOMBSTATE_ATTACHOBJECT) && (m_pAttachObject != nullptr))
				{
					throwInfo._nAddStateFlag |= NET_GRENADE_STATE_TAKEOBJECT;
					throwInfo._nBoundType = m_pAttachObject->getNetIndex();
				}

				GAMEEVENT::Write_ThrowingWeapon(m_pWeaponBase->getNetworkIdx(), &throwInfo);
			}

			if (bExplosion && !isBombState(WEAPON_BOMBSTATE_EXPLOSION))
				Explosion();
		}
	}
}

void MainWeapon_M14::OnGrenadeAniRotate(REAL32 rDeltaTime)
{
	const REAL32 rRotate = 360.0f;
	i3Matrix::PreRotateX(m_pWeaponBase->GetMatrix(), -I3_DEG2RAD(rDeltaTime * rRotate));
}
