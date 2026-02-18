#include "pch.h"
#include "GameCharaBase.h"
#include "GameCharaUpdateContext.h"
#include "GameCharaAnimContext.h"

#include "Character/GameCharaUpdater1PV.h"
#include "Character/GameCharaUpdater3PV.h"
#include "./Character/GameCharaUpdater3PVProfile.h"

#include "Character/GameCharaUpdaterFullbody.h"

I3_CLASS_INSTANCE( CGameCharaUpdateContext);

CGameCharaUpdateContext::~CGameCharaUpdateContext()
{
	OnDestroy();
}

void CGameCharaUpdateContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	// Updater
	m_pCurrentUpdater = nullptr;
	m_p1PVUpdater = CGameCharaUpdater1PV::new_object();
	m_p1PVUpdater->BindOwner( pOwner);
	m_p3PVUpdater = CGameCharaUpdater3PV::new_object();
	m_p3PVUpdater->BindOwner( pOwner);
	m_p3PVProfileUpdater = CGameCharaUpdater3PVProfile::new_object();
	m_p3PVProfileUpdater->BindOwner( pOwner);
	m_pFullbodyUpdater = CGameCharaUpdaterFullbody::new_object();
	m_pFullbodyUpdater->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaUpdateContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();
	
	m_pCurrentUpdater = nullptr;
	I3_SAFE_RELEASE( m_p1PVUpdater);
	I3_SAFE_RELEASE( m_p3PVUpdater );
	I3_SAFE_RELEASE( m_p3PVProfileUpdater);
	I3_SAFE_RELEASE( m_pFullbodyUpdater);
}

void CGameCharaUpdateContext::OnReset( void)
{
	m_rHideTimer = m_rToHideTime = 0.f;
	m_rDeathTime	= 0.f;

	if( m_p1PVUpdater != nullptr)		m_p1PVUpdater->Reset();
	if( m_p3PVUpdater != nullptr)		m_p3PVUpdater->Reset();
	if( m_p3PVProfileUpdater != nullptr)	m_p3PVProfileUpdater->Reset();
	if (m_pFullbodyUpdater != nullptr) m_pFullbodyUpdater->Reset();
}

void CGameCharaUpdateContext::OnChange1PV()
{	
	m_pCurrentUpdater = m_p1PVUpdater;
}

void CGameCharaUpdateContext::OnChange3PV()
{
	m_pCurrentUpdater = m_p3PVUpdater;
}

void CGameCharaUpdateContext::OnChangeProfile()
{
	m_pCurrentUpdater = m_p3PVProfileUpdater;
}

void CGameCharaUpdateContext::OnChangeLocal()
{
	if( m_pOwner->is1PV() )
		m_pCurrentUpdater = m_p1PVUpdater;
	else
		m_pCurrentUpdater = m_p3PVUpdater;
}

void CGameCharaUpdateContext::OnChangeNetwork()
{
	if( m_pOwner->is1PV() )
		m_pCurrentUpdater = m_p1PVUpdater;
	else
		m_pCurrentUpdater = m_p3PVUpdater;
}

void CGameCharaUpdateContext::OnChangeNPC()
{	
}

void CGameCharaUpdateContext::OnChangeFullbody()
{
	m_pCurrentUpdater = m_pFullbodyUpdater;
}

void CGameCharaUpdateContext::OnFirstUpdate( REAL32 rDeltatime)
{
	if( m_pCurrentUpdater == nullptr)
		return;

	m_pCurrentUpdater->DoFirstUpdate( rDeltatime);

	if( getAnimContext() && getAnimContext()->isAIUpper() )
	{
		REAL32 upperAniTime = getAnimContext()->getUpperAniTimer();
		upperAniTime += rDeltatime;

		if( getAnimContext()->getAIUpper()->getCurrentAIState() != nullptr &&
			upperAniTime > getAnimContext()->getUpperDuration())
			getAnimContext()->setUpperAniTimer( 0.f);
		else
			getAnimContext()->setUpperAniTimer( upperAniTime);
	}
}

void CGameCharaUpdateContext::OnUpdate( REAL32 rDeltatime)
{
	if( m_pCurrentUpdater == nullptr)	return;

	m_pCurrentUpdater->DoUpdate( rDeltatime);
}

void CGameCharaUpdateContext::OnLastUpdate( REAL32 rDeltatime)
{
	if( m_pCurrentUpdater == nullptr)	return;

	m_pCurrentUpdater->DoLastUpdate( rDeltatime);
}


