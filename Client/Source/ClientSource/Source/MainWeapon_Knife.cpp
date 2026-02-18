#include "pch.h"
#include "MainWeapon_Knife.h"
#include "WeaponFunction.h"

#include "i3Base/string/ext/utf16_to_mb.h"

MainWeapon_Knife::MainWeapon_Knife(WeaponBase* p) : MainWeapon_Melee(p), m_KnifeAttackType(WEAPON_KNIFE_ATTACK)
{
	m_aiiChangeLeft		= -1;
	m_aiiAttackA_Left	= -1;
	m_aiiAttackB_Left	= -1;

	m_aiiUI_RebelMale_Left = -1;
	m_aiiUI_RebelFemale_Left = -1;

	m_aiiUI_RebelMale_Left2 = -1;
	m_aiiUI_RebelFemale_Left2 = -1;

	m_aiiUI_Rebel_Common = -1;
	m_aiiUI_Swat_Common = -1;
	m_idxCandyEffect =-1;
}

void	MainWeapon_Knife::OnCreate( bool bCreateInWeapon)
{

	if( m_pWeaponBase->getWeaponInfo()->hasFireEffect())
	{	
		i3::rc_wstring	wstrFireEffect;
		m_pWeaponBase->getWeaponInfo()->GetFireEffect(wstrFireEffect);
		i3::string strFireEffect;	i3::utf16_to_mb(wstrFireEffect, strFireEffect);
		m_idxCandyEffect = g_pEffectManager->FindEffect(strFireEffect.c_str());
	}
}

void	MainWeapon_Knife::OnInitVariable()
{
	m_KnifeAttackType	= WEAPON_KNIFE_ATTACK;
}

void	MainWeapon_Knife::ReturnToReady()
{
	m_KnifeAttackType		= WEAPON_KNIFE_ATTACK;
}


void	MainWeapon_Knife::OnBindResource()
{
	i3AIContext* pAICtx = m_pWeaponBase->GetAIContext();

	I3ASSERT( pAICtx != nullptr);

	m_aiiChangeLeft		= (INT8) pAICtx->FindAIStateByName( "Change_Left");
	m_aiiAttackA_Left	= (INT8) pAICtx->FindAIStateByName( "AttackA_Left");
	m_aiiAttackB_Left	= (INT8) pAICtx->FindAIStateByName( "AttackB_Left");

	m_aiiUI_RebelMale_Left		= (INT8) pAICtx->FindAIStateByName( "UI_RebelMale_Left");
	m_aiiUI_RebelFemale_Left	= (INT8) pAICtx->FindAIStateByName( "UI_RebelFemale_Left");

	m_aiiUI_RebelMale_Left2		= (INT8) pAICtx->FindAIStateByName( "UI_RebelMale_Left2");
	m_aiiUI_RebelFemale_Left2	= (INT8) pAICtx->FindAIStateByName( "UI_RebelFemale_Left2");

	m_aiiUI_Rebel_Common		= (INT8) pAICtx->FindAIStateByName( "UI_Rebel_Common");
	m_aiiUI_Swat_Common			= (INT8) pAICtx->FindAIStateByName( "UI_Swat_Common");
}

void	MainWeapon_Knife::OnPlayChangeAni( REAL32 rTimeScale)
{
	if ( m_pWeaponBase->getPairWeapon() && m_aiiChangeLeft != -1 )
	{// paired weapon이면 왼손잡이용 애니메이션을 한다.
		if( g_pFramework->IsBattleStage() )
			WF::PlayWeaponAnim(m_pWeaponBase, m_aiiChangeLeft , rTimeScale );
	}
	else
	{
		m_pWeaponBase->OnPlayChangeAniImpl_Default(rTimeScale);
	}
}

void MainWeapon_Knife::PlayAnim_UI_RebelMale_Left( void)
{
	if( m_aiiUI_RebelMale_Left != -1)
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiUI_RebelMale_Left);
}

void MainWeapon_Knife::PlayAnim_UI_RebelFemale_Left( void)
{
	if( m_aiiUI_RebelFemale_Left != -1)
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiUI_RebelFemale_Left);
}

void MainWeapon_Knife::PlayAnim_UI_RebelMale_Left2( void)
{
	if( m_aiiUI_RebelMale_Left2 != -1)
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiUI_RebelMale_Left2);
}

void MainWeapon_Knife::PlayAnim_UI_RebelFemale_Left2( void)
{
	if( m_aiiUI_RebelFemale_Left2 != -1)
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiUI_RebelFemale_Left2);
}

void MainWeapon_Knife::PlayAnim_UI_Rebel_Common(void)
{
	if( m_aiiUI_Rebel_Common != -1)
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiUI_Rebel_Common);
}

void MainWeapon_Knife::PlayAnim_UI_Swat_Common(void)
{
	if( m_aiiUI_Swat_Common != -1)
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiUI_Swat_Common);
}

