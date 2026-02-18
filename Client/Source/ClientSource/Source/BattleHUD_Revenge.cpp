#include "pch.h"
#include "BattleHUD_Revenge.h"

#include "ConfigRenderBattle.h"
#include "BattleSlotContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"

I3_CLASS_INSTANCE(CHUD_Revenge);

namespace inner { CHUD_Revenge* this_ptr = 0; }
CHUD_Revenge* CHUD_Revenge::instance() { return inner::this_ptr; }

CHUD_Revenge::CHUD_Revenge(void)
{
	inner::this_ptr = this;

	m_pTexture = nullptr;
	m_pRevengeInfo = nullptr;
	m_pRevengeTextBlue = nullptr;
	m_pRevengeTextRed = nullptr;
	m_pRevengeTextBlueCount = nullptr;
	m_pRevengeTextRedCount = nullptr;

	for(INT32 i = 0 ; i < 16; ++i)
	{
		m_Revenge[i].nWinPoint = 0;
		m_Revenge[i].nLosePoint = 0;
	}

	m_bEnableRevengeUpdate = false;
	m_fRevengeLocalTime = 0.0f;
}

CHUD_Revenge::~CHUD_Revenge(void)
{
	inner::this_ptr = 0;

	I3_SAFE_RELEASE(m_pTexture);
	I3_SAFE_NODE_RELEASE(m_pRevengeInfo);
	I3_SAFE_NODE_RELEASE(m_pRevengeTextBlue);
	I3_SAFE_NODE_RELEASE(m_pRevengeTextRed);
	I3_SAFE_NODE_RELEASE(m_pRevengeTextBlueCount);
	I3_SAFE_NODE_RELEASE(m_pRevengeTextRedCount);
}

void CHUD_Revenge::CreateHUD(i3AttrSet* pHudRoot)
{
	i3Viewer * pViewer = g_pFramework->GetViewer();

	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	REAL32 fViewHalfWidth	= (REAL32)( nScreenWidth >> 1);	//È­¸éÀÇ Áß¾Ó ÁÂÇ¥, È­¸éÅ©±âÀÇ ¹Ý

	m_pTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD2);

	m_pRevengeInfo = i3Sprite2D::new_object();
	m_pRevengeInfo->SetTexture( m_pTexture );
	m_pRevengeInfo->Create(1, true, true);

	m_pRevengeInfo->SetEnable(0, false);
	m_pRevengeInfo->SetRect(0, fViewHalfWidth - 192.0f, (REAL32)nScreenHeight - 255.0f, 383.0f, 84.0f);
	m_pRevengeInfo->SetColor( 0, 255,255,255,255);
	m_pRevengeInfo->SetTextureCoord(0, 324.0f, 409.0f, 707.0f, 493.0f);
	pHudRoot->AddChild(m_pRevengeInfo);

	m_pRevengeTextRed = i3TextNodeDX2::new_object();
	m_pRevengeTextRed->Create( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_SIZE_HUD_REVENGE, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
	m_pRevengeTextRed->SetText(L"");	
	m_pRevengeTextRed->SetColor( 252, 88, 88, 255);
	m_pRevengeTextRed->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pRevengeTextRed->setAutoWrapEnable( false);
	m_pRevengeTextRed->SetEnable(false);
	m_pRevengeTextRed->setPos( (UINT32)(fViewHalfWidth - 185.0f), (UINT32)((REAL32)nScreenHeight - 245.0f));
	m_pRevengeTextRed->setSize( 169, 16);
	pHudRoot->AddChild(m_pRevengeTextRed);

	m_pRevengeTextBlue = i3TextNodeDX2::new_object();
	m_pRevengeTextBlue->Create( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_SIZE_HUD_REVENGE, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
	m_pRevengeTextBlue->SetText(L"");	
	m_pRevengeTextBlue->SetColor( 67, 182, 243, 255);
	m_pRevengeTextBlue->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pRevengeTextBlue->setAutoWrapEnable( false);
	m_pRevengeTextBlue->SetEnable(false);
	m_pRevengeTextBlue->setPos( (UINT32)(fViewHalfWidth + 10.0f), (UINT32)((REAL32)nScreenHeight - 245.0f));
	m_pRevengeTextBlue->setSize( 169, 16);
	pHudRoot->AddChild(m_pRevengeTextBlue);

	m_pRevengeTextRedCount = i3TextNodeDX2::new_object();
	m_pRevengeTextRedCount->Create(BATTLE_CHARA_SET, 8, 29, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
	m_pRevengeTextRedCount->SetText(L"");	
	m_pRevengeTextRedCount->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRevengeTextRedCount->setAutoWrapEnable( false);
	m_pRevengeTextRedCount->SetEnable(false);
	m_pRevengeTextRedCount->setPos( (UINT32)(fViewHalfWidth - 185.0f), (UINT32)((REAL32)nScreenHeight - 225.0f));
	m_pRevengeTextRedCount->setSize( 169, 45);
	pHudRoot->AddChild(m_pRevengeTextRedCount);

	m_pRevengeTextBlueCount = i3TextNodeDX2::new_object();
	m_pRevengeTextBlueCount->Create(BATTLE_CHARA_SET, 8, 29, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
	m_pRevengeTextBlueCount->SetText(L"");
	m_pRevengeTextBlueCount->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRevengeTextBlueCount->setAutoWrapEnable( false);
	m_pRevengeTextBlueCount->SetEnable(false);
	m_pRevengeTextBlueCount->setPos( (UINT32)(fViewHalfWidth + 10.0f), (UINT32)((REAL32)nScreenHeight - 225.0f));
	m_pRevengeTextBlueCount->setSize( 169, 45);
	pHudRoot->AddChild(m_pRevengeTextBlueCount);
}

void CHUD_Revenge::OnUpdate( REAL32 rDeltaSeconds )
{
	if(m_bEnableRevengeUpdate)
	{
		UpdateRevenge(rDeltaSeconds);
	}
}


void CHUD_Revenge::RecordRevenge(INT32 index, INT32 nPoint)
{
	INT32 i = 0;	
	const SLOT_INFO * pSlotInfo;
//	char szTemp[256];
	INT32 MySlotIdx = BattleSlotContext::i()->getMySlotIdx();

	i3::rc_wstring wstrTarget = BattleSlotContext::i()->getNickForSlot(index);
//	I3ASSERT( pszTarget != nullptr);

	if(BattleSlotContext::i()->IsRedTeam())
	{
		// Red
		for(i=0; i< SLOT_MAX_COUNT; i++)
		{				
			if(BattleSlotContext::i()->IsRedTeam(i) == false)
			{	
				pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);

				if(pSlotInfo->_State == SLOT_STATE_BATTLE)
				{
					if( m_Revenge[i].wstrTargetName != BattleSlotContext::i()->getNickForSlot(i) ) // != 0)
					{
						m_Revenge[i].wstrTargetName = BattleSlotContext::i()->getNickForSlot(i);
						m_Revenge[i].nWinPoint = 0;
						m_Revenge[i].nLosePoint = 0;
					}
				}
			}
		}
	}
	else
	{
		// Blue
		for(i=0; i< SLOT_MAX_COUNT; i++)
		{				
			if(BattleSlotContext::i()->IsRedTeam(i))
			{	
				pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);

				if(pSlotInfo->_State == SLOT_STATE_BATTLE)
				{
					if( m_Revenge[i].wstrTargetName != BattleSlotContext::i()->getNickForSlot(i))  // != 0)
					{
						m_Revenge[i].wstrTargetName = BattleSlotContext::i()->getNickForSlot(i);
						m_Revenge[i].nWinPoint = 0;
						m_Revenge[i].nLosePoint = 0;
					}
				}
			}
		}
	}

//	i3::wstring			wstrTemp;
	i3::stack_wstring	wstrStackTemp;

	for( i=0; i<SLOT_MAX_COUNT; i++)
	{
		if( m_Revenge[i].wstrTargetName == wstrTarget) 
		{
			if( nPoint < 0)
			{
				m_Revenge[i].nLosePoint -= nPoint;
				m_bEnableRevengeUpdate = true;
				m_fRevengeLocalTime = 0.0f;
			}
			else
			{
				m_Revenge[i].nWinPoint += nPoint;
			}

			if( BattleSlotContext::i()->IsRedTeam(index))
			{
				// Red(Enemy)
				m_pRevengeTextRed->SetTextEllipsis(m_Revenge[i].wstrTargetName);		

				i3::sprintf( wstrStackTemp, L"%d", m_Revenge[i].nLosePoint);
				m_pRevengeTextRedCount->SetText( wstrStackTemp);

				// Blue(Me)
				m_pRevengeTextBlue->SetTextEllipsis( BattleSlotContext::i()->getNickForSlot(MySlotIdx) );
				i3::sprintf( wstrStackTemp, L"%d", m_Revenge[i].nWinPoint);
				m_pRevengeTextBlueCount->SetText( wstrStackTemp);

				if( m_Revenge[i].nWinPoint > m_Revenge[i].nLosePoint)
				{
					m_pRevengeTextBlueCount->SetColor( 255, 200, 100, 255);
					m_pRevengeTextRedCount->SetColor( 255, 255, 255, 255);
				}
				else if(m_Revenge[i].nWinPoint < m_Revenge[i].nLosePoint)
				{
					m_pRevengeTextRedCount->SetColor( 255, 200, 100, 255);
					m_pRevengeTextBlueCount->SetColor( 255, 255, 255, 255);
				}		
				else
				{
					m_pRevengeTextRedCount->SetColor( 255, 255, 255, 255);
					m_pRevengeTextBlueCount->SetColor( 255, 255, 255, 255);
				}	
			}
			else
			{
				// Blue(Enemy)
				m_pRevengeTextBlue->SetTextEllipsis( m_Revenge[i].wstrTargetName );
				i3::sprintf( wstrStackTemp, L"%d", m_Revenge[i].nLosePoint);
				m_pRevengeTextBlueCount->SetText( wstrStackTemp);

				// Red(Me)
				m_pRevengeTextRed->SetTextEllipsis( BattleSlotContext::i()->getNickForSlot(MySlotIdx) );
				i3::sprintf( wstrStackTemp, L"%d", m_Revenge[i].nWinPoint);
				m_pRevengeTextRedCount->SetText( wstrStackTemp);

				if( m_Revenge[i].nWinPoint > m_Revenge[i].nLosePoint)
				{
					m_pRevengeTextRedCount->SetColor( 255, 200, 100, 255);
					m_pRevengeTextBlueCount->SetColor( 255, 255, 255, 255);
				}
				else if(m_Revenge[i].nWinPoint < m_Revenge[i].nLosePoint)
				{
					m_pRevengeTextBlueCount->SetColor( 255, 200, 100, 255);
					m_pRevengeTextRedCount->SetColor( 255, 255, 255, 255);
				}	
				else
				{
					m_pRevengeTextRedCount->SetColor( 255, 255, 255, 255);
					m_pRevengeTextBlueCount->SetColor( 255, 255, 255, 255);
				}	
			}	
			break;
		}
	}
}

void CHUD_Revenge::DeleteRecordRevenge(INT32 index)
{
	if( g_pEnvSet->m_b1VS1)
	{
		I3ASSERT( index < 16 && index >= 0);

		m_Revenge[index].wstrTargetName.clear();
		m_Revenge[index].nWinPoint = 0;
		m_Revenge[index].nLosePoint = 0;
	}
}

void CHUD_Revenge::UpdateRevenge(REAL32 rDeltaSeconds)
{
	if( g_pEnvSet->m_b1VS1)
	{
		m_fRevengeLocalTime += rDeltaSeconds;

		if( m_fRevengeLocalTime < 4.0f)
		{
			SetRevenge( true);
		}
		else
		{
			SetRevenge( false);		
		}
	}
}

void CHUD_Revenge::SetRevenge(bool bTrue)
{
	if( g_pEnvSet->m_b1VS1)
	{
		if (m_pRevengeInfo)
		{
			m_pRevengeInfo->SetEnable(0, bTrue);
			m_pRevengeTextRed->SetEnable(bTrue);
			m_pRevengeTextBlue->SetEnable(bTrue);
			m_pRevengeTextRedCount->SetEnable(bTrue);
			m_pRevengeTextBlueCount->SetEnable(bTrue);	
		}

		if( !bTrue)
		{
			m_bEnableRevengeUpdate = false;
			m_fRevengeLocalTime = 0.0f;
		}
	}
}
