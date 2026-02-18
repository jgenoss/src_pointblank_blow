#include "pch.h"
#include "WeaponDinoButt.h"
#include "GameCharaDino.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"

I3_CLASS_INSTANCE( WeaponDinoButt);

WeaponDinoButt::WeaponDinoButt()
{
}

WeaponDinoButt::~WeaponDinoButt()
{
}

void WeaponDinoButt::ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo)
{
	const REAL32 rRange =  pDamageInfo->_rExplosionRange;

	REAL32 rLength = 0.0f;

	for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr )	continue;

		INT32 idx = pChara->getCharaNetIndex();
		if( idx == -1)	continue;

		if( BattleServerContext::i()->IsUseDedicationMode())
		{// Dedication Mode에서는 자신이 계산합니다.
			if( m_pOwner->isLocal() == false)
				return;
		}
		else
		{
			if( !BattleServerContext::i()->IsP2PHost() &&	(idx != BattleSlotContext::i()->getMySlotIdx()) )
				continue;
		}

		if( pChara->getCharaTeam() & m_pOwner->getCharaTeam() )
			continue;

		VEC3D vLength, vDir;
		i3Vector::Sub( &vLength, pChara->GetPos(), m_StartPos );
		rLength = i3Vector::Normalize( &vDir, &vLength);

		if( rLength < rRange)
		{
			pDamageInfo->_nKillerIdx	= m_pOwner->getCharaNetIndex();
			pDamageInfo->_pVecDir		= &vDir;
			pDamageInfo->_pVecPos		= m_StartPos;
			pDamageInfo->_rDistance		= rLength;
			pDamageInfo->_nVictimIdx	= pChara->getCharaNetIndex();

			pChara->OnHit( pDamageInfo );
		}
	}

	if( g_pCamera == nullptr)
		return;

	MATRIX * pmtxCam = g_pCamera->getCamMatrix();

	{
		VEC3D vLength;
		i3Vector::Sub( &vLength, m_StartPos, i3Matrix::GetPos( pmtxCam));
		rLength = i3Vector::Length( &vLength);

		REAL32 rMaxLen = rRange * 3;
		if( rLength < rMaxLen)
		{
			REAL32 TrembAmpRate = 1.0f - (rLength/rMaxLen);
			g_pCamera->SetTremble( 0.8f * TrembAmpRate * TrembAmpRate, 3.0f * TrembAmpRate, 0.1f, 2.f * TrembAmpRate);
		}
	}
}