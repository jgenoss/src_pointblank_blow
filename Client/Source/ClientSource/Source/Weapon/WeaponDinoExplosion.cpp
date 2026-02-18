#include "pch.h"
#include "WeaponDinoExplosion.h"
#include "GameCharaDino.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"

I3_CLASS_INSTANCE( WeaponDinoExplosion);

WeaponDinoExplosion::WeaponDinoExplosion()
{
}

WeaponDinoExplosion::~WeaponDinoExplosion()
{
}

void WeaponDinoExplosion::ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo)
{
	for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		REAL32 rLength = pDamageInfo->_rExplosionRange;

		i3CollideeLine			ColLine;
		MATRIX *				pMtx;
		I3_PHYSIX_HIT_RESULT *	pResult;
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr )	continue;

		INT32 idx = pChara->getCharaNetIndex();
		if( idx == -1)	continue;

		if( pChara->getCharaTeam() & m_pOwner->getCharaTeam() )
		{
			if(pChara->getCharaNetIndex() == m_pOwner->getCharaNetIndex())
			{
				pDamageInfo->_DeathType			= CHARA_DEATH_F;
				pDamageInfo->_rDamage			= m_pOwner->GetDinoFullHP() * 1.5f;
			}
			else
				continue;
		}

		// 방해물 관련 처리 GetPos는 발 밑의 값을 가져옵니다.
		// 그래서 바로 밑에 있는 물체를 뚫고 내려가기 때문에 약간 올려줍니다. Jinsik.kim(2015. 02. 12)
		VEC3D vec;
		i3Vector::Set(&vec, m_pOwner->GetPos()->x, m_pOwner->GetPos()->y, m_pOwner->GetPos()->z);
		i3Vector::AddY(&vec, 0.4f);

		ColLine.SetStart( &vec);
		pMtx	= pChara->getBoneContext()->getHeadMatrix();
		ColLine.SetEnd( i3Matrix::GetPos( pMtx));
		//World에 대해서 한번 테스트
		pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK );
		if( pResult != nullptr )
		{
			if( rLength > pResult->m_T )
				rLength = pResult->m_T;
		}

		if( rLength > i3Vector::Length( ColLine.GetDir()) )
		{
			VEC3D vLength, vDir;
			i3Vector::Sub( &vLength, pChara->GetPos(), m_pOwner->GetPos()  );
			rLength = i3Vector::Normalize( &vDir, &vLength);

			if( rLength < pDamageInfo->_rExplosionRange)
			{
				if( BattleServerContext::i()->IsP2PHost() )
					pDamageInfo->_nKillerIdx	= m_pOwner->getCharaNetIndex();
				else
					pDamageInfo->_nKillerIdx	= pChara->getCharaNetIndex();

				pDamageInfo->_pVecDir		= &vDir;
				pDamageInfo->_pVecPos		= m_pOwner->GetPos();
				pDamageInfo->_rDistance		= rLength;
				pDamageInfo->_nVictimIdx	= pChara->getCharaNetIndex();

				i3Vector::Copy( &pDamageInfo->_vStartPos, ColLine.GetStart());
				if(pResult == nullptr) 
				{
					VEC3D Vec;
					
					i3Vector::Copy( &pDamageInfo->_vHitPos, &Vec);
				}
				else
					i3Vector::Copy( &pDamageInfo->_vHitPos, &pResult->m_Intersect);

				pChara->OnHit( pDamageInfo, m_pOwner );
			}
		}
	}

	if( g_pCamera == nullptr)
		return;

	MATRIX * pmtxCam = g_pCamera->getCamMatrix();
	{
		REAL32 rLength = pDamageInfo->_rExplosionRange;
		VEC3D vLength;
		i3Vector::Sub( &vLength, m_pOwner->GetPos(), i3Matrix::GetPos( pmtxCam));
		rLength = i3Vector::Length( &vLength);

		REAL32 rMaxLen = pDamageInfo->_rExplosionRange * 3;
		if( rLength < rMaxLen)
		{
			REAL32 TrembAmpRate = 1.0f - (rLength/rMaxLen);
			g_pCamera->SetTremble( 5.0f * TrembAmpRate * TrembAmpRate, 7.0f * TrembAmpRate, 0.1f, 2.f * TrembAmpRate);
		}
	}
}