#include "pch.h"
#include "Crosshair.h"
#include "GlobalVariables.h"
#include "GunInfo.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"
#include "UserInfoContext.h"
#include "BattleSlotContext.h"
#include "MyRoomInfoContext.h"

I3_CLASS_INSTANCE( Crosshair);//, i3GameObjBase);

#define		CR_R		0
#define		CR_G		255
#define		CR_B		0


Crosshair::Crosshair(void)
{
	
	m_pSceneNode = m_pSg;
	I3_MUST_ADDREF(m_pSceneNode);

	m_CR_R = CR_R;
	m_CR_Org_R = CR_R;
	m_CR_G = CR_G;
	m_CR_Org_G = CR_G;
	m_CR_B = CR_B;
	m_CR_Org_B = CR_B;
	m_CR_A = 255;
	m_CR_Org_A = 255;

	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_COLOR_CROSSHAIR) == true ) // 칼라 크로스 헤어체크
	{	
		USER_INFO_INVITEM_DATA MyInvenData;
		UserInfoContext::i()->GetMyUserInfoInvItemData(&MyInvenData);

		UINT8 color = MyInvenData._ui8CrossHairColor;

		const I3COLOR* cashColor = GetColorCashItem(color);

		m_CR_Org_R = cashColor->r;
		m_CR_Org_G = cashColor->g;
		m_CR_Org_B = cashColor->b;
	}
	m_pTargetChar = NULL;
}

Crosshair::~Crosshair(void)
{
	I3_SAFE_NODE_RELEASE( m_pSg);

	I3_SAFE_NODE_RELEASE( m_pHitSprite);
	I3_SAFE_RELEASE( m_pHitTexture);
	I3_SAFE_RELEASE(m_pTargetChar);
}

CGameCharaBase * Crosshair::_GetCurrentChara( void)
{
	if (m_pTargetChar)
	{
		return g_pCharaManager->getCharaByNetIdx(m_pTargetChar->getCharaNetIndex());
	}
	else
	{
		return g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	}
}

WeaponBase * Crosshair::_GetCurrentWeapon( void)
{
	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer == nullptr)
		return nullptr;

	return pPlayer->GetCurrentCharaWeapon();
}

CWeaponInfo * Crosshair::_GetCurrentWeaponInfo( void)
{
	WeaponBase * pWeapon = _GetCurrentWeapon();
	if( pWeapon == nullptr)
		return nullptr;

	return pWeapon->getWeaponInfo();
}

void Crosshair::Create(void)
{
	REAL32 scrCX, scrCY;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}

	i3Node * pLayerRoot = g_pFramework->GetLayerRoot( "HudRoot");
	I3ASSERT( pLayerRoot != nullptr);

	pLayerRoot->AddChild( m_pSg);

	//  Hit Effect
	{
#if defined (MULTYLANGUAGE)
		i3::stack_string strLoadFilePath;
		i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/HUD_01.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
		m_pHitTexture = g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
#else
#if defined (USE_LOCALE_UI)
		m_pHitTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Hud_01.i3i");
#else
		m_pHitTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Hud.i3i");
#endif
#endif
		m_pHitSprite = i3Sprite2D::new_object();	
		m_pHitSprite->SetTexture( m_pHitTexture );
		m_pHitSprite->Create( 1, true, true);
		m_pHitSprite->SetSrcBlend( I3G_BLEND_SRCALPHA);
		m_pHitSprite->SetDestBlend( I3G_BLEND_ONE);
		m_pHitSprite->SetEnable( 0, true);
		m_pHitSprite->SetRect( 0, scrCX - (33.0f/2.0f), scrCY - (31.0f/2.0f), 33.0f, 31.0f);
		m_pHitSprite->SetTextureCoord( 0, 286.0f, 122.0f, 319.0f, 154.0f);

		m_pSg->AddChild(m_pHitSprite);
	}

	SetEnableHitEffect( false);
}

void Crosshair::OnUpdate( REAL32 tm)
{
	INT32 nAlpha = 0;

	if( m_bHitEffect)
	{
		REAL32 tmpHitEffectSize = m_fHitEffectSize;
		m_fHitEffectSize = (tmpHitEffectSize + 1);

		if( m_fHitEffectSize > 43)
		{
			m_fHitEffectSize = 43;
		}

		nAlpha = i3Color::GetA( m_pHitSprite->GetColor( 0));

		nAlpha -= 10;
		if( nAlpha < 0) nAlpha = 0;

		m_pHitSprite->SetSize(	0, m_fHitEffectSize, m_fHitEffectSize);
		m_pHitSprite->SetColor(	0, 255, 255, 255, (UINT8)nAlpha);

		if( nAlpha == 0) 
		{
			SetEnableHitEffect( false);
			m_fHitEffectSize = 33.0f;
		}
	}

	i3GameObjBase::OnUpdate( tm);
}

void Crosshair::SetEnableHitEffect( bool bEnable)	
{	
	m_bHitEffect = bEnable;		

	if( bEnable)
	{

		m_pHitSprite->SetEnable( 0, true);
		m_pHitSprite->SetSize( 0, 33.0f, 33.0f);
		m_pHitSprite->SetColor(	0, 255, 255, 255, 255);
		m_fHitEffectSize = 33.0f;

/*
		m_pSprite->SetEnable(	4, true);
		m_pSprite->SetEnable(	5, true);
		m_pSprite->SetEnable(	6, true);
		m_pSprite->SetEnable(	7, true);

		m_fHitEffectSize = 0.0f;

		m_pSprite->SetSize(		4, 0.0f, 0.0f);
		m_pSprite->SetSize(		5, 0.0f, 0.0f);
		m_pSprite->SetSize(		6, 0.0f, 0.0f);
		m_pSprite->SetSize(		7, 0.0f, 0.0f);

		m_pSprite->SetColor(	4, 255, 0, 0, 200);
		m_pSprite->SetColor(	5, 255, 0, 0, 200);
		m_pSprite->SetColor(	6, 255, 0, 0, 200);
		m_pSprite->SetColor(	7, 255, 0, 0, 200);
*/
	}
	else
	{
		m_pHitSprite->SetEnable( 0, false);

/*		m_pSprite->SetEnable(	4, false);
		m_pSprite->SetEnable(	5, false);
		m_pSprite->SetEnable(	6, false);
		m_pSprite->SetEnable(	7, false);
*/
	}
}

void Crosshair::ResetHitEffect()
{
	SetEnableHitEffect( false);
	m_fHitEffectSize = 33.0f;
}

void Crosshair::SetEnable( bool bTrue)
{
	if ( MyRoomInfoContext::i()->getStage()->IsHudOff() ) bTrue = false;

	i3GameObjBase::SetEnable( bTrue);

	if( bTrue)
	{
		m_pSg->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
	else
		m_pSg->AddFlag( I3_NODEFLAG_DISABLE);

	if( !bTrue) ResetHitEffect();
}

void Crosshair::CalcRminLen(REAL32 &rmin, REAL32& len, REAL32 scrCY, CWeaponInfo* pWeaponInfo)
{
	CGameCharaBase * pPlayer = _GetCurrentChara();
	if( pPlayer != nullptr)
	{
		rmin = tanf( I3_DEG2RAD( pPlayer->getBoneContext()->getDeviationAngle())) * scrCY;
		len = pWeaponInfo->GetCrossHairExpand();
	}
	else
	{
		rmin = 0.f;
		len = 0.f;
		I3TRACE( "warning not set chara!!!!\n");
	}
}

void Crosshair::setTargetChara(CGameCharaBase* pChara)
{
	I3_SAFE_RELEASE(m_pTargetChar);

	if (pChara)
	{
		m_pTargetChar = pChara;
		I3_SAFE_ADDREF(m_pTargetChar);
	}
}