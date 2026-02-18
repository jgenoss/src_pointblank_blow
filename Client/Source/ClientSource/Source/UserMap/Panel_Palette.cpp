#include "pch.h"
#include "Panel_Palette.h"
#include "UserMap_Define.h"
#include "StageMapEditor.h"

I3_CLASS_INSTANCE( CPanel_Palette, i3GameNode);

const INT32		BLOCK_BTN_CX	= 42;
const INT32		BLOCK_BTN_CY	= 41;
const INT32		BLOCK_GAP_CX	= 4;
const INT32		BLOCK_GAP_CY	= 4;
const INT32		BLOCK_START_X	= 20;
const INT32		BLOCK_START_Y	= 37;
const INT32		BLOCK_BORDER_CX	= 13;
const INT32		BLOCK_BORDER_CY	= 32;

CPanel_Palette::CPanel_Palette(void)
{
	m_pStage = NULL;
}

CPanel_Palette::~CPanel_Palette(void)
{
}

void CPanel_Palette::Create( CStageMapEditor * pStage)
{
	I3ASSERT( pStage != NULL);

	m_pStage = pStage;

	m_pWnd		= (i3UIFrameWnd *) i3UI::FindFrameWndByName( "Palette", this);	
	I3ASSERT( m_pWnd != NULL);

	m_pWnd->SetEnable( FALSE);

	m_pCTRL_TImgBtn_Palette		= (i3UIImageButton *) m_pWnd->FindChildByName( "TImgBtn_Palette");
	I3ASSERT( m_pCTRL_TImgBtn_Palette != NULL);

	m_pCTRL_TImgBtn_Palette->EnableCtrl( FALSE);

	_InitBlocks();
}

void CPanel_Palette::_InitBlocks(void)
{
	INT32 i;
	USMAP::BLOCK::INFO * pInfo;

	for( i = 0; i < USMAP::BLOCK::getCount(); i++)
	{
		pInfo = USMAP::BLOCK::getInfo( i);

		{
			i3UIImageButton * pBtn;

			{
				pBtn = i3UIImageButton::NewObject();

				m_pCTRL_TImgBtn_Palette->CopyTo( pBtn, I3_COPY_INSTANCE);
				
				m_pWnd->AddControl( pBtn);

				pBtn->SetControlShapeUV( I3UI_IMAGEBUTTON_SHAPE_ICON_NORMAL,	(INT32) pInfo->m_UV.x, (INT32) pInfo->m_UV.y);
				pBtn->SetControlShapeUV( I3UI_IMAGEBUTTON_SHAPE_ICON_PUSHED,	(INT32) pInfo->m_UV.x, (INT32) pInfo->m_UV.y + 40);
				pBtn->SetControlShapeUV( I3UI_IMAGEBUTTON_SHAPE_ICON_ONMOUSED,	(INT32) pInfo->m_UV.x, (INT32) pInfo->m_UV.y + 40);
				pBtn->SetControlShapeUV( I3UI_IMAGEBUTTON_SHAPE_ICON_DISABLED,	(INT32) pInfo->m_UV.x, (INT32) pInfo->m_UV.y);

				m_BlockBtnList.Add( pBtn);
			}
		}
	}
}

void CPanel_Palette::Popup( REAL32 x, REAL32 y)
{
	INT32 BlockCount, cx, cy, w, h;
	INT32 iconCX, iconCY;
	INT32 i, j, idx;

	if( m_pWnd->isEnable())
	{
		m_pWnd->SetEnable( FALSE);
		return;
	}

	m_pWnd->SetEnable( TRUE);

	BlockCount = USMAP::BLOCK::getCount();

	cx = min( BlockCount, 4);
	cy = min( ((BlockCount + cx - 1) / cx), 12);

	iconCX = BLOCK_BTN_CX + BLOCK_GAP_CX;
	iconCY = BLOCK_BTN_CY + BLOCK_GAP_CY;

	w = (BLOCK_BORDER_CX * 2) + (cx * iconCX) + BLOCK_GAP_CX;
	h = (BLOCK_BORDER_CY * 2) + (cy * iconCY) + BLOCK_GAP_CY;

	if( (y + h) > g_pViewer->GetViewHeight())
	{
		y = (REAL32)g_pViewer->GetViewHeight() - h;
	}

	m_pWnd->setPosition( x, y);
	m_pWnd->setSize( (REAL32)w, (REAL32)h);

	y = BLOCK_START_Y;

	for( i = 0, idx = 0; i < cy; i++)
	{
		x = BLOCK_START_X;

		for( j = 0; j < cx; j++, idx++)
		{
			if( idx < USMAP::BLOCK::getCount())
			{
				i3UIImageButton * pBtn = (i3UIImageButton *) m_BlockBtnList.Items[idx];
				pBtn->setPosition( x, y);
				pBtn->SetEnable( TRUE);

				x += iconCX;
			}
		}

		y += iconCY;
	}
}

BOOL CPanel_Palette::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pInfo = (I3UI_CONTROL_NOTIFY *) param2;
		i3UIControl * pCtrl = (i3UIControl *) pInfo->m_pCtrl;

		switch( pInfo->m_nEvent)
		{
			case I3UI_EVT_KILLFOCUS:
				I3TRACE( "KILLFOCUS\n");
				break;

			case I3UI_EVT_CLICKED :
				{
					INT32 idx = m_BlockBtnList.IndexOf( pCtrl);
					I3ASSERT( idx != -1);

					USMAP::BLOCK::INFO * pBlockInfo = USMAP::BLOCK::getInfo( idx);

					// Block 배치 모드로 설정.
					m_pStage->StartPlaceObject( pBlockInfo->m_ID);

					m_pWnd->EnableCtrl( FALSE);
				}
				break;
		}
	}

	return i3GameNode::OnEvent( event, pObj, param2, code);
}
