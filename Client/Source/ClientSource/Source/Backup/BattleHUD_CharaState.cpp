#include "pch.h"

#if 지워질_것들

#include "BattleHUD_CharaState.h"
#include "BattleHud.h"
#include "StageBattle.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaEquipContext.h"
#include "Camera/GameCamera_OnObject.h"
#include "Weapon/WeaponOnObject.h"
#include "Weapon/WeaponM197.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"
#include "GameCharaDino.h"
#include "Shop.h"
#include "ConfigRenderBattle.h"

I3_CLASS_INSTANCE( CHUD_CharaState);//, i3GameObjBase);

CHUD_CharaState::CHUD_CharaState(void)
{
	m_pHUD					= NULL;

	m_pSprite				= NULL;
	m_pTextHP				= NULL;
	m_pTextBullet			= NULL;
	m_pTextWeapon			= NULL;
	m_pTextDinoWeapon			= NULL;
	m_pTextDinoWeapon2			= NULL;

	m_pTextPointTotal		= NULL;
	m_pTextPoint			= NULL;

	m_pSpriteBulletGage1	= NULL;
	m_pSpriteBulletGage2	= NULL;
	m_pSpriteBullet			= NULL;

	m_bUpdateCharaHpState			= false;
	m_UpdateCharaHpStateElapsed		= 0.0f;

	m_nPlayerTotalPoint				= 0;
	m_fPlayerPointTimeDelta			= 0.0f;

	m_nCurWeaponFlag				= 0;
	m_nOldHp						= 0;

	m_bIsEnduranceZeroWeapon		= FALSE;
	m_fEnduranceZeroWeaponBlinkTime	 = 0.0f;

	m_bEnableDualGage				= FALSE;
	m_bActivateDualIdx				= 0;

	m_pM197					= NULL;
	m_pM197GaugeHUD			= NULL;

	m_nOldBulletCount				= 0;
	m_nOldBulletDualCount			= 0;
	m_OldBulletClass				= WEAPON_CLASS_UNKNOWN;
	m_nOldBulletNumber				= 0;

	m_pTex					= NULL;
	m_pTexHUD2				= NULL;
	m_pTexHUD3				= NULL;

	m_pSprHUD2				= NULL;
	m_pSprHUD3				= NULL;
	m_pTextHPPercent		= NULL;

	m_bUpdateHelmet		= FALSE;
	m_bUpdateC5			= FALSE;

	m_OldCharaLowerState = CHARA_BODYLOWER_STAND;
	m_OldWeaponUsageType	= WEAPON_SLOT_UNKNOWN;

	m_pTextLife				= NULL;

	i3mem::FillZero( m_psprtRecoverImg, sizeof( m_psprtRecoverImg ) );
	m_psprtRecoverEfft	= NULL;

	for( INT32 i = 0; i < HUD_RECOVER_MAX; i++ )
	{
		for( INT32 j = 0; j < RECOVER_EFFT_MAX; j++)
		{
			m_pfUpdateRecoverMove[i][j]	= &CHUD_CharaState::UpdateRecoverEfftMoveNull;
		}

		m_pfUpdateRecoverEfft[i]	= &CHUD_CharaState::UpdateRecoverEfftVisibleNull;
	}
}

CHUD_CharaState::~CHUD_CharaState(void)
{
	I3_SAFE_NODE_RELEASE( m_pSprite);

	I3_SAFE_NODE_RELEASE( m_pTextHP);
	I3_SAFE_NODE_RELEASE( m_pTextBullet);
	I3_SAFE_NODE_RELEASE( m_pTextWeapon);
	I3_SAFE_NODE_RELEASE( m_pTextDinoWeapon);
	I3_SAFE_NODE_RELEASE( m_pTextDinoWeapon2);
	I3_SAFE_NODE_RELEASE( m_pTextPointTotal);
	I3_SAFE_NODE_RELEASE( m_pTextPoint);

	I3_SAFE_NODE_RELEASE( m_pSpriteBulletGage1);
	I3_SAFE_NODE_RELEASE( m_pSpriteBulletGage2);
	I3_SAFE_NODE_RELEASE( m_pSpriteBullet);

	I3_SAFE_NODE_RELEASE( m_pSprHUD2);
	I3_SAFE_NODE_RELEASE( m_pSprHUD3);
	I3_SAFE_NODE_RELEASE( m_pTextHPPercent);

	I3_SAFE_RELEASE( m_pTex);
	I3_SAFE_RELEASE( m_pTexHUD2);
	I3_SAFE_RELEASE( m_pTexHUD3);

	I3_SAFE_RELEASE( m_pM197GaugeHUD);

	I3_SAFE_RELEASE( m_pTextLife );

	for( INT32 i = 0; i < RECOVER_EFFT_MAX; i++ ) I3_SAFE_NODE_RELEASE( m_psprtRecoverImg[i] );
	I3_SAFE_NODE_RELEASE( m_psprtRecoverEfft );
}

bool CHUD_CharaState::Create( CHud * pHUD)
{
	i3AttrSet * pHUDRoot;
	INT32 i = 0;

	I3ASSERT( pHUD != NULL);

	m_pHUD = pHUD;
	pHUDRoot = m_pHUD->getHUDRoot();

	m_nCurWeaponFlag = 0;
	m_nOldHp = 0;

	m_bIsEnduranceZeroWeapon = FALSE;
	m_fEnduranceZeroWeaponBlinkTime	 = 0.0f;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	// HUD 기본 Texture
	{
		m_pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD);

		m_pTexHUD2 = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD2);

		m_pTexHUD3 = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);
	}

	m_pSprite = i3Sprite2D::new_object();
	pHUDRoot->AddChild( m_pSprite);
	m_pSprite->SetTexture( m_pTex );
	m_pSprite->Create( HUD_CSSPR_COUNT, TRUE, TRUE);

	// Hud Sprite를 설정합니다.
	{
		INT32 nPosX = nScreenWidth - 175;
		INT32 nPosY = nScreenHeight - 104;

		// Info Frame
		m_pSprite->SetEnable( HUD_CSSPR_STATUS_SLOT, TRUE);
		m_pSprite->SetRect( HUD_CSSPR_STATUS_SLOT, (REAL32)(nPosX - 1), (REAL32)(nPosY), (162.0f), (96.0f));
		m_pSprite->SetColor( HUD_CSSPR_STATUS_SLOT, 255,255,255,255);
		m_pSprite->SetTextureCoord( HUD_CSSPR_STATUS_SLOT, 244.0f, 206.0f, 406.0f, 302.0f);
		
		m_pSprite->SetEnable(HUD_CSSPR_POINT, FALSE);
		m_pSprite->SetColor(HUD_CSSPR_POINT, 255,255,255,255);
		m_pSprite->SetRect(HUD_CSSPR_POINT, REAL32(nPosX + 62), REAL32(nPosY + 75), 99.0f, 21.0f);
		m_pSprite->SetTextureCoord(HUD_CSSPR_POINT, 307.0f, 280.0f, 406.0f, 302.0f);

		// Character State(Stand상태인지, Crouch상태인지.)
		m_pSprite->SetEnable( HUD_CSSPR_CHAR_STATE, TRUE);
		m_pSprite->SetRect( HUD_CSSPR_CHAR_STATE, (REAL32)(nPosX+8), (REAL32)(nPosY+10), (42.0f), (50.0f));
		m_pSprite->SetColor( HUD_CSSPR_CHAR_STATE, 255,255,255,255);
		m_pSprite->SetTextureCoord( HUD_CSSPR_CHAR_STATE, (110.0f), (1.0f), (152.0f), (51.0f));	// 서있는 자세

		// 무기
		{
			// 나이프
			m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
			m_pSprite->SetRect( HUD_CSSPR_KNIFE, (REAL32)(nPosX+74), (REAL32)(nPosY+25), (77.0f), (15.0f));
			m_pSprite->SetColor( HUD_CSSPR_KNIFE, 255,255,255,255);
			m_pSprite->SetTextureCoord( HUD_CSSPR_KNIFE, (187.0f), (185.0f), (269.0f), (200.0f));

			// 수류탄
			m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
			m_pSprite->SetRect( HUD_CSSPR_GRENADE, (REAL32)(nPosX+93), (REAL32)(nPosY+20), (22.0f), (27.0f));
			m_pSprite->SetColor( HUD_CSSPR_GRENADE, 255,255,255,255);
			m_pSprite->SetTextureCoord( HUD_CSSPR_GRENADE, (280.0f), (168.0f), (302.0f), (198.0f));

			
			// 연막,플래쉬 뱅
			m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, FALSE);
			m_pSprite->SetRect( HUD_CSSPR_GRENADE_ETC, (REAL32)(nPosX+93), (REAL32)(nPosY+18), (22.0f), (31.0f));
			m_pSprite->SetColor( HUD_CSSPR_GRENADE_ETC, 255,255,255,255);
			m_pSprite->SetTextureCoord( HUD_CSSPR_GRENADE_ETC, 305.0f, 166.0f, 327.0f, 200.0f);

			// 폭탄
			m_pSprite->SetEnable( HUD_CSSPR_BOMB, FALSE);
			m_pSprite->SetRect( HUD_CSSPR_BOMB, (REAL32)(nPosX+92), (REAL32)(nPosY+19), (47.0f), (28.0f));
			m_pSprite->SetColor( HUD_CSSPR_BOMB, 255,255,255,255);
			m_pSprite->SetTextureCoord( HUD_CSSPR_BOMB, 199.0, 75.0f, 246.0f, 107.0f);

			// C5
			m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
			m_pSprite->SetRect( HUD_CSSPR_C5_BULLET, (REAL32)(nPosX +80), (REAL32)(nPosY+20), 42.f, 28.f);
			m_pSprite->SetColor( HUD_CSSPR_C5_BULLET, 255, 255, 255, 255);
			m_pSprite->SetTextureCoord( HUD_CSSPR_C5_BULLET, 185.f, 280.f, 235.f, 312.f);

			// MiniAxe
			m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
			m_pSprite->SetRect( HUD_CSSPR_MINIAXE, (REAL32) (nPosX + 80), (REAL32) (nPosY + 20), 54.f, 28.f);
			m_pSprite->SetColor( HUD_CSSPR_MINIAXE, 255, 255, 255, 255);
			m_pSprite->SetTextureCoord( HUD_CSSPR_MINIAXE, 512.f, 302.f, 570.f, 330.f);

			// BallisticKnife
			m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
			m_pSprite->SetRect( HUD_CSSPR_BALLISTIC, (REAL32) (nPosX + 72), (REAL32) (nPosY + 18), 47.f, 28.f);
			m_pSprite->SetColor( HUD_CSSPR_BALLISTIC, 255, 255, 255, 255);
			m_pSprite->SetTextureCoord( HUD_CSSPR_BALLISTIC, 216.f, 158.f, 273.f, 181.f);
		}

		// 장비 (헬멧)
		{
			m_pSprite->SetEnable( HUD_CSSPR_HELMET, FALSE );
			m_pSprite->SetRect( HUD_CSSPR_HELMET, (REAL32)(nPosX + 115), (REAL32)(nPosY - 47), 46.f, 46.f);
			m_pSprite->SetColor( HUD_CSSPR_HELMET, 255, 255, 255, 255 );
			m_pSprite->SetTextureCoord( HUD_CSSPR_HELMET, 463.f, 323.f, 509.f, 369.f );
		}

		// 장비 (C5)
		{
			m_pSprite->SetEnable( HUD_CSSPR_C5,	FALSE);
			m_pSprite->SetRect( HUD_CSSPR_C5, (REAL32) (nPosX + 115), (REAL32) (nPosY - 96), 46.f, 46.f);
			m_pSprite->SetColor( HUD_CSSPR_C5, 255, 255, 255, 255);
			m_pSprite->SetTextureCoord( HUD_CSSPR_C5, 413.f, 359.f, 458.f, 403.f);
		}
	}

	///////////////////////////////////////////////
	// HUD 2 (공룡)
	{
		m_pSprHUD2 = i3Sprite2D::new_object();
		pHUDRoot->AddChild( m_pSprHUD2);
		m_pSprHUD2->SetTexture( m_pTexHUD2 );
		m_pSprHUD2->Create( HUD_CSSPR2_COUNT, TRUE, TRUE);

		// HUD 3 (공룡)
		m_pSprHUD3 = i3Sprite2D::new_object();
		pHUDRoot->AddChild( m_pSprHUD3);
		m_pSprHUD3->SetTexture( m_pTexHUD3 );
		m_pSprHUD3->Create( HUD_CSSPR3_COUNT, TRUE, TRUE);


		INT32 nPosX = nScreenWidth - 175;
		INT32 nPosY = nScreenHeight - 104;

		// Dino의 공격 상태
		m_pSprHUD2->SetRect( HUD_CSSPR2_CHAR_STATE2, (REAL32)(nPosX+8), (REAL32)(nPosY+10), (42.0f), (50.0f));
		m_pSprHUD2->SetColor( HUD_CSSPR2_CHAR_STATE2, 255,255,255,255);
		m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, 	(762.0f), (397.0f), (819.0f), (443.0f));	// 서있는 자세

		// Dino의 공격 상태
		m_pSprHUD3->SetRect( HUD_CSSPR2_CHAR_STATE2, (REAL32)(nPosX+8), (REAL32)(nPosY+10), (42.0f), (50.0f));
		m_pSprHUD3->SetColor( HUD_CSSPR2_CHAR_STATE2, 255,255,255,255);
		m_pSprHUD3->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, 	(762.0f), (397.0f), (819.0f), (443.0f));	// 서있는 자세
	
		{ //EscapeMission   Dino 무기타입에 대한 UI설정
			// Howl
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_HOWL, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_HOWL, (REAL32) (nPosX + 110), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_HOWL, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_HOWL, 797.f, 263.f, 843.f, 306.f);

			// Bite2
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_BITE2, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_BITE2, (REAL32) (nPosX + 70), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_BITE2, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_BITE2, 902, 263.f, 944.f, 306.f);

			// Bite
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_BITE, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_BITE, (REAL32) (nPosX + 110), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_BITE, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_BITE, 902, 263.f, 944.f, 306.f);

			// Scratch
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_SCRACH, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_SCRACH, (REAL32) (nPosX + 70), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_SCRACH, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_SCRACH, 852.f, 263.f, 894.f, 306.f);

			//Spit
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_SPIT, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_SPIT, (REAL32) (nPosX + 70), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_SPIT, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_SPIT, 881.f, 380.f, 943.f, 431.f);

			//Poison
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_POISON, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_POISON, (REAL32) (nPosX + 110), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_POISON, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_POISON, 960.f, 347.f, 1012.f, 381.f);

			//BUTT
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_BUTT, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_BUTT, (REAL32) (nPosX + 70), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_BUTT, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_BUTT, 876, 446.f, 951.f, 509.f);


			//DASH
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_DASH, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_DASH, (REAL32) (nPosX + 110), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_DASH, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_DASH, 966.f, 456.f, 1017.f, 508.f);

			//EXPLOSION
			m_pSprHUD2->SetEnable( HUD_CSSPR2_DINO_EXPLOSION, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_DINO_EXPLOSION, (REAL32) (nPosX + 114), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_DINO_EXPLOSION, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_DINO_EXPLOSION, 820, 456.f, 872.f, 506.f);

			//ELITE_SCRACH
			m_pSprHUD3->SetEnable( HUD_CSSPR3_ELITE_SCRACH, FALSE);
			m_pSprHUD3->SetRect( HUD_CSSPR3_ELITE_SCRACH, (REAL32) (nPosX + 70), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD3->SetColor( HUD_CSSPR3_ELITE_SCRACH, 255, 255, 255, 255);
			m_pSprHUD3->SetTextureCoord( HUD_CSSPR3_ELITE_SCRACH, 960.f, 69.f, 1001.f, 109.f);

			//ELITE_BUFF
			m_pSprHUD3->SetEnable( HUD_CSSPR3_ELITE_BUFF, FALSE);
			m_pSprHUD3->SetRect( HUD_CSSPR3_ELITE_BUFF, (REAL32) (nPosX + 114), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD3->SetColor( HUD_CSSPR3_ELITE_BUFF, 255, 255, 255, 255);
			m_pSprHUD3->SetTextureCoord( HUD_CSSPR3_ELITE_BUFF, 958.f, 14.f, 1010.f, 46.f);

		}

		{

			// Scratch
			m_pSprHUD2->SetEnable( HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_CC_DINO_SCRACH, (REAL32) (nPosX + 70), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_CC_DINO_SCRACH, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CC_DINO_SCRACH, 852.f, 263.f, 894.f, 306.f);

			// Bite
			m_pSprHUD2->SetEnable( HUD_CSSPR2_CC_DINO_BITE, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_CC_DINO_BITE, (REAL32) (nPosX + 70), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_CC_DINO_BITE, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CC_DINO_BITE, 902, 263.f, 944.f, 306.f);

			//Spit
			m_pSprHUD2->SetEnable( HUD_CSSPR2_CC_DINO_SPIT, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_CC_DINO_SPIT, (REAL32) (nPosX + 110), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_CC_DINO_SPIT, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CC_DINO_SPIT, 881.f, 380.f, 943.f, 431.f);

			//EXPLOSION
			m_pSprHUD2->SetEnable( HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
			m_pSprHUD2->SetRect( HUD_CSSPR2_CC_DINO_EXPLOSION, (REAL32) (nPosX + 114), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD2->SetColor( HUD_CSSPR2_CC_DINO_EXPLOSION, 255, 255, 255, 255);
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CC_DINO_EXPLOSION, 820, 456.f, 872.f, 506.f);

			//BUFF
			m_pSprHUD3->SetEnable( HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
			m_pSprHUD3->SetRect( HUD_CSSPR3_CC_RAPTOR_BUFF, (REAL32) (nPosX + 114), (REAL32) (nPosY + 20), 34.f, 28.f);
			m_pSprHUD3->SetColor( HUD_CSSPR3_CC_RAPTOR_BUFF, 255, 255, 255, 255);
			m_pSprHUD3->SetTextureCoord( HUD_CSSPR3_CC_RAPTOR_BUFF, 958.f, 14.f, 1010.f, 46.f);
		}
		
		m_pSprHUD2->SetEnable( FALSE);
		m_pSprHUD3->SetEnable( FALSE);


		{
			m_pTextHPPercent = i3TextNodeDX2::new_object();

			m_pTextHPPercent->Create( BATTLE_CHARA_SET, FONT_COUNT_32, 22, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
			m_pTextHPPercent->setSize( 70, 50);
			m_pTextHPPercent->SetEnable( TRUE);

			pHUDRoot->AddChild( m_pTextHPPercent);
		}
	}

	{
		m_pTextHP = i3TextNodeDX2::new_object();

		m_pTextHP->Create( BATTLE_CHARA_SET, FONT_COUNT_32, 28, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
		m_pTextHP->setSize( 70, 50);
		m_pTextHP->SetEnable( TRUE);

		pHUDRoot->AddChild( m_pTextHP);
	}

	{
		m_pTextBullet = i3TextNodeDX2::new_object();

		m_pTextBullet->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 19, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
		m_pTextBullet->setSize(100, 30);
		m_pTextBullet->SetEnable(TRUE);

		pHUDRoot->AddChild( m_pTextBullet);
	}

	// Weapon Text
	{
		m_pTextWeapon = i3TextNodeDX2::new_object();

		m_pTextWeapon->Create( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_pTextWeapon->setSize( 92, 13);
		m_pTextWeapon->setPos(0, 0);
		m_pTextWeapon->SetColor(180, 180, 180, 255);
		m_pTextWeapon->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

		pHUDRoot->AddChild( m_pTextWeapon);

	}
	// EscapeMission Dino는 두타입의 무기를 같은곳에 설정하므로 추가합니다.
	{
		INT32 WeaponFontSize = LocaleValue::GetInt32("WinPosition/EscapeMission_WeaponString_FontSize");

		m_pTextDinoWeapon = i3TextNodeDX2::new_object();

		m_pTextDinoWeapon->Create( GetDefaultFontName(), FONT_COUNT_32, WeaponFontSize);
		m_pTextDinoWeapon->setSize( 92, 13);
		m_pTextDinoWeapon->setPos(0, 0);
		m_pTextDinoWeapon->SetColor(180, 180, 180, 255);
		m_pTextDinoWeapon->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pTextDinoWeapon->SetEnable( FALSE );
		
		pHUDRoot->AddChild( m_pTextDinoWeapon);
		
		m_pTextDinoWeapon2 = i3TextNodeDX2::new_object();

		m_pTextDinoWeapon2->Create( GetDefaultFontName(), FONT_COUNT_32, WeaponFontSize);
		m_pTextDinoWeapon2->setSize( 92, 13);
		m_pTextDinoWeapon2->setPos(0, 0);
		m_pTextDinoWeapon2->SetColor(180, 180, 180, 255);
		m_pTextDinoWeapon2->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pTextDinoWeapon2->SetEnable( FALSE );

		pHUDRoot->AddChild( m_pTextDinoWeapon2);
	}

	///////////////////////////////
	// Bullet
	{
		// 탄환2
		{
			m_pSpriteBulletGage2 = i3Sprite2D::new_object();	
			m_pSpriteBulletGage2->SetTexture( m_pTex );
			m_pSpriteBulletGage2->Create(2, TRUE, TRUE);

			m_pSpriteBulletGage2->SetEnable( 0, FALSE);
			m_pSpriteBulletGage2->SetRect( 0, (REAL32)(nScreenWidth - 103), (REAL32)(nScreenHeight - 49), (84.0f), (18.0f));
			m_pSpriteBulletGage2->SetColor( 0, 255,255,255,255);
			m_pSpriteBulletGage2->SetTextureCoord( 0, (155.0f), (1.0f), (237.0f), (19.0f));

			m_pSpriteBulletGage2->SetEnable( 1, FALSE);
			m_pSpriteBulletGage2->SetRect( 1, (REAL32)(nScreenWidth - 103), (REAL32)(nScreenHeight - 49), (84.0f), (18.0f));
			m_pSpriteBulletGage2->SetColor( 1, 255,255,255,255);
			m_pSpriteBulletGage2->SetTextureCoord( 1, (155.0f), (22.0f), (239.0f), (40.0f));

			pHUDRoot->AddChild( m_pSpriteBulletGage2);
		}

		// 탄환1
		{
			m_pSpriteBulletGage1 = i3Sprite2D::new_object();	
			m_pSpriteBulletGage1->SetTexture( m_pTex );
			m_pSpriteBulletGage1->Create(2, TRUE, TRUE);

			m_pSpriteBulletGage1->SetEnable( 0, TRUE);
			m_pSpriteBulletGage1->SetRect( 0, (REAL32)(nScreenWidth - 103), (REAL32)(nScreenHeight - 52), (84.0f), (18.0f));
			m_pSpriteBulletGage1->SetColor( 0, 255,255,255,255);
			m_pSpriteBulletGage1->SetTextureCoord( 0, (155.0f), (1.0f), (237.0f), (19.0f));

			m_pSpriteBulletGage1->SetEnable( 1, TRUE);
			m_pSpriteBulletGage1->SetRect( 1, (REAL32)(nScreenWidth - 103), (REAL32)(nScreenHeight - 52), (84.0f), (18.0f));
			m_pSpriteBulletGage1->SetColor( 1, 255,255,255,255);
			m_pSpriteBulletGage1->SetTextureCoord( 1, (155.0f), (22.0f), (239.0f), (40.0f));

			pHUDRoot->AddChild(m_pSpriteBulletGage1);
		}

		// 탄환이 튀는 Sprite
		{
			m_pSpriteBullet = i3Sprite2D::new_object();	
			m_pSpriteBullet->SetTexture( m_pTex );
			m_pSpriteBullet->Create(HUD_BULLET_ANIM_COUNT, TRUE, TRUE);

			for( i = 0; i<HUD_BULLET_ANIM_COUNT; i++)
			{
				// 탄환(1발)
				m_pSpriteBullet->SetEnable( i, FALSE);
				m_pSpriteBullet->SetRect( i, 0.0f, 0.0f, (4.0f), (17.0f));
				m_pSpriteBullet->SetColor( i, 255,255,255,255);
				m_pSpriteBullet->SetTextureCoord( i, (157.0f), (23.0f), (162.0f), (40.0f));
			}

			pHUDRoot->AddChild(m_pSpriteBullet);
		}
	}

	// HUD에서 사용하는 Player Point및 누적 Point를 설정합니다.
	{
		VEC3D vecRightBottom;

		m_pSprite->GetRightBottom( HUD_CSSPR_STATUS_SLOT, &vecRightBottom);
		
		INT32 nPosX = (INT32)(i3Vector::GetX(&vecRightBottom) - 110.0f);
		INT32 nPosY = (INT32)(i3Vector::GetY(&vecRightBottom) - 25.0f);

		m_pTextPoint = i3TextNodeDX2::new_object();
		m_pTextPoint->Create( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY, TRUE);
		m_pTextPoint->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pTextPoint->SetColor( 255, 255, 255, 255);
		m_pTextPoint->setPos( nPosX + 50, nPosY);
		m_pTextPoint->setSize( 65, 30);		
		pHUDRoot->AddChild( m_pTextPoint);

		m_pTextPointTotal = i3TextNodeDX2::new_object();
		m_pTextPointTotal->Create( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY, TRUE);
		m_pTextPointTotal->SetColor( 255, 255, 255, 255);
		m_pTextPointTotal->SetAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);	
		m_pTextPointTotal->setPos( nPosX, nPosY);
		m_pTextPointTotal->setSize( 45, 30);
		m_pTextPointTotal->SetText( "0");	// 기본값
		pHUDRoot->AddChild( m_pTextPointTotal);
	}

	// M197 HUD
	{
		// M197 HUD를 위한 무기 검사
		CStageBattle * pStage = g_pFramework->GetStageBattle();
		I3ASSERT( pStage != NULL);

		m_pM197 = NULL;

		STAGE_ID StageID = g_pGameContext->getStageID();
		if( StageID == STAGE_ID_OUTPOST || StageID == STAGE_ID_TUTORIAL)
		{
			StageObjectListPtr pList = pStage->GetStageObjectList();

			for( size_t i = 0; i < pList->size(); i++)
			{
				i3Object * pObj = (*pList)[i];

				if( i3::same_of<CGameObjectRidingHelicopter*>(pObj)) //->IsExactTypeOf( CGameObjectRidingHelicopter::static_meta()))
				{
					m_pM197 = ((CGameObjectRidingHelicopter*)pObj)->GetWeaponOnObject();
					break;
				}
			}
		}
		else if( StageID == STAGE_ID_HELISPOT)
		{
			StageObjectListPtr pList = pStage->GetStageObjectList();

			for( size_t i = 0; i < pList->size(); i++)
			{
				i3Object * pObj = (*pList)[i];

				if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj)) //->IsExactTypeOf( CGameObjectRespawnHelicopter::static_meta()))
				{
					if( (((CGameObjectRespawnHelicopter*)pObj)->getTeamIdx() == 0) &&
						(g_pGameContext->getMyTeam() & CHARACTER_TEAM_RED) )
					{
						m_pM197 = ((CGameObjectRespawnHelicopter*)pObj)->GetWeaponOnObject();
						break;
					}
					else if( (((CGameObjectRespawnHelicopter*)pObj)->getTeamIdx() == 1) &&
						(g_pGameContext->getMyTeam() & CHARACTER_TEAM_BLUE) )
					{
						m_pM197 = ((CGameObjectRespawnHelicopter*)pObj)->GetWeaponOnObject();
						break;
					}
				}
			}
		}

		if( m_pM197 != NULL || g_pGameContext->IsBossMissionMode())
		{
			bool bShowIcon = false;
			if( m_pM197 != NULL)	bShowIcon = true;

			m_pM197GaugeHUD = CHUD_WeaponHeatGauge::new_object();
			m_pM197GaugeHUD->Create( m_pHUD, bShowIcon);

			I3_GAMENODE_ADDCHILD( this, m_pM197GaugeHUD);
		}
	}

	if( g_pGameContext->IsDominationMode() )
	{
		// Life
		m_pTextLife	= i3TextNodeDX2::new_object();

		m_pTextLife->Create( BATTLE_CHARA_SET, FONT_COUNT_32, 28, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
		m_pTextLife->setSize( 70, 50);
		m_pTextLife->SetEnable( TRUE);

		pHUDRoot->AddChild( m_pTextLife);


		// 회복 이펙트
		m_v2RecoverImgSize.x	= 58.0f;
		m_v2RecoverImgSize.y	= 50.0f;

		RT_VEC3D*	pvEnd	= m_pSprite->GetCenter( HUD_CSSPR_STATUS_SLOT );

		m_v2RecoverImgStart.x	= ( nScreenWidth - m_v2RecoverImgSize.x ) / 2.0f;
		m_v2RecoverImgStart.y	= ( nScreenHeight - m_v2RecoverImgSize.y ) / 2.0f;
		m_v2RecoverImgEnd.x		= pvEnd->x - ( m_v2RecoverImgSize.x / 2.0f );
		m_v2RecoverImgEnd.y		= pvEnd->y - ( m_v2RecoverImgSize.x / 2.0f );

		i3Vector::Sub( &m_v2RecoverImgSpd, &m_v2RecoverImgEnd, &m_v2RecoverImgStart );
		i3Vector::Scale( &m_v2RecoverImgSpd, &m_v2RecoverImgSpd, 3.0f );

		for( INT32 i = 0; i < RECOVER_EFFT_MAX; i++ )
		{
			m_psprtRecoverImg[i]	= i3Sprite2D::new_object();
			m_psprtRecoverImg[i]->SetTexture( m_pTexHUD3 );
			m_psprtRecoverImg[i]->Create( HUD_RECOVER_MAX, TRUE, FALSE);

			m_psprtRecoverImg[i]->SetRect( HUD_RECOVER_HP, 100.0f, 100.0f, m_v2RecoverImgSize.x, m_v2RecoverImgSize.y );
			m_psprtRecoverImg[i]->SetRect( HUD_RECOVER_AMMO, 200.0f, 200.0f, m_v2RecoverImgSize.x, m_v2RecoverImgSize.y );
			m_psprtRecoverImg[i]->SetTextureCoord( HUD_RECOVER_HP, 289.0f, 228.0f, 347.0f, 278.0f );
			m_psprtRecoverImg[i]->SetTextureCoord( HUD_RECOVER_AMMO, 361.0f, 228.0f, 419.0f, 278.0f );

			pHUDRoot->AddChild( m_psprtRecoverImg[i] );
		}


		INT32 nPosX = nScreenWidth - 176;
		INT32 nPosY = nScreenHeight - 104;

		m_psprtRecoverEfft = i3Sprite2D::new_object();
		m_psprtRecoverEfft->SetTexture( m_pTexHUD3 );
		m_psprtRecoverEfft->Create( HUD_RECOVER_MAX, TRUE, FALSE);

		m_psprtRecoverEfft->SetRect( HUD_RECOVER_HP, (REAL32)nPosX, (REAL32)nPosY, 64.0f, 99.0f );
		m_psprtRecoverEfft->SetRect( HUD_RECOVER_AMMO, (REAL32)(nPosX + 63), (REAL32)(nPosY), 102.0f, 76.0f );
		m_psprtRecoverEfft->SetTextureCoord( HUD_RECOVER_HP, 534.0f, 340.0f, 598.0f, 439.0f);
		m_psprtRecoverEfft->SetTextureCoord( HUD_RECOVER_AMMO, 602.0f, 415.0f, 704.0f, 491.0f);

		pHUDRoot->AddChild( m_psprtRecoverEfft );
	}

	return true;
}

void CHUD_CharaState::Init(void)
{
	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	INT32 nPosX = nScreenWidth - 100;
	INT32 nPosY = nScreenHeight - 96;

	INT32 offsetWeaponString1 = LocaleValue::GetInt32("WinPosition/EscapeMission_WeaponString_Offset1");
	INT32 offsetWeaponString2 = LocaleValue::GetInt32("WinPosition/EscapeMission_WeaponString_Offset2");


	m_pTextWeapon->setPos(nPosX-9, nPosY-1);

	m_pTextDinoWeapon->setPos(nPosX + offsetWeaponString1 - 30, nPosY-1);
	m_pTextDinoWeapon2->setPos(nPosX + offsetWeaponString2 + 10, nPosY-1);

	nPosX = (nScreenWidth >> 1) - (84 >> 1);

	m_pTextHP->setPos( g_pViewer->GetViewWidth() - 170, g_pViewer->GetViewHeight() - 43);
	m_pTextHP->SetColor( 142, 183,  64, 255);
	m_pTextHP->SetText( "100");

	m_pTextHPPercent->setPos( g_pViewer->GetViewWidth() - 171, g_pViewer->GetViewHeight() - 43);
	m_pTextHPPercent->SetColor( 142, 183,  64, 255);
	m_pTextHPPercent->SetText( "100%");

	m_pTextBullet->setPos(g_pViewer->GetViewWidth() - 100, g_pViewer->GetViewHeight() - 82);
	m_pTextBullet->SetText("000/000");

	if( m_pTextLife != NULL)
	{
		m_pTextLife->setPos( g_pViewer->GetViewWidth() - 170, g_pViewer->GetViewHeight() - 83);
		m_pTextLife->SetText( "X0");
	}

	for( INT32 i = 0; i < HUD_RECOVER_MAX; i++ )
	{
		for( INT32 j = 0; j < RECOVER_EFFT_MAX; j++)
		{
			m_pfUpdateRecoverMove[i][j]	= &CHUD_CharaState::UpdateRecoverEfftMoveNull;
		}

		m_pfUpdateRecoverEfft[i]	= &CHUD_CharaState::UpdateRecoverEfftVisibleNull;
	}

	Reset();
}

void	CHUD_CharaState::Reset(void)
{
	m_nCurWeaponFlag = 0;
	m_nOldHp = 0;

	m_bIsEnduranceZeroWeapon = FALSE;
	m_fEnduranceZeroWeaponBlinkTime	 = 0.0f;
}

void	CHUD_CharaState::SetEnableWithoutNotice( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath)
{	
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
		return;
#endif

	INT32 i = 0;

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( bFlag && pPlayer != NULL)
	{
		m_pSprite->SetEnable( TRUE);
		
		if( pPlayer->IsDino() == false)
		{
			// 사람인 경우
			// KEYWORD : WEAPON_C5
			CGameCharaWeaponContext * pCtx = pPlayer->getWeaponContext();
			if( pCtx == NULL )		return ;
			if( pCtx->getWeapon( pCtx->getCurrentSlotNum()) != NULL)
			{
				if( i3::same_of<WeaponBombTrigger*>(pCtx->getWeapon( pCtx->getCurrentSlotNum()))) //->IsExactTypeOf( WeaponBombTrigger::static_meta()))
					m_pSprite->SetEnable( HUD_CSSPR_C5, TRUE);
				else
					m_pSprite->SetEnable( HUD_CSSPR_C5, FALSE);
			}

			m_pTextBullet->SetEnable(TRUE);		
			m_pTextHP->SetEnable(TRUE);
			m_pTextPoint->SetEnable( TRUE);
			m_pTextPointTotal->SetEnable( TRUE);

			m_pTextHPPercent->SetEnable( FALSE);
			m_pSprHUD2->SetEnable( FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_CHAR_STATE, TRUE);

			m_pTextWeapon->SetEnable( TRUE);
			m_pTextDinoWeapon->SetEnable( FALSE);
			m_pTextDinoWeapon2->SetEnable( FALSE);

			if( m_pTextLife != NULL) m_pTextLife->SetEnable( TRUE);
		}
		else
		{
			// 공룡인 경우
			m_pSprite->SetEnable( HUD_CSSPR_CHAR_STATE, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_HELMET, FALSE );

			// KEYWORD : WEAPON_C5 는 강제로 꺼줍니다.
			m_pSprite->SetEnable( HUD_CSSPR_C5, FALSE);

			m_pTextBullet->SetEnable( FALSE);
			m_pTextHP->SetEnable( FALSE);
			m_pTextPoint->SetEnable( FALSE);
			m_pTextPointTotal->SetEnable( FALSE);

			m_pTextHPPercent->SetEnable( TRUE);
			m_pSprHUD2->SetEnable( TRUE);

			m_pTextWeapon->SetEnable( FALSE);
			m_pTextDinoWeapon->SetEnable( TRUE);
			m_pTextDinoWeapon2->SetEnable( TRUE);

			if( m_pTextLife != NULL) m_pTextLife->SetEnable( FALSE);
		}

		UpdateBullet();

		m_pSprite->SetEnable(HUD_CSSPR_POINT, TRUE);
	}
	else
	{
		m_pSprite->SetEnable( FALSE);	

		// KEYWORD : WEAPON_C5 는 강제로 꺼줍니다.
		m_pSprite->SetEnable( HUD_CSSPR_C5, FALSE);

		m_pTextHP->SetEnable(FALSE);
		if( m_pTextLife != NULL) m_pTextLife->SetEnable( FALSE);

		m_pTextBullet->SetEnable(FALSE);
		m_pTextWeapon->SetEnable( FALSE);
		m_pTextDinoWeapon->SetEnable( FALSE);
		m_pTextDinoWeapon2->SetEnable( FALSE);

		// 획득예정 게임머니 표시부분은 리소폰 처리 때에도 유지되야한다.
		m_pSprite->SetEnable( HUD_CSSPR_POINT, TRUE);
		m_pTextPoint->SetEnable(TRUE);
		m_pTextPointTotal->SetEnable(TRUE);

		m_pTextHPPercent->SetEnable( FALSE);
		m_pSprHUD2->SetEnable( FALSE);

		m_pSpriteBulletGage1->SetEnable( FALSE);
		m_pSpriteBulletGage2->SetEnable( FALSE);
		for( i=0; i<HUD_BULLET_ANIM_COUNT; i++) m_pSpriteBullet->SetEnable( i, FALSE);
	}
}

void CHUD_CharaState::UpdateStandState(void)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call UpdateStandState.....\n");
		return;
	}
#endif

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer != NULL)
	{
		if( pPlayer->IsDino() == false)
		{
			// 캐릭터의 상태가 이전상태와 같은지 체크하여 같다면 리턴!
			if( pPlayer->getActionContext()->isBody( m_OldCharaLowerState))
				return;

			m_OldCharaLowerState = pPlayer->getActionContext()->getBodyLower();

			if( pPlayer->getActionContext()->isBody( CHARA_BODYLOWER_STAND))
			{
				m_pSprite->SetTextureCoord( HUD_CSSPR_CHAR_STATE, (110.0f), (1.0f), (152.0f), (51.0f));	// 서있는 자세
						
			}
			else if( pPlayer->getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
			{
				m_pSprite->SetTextureCoord( HUD_CSSPR_CHAR_STATE, (67.0f), (1.0f), (109.0f), (51.0f));	// 앉아있는 자세
			}
		}
		else if( pPlayer->IsRaptorBase() || g_pPlayer->IsRaptorGreen() || pPlayer->IsSting() || pPlayer->IsElite() )	
			//EscapeMission : 렙터 CROUCH 및 Stand상태에 대한 UI처리
		{
			// 캐릭터의 상태가 이전상태와 같은지 체크하여 같다면 리턴!
			m_OldCharaLowerState = pPlayer->getActionContext()->getBodyLower();
			
			if(( pPlayer->IsRaptor()) || (pPlayer->IsSting()|| (pPlayer->IsElite())))
				m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, (762.0f), (397.0f), (819.0f), (443.0f));	// 스팅과 랩터
			else if( pPlayer->IsAcid())
				m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, (826.0f), (400.0f), (872.0f), (448.0f));	// 에시드
			else if( pPlayer->IsTank())
				m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, (768.0f), (448.0f), (824.0f), (510.0f));	// 스팅과 랩터
			else if ( pPlayer->IsElite())
				m_pSprHUD3->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, (879.0f), (70.0f), (941.0f), (120.0f));	// 엘리트
			else if (pPlayer->IsCCRaptor() || pPlayer->IsRaptorMercury())
			{
				m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, (762.0f), (397.0f), (819.0f), (443.0f));	// 스팅과 랩터
			}
			else if (pPlayer->IsCCSting() || pPlayer->IsStingMars())
			{
				m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, (762.0f), (397.0f), (819.0f), (443.0f));	// 스팅과 랩터
			}
			else if (pPlayer->IsCCAcid() || pPlayer->IsAcidVulcan())
			{
				m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, (826.0f), (400.0f), (872.0f), (448.0f));	// 에시드
			}
		}
		else		//EscapeMission : 티렉스는 앉기 모드가 없으므로 고정 
		{ 
			m_pSprHUD2->SetTextureCoord( HUD_CSSPR2_CHAR_STATE2, (712.0f), (452.0f), (768.0f), (500.0f));	// 서있는 자세
		}
	}
	else
		m_pSprite->SetTextureCoord( HUD_CSSPR_CHAR_STATE, (110.0f), (1.0f), (152.0f), (51.0f));	// 서있는 자세
}

bool CHUD_CharaState::UpdateCharaHpState(REAL32 tm)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call UpdateCharaHpState....\n");
		return false;
	}
#endif

	CGameCharaBase * pPlayer = _GetCurrentChara();
	if( pPlayer == NULL)
		return false;

	m_UpdateCharaHpStateElapsed -= tm * 4.0f;

	if( m_UpdateCharaHpStateElapsed > 0.0f)
	{
		I3COLOR color;
		INT32 hp;

		if( pPlayer->IsDino() == false)
		{
			hp = pPlayer->getCurHP();
		}
		else
		{
			// 공룡인 경우, HP의 실치를 출력하지 않고 %로 표현하기 때문..
			hp = (INT32) ((REAL32)pPlayer->getCurHP() / pPlayer->getFullHP() * 100.0f);
		}

		// 일반 사람인 경우.
		if( hp >= 100)
		{
			i3Color::Set(&color, (UINT8) 255, 255,  255, 255);

			if( pPlayer->IsDino()) {
				m_pSprHUD2->SetColor( HUD_CSSPR2_CHAR_STATE2, &color);
			}
			else {
				m_pSprite->SetColor( HUD_CSSPR_CHAR_STATE, &color);
			}
			

			return false;
		}
		else
		{
			INT32	stat = (INT32)m_UpdateCharaHpStateElapsed;
			stat = stat%2;
			if( stat > 0 )
			{
				i3Color::Set(&color, (UINT8) 181,  35,   0, 255);
			}
			else
			{
				i3Color::Set(&color, (UINT8) 255, 255,  255, 255);
			}

			if( pPlayer->IsDino()) {
				m_pSprHUD2->SetColor( HUD_CSSPR2_CHAR_STATE2, &color);
			}
			else {
				m_pSprite->SetColor( HUD_CSSPR_CHAR_STATE, &color);
			}

			return true;
		}
	}

	return false;
}

void CHUD_CharaState::UpdateHp( REAL32 tm)
{
	char strHp[MAX_STRING_COUNT];

#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call UpdateHp.....\n");
		return;
	}
#endif

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer == NULL)
		return;

	if( m_bUpdateCharaHpState)
	{
		m_bUpdateCharaHpState = UpdateCharaHpState( tm);
	}

	if( m_nOldHp == pPlayer->getCurHP()) return;

	// 체력 회복의 경우에는 HUD의 깜빡임을 표현하지 않는다
	bool bRecovery = (m_nOldHp > 0) && (m_nOldHp < pPlayer->getCurHP()) ?	true : false;

	m_nOldHp = pPlayer->getCurHP();

	// HP에 따른 색 적용
	I3COLOR color;

	switch( pPlayer->getCurHP_Percent() / 20)
	{
	case 0:		i3Color::Set(&color, (UINT8) 181,  35,   0, 255);	break;	//  0 ~ 19
	case 1:		i3Color::Set(&color, (UINT8) 211,  92,  34, 255);	break;	// 20 ~ 39
	case 2:		i3Color::Set(&color, (UINT8) 202, 134,  54, 255);	break;	// 40 ~ 59
	case 3:		i3Color::Set(&color, (UINT8) 202, 176,  54, 255);	break;	// 60 ~ 79
	default:	i3Color::Set(&color, (UINT8) 142, 183,  64, 255);	break;	// 80 ~ 100		
	}

	//	인간은 HP 수치 그대로 표기
	if( pPlayer->IsDino() == false)
	{
		m_pTextHP->SetColor(&color);

		INT32 hp = pPlayer->getCurHP();

		if( hp > ENCRYPTION_MAX_HP_VALUE)	hp = ENCRYPTION_MAX_HP_VALUE;

		sprintf_s(strHp, "%03d", hp);

		m_pTextHP->SetText(strHp);

		if( m_pTextLife != NULL) 
		{
			INT32	nLife	= g_pGameContext->GetMyLife();
			char	szLife[MAX_STRING_COUNT];

			sprintf_s(szLife, "X%02d", nLife);

			m_pTextLife->SetText( szLife);
		}

	}	
	//	공룡은 HP를 100%를 기준으로 표기
	else
	{
		m_pTextHPPercent->SetColor( &color);

		INT32 hp = pPlayer->getCurHP_Percent();

		sprintf_s( strHp, "%03d%%", hp);

		m_pTextHPPercent->SetText( strHp);
	}

	{
		if( pPlayer->getCurHP_Percent() <= 0)
		{
			m_pSprite->SetColor( HUD_CSSPR_CHAR_STATE, &color);
			m_pSprHUD2->SetColor( HUD_CSSPR2_CHAR_STATE2, &color);
			m_bUpdateCharaHpState = false;
			m_UpdateCharaHpStateElapsed = 0.0f;
		}
		else
		{
			if(!bRecovery)
			{
				m_bUpdateCharaHpState = true;
				m_UpdateCharaHpStateElapsed = 10.0f;
			}
		}
	}
}

void CHUD_CharaState::UpdatePlayerPoint(  REAL32 rDeltaSeconds)
{
	COLOR col;
	BOOL bGainPoint = FALSE;
	char szPoint[36];	
	szPoint[0] = 0;

	// 총 획득 포인트
	INT32 nTotalPoint = g_pGameContext->GetAccPino();
	if( nTotalPoint < 0) nTotalPoint = 0;

	if( m_nPlayerTotalPoint != nTotalPoint)
	{
		m_nPlayerTotalPoint = nTotalPoint;
		
		sprintf_s( szPoint, "%d", nTotalPoint);
		m_pTextPointTotal->SetText( szPoint);
		bGainPoint = TRUE;
		m_fPlayerPointTimeDelta = 0.0f;
	}
	

	// 현재 획득 포인트	
	if( bGainPoint)
	{
		i3Color::Set( &col, (UINT8) 255, 255, 0, 255);
		sprintf_s( szPoint, "+%dP", g_pGameContext->GetAddPino());

		m_pTextPoint->SetEnable( TRUE);
		m_pTextPoint->SetColor( &col);
		m_pTextPoint->SetText( szPoint);
	}

	if( m_pTextPoint->GetEnable())
	{
		m_fPlayerPointTimeDelta += rDeltaSeconds;

		if( m_fPlayerPointTimeDelta > 2.0f)
		{
			m_pTextPoint->SetText( "");
			m_pTextPoint->SetEnable( FALSE);
			m_fPlayerPointTimeDelta = 0.0f;
		}
	}
}

void CHUD_CharaState::SetEnableCharaState( BOOL bFlag)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe() : call SetEnableCharaState...\n");
		return;
	}
#endif

	m_pSprite->SetEnable( HUD_CSSPR_HELMET, bFlag);
	m_pSprite->SetEnable( HUD_CSSPR_STATUS_SLOT, bFlag);
	m_pSprite->SetEnable( HUD_CSSPR_POINT, !bFlag);
	m_pSprite->SetEnable( HUD_CSSPR_KNIFE, bFlag);
	m_pSprite->SetEnable( HUD_CSSPR_GRENADE, bFlag);
	m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, bFlag);
	m_pSprite->SetEnable( HUD_CSSPR_BOMB, bFlag);
	m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, bFlag);
	
	m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, bFlag);
	m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, bFlag);
	m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, bFlag);
	m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, bFlag);
	

	if(g_pGameContext->IsBossMissionMode() == false)
	{
		bool bUseC5 = false;
		CGameCharaBase * pPlayer = _GetCurrentChara();
		if( pPlayer != NULL)
		{
			WeaponBase * pWeapon = pPlayer->getWeaponContext()->getWeapon( WEAPON_SLOT_THROWING1);
			if(  i3::same_of<WeaponBombTrigger*>(pWeapon)) //->IsExactTypeOf( WeaponBombTrigger::static_meta() ))
			{
				if( (((WeaponBombTrigger*)pWeapon)->getDropedBombCount() > 0) ||
					(pWeapon->getLoadedBulletCount() > 0))
				{
					if( pPlayer->getWeaponContext()->getCurrentSlotNum() == WEAPON_SLOT_THROWING1)
						m_pSprite->SetEnable( HUD_CSSPR_C5, bFlag);
					else
						m_pSprite->SetEnable( HUD_CSSPR_C5, FALSE);
					m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, bFlag);

					bUseC5 = true;
				}
				
			}
		}

		if( bUseC5 == false)
		{
			m_pSprite->SetEnable( HUD_CSSPR_C5, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
		}
	}

	m_pSprite->SetEnable( HUD_CSSPR_CHAR_STATE, bFlag);

	m_pTextHP->SetEnable(bFlag);

	if( m_pTextLife != NULL) m_pTextLife->SetEnable( bFlag);

	m_pTextBullet->SetEnable(bFlag);
	m_pTextWeapon->SetEnable( bFlag);
	m_pTextDinoWeapon->SetEnable( bFlag);
	m_pTextDinoWeapon2->SetEnable( bFlag);

	m_pSpriteBulletGage1->SetEnable( bFlag);

	m_pSprite->SetEnable(HUD_CSSPR_POINT, bFlag);
	m_pTextPointTotal->SetEnable( bFlag);
	m_pTextPoint->SetEnable( bFlag);
	
	if( !bFlag)
	{
		m_pSpriteBullet->SetEnable( bFlag);		
	}

	//
	m_pSprHUD2->SetEnable( bFlag);
	m_pTextHPPercent->SetEnable( bFlag);
}


void CHUD_CharaState::UpdateWeaponInfo( REAL32 rDeltaSeconds)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe() : call UpdateWeaponInfo...\n");
		return;
	}
#endif

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer == NULL)
		return;

	if( pPlayer->IsDino() == false)
	{
		if( i3::same_of<CGameCamera_OnObject*>(g_pCamera) //->IsExactTypeOf( CGameCamera_OnObject::static_meta()) 
			&& (m_pM197 != NULL ))
		{
			CWeaponInfo* pInfo = m_pM197->getWeaponInfo();

			// 탄환정보를 Update합니다.
			UpdateBullet();

			for( INT32 i = 0; i < HUD_BULLET_ANIM_COUNT; i++)
			{
				m_pSpriteBullet->SetEnable( i, FALSE);
			}
			/*
			switch( pInfo->GetTypeUsage() )
				case WEAPON_SLOT_PRIMARY	= 0,	//	주무기
				WEAPON_SLOT_SECONDARY	= 1,	//	보조무기
				WEAPON_SLOT_MELEE		= 2,	//	근접무기
				WEAPON_SLOT_THROWING1	= 3,	//	투척무기	
				WEAPON_SLOT_THROWING2	= 4,	//	아이템
				WEAPON_SLOT_MISSION		= 5,	//	미션 아이템
			*/

			CheckWeaponChange(m_pM197);
		}
		else
		{
			WeaponBase * pWeapon = pPlayer->getWeaponContext()->getCurrent();

			if( pWeapon == NULL )			return;
			pWeapon = pWeapon->getUsedWeapon();
			I3ASSERT( pWeapon != NULL);

			if( i3::same_of<WeaponBombTrigger*>(pWeapon)) //->IsExactTypeOf( WeaponBombTrigger::static_meta()) )
			{
				pWeapon = ((WeaponBombTrigger*) pWeapon)->getBomb();
				I3ASSERT( pWeapon != NULL);
			}

			CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
			if( pInfo == NULL)	return;

			// 탄환정보를 Update합니다.
			UpdateBullet();

			// 탄환이 발사되는 애니메이션을 Update합니다.
			UpdateBulletAnim( rDeltaSeconds);

#if defined( USE_REPAIR_WEAPON )
			//	내구도 무기가 0가 되면 빨간색이 깜빡이게
			if( m_bIsEnduranceZeroWeapon)
			{
				if( m_fEnduranceZeroWeaponBlinkTime < 10.0f)
					m_fEnduranceZeroWeaponBlinkTime += rDeltaSeconds;
				else
					m_fEnduranceZeroWeaponBlinkTime = 0.0f;

				if( m_fEnduranceZeroWeaponBlinkTime - (INT32) m_fEnduranceZeroWeaponBlinkTime < 0.5f)
					m_pTextWeapon->SetColor( 210,0,0,255);	//	red color
				else
					m_pTextWeapon->SetColor(180, 180, 180, 255);
			}
			else
			{
				m_pTextWeapon->SetColor(180, 180, 180, 255);
			}

			m_bIsEnduranceZeroWeapon	= pWeapon->IsEnduranceZero();
#endif

			CheckWeaponChange(pWeapon);
		}
	}
	else
	{
//		CGameCharaDino * pDino = (CGameCharaDino *) pChara;
		//EscapeMission Dino무기 Text수정
		if( pPlayer->IsRaptor() || pPlayer->IsRaptorBlue() || pPlayer->IsRaptorGreen() )
		{
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_RAPTER_ATTACK_A"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_RAPTER_ATTACK_B"));
			UpdateBullet();
		}
		else if( pPlayer->IsElite())
		{
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_RAPTER_ATTACK_A"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_DINO_ATTACK_HOWL"));
			UpdateBullet();
		}
		else if (pPlayer->IsSting())
		{
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_STING_ATTACK_A"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_STING_ATTACK_B"));
	
			UpdateBullet();
		}
		else if (pPlayer->IsTank())
		{
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_TANK_ATTACK_A"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_TANK_ATTACK_B"));

			UpdateBullet();
		}
		else if (pPlayer->IsAcid())
		{
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_RAPTER_ATTACK_B"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_ACID_ATTACK_B"));

			UpdateBullet();
		}
		else if( g_pPlayer->IsCCRaptor() || g_pPlayer->IsRaptorMercury())
		{
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_CC_RAPTER_ATTACK_A"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_CC_RAPTER_ATTACK_B"));
			UpdateBullet();
		}
		else if (g_pPlayer->IsCCSting() || g_pPlayer->IsStingMars())
		{
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_CC_STING_ATTACK_A"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_CC_STING_ATTACK_B"));
			UpdateBullet();
		}		
		else if (g_pPlayer->IsCCAcid() || g_pPlayer->IsAcidVulcan())
		{
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_CC_ACID_ATTACK_A"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_CC_ACID_ATTACK_B"));
			UpdateBullet();
		}
		else
		{
			// T-Rex인 경우.
			m_pTextDinoWeapon->SetText( GAME_STRING("STR_TBL_INGAME_HUD_RAPTER_ATTACK_B"));
			m_pTextDinoWeapon2->SetText( GAME_STRING("STR_TBL_INGAME_HUD_DINO_ATTACK_HOWL"));
			UpdateBullet();

		
		}
	}
		
}

void CHUD_CharaState::SetBulletGage()
{
	INT32 nBulletCount, nDualCount, nLoadedBullet, nDualBullet, nReloadCount;

#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call SetBulletGage....\n");
		return;
	}
#endif

	INT32 i = 0;
	i3Sprite2D *pSprite = NULL;
	static INT32 nOldnLoadedBullet = 0;

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer == NULL) return;
	if( pPlayer->IsDino())	return;

	WeaponBase * pWeapon = pPlayer->getWeaponContext()->getCurrent();
	if( pWeapon == NULL)	return;
	if( i3::same_of<WeaponKrissSuperV*>(pWeapon)) //->IsExactTypeOf( WeaponKrissSuperV::static_meta()))
	{
		if( ((WeaponKrissSuperV*)pWeapon)->isUsedDualWeapon())
			pWeapon = ((WeaponKrissSuperV*)pWeapon)->getDualWeapon();
	}

	nDualCount = 1;

	nReloadCount = pWeapon->getWeaponInfo()->GetReloadBulletCount();

	WEAPON_DUAL_MAGAZINE_TYPE type = pWeapon->getMagazineType();

	switch( type)
	{
	case WEAPON_DUAL_MAGAZINE_IDLE:
		{
			nLoadedBullet = pWeapon->getLoadedBulletCount();
			nDualBullet = pWeapon->getLoadedBulletDualCount();

			if( nReloadCount > 20 )
			{
				REAL32 nBulletAvr = 20.f / (REAL32) nReloadCount;
				nDualCount = (INT32) (nBulletAvr * nDualBullet);
				if( nDualCount == 0 && nDualBullet > 0)
				{
					nDualCount = 1;
				}
			}
			else
			{
				nDualCount = nDualBullet;
			}
		}
		break;
	case WEAPON_DUAL_MAGAZINE_RIGHT:	nLoadedBullet = pWeapon->getLoadedBulletDualCount();	break;
	default :
		{
			nLoadedBullet = pWeapon->getLoadedBulletCount();
			if( i3::kind_of<WeaponGrenadeLauncher*>(pWeapon)) //->IsTypeOf( WeaponGrenadeLauncher::static_meta()))
			{
				if( pWeapon->isExtensionActivate())
					nLoadedBullet = ((WeaponGrenadeLauncher*)pWeapon)->getLoadedShellCount();
			}
			else if( i3::kind_of<WeaponShotGunLauncher*>(pWeapon)) //->IsTypeOf( WeaponShotGunLauncher::static_meta()))
			{
				if( pWeapon->isExtensionActivate())
					nLoadedBullet = ((WeaponShotGunLauncher*)pWeapon)->getLoadedShellCount();
			}
		}
		break;
	}

	// 암호화
	// UI에 수정된 값을 절삭합니다.
	nLoadedBullet = MINMAX( 0, nLoadedBullet, 255);

	if( nReloadCount < nLoadedBullet )
		nReloadCount = nLoadedBullet;
	
	// 탄환 게이지는 현재 최대 20개입니다. 
	if( nReloadCount > 20) 
	{
		REAL32 nBulletAvr = 20.0f / (REAL32)nReloadCount;
		nBulletCount = (INT32)(nBulletAvr * nLoadedBullet);

		if( nBulletCount == 0 && nLoadedBullet > 0)
		{
			nBulletCount = 1;
		}
	}
	else
	{
		nBulletCount = nLoadedBullet;
	}

	REAL32 fScale = (REAL32)nBulletCount * 4.0f;

	VEC3D pos;

	if( m_bEnableDualGage)
	{
		if( m_bActivateDualIdx == 0)
		{
			pSprite = m_pSpriteBulletGage1;

			REAL32 fDualScale = (REAL32)nDualCount * 4.f;
			m_pSpriteBulletGage2->GetLeftTop( 1, &pos);
			m_pSpriteBulletGage2->SetRect( 1, i3Vector::GetX( &pos), i3Vector::GetY( &pos), fDualScale, (17.0f));
			m_pSpriteBulletGage2->SetTextureCoord( 1, (157.0f), (23.0f), (157.0f + fDualScale), (40.0f));
		}
		else
		{
			pSprite = m_pSpriteBulletGage2;
		}
	}
	else
	{
		pSprite = m_pSpriteBulletGage1;
	}

	pSprite->GetLeftTop( 1, &pos);
	pSprite->SetRect( 1, i3Vector::GetX( &pos), i3Vector::GetY( &pos), fScale, (17.0f));
	pSprite->SetTextureCoord( 1, (157.0f), (23.0f), (157.0f + fScale), (40.0f));

	if( pPlayer->getActionContext()->isUpper( CHARA_UPPER_FIRE))
	{
		if( nOldnLoadedBullet != nLoadedBullet)
		{
			// 탄환이 튀는 스프라이트 활성화
			for( i=0; i<HUD_BULLET_ANIM_COUNT; i++)
			{
				if( !m_pSpriteBullet->GetEnable( i))
				{
					m_pSpriteBullet->SetRect( i, i3Vector::GetX( &pos) + fScale, i3Vector::GetY( &pos), 4.0f, 17.0f);
					m_pSpriteBullet->SetEnable( i, TRUE);
					break;
				}
			}
		}

		nOldnLoadedBullet = nLoadedBullet;
	}
}

void CHUD_CharaState::UpdateBullet()
{
	// 배경슬롯이 활성화되지 않았다면, 처리하지 않는다
	if (FALSE == m_pSprite->GetEnable(HUD_CSSPR_STATUS_SLOT))
	{
		return;
	}

	bool bEnable = true;
	bool bHuman = true;

	if( i3::same_of<CGameCamera_OnObject*>(g_pCamera)) //->IsExactTypeOf( CGameCamera_OnObject::static_meta()))
	{
		if( m_pM197GaugeHUD != NULL)
		{
			I3ASSERT(m_pM197 != NULL);

			WeaponM197 * pM197 = (WeaponM197 * ) m_pM197;

			REAL32 rGauge = pM197->getFireGauge();
			REAL32 rMax = ((CM197Info*) pM197->getWeaponInfo())->GetContinueFireTime();

			SetDualGage( FALSE);

			m_pTextBullet->SetEnable(FALSE);
			m_pSpriteBulletGage1->SetEnable( FALSE);
			m_pSpriteBulletGage2->SetEnable( FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_BOMB, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
			//UI 스팅 오류 수정
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);

			m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
			m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

#ifdef CC_DINO
			m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
			m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif

			bEnable = false;

			// 타이머가 5초로 되어 있습니다.
			m_pM197GaugeHUD->UpdateGauge( (rGauge / rMax) * 100.f);
		}
	}
	else
	{
		BOOL	bUseDualMagagineRight = FALSE;
		INT32	nLoadedBullet = 0;
		INT32	nLoadedDualBullet = 0;

		CGameCharaBase * pPlayer = _GetCurrentChara();

		if( pPlayer == NULL)
		{
			bEnable = false;
			goto EXIT_UPDATE_BULLET;
		}

		if( pPlayer->isCharaStateMask( CHARA_STATE_DEATH) )
		{
			bEnable = false;
			goto EXIT_UPDATE_BULLET;
		}
		
		if( pPlayer->IsDino() == false)
		{
			WeaponBase * pWeapon = pPlayer->getWeaponContext()->getCurrent();
			if( pWeapon == NULL)
			{
				bEnable = false;
				goto EXIT_UPDATE_BULLET;
			}

			pWeapon = pWeapon->getUsedWeapon();
			I3ASSERT( pWeapon != NULL);
			CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
			if( pInfo == NULL)
			{
				bEnable = false;
				goto EXIT_UPDATE_BULLET;
			}
			
			if( pInfo->isRange())
			{		
				// 일반 장탄 무기
				m_pTextBullet->SetEnable(TRUE);

				nLoadedBullet = pWeapon->getLoadedBulletCount();
			
				if( i3::kind_of<WeaponGrenadeLauncher*>(pWeapon)) //->IsTypeOf( WeaponGrenadeLauncher::static_meta()))
				{
					if( pWeapon->isExtensionActivate())
						nLoadedBullet = ((WeaponGrenadeLauncher*)pWeapon)->getLoadedShellCount();
				}
				else if( i3::kind_of<WeaponShotGunLauncher*>(pWeapon)) //->IsTypeOf( WeaponShotGunLauncher::static_meta()))
				{
					if( pWeapon->isExtensionActivate())
						nLoadedBullet = ((WeaponShotGunLauncher*)pWeapon)->getLoadedShellCount();
				}

				WEAPON_DUAL_MAGAZINE_TYPE type = pWeapon->getMagazineType();

				switch( type)
				{
				case WEAPON_DUAL_MAGAZINE_IDLE:
					{
						SetDualGage( TRUE); 
						ChageDualGage( pWeapon->getMagazineType() - 1);
						nLoadedDualBullet = pWeapon->getLoadedBulletDualCount();
					}
					break;
				case WEAPON_DUAL_MAGAZINE_RIGHT:
					{
						bUseDualMagagineRight = TRUE;
						SetDualGage( TRUE); 
						ChageDualGage( pWeapon->getMagazineType() - 1);
						nLoadedBullet = pWeapon->getLoadedBulletCount();
						nLoadedDualBullet = pWeapon->getLoadedBulletDualCount();
					}
					break;
				default :
					{
						SetDualGage( FALSE);
					}
					break;
				}

				m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_BOMB, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				//UI 스팅 오류 수정
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);

				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif

				if (m_nOldBulletCount != nLoadedBullet || m_nOldBulletDualCount != nLoadedDualBullet || m_OldBulletClass != pInfo->GetTypeClass() || m_nOldBulletNumber != pInfo->GetNumber())
				{
					m_nOldBulletCount = nLoadedBullet;
					m_nOldBulletDualCount = nLoadedDualBullet;
					m_OldBulletClass = pInfo->GetTypeClass();
					m_nOldBulletNumber = pInfo->GetNumber();
				
					// Bullet Gage Set
					SetBulletGage();
				}
				else if (0 == nLoadedBullet)
				{
					VEC3D pos;

					m_pSpriteBulletGage1->GetLeftTop( 1, &pos);
					m_pSpriteBulletGage1->SetRect( 1, i3Vector::GetX( &pos), i3Vector::GetY( &pos), 0.0f, (17.0f));
					m_pSpriteBulletGage1->SetTextureCoord( 1, (157.0f), (23.0f), (157.0f), (40.0f));						
				}
			    
				char strBullet[MAX_STRING_COUNT];
				
				INT32 nTotalBullet = MAX(0, pWeapon->getTotalBulletCount());
				if( i3::kind_of<WeaponGrenadeLauncher*>(pWeapon)) //->IsTypeOf( WeaponGrenadeLauncher::static_meta()))
				{
					if( pWeapon->isExtensionActivate())
						nTotalBullet = ((WeaponGrenadeLauncher*)pWeapon)->getTotalShellCount();
				}
				else if( i3::kind_of<WeaponShotGunLauncher*>(pWeapon)) //->IsTypeOf( WeaponShotGunLauncher::static_meta()))
				{
					if( pWeapon->isExtensionActivate())
						nTotalBullet = ((WeaponShotGunLauncher*)pWeapon)->getTotalShellCount();
				}

				if(bUseDualMagagineRight)	sprintf_s(strBullet, "%03d/%03d", nLoadedDualBullet, nTotalBullet);			
				else						sprintf_s(strBullet, "%03d/%03d", nLoadedBullet, nTotalBullet);			
				
				m_pTextBullet->SetText(strBullet);
			}
			else if( pInfo->isMelee())
			{
				SetDualGage( FALSE);

				// 밀리
				m_pTextBullet->SetEnable(FALSE);

				m_pSpriteBulletGage1->SetEnable( FALSE);
				m_pSpriteBulletGage2->SetEnable( FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_BOMB, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				//UI 스팅 오류 수정
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);	
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);

				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif

				if(pInfo->GetTypeClass() == WEAPON_CLASS_KNUCKLE)
				{
					m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
					m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
					m_pSprite->SetEnable( HUD_CSSPR_KNIFE, TRUE);
				}
				else if( pInfo->GetRealNumber() == GetItemIndex("KNIFE_MINIAXE"))
				{
					m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, TRUE);
					m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
					m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
				}
				else if (pInfo->GetRealNumber() == GetItemIndex("KNIFE_BALLISTIC") )
				{
					m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, TRUE);
					m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
					m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
					m_pTextBullet->SetEnable(TRUE);
					char strBullet[MAX_STRING_COUNT];
					sprintf_s(strBullet, "        %d", pWeapon->getLoadedBulletCount());
					m_pTextBullet->SetText( strBullet);
				}
				else
				{
					m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
					m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
					m_pSprite->SetEnable( HUD_CSSPR_KNIFE, TRUE);
				}
			}
			else if( pInfo->isThrowing())
			{
				SetDualGage( FALSE);

				// 투척무기
				m_pTextBullet->SetEnable(TRUE);

				m_pSpriteBulletGage1->SetEnable( FALSE);
				m_pSpriteBulletGage2->SetEnable( FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_BOMB, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
				//티라로 렙터 공용
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				//스팅
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				//에시드
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				//탱크
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);
				//엘리트
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif
			
				char strBullet[MAX_STRING_COUNT];

				if( i3::same_of<WeaponBombTrigger*>(pWeapon)) //->IsExactTypeOf( WeaponBombTrigger::static_meta()))
				{
					m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
					m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, TRUE);
				}
				else
				{
					m_pSprite->SetEnable( HUD_CSSPR_GRENADE, TRUE);
					m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
				}

				sprintf_s(strBullet, "        %d", pWeapon->getLoadedBulletCount());
				m_pTextBullet->SetText( strBullet);
			}
			else if( pInfo->isItem())
			{
				SetDualGage( FALSE);

				// 아이템
				m_pTextBullet->SetEnable(TRUE);

				m_pSpriteBulletGage1->SetEnable( FALSE);
				m_pSpriteBulletGage2->SetEnable( FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_BOMB, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, TRUE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
				//티라노 렙터 공용
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				//스팅 
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				//에시드
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				//탱크
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);
				//엘리트
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif
			
				char strBullet[MAX_STRING_COUNT];

				sprintf_s(strBullet, "        %d", pWeapon->getLoadedBulletCount());
				m_pTextBullet->SetText(strBullet);
			}
			else if( pInfo->isMission())
			{
				SetDualGage( FALSE);

				// 폭탄이미지(미션용)
				m_pTextBullet->SetEnable(FALSE);
				m_pSpriteBulletGage1->SetEnable( FALSE);
				m_pSpriteBulletGage2->SetEnable( FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_BOMB, TRUE);
				m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
				m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
				//티라노 렙터 공용
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				//스팅
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				//에시드
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				//탱크
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);

#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif
			}
		}
		else
		{
			bHuman = false;

			if ( pPlayer->IsRaptor() || pPlayer->IsRaptorBlue()|| pPlayer->IsRaptorGreen())
			{
				//렙터
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, TRUE);
				//티라노 off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				//스팅 off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				//탱크 off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);
				//에시드 off			
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				//엘리트 off
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);
#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif

			}
			else if (pPlayer->IsTank())
			{
				//탱크
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, TRUE);
				//티랙스 렙터off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE,	 FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				//스팅 off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				//에시드 off			
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				//엘리트 off
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);
#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif


			}
			else if (pPlayer->IsAcid())
			{
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, TRUE);

				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);
				//엘리트 off
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);
#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif

			}
			else if (pPlayer->IsElite())
			{
				//엘리트
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, TRUE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, TRUE);
				//티라노 렙터 off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);			
				//스팅 off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				//에시드 off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				//탱크 off
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);
#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif

			}
			else if (pPlayer->IsSting())
			{
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);

				//엘리트 off
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);
#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif

			}
#ifdef CC_DINO
			else if (pPlayer->IsCCRaptor() || pPlayer->IsRaptorMercury())
			{
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, TRUE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, TRUE);

				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);

				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);		
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

			}
			else if (pPlayer->IsCCSting() || pPlayer->IsStingMars())
			{
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, TRUE);

				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);

				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);		
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);
			}
			else if (pPlayer->IsCCAcid() || pPlayer->IsAcidVulcan())
			{
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, TRUE);

				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);

				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);		
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);
			}
#endif
			else if(pPlayer->IsRex())
			{
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);

				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, TRUE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);

				//엘리트 off
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

#ifdef CC_DINO
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
				m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
				m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif
			}
			else
			{
				I3FATAL( "invalid update dino HUD!!!");
			}

			SetDualGage( FALSE);

			CGameCharaBase * pTargetChara = g_pCamera->getCurTarget();
			REAL32 GaugeValue = 0.f;

			if( pTargetChara != NULL)
			{
				if( pTargetChara->IsRex() )
				{
					CGameCharaDinoTRex * pRex = (CGameCharaDinoTRex*)pTargetChara;
					GaugeValue = pRex->GetHowlCooldownRate();
				}
				else if( pTargetChara->IsElite())
				{
					CGameCharaDinoElite * pElite = (CGameCharaDinoElite*)pTargetChara;
					GaugeValue = pElite->GetHowlCooldownRate();
				}
				else if( pTargetChara->IsSting())
				{
					CGameCharaDinoSting * pSting = (CGameCharaDinoSting *)pTargetChara;
					GaugeValue = pSting->GetPoisonCooldownRate();
				}
#ifdef CC_DINO
				else if( pTargetChara->IsCCRaptor())
				{
					CGameCharaDinoCCRaptor * pRaptor = (CGameCharaDinoCCRaptor *)pTargetChara;
					GaugeValue = pRaptor->GetBuff()->CooldownRate();
				}
				else if( pTargetChara->IsCCAcid())
				{
					CGameCharaDinoCCAcid * pAcid = (CGameCharaDinoCCAcid *)pTargetChara;
					REAL32 rGauge =pAcid->getExplosionGauge();
					REAL32 rMax = pAcid->getDBInfo()->getAcidExplosionTime();

					GaugeValue = rGauge / rMax;
				}
				else if( pTargetChara->IsRaptorMercury())
				{
					CGameCharaDinoRaptorMercury * pRaptor = (CGameCharaDinoRaptorMercury *)pTargetChara;
					GaugeValue = pRaptor->GetBuff()->CooldownRate();
				}
				else if( pTargetChara->IsAcidVulcan())
				{
					CGameCharaDinoAcidVulcan * pAcid = (CGameCharaDinoAcidVulcan *)pTargetChara;
					REAL32 rGauge =pAcid->getExplosionGauge();
					REAL32 rMax = pAcid->getDBInfo()->getAcidExplosionTime();

					GaugeValue = rGauge / rMax;
				}
#endif
				else if( pTargetChara->IsTank())
				{
					CGameCharaDinoTank * pTank = (CGameCharaDinoTank *)pTargetChara;

					REAL32 rGauge =pTank->getDashGauge();
					REAL32 rMax = 8.0f;

					GaugeValue = rGauge / rMax;
				}
				else if( pTargetChara->IsAcid())
				{
					CGameCharaDinoAcid * pAcid = (CGameCharaDinoAcid *)pTargetChara;

					REAL32 rGauge =pAcid->getExplosionGauge();
					REAL32 rMax = 2.0f;
					
					GaugeValue = rGauge / rMax;
				}
			}

			if( m_pM197GaugeHUD != NULL)
				m_pM197GaugeHUD->UpdateGauge( GaugeValue * 100.f);
		}
	}

EXIT_UPDATE_BULLET:

	if( bEnable == true)
	{
		if( bHuman == true)
		{
			// Dino HUD는 끈다.
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
			//UI 스팅 오류 수정
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);

			m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
			m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

			m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
			m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
			m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
		}
		else
		{	// Human의 Bullet HUD를 끈다.
			m_pTextBullet->SetEnable(FALSE);

			m_pSpriteBulletGage1->SetEnable( FALSE);
			m_pSpriteBulletGage2->SetEnable( FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_BOMB, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
			m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);
		}
	}
	else
	{
		// 모두 Off
		m_pTextBullet->SetEnable(FALSE);
		m_pSpriteBulletGage1->SetEnable( FALSE);
		m_pSpriteBulletGage2->SetEnable( FALSE);
		m_pSprite->SetEnable( HUD_CSSPR_GRENADE, FALSE);
		m_pSprite->SetEnable( HUD_CSSPR_GRENADE_ETC, FALSE);
		m_pSprite->SetEnable( HUD_CSSPR_BOMB, FALSE);
		m_pSprite->SetEnable( HUD_CSSPR_KNIFE, FALSE);
		m_pSprite->SetEnable( HUD_CSSPR_C5_BULLET, FALSE);
		m_pSprite->SetEnable( HUD_CSSPR_MINIAXE, FALSE);
		m_pSprite->SetEnable( HUD_CSSPR_BALLISTIC, FALSE);

		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SCRACH, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_HOWL, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BITE2, FALSE);
		
		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_SPIT, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_POISON, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_EXPLOSION, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_BUTT, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_DINO_DASH, FALSE);

		m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_SCRACH, FALSE);
		m_pSprHUD3->SetEnable(HUD_CSSPR3_ELITE_BUFF, FALSE);

#ifdef CC_DINO
		m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SCRACH, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_BITE, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_SPIT, FALSE);
		m_pSprHUD2->SetEnable(HUD_CSSPR2_CC_DINO_EXPLOSION, FALSE);
		m_pSprHUD3->SetEnable(HUD_CSSPR3_CC_RAPTOR_BUFF, FALSE);
#endif
	}
}

void CHUD_CharaState::UpdateBulletAnim( REAL32 rDeltaSeconds )
{
	INT32 i = 0;
	VEC3D pos;
	static REAL32 fRot[HUD_BULLET_ANIM_COUNT];
	static REAL32 fHeight[HUD_BULLET_ANIM_COUNT];

	for( i=0; i<HUD_BULLET_ANIM_COUNT; i++)
	{
		if( m_pSpriteBullet->GetEnable( i))
		{
			fRot[i] += rDeltaSeconds * 3.0f;
			fHeight[i] += rDeltaSeconds;

			m_pSpriteBullet->GetLeftTop( i, &pos);
			REAL32 fX = i3Vector::GetX(&pos) + rDeltaSeconds*400.0f;
			REAL32 fY = i3Vector::GetY(&pos) + (i3Math::sin(fHeight[i])*7.0f);
			
			m_pSpriteBullet->SetRect( i, fX, fY, 4.0f, 17.0f);
			m_pSpriteBullet->SetRotation( i, -fRot[i]);

			if( fX > (REAL32)g_pViewer->GetViewWidth() ||
				fY > (REAL32)g_pViewer->GetViewHeight())
			{
				fRot[i] = 0.0f;
				fHeight[i] = 0.0f;
				
				m_pSpriteBullet->SetEnable( i, FALSE);
			}
		}
	}
}

void CHUD_CharaState::UpdateEquipHelmet( REAL32 rDeltaSeconds )
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call UpdateEquipHelmet....\n");
		return;
	}
#endif

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer == NULL || (m_pHUD->getHudMode() == HUD_MODE_MISSION_OBSERVER) )
		return;

	UINT16 * pPlaySlot = g_pGameContext->getPlayingSlotFlag();

	if( pPlayer->IsDino() == false)
	{
		if( ((*pPlaySlot >> g_pGameContext->getMySlotIdx()) & 0x01) &&
			!pPlayer->isCharaStateMask( CHARA_STATE_DEATH))
		{
			if( pPlayer->getEquipContext()->isAttachOnChara( EQUIP::ePART_HEADGEAR ) )
			{
				// 장비 (헬멧)
				if( !m_bUpdateHelmet )
				{
					m_pSprite->SetEnable( HUD_CSSPR_HELMET, TRUE );
					m_pSprite->SetTextureCoord( HUD_CSSPR_HELMET,  463.f, 323.f, 509.f, 369.f );
					m_bUpdateHelmet = TRUE;
				}
			}
			else
			{
				// 장비 (헬멧)
				if( m_bUpdateHelmet && pPlayer->getEquipContext()->getHelmetProtect() )
				{
					m_pSprite->SetEnable( HUD_CSSPR_HELMET, TRUE );
					m_pSprite->SetTextureCoord( HUD_CSSPR_HELMET, 463.f, 373.f, 509.f, 419.f );
					m_bUpdateHelmet = FALSE;

					// 헬멧 프로텍션
					m_pHUD->AddHelmetProtection();
				}
			}
		}
	}
}

void CHUD_CharaState::UpdateEquipC5( REAL32 rDeltaTime)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call UpdateEquipC5....\n");
		return;
	}
#endif

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer == NULL || (m_pHUD->getHudMode() == HUD_MODE_MISSION_OBSERVER))
		return;

	BOOL bEnable = FALSE;

	if(pPlayer->IsDino()) {
		return;
	}

	if( !pPlayer->isCharaStateMask( CHARA_STATE_DEATH))
	{
		CGameCharaWeaponContext * pCtx = pPlayer->getWeaponContext();
		if( pCtx == NULL )		return;

		if( pCtx->getWeapon( WEAPON_SLOT_THROWING1) == NULL)
			return;

		if( i3::same_of<WeaponBombTrigger*>(pCtx->getWeapon( WEAPON_SLOT_THROWING1))) //->IsExactTypeOf( WeaponBombTrigger::static_meta()))
		{	
			WeaponBombTrigger * pBombTrigger = (WeaponBombTrigger*)pCtx->getWeapon( WEAPON_SLOT_THROWING1);

			if(  i3::same_of<WeaponC5*>(pBombTrigger->getBomb())) //->IsExactTypeOf( WeaponC5::static_meta()))
			{
				for( INT32 i = 0; i < pBombTrigger->getDropedBombCount(); i++)
				{
					WeaponC5 * pC5 = (WeaponC5*) pBombTrigger->getDropedBomb( i);
					
					if( pC5->isEnable() && pC5->isBombState( WEAPON_BOMBSTATE_SETTLED) )
					{	
						bEnable = TRUE;
						break;
					}
				}

				if( pCtx->getCurrentSlotNum() == WEAPON_SLOT_THROWING1)
				{	
					if( bEnable )
					{
						if( m_bUpdateC5)
						{
							m_pSprite->SetEnable( HUD_CSSPR_C5, TRUE);
							m_pSprite->SetTextureCoord( HUD_CSSPR_C5, 413.f, 359.f, 458.f, 405.f);
							m_bUpdateC5 = FALSE;
						}
					}
					else
					{
						if( !m_bUpdateC5)
						{
							m_pSprite->SetEnable( HUD_CSSPR_C5, TRUE);
							m_pSprite->SetTextureCoord( HUD_CSSPR_C5,	413.f, 407.f, 458.f, 453.f );
							m_bUpdateC5 = TRUE;
						}
					}
				}
				else if( !bEnable )
				{
					m_pSprite->SetEnable( HUD_CSSPR_C5, FALSE);
					m_bUpdateC5 = FALSE;
				}

			}
		}
	}
}

void CHUD_CharaState::SetDualGage( BOOL bFalg)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call SetDualGage.....\n");
		return;
	}
#endif

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer == NULL || pPlayer->IsDino()) {
		return;
	}

	if( bFalg )
	{
		m_pSpriteBulletGage1->SetEnable( TRUE);
		m_pSpriteBulletGage2->SetEnable( TRUE);
	}
	else
	{
		m_pSpriteBulletGage1->SetEnable( TRUE);
		m_pSpriteBulletGage2->SetEnable( FALSE);
	}

	if( m_bEnableDualGage != bFalg)
	{
		m_bEnableDualGage = bFalg;

		if( m_bEnableDualGage)
		{
			m_pSpriteBulletGage1->SetRect( 0, (REAL32)(nScreenWidth - 102), (REAL32)(nScreenHeight - 54), (82.0f), (19.0f));
			m_pSpriteBulletGage1->SetRect( 1, (REAL32)(nScreenWidth - 102), (REAL32)(nScreenHeight - 54), (82.0f), (17.0f));

			if( m_bActivateDualIdx == 0)
			{
				m_pSpriteBulletGage1->SetColor( 1, 255,255,255,255);
				m_pSpriteBulletGage2->SetColor( 1, 160,160,160,160);

				SetBulletGage();
			}
			else
			{
				m_pSpriteBulletGage2->SetColor( 1, 255,255,255,255);
				m_pSpriteBulletGage1->SetColor( 1, 160,160,160,160);

				INT32 nBulletCount, nDualCount, nLoadedBullet, nDualBullet, nReloadCount;
				WeaponBase * pWeapon = pPlayer->getWeaponContext()->getCurrent();
				I3ASSERT( pWeapon != NULL );
				
				nDualCount = 1;

				nReloadCount = pWeapon->getWeaponInfo()->GetReloadBulletCount();
				
				{
					nLoadedBullet = pWeapon->getLoadedBulletCount();
					nDualBullet = pWeapon->getLoadedBulletDualCount();

					if( nReloadCount > 20 )
					{
						REAL32 nBulletAvr = 20.f / (REAL32) nReloadCount;
						nDualCount = (INT32) (nBulletAvr * nDualBullet);
						if( nDualCount == 0 && nDualBullet > 0)
						{
							nDualCount = 1;
						}
					}
					else
					{
						nDualCount = nDualBullet;
					}
				}
				
				// 탄환 게이지는 현재 최대 20개입니다. 
				if( nReloadCount > 20) 
				{
					REAL32 nBulletAvr = 20.0f / (REAL32)nReloadCount;
					nBulletCount = (INT32)(nBulletAvr * nLoadedBullet);

					if( nBulletCount == 0 && nLoadedBullet > 0)
					{
						nBulletCount = 1;
					}
				}
				else
				{
					nBulletCount = nLoadedBullet;
				}

				REAL32 fScale = (REAL32)nBulletCount * 4.0f;

				VEC3D pos;

				m_pSpriteBulletGage1->GetLeftTop( 1, &pos);
				m_pSpriteBulletGage1->SetRect( 1, i3Vector::GetX( &pos), i3Vector::GetY( &pos), fScale, (17.0f));
				m_pSpriteBulletGage1->SetTextureCoord( 1, (157.0f), (23.0f), (157.0f + fScale), (40.0f));
			}
		}
		else
		{
			m_pSpriteBulletGage1->SetRect( 0, (REAL32)(nScreenWidth - 103), (REAL32)(nScreenHeight - 52), (82.0f), (19.0f));
			m_pSpriteBulletGage1->SetRect( 1, (REAL32)(nScreenWidth - 103), (REAL32)(nScreenHeight - 52), (82.0f), (17.0f));

			m_pSpriteBulletGage1->SetColor( 1, 255,255,255,255);
		}
	}
}

void CHUD_CharaState::ChageDualGage( INT32 nID)
{
	if( m_bActivateDualIdx == nID) return;

	m_pSpriteBulletGage1->SetEnable( TRUE);
	m_pSpriteBulletGage2->SetEnable( TRUE);

	i3Node * pHUDRoot = m_pHUD->getHUDRoot();

	if( nID == 0)	// 1번이 활성화
	{	
		// 랜더링 순서를 조절하기 위한 코드
		pHUDRoot->RemoveChild( m_pSpriteBulletGage1);
		pHUDRoot->AddChild( m_pSpriteBulletGage1);

		m_pSpriteBulletGage1->SetColor( 1, 255,255,255,255);
		m_pSpriteBulletGage2->SetColor( 1, 160,160,160,160);
	}
	else			// 2번이 활성화
	{
		pHUDRoot->RemoveChild( m_pSpriteBulletGage2);
		pHUDRoot->AddChild( m_pSpriteBulletGage2);

		m_pSpriteBulletGage1->SetColor( 1, 160,160,160,160);
		m_pSpriteBulletGage2->SetColor( 1, 255,255,255,255);
	}

	m_bActivateDualIdx = nID;
}

void CHUD_CharaState::CheckWeaponChange(WeaponBase* pWeapon)
{ 
	if( pWeapon == NULL || m_nCurWeaponFlag == pWeapon->getIdx() )
		return;

	CWeaponInfo*	pInfo	= pWeapon->getWeaponInfo();
	if( pInfo == NULL )
		return;

	char szWeaponName[ MAX_STRING_COUNT];

	pInfo->GetWeaponInfoName( szWeaponName);
	m_pTextWeapon->SetText( szWeaponName);

	m_nCurWeaponFlag	= pWeapon->getIdx();
}


INT32 CHUD_CharaState::_GetCurrentSlotIndex( void)
{
	I3ASSERT( m_pHUD != NULL);

	return m_pHUD->GetCurrentSlotIndex();
}

CGameCharaBase * CHUD_CharaState::_GetCurrentChara( void)
{
	return g_pCharaManager->getCharaByNetIdx( _GetCurrentSlotIndex());
}

void CHUD_CharaState::OnUpdate( REAL32 tm)
{
	// 캐릭터의 Hp를 Update합니다.
	UpdateHp( tm);

	// 캐릭터의 포인트를 Update합니다.
	UpdatePlayerPoint( tm);

	// 캐릭터의 상태(Stand/Crouch)를 Update합니다.
	UpdateStandState();

	// 탄환정보를 Update합니다.
	UpdateWeaponInfo( tm);

	CGameCharaBase * pPlayer = _GetCurrentChara();

	if( pPlayer != NULL && pPlayer->IsDino() == false)
	{
		// 헬멧 상태
		UpdateEquipHelmet( tm );

		// KEYWORD : WEAPON_C5
		UpdateEquipC5( tm);
	}


	// 회복 이펙트를 출력
	if( g_pGameContext->IsDominationMode() )
		UpdateRecoverEffect( tm );


	i3GameObjBase::OnUpdate( tm);
}

void CHUD_CharaState::SetHudMode( HUD_MODE mod)
{
	switch( mod)
	{
		case HUD_MODE_DEFAULT:
			// TRUE
			SetEnableWithoutNotice( TRUE, GAME_GUI_NONE);
			break;

		case HUD_MODE_MISSION_ROUND_START:
			// TRUE
			SetEnableWithoutNotice( TRUE, GAME_GUI_NONE);
			break;

		case HUD_MODE_MISSION_SCORE:
			// FALSE
			SetEnableWithoutNotice( FALSE, GAME_GUI_NONE);
			break;

		case HUD_MODE_MISSION_OBSERVER:
			// FALSE
			SetEnableWithoutNotice( FALSE, GAME_GUI_NONE);
			break;

		default:
			break;
	}
}

const static REAL32		RECOVER_BLINK_TIME	= 0.3f;

void CHUD_CharaState::SetRecover( HUD_RECOVER_TYPE type )
{
	if( !g_pGameContext->IsDominationMode() ) return;

	m_nRecoverIndex[ type ]++;
	m_nRecoverIndex[ type ]	= m_nRecoverIndex[ type ] % RECOVER_EFFT_MAX;

	INT32	nIndex	= m_nRecoverIndex[ type ];

	m_v2RecoverEfftCur[type][nIndex]	= m_v2RecoverImgStart;

	// 회복 이펙트
	m_psprtRecoverImg[nIndex]->SetEnable( type, TRUE );

	m_pfUpdateRecoverMove[type][nIndex]	= &CHUD_CharaState::UpdateRecoverEfftMove;
}

void CHUD_CharaState::UpdateRecoverEffect( REAL32 rDeltaSeconds )
{
	for(INT32 i = 0; i < HUD_RECOVER_MAX; i++)
	{
		for(INT32 j = 0; j < RECOVER_EFFT_MAX; j++)
		{
			(this->*m_pfUpdateRecoverMove[i][j])( rDeltaSeconds, i, j );
		}

		(this->*m_pfUpdateRecoverEfft[i])( rDeltaSeconds, i );
	}
}

void CHUD_CharaState::UpdateRecoverEfftMove( REAL32 tm, INT32 type, INT32 nIdx )
{
	VEC2D*	pPosCur	= &m_v2RecoverEfftCur[ type ][ nIdx ];
	VEC2D	vSpd;

	i3Vector::Scale( &vSpd, &m_v2RecoverImgSpd, tm );
	i3Vector::Add( pPosCur, pPosCur, &vSpd );

	if( *pPosCur >= m_v2RecoverImgEnd )
	{
		m_psprtRecoverImg[ nIdx ]->SetEnable( type, FALSE );

		if( m_nRecoverEfftCount[ type ] <= 0 )
		{
			m_psprtRecoverEfft->SetEnable( type, TRUE );
			m_pfUpdateRecoverEfft[ type ]	= &CHUD_CharaState::UpdateRecoverEfftVisible;
		}
		m_nRecoverEfftCount[ type ]	= 3;

		m_pfUpdateRecoverMove[ type ][ nIdx ]	= &CHUD_CharaState::UpdateRecoverEfftMoveNull;
		return;
	}

	m_psprtRecoverImg[ nIdx ]->SetRect( type, 
		pPosCur->x, pPosCur->y, m_v2RecoverImgSize.x, m_v2RecoverImgSize.y );
}
void CHUD_CharaState::UpdateRecoverEfftVisible( REAL32 tm, INT32 type )
{
	m_rRecoverEfftTime[type]	-= tm;

	if( m_rRecoverEfftTime[type] <= 0.0f )
	{
		m_psprtRecoverEfft->SetEnable( type, FALSE );

		m_nRecoverEfftCount[type]--;
		if( m_nRecoverEfftCount[type] <= 0 )
		{
			m_pfUpdateRecoverEfft[type]	= &CHUD_CharaState::UpdateRecoverEfftVisibleNull;
			return;
		}

		m_rRecoverEfftTime[type]	+= RECOVER_BLINK_TIME;
		m_pfUpdateRecoverEfft[type]	= &CHUD_CharaState::UpdateRecoverEfftNonVisible;
	}
}
void CHUD_CharaState::UpdateRecoverEfftNonVisible( REAL32 tm, INT32 type )
{
	m_rRecoverEfftTime[type]	-= tm;

	if( m_rRecoverEfftTime[type] <= 0.0f )
	{
		m_psprtRecoverEfft->SetEnable( type, TRUE );

		m_rRecoverEfftTime[type]	+= RECOVER_BLINK_TIME;
		m_pfUpdateRecoverEfft[type]	= &CHUD_CharaState::UpdateRecoverEfftVisible;
	}
}

#endif