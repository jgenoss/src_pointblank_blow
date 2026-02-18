#include "pch.h"
#include "CrosshairMgr.h"

#include "Crosshair.h"
#include "Crosshair_Cross.h"
#include "Crosshair_Scope.h"
#include "Crosshair_Scope2.h"
#include "Crosshair_Dino_Sting.h"
#include "Crosshair_Dino_Raptor.h"
#include "Crosshair_Dino_Elite.h"
#include "Crosshair_Dino_TRex.h"
#include "BattleSlotContext.h"


CrosshairMgr::CrosshairMgr() : m_pCrosshair(nullptr), m_parent(nullptr)
{
	::memset(m_apCrosshairs, 0, sizeof(Crosshair*) * CH_MODE_COUNT);
	::memset(m_UserScopeTypes, 0, sizeof(m_UserScopeTypes));
}

CrosshairMgr::~CrosshairMgr()
{
	
}

void		CrosshairMgr::ProcessDestroyStage()
{
	m_pCrosshair = nullptr;
		
	for (INT32 i = 0 ; i < CH_MODE_COUNT ; ++i)
		I3_SAFE_RELEASE(m_apCrosshairs[i]);				// 여기서 실제로 삭제 안될수 있음..StageBattle에 자식 링크 연결된 상태..

	m_parent = nullptr;
}

void	CrosshairMgr::ProcessCreateStage(i3Stage* parent)
{
	m_parent = parent;

	CrosshairMgr::ApplyEnvSetCrosshair();		// 디폴트 CH
	CrosshairMgr::_ProcessCreateDinoCH();		// 공룡이 없다면 통째로 스킵하는게 더 좋을것 같다..
	CrosshairMgr::_ProcessCreateScopeCH();
}


void	CrosshairMgr::ApplyEnvSetCrosshair()
{
	Crosshair*& ch_norm = m_apCrosshairs[CH_MODE_NORMAL];

	if (ch_norm)
	{
		m_parent->RemoveChild(ch_norm);
		I3_SAFE_RELEASE(ch_norm);
	}

	i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(g_pEnvSet->GetCrosshairName());	// i3ClassMeta::FindClassMetaByName( g_pEnvSet->GetCrosshairName() );

	if( pMeta == nullptr)
	{
		pMeta = Crosshair_Cross::static_meta();
	}

	ch_norm = (Crosshair *) pMeta->create_instance();	//CREATEINSTANCE( pMeta);
	I3_MUST_ADDREF(ch_norm);
	ch_norm->Create();

	m_pCrosshair = ch_norm;

	I3_GAMENODE_ADDCHILD( m_parent, ch_norm);
	
	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	if( pPlayer != nullptr)
		pPlayer->Cmd_ApplyCrosshair();
}

void	CrosshairMgr::_ProcessCreateDinoCH()
{
	CrosshairMgr::_ProcessCreateDinoCH_Detail( CH_MODE_RAPTOR);
	CrosshairMgr::_ProcessCreateDinoCH_Detail( CH_MODE_TREX);
	CrosshairMgr::_ProcessCreateDinoCH_Detail( CH_MODE_STING);	
	CrosshairMgr::_ProcessCreateDinoCH_Detail( CH_MODE_ELITE);
}

void	CrosshairMgr::_ProcessCreateDinoCH_Detail(CROSSHAIR_MODE mode)
{
	Crosshair*& rch = m_apCrosshairs[mode];
	if (rch)
	{
		m_parent->RemoveChild( rch);
		I3_SAFE_RELEASE( rch);
	}
	
	switch (mode)
	{
	case CH_MODE_RAPTOR:	rch = Crosshair_Dino_Raptor::new_object();	break;
	case CH_MODE_TREX:		rch = Crosshair_Dino_TRex::new_object();		break;
	case CH_MODE_STING:		rch = Crosshair_Dino_Sting::new_object();	break;
	case CH_MODE_ELITE:		rch = Crosshair_Dino_Elite::new_object();	break;
	default: return;
	}
	
	rch->Create();
	rch->SetEnable( false);
	I3_GAMENODE_ADDCHILD( m_parent, rch);
}

void	CrosshairMgr::_ProcessCreateScopeCH()
{
	Crosshair*& rch = m_apCrosshairs[CH_MODE_SCOPE];

	rch = Crosshair_Scope2::new_object();
	rch->Create();
	I3_GAMENODE_ADDCHILD( m_parent, rch);
	rch->SetEnable( false);
}

void CrosshairMgr::ApplyTargetCrosshair(CGameCharaBase* pChara)
{
	Crosshair* _Observer = m_apCrosshairs[CH_MODE_OBSERVER];

	if (_Observer)
	{
		m_parent->RemoveChild(_Observer);
		I3_SAFE_RELEASE(_Observer);
	}

	UINT8 nType = m_UserScopeTypes[pChara->getCharaNetIndex()];

	i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(g_pEnvSet->GetCrosshairName(nType));	// i3ClassMeta::FindClassMetaByName( g_pEnvSet->GetCrosshairName() );

	if (pMeta == NULL)
	{
		pMeta = Crosshair_Cross::static_meta();
	}

	_Observer = (Crosshair *)pMeta->create_instance();	//CREATEINSTANCE( pMeta);
	I3_MUST_ADDREF(_Observer);
	_Observer->Create();

	I3_GAMENODE_ADDCHILD(m_parent, _Observer);

	m_pCrosshair = m_apCrosshairs[CH_MODE_OBSERVER] = _Observer;

	enableCrossHair(true);
	setTargetChara(pChara);
}

void	CrosshairMgr::setCrosshairType(CROSSHAIR_MODE mode)
{
	Crosshair* tgt = m_apCrosshairs[mode];

	if ( tgt != m_pCrosshair)
	{
		bool bEnable = m_pCrosshair->isEnable() != false;
		enableCrossHair( false );
		m_pCrosshair = tgt;
		enableCrossHair( bEnable);
	}
}

void CrosshairMgr::BackupEnable(bool force_hide)
{
	m_BackupEnableCrosshair = isEnableCrossHair();
	enableCrossHair( force_hide ? false : m_BackupEnableCrosshair ) ;
}
void CrosshairMgr::RestoreEnable(bool force_show)
{
	enableCrossHair(force_show ? true : m_BackupEnableCrosshair);
}

void	CrosshairMgr::enableCrossHair(bool enable)
{
	if(m_pCrosshair)
		m_pCrosshair->SetEnable(enable);	
}

bool	CrosshairMgr::isEnableCrossHair() const 
{ 
	return (m_pCrosshair->isEnable()==true);// != false; 
}

void CrosshairMgr::setTargetChara(CGameCharaBase* pChara)
{
	m_pCrosshair->setTargetChara(pChara);
}