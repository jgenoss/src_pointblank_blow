// i3TDKTimeEventCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKTimeEventCtrl.h"
#include "i3TDKUtilGDI.h"
#include "i3TDKGlobalVariable.h"
#include "resource.h"
#include "i3TDKUpdateSystem.h"
#include "i3TDKClipboard.h"
#include "i3TDKDlgRename.h"
#include "i3TDKDlgSelectAction.h"
#include "i3TDKDlgTimeEventSetting.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKTimeEventCtrl

IMPLEMENT_DYNAMIC(i3TDKTimeEventCtrl, CWnd)

i3TDKTimeEventCtrl::~i3TDKTimeEventCtrl()
{
	if( m_hdcDraw != nullptr)
	{
		DeleteDC( m_hdcDraw);
		m_hdcDraw = nullptr;
	}

	if( m_hdcTemp != nullptr)
	{
		::DeleteDC( m_hdcTemp);
		m_hdcTemp = nullptr;
	}

	if( m_hpenBorder != nullptr)
	{
		DeleteObject( m_hpenBorder);
		m_hpenBorder = nullptr;
	}

	if( m_hbrBack != nullptr)
	{
		::DeleteObject( m_hbrBack);
		m_hbrBack = nullptr;
	}

	if( m_hFont != nullptr)
	{
		::DeleteObject( m_hFont);
		m_hFont = nullptr;
	}

	if( m_hbmpDraw != nullptr)
	{
		::DeleteObject( m_hbmpDraw);
		m_hbmpDraw = nullptr;
	}

	if( m_hRegion != nullptr)
	{
		::DeleteObject( m_hRegion);
		m_hRegion = nullptr;
	}

	if( m_hRulerRgn != nullptr)
	{
		::DeleteObject( m_hRulerRgn);
		m_hRulerRgn = nullptr;
	}

	I3_SAFE_RELEASE( m_pTimeEvent);
}

void	i3TDKTimeEventCtrl::SetTimeSequence( i3TimeEventGen * pTimeSeq)
{	
	I3_REF_CHANGE( m_pTimeEvent, pTimeSeq);

	if( m_pTimeEvent == nullptr)
	{
		Invalidate();
		return;
	}

	m_ShowStartTime = 0.0f;
	m_ShowStartIndex = 0;
	m_MaxDuration = pTimeSeq->getDuration();
	m_timeGuage = 0.0f;

	OnChangeTimeSequence();

	_UpdateScrollInfo();

	Invalidate();
}

void i3TDKTimeEventCtrl::AddSequence( i3TimeEventInfo * pInfo)
{
	if( m_pTimeEvent == nullptr)
		return;

	// 새 Event를 추가
	m_pTimeEvent->AddEvent( pInfo);

	_UpdateScrollInfo();

	Invalidate();
}

#define		INFO_CY				0
#define		RULER_CY			19
#define		ITEM_CY				16
#define		TRACK_CX			100
#define		TRACK_CY			17
#define		GUAGE_CX			9

#define		TRACK_SY			(1 + RULER_CY + INFO_CY)

void	i3TDKTimeEventCtrl::_GetRulerRect( RECT * pRect)
{
	pRect->left = TRACK_CX;
	pRect->top	= INFO_CY + 1;
	pRect->right = m_ClientWidth;
	pRect->bottom = pRect->top + RULER_CY;
}

void	i3TDKTimeEventCtrl::_GetGuageRect( RECT * pRect)
{
	INT32 px;

	px = _tm2pix( m_timeGuage);

	pRect->left = TRACK_CX + px - (GUAGE_CX >> 1);
	pRect->top = INFO_CY + 2;
	pRect->right = TRACK_CX + px + (GUAGE_CX >> 1);
	pRect->bottom = pRect->top + RULER_CY;
}

void	i3TDKTimeEventCtrl::_GetTrackRect( INT32 idx, RECT * pRect)
{
	idx -= m_ShowStartIndex;

	pRect->left = 1;
	pRect->top = TRACK_SY + (idx * TRACK_CY);
	pRect->right = TRACK_CX - 1;
	pRect->bottom = pRect->top + TRACK_CY;
}

void	i3TDKTimeEventCtrl::_GetTrackEnableRect( INT32 idx, RECT * pRect)
{
	_GetTrackRect( idx, pRect);

	pRect->left		+= 4;
	pRect->top		+= 4;
	pRect->right	= pRect->left + 7;
	pRect->bottom	= pRect->top + 7;
}

void	i3TDKTimeEventCtrl::_GetSlideRect( INT32 idx, RECT * pHeadRect, RECT * pBodyRect, RECT * pTailRect)
{
	INT32 startPix, endPix;
	i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( idx);

	idx -= m_ShowStartIndex;

	startPix	= TRACK_CX + _tm2pix( pInfo->getStartTime());
	endPix		= startPix;

	pHeadRect->left = startPix;
	pHeadRect->top = TRACK_SY + (idx * TRACK_CY);
	pHeadRect->right = pHeadRect->left + 5;
	pHeadRect->bottom = pHeadRect->top + TRACK_CY;

	pBodyRect->left = pHeadRect->right;
	pBodyRect->top = pHeadRect->top;
	pBodyRect->right = endPix - 5;
	pBodyRect->bottom = pHeadRect->bottom;

	pTailRect->left = pBodyRect->right;
	pTailRect->top = pBodyRect->top;
	pTailRect->right = endPix;
	pTailRect->bottom = pBodyRect->bottom;
}

void	i3TDKTimeEventCtrl::_GetSlideRect( INT32 idx, RECT * pRect)
{
	idx -= m_ShowStartIndex;

	pRect->left = TRACK_CX;
	pRect->top = TRACK_SY + (idx * TRACK_CY);
	pRect->right = m_ClientWidth;
	pRect->bottom = pRect->top + TRACK_CY;
}

void	i3TDKTimeEventCtrl::_DrawRuler(void)
{
	const int	_bmpCX = 24, _bmpCY = 17;
	INT32	i, count, _x, x, y;
	HBITMAP hbmpOld;
	TCHAR conv[256];

	{
		HBRUSH hbr = CreateSolidBrush( RGB( 255, 0, 0));
		CRect rt;

		rt.SetRect( 0, 0, m_ClientWidth + 1, m_ClientHeight + 1);

		::FillRect( m_hdcDraw, &rt, hbr);

		::DeleteObject( hbr);
	}

	// Border
	{
		::SelectObject( m_hdcDraw, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_SHADOW));
		::MoveToEx( m_hdcDraw, m_ClientWidth, 0, nullptr);
		::LineTo( m_hdcDraw, 0, 0);
		::LineTo( m_hdcDraw, 0, m_ClientHeight);

		::SelectObject( m_hdcDraw, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_LIT));
		::MoveToEx( m_hdcDraw, m_ClientWidth - 1, 1, nullptr);
		::LineTo( m_hdcDraw, m_ClientWidth - 1, m_ClientHeight - 1);
		::LineTo( m_hdcDraw, 0, m_ClientHeight - 1);
	}

	// 바닥
	{
		::SelectObject( m_hdcDraw, m_hpenBorder);
		::Rectangle( m_hdcDraw, 1, 1, m_ClientWidth - 1, 1 + RULER_CY);

		hbmpOld = (HBITMAP) ::SelectObject( m_hdcTemp, m_hbmpRulerBack);

		count = (m_ClientWidth - 2) / _bmpCX;
		_x = 2;

		// 배경 이미지를 바른다.
		for( i = 0; i < count; i++)
		{
			::BitBlt( m_hdcDraw, _x, INFO_CY + 2, _bmpCX, _bmpCY, m_hdcTemp, 0, 0, SRCCOPY);
			_x += _bmpCX;
		}

		// 마지막 끝부분
		::BitBlt( m_hdcDraw, (m_ClientWidth - 1) - _bmpCX, 2, _bmpCX, _bmpCY, m_hdcTemp, 0, 0, SRCCOPY);

		::SelectObject( m_hdcTemp, hbmpOld);
	}

	// Time Line
	{
		CRect rt;

		::SetTextColor( m_hdcDraw, RGB( 81, 81, 81));
		::SetBkMode( m_hdcDraw, TRANSPARENT);
		::SelectObject( m_hdcDraw, m_hFont);

		rt.SetRect( 6, INFO_CY + 4, 47, 15);

		if((m_pTimeEvent != nullptr) && ( m_pTimeEvent->isStyle( I3_TIMEEVENT_REPEAT)))
			::DrawText( m_hdcDraw, _T("Time Line (Repeat)"), -1, &rt, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
		else
			::DrawText( m_hdcDraw, _T("Time Line"), -1, &rt, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
	}

	// 시간 단위 간격을 구한다.
	// Zoom 100%일 때, 2 ms가 1 Pixel에 해당한다. 즉, 1초는 500 Pixel 길이로 표현된다.
	REAL32	timeStep = 0.1f, tm, tmLength, accm;
	CRect rc, rc2;
	INT32 len;
	SIZE Sz;
	bool bFirst = true;

	::SelectClipRgn( m_hdcDraw, m_hRulerRgn);

	rc.SetRect( 100, 11, 100 + _tm2pix( m_MaxDuration), 19);

	::SelectObject( m_hdcDraw, GetStockObject( WHITE_BRUSH));
	::SelectObject( m_hdcDraw, GetStockObject( BLACK_PEN));
	::Rectangle( m_hdcDraw, rc.left, rc.top, rc.right, rc.bottom);

	rc.SetRect( 100, 11, m_ClientWidth, 19);

	// 5 pixel의 간격으로 눈금을 표시한다.
	timeStep = 0.01f;
	tmLength = _pix2tm( rc.Width());

	{
		INT32 div;

		div = (INT32)(m_ShowStartTime / timeStep);
		tm = timeStep * div;
	}

	y = RULER_CY - 1;

	SetTextColor( m_hdcDraw, RGB( 51, 54, 51));

	for( ; tm < tmLength; tm += timeStep)
	{
		x = rc.left + _tm2pix( tm);

		MoveToEx( m_hdcDraw, x, y, nullptr);

		
		{
			INT32 div = (INT32) (tm / 0.2f);
			accm = ((div + 1) * 0.2f);
			accm = i3Math::abs( accm - tm);
		}

		if( accm <= timeStep)
		{
			LineTo( m_hdcDraw, x, y - 6);

			i3::snprintf( conv, _countof(conv), _T("%.1f"), tm);

			len = i3::generic_string_size( conv);

			::GetTextExtentPoint32( m_hdcDraw, conv, len, &Sz);
			::TextOut( m_hdcDraw, x - (Sz.cx >> 1), 1, conv, len);

			accm = 0.0f;
			bFirst = false;
		}
		else
		{
			LineTo( m_hdcDraw, x, y - 4);
			accm += timeStep;
		}
	}

	// Guage
	{
		_GetGuageRect( &rc);

		i3TDK::DrawButton( m_hdcDraw, &rc, m_hdcTemp, nullptr, &m_hbmpTable[TSQBMP_QUAGE], nullptr);
	}

	::SelectClipRgn( m_hdcDraw, nullptr);
}

void	i3TDKTimeEventCtrl::_DrawItem( RECT * pRect, i3TimeEventInfo * pInfo)
{
	CRect rt;
	INT32 idx;

	// Track Name
	{
		rt = *pRect;

		idx = TSQBMP_TRACK_NORMAL_LEFT;
		if( pInfo->getState() & I3_TIMEEVENT_STATE_SELECTED)
			idx = TSQBMP_TRACK_SELECT_LEFT;
		if( pInfo->getState() & I3_TIMEEVENT_STATE_DISABLE)
			idx = TSQBMP_TRACK_DISABLE_LEFT;

		rt.right = rt.left + TRACK_CX;
		rt.top += 1;
		rt.bottom -= 1;
		i3TDK::DrawButton( m_hdcDraw, &rt, m_hdcTemp, &m_hbmpTable[idx], &m_hbmpTable[idx + 1], &m_hbmpTable[idx + 2]);

		// Icon
		{
			switch( (pInfo->getState() >> 16) & 0x0000FFFF)
			{
				case NODE_TYPE_SOUND :		idx = I3TDK_IMAGELIST_SOUND;	break;
				//case NODE_TYPE_PARTICLE :	idx = 
				default :					idx = I3TDK_IMAGELIST_GEOMETRY;	break;
			}

			if( pInfo->getState() & I3_TIMEEVENT_STATE_DISABLE)
				idx++;

			CImageList * pImageList = g_pTDKGlobalRes->m_pTDKImageList;

			{
				CDC * pDC;
				POINT pt;

				pDC = CDC::FromHandle( m_hdcDraw);

				pt.x = rt.left + 15;
				pt.y = rt.top + 0;

				pImageList->Draw( pDC, idx, pt, ILD_TRANSPARENT);			
			}
		}

		// Name
		{
			const TCHAR* pszName;
#ifdef _UNICODE
			i3::stack_wstring wstrName;
#endif

			if( pInfo->hasName())
			{
#ifdef _UNICODE
				i3::mb_to_utf16(pInfo->GetName(), wstrName);
				pszName = wstrName.c_str();
#else
				pszName = pInfo->GetName();
#endif
			}
			else
			{
				pszName = _T("<Noname>");
			}

			rt.left += 33;
			rt.right -= 1;

			if( pInfo->getState() & I3_TIMEEVENT_STATE_DISABLE)
				SetTextColor( m_hdcDraw, RGB( 150, 150, 150));
			else
				SetTextColor( m_hdcDraw, RGB( 220, 223, 220));
	
			::DrawText( m_hdcDraw, pszName, -1, &rt, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER); 
		}
	}

	// Track
	{
		rt = *pRect;

		rt.left += TRACK_CX;
		rt.top += 1;
		rt.bottom -= 1;
		i3TDK::DrawButton( m_hdcDraw, &rt, m_hdcTemp, nullptr, &m_hbmpTable[ TSQBMP_SLIDE_BACK], nullptr);
	}

	// Slide
	{
		rt.left = (pRect->left + TRACK_CX) + _tm2pix( pInfo->getStartTime()) - 4;
		rt.right = rt.left;
		rt.top = pRect->top;
		rt.bottom = pRect->bottom;

		idx = TSQBMP_SLIDE_NORMAL_LEFT;
		if( pInfo->getState() & I3_TIMEEVENT_STATE_SELECTED)
			idx = TSQBMP_SLIDE_SELECT_LEFT;
		if( pInfo->getState() & I3_TIMEEVENT_STATE_DISABLE)
			idx = TSQBMP_SLIDE_DISABLE_LEFT;

		SelectClipRgn( m_hdcDraw, m_hRegion);

		i3TDK::DrawButton( m_hdcDraw, &rt, m_hdcTemp, &m_hbmpTable[ idx], &m_hbmpTable[ idx + 1], nullptr);

		SelectClipRgn( m_hdcDraw, nullptr);
	}
}

void	i3TDKTimeEventCtrl::_DrawItems(void)
{
	UINT32 i;
	CRect rt;

	rt.SetRect( 1, TRACK_SY, m_ClientWidth - 1, m_ClientHeight - 1);

	::FillRect( m_hdcDraw, &rt, m_hbrBack);

	if( m_pTimeEvent != nullptr)
	{
		for( i = 0; i < m_pTimeEvent->getEventCount(); i++)
		{
			i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( i);

			rt.bottom = rt.top + ITEM_CY;

			// DrawItem
			_DrawItem( &rt, pInfo);

			if( rt.bottom > m_ClientHeight - 1)
				break;

			rt.top = rt.bottom + 1;
		}

		// Event Name
		{
			CRect rt2;

			::SetTextColor( m_hdcDraw, RGB( 255, 255, 255));
			::SetBkMode( m_hdcDraw, TRANSPARENT);
			::SelectObject( m_hdcDraw, m_hFont);

			rt2.SetRect( 3, m_ClientHeight - RULER_CY , m_ClientWidth - 1, m_ClientHeight - 1);

			TCHAR conv[MAX_PATH];
			i3::snprintf( conv, _countof(conv), _T("Event Resouce : %s"), m_pTimeEvent->GetName());

			::DrawText( m_hdcDraw, conv, -1, &rt2, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
		}
	}
}

void	i3TDKTimeEventCtrl::_Draw(void)
{
	_DrawRuler();
	_DrawItems();
}

NC_HIT	i3TDKTimeEventCtrl::HitTest( POINT pt)
{
	CRect rt;
	UINT32	count, i;

	_GetGuageRect( &rt);
	if( PtInRect( &rt, pt))
		return NC_HIT_GUAGE;

	_GetRulerRect( &rt);
	if( PtInRect( &rt, pt))
		return NC_HIT_RULER;

	if( m_pTimeEvent == nullptr)
		return NC_HIT_NONE;
	

	CRect rtHead, rtBody, rtTail;

	count = ((m_ClientHeight - rt.bottom) / TRACK_CY) + 1;

	for( i = m_ShowStartIndex; i < (m_ShowStartIndex + count); i++)
	{
		if( i >= m_pTimeEvent->getEventCount())
			break;

		_GetTrackEnableRect( i, &rt);
		if( PtInRect( &rt, pt))
			return NC_HIT_TRACK_ENABLE;

		_GetTrackRect( i, &rt);
		if( PtInRect( &rt, pt))
			return NC_HIT_TRACK;

		_GetSlideRect( i, &rtHead, &rtBody, &rtTail);
		if( PtInRect( &rtHead, pt))
			return NC_HIT_SLIDE_HEAD;

		if( PtInRect( &rtBody, pt))
			return NC_HIT_SLIDE_BODY;

		if( PtInRect( &rtTail, pt))
			return NC_HIT_SLIDE_TAIL;

		_GetSlideRect( i, &rt);
		if( PtInRect( &rt, pt))
			return NC_HIT_SLIDE_EMPTY;
	}

	return NC_HIT_NONE;
}

INT32 i3TDKTimeEventCtrl::FindItem( POINT pt, NC_HIT * pHitTest)
{
	NC_HIT hit;

	hit = HitTest( pt);

	if( pHitTest != nullptr)
	{
		*pHitTest = hit;
	}

	switch( hit)
	{
		case NC_HIT_TRACK_ENABLE :
		case NC_HIT_TRACK :
		case NC_HIT_SLIDE_HEAD :
		case NC_HIT_SLIDE_BODY :
		case NC_HIT_SLIDE_TAIL :
		case NC_HIT_SLIDE_EMPTY :
			{
				CRect rt;

				_GetRulerRect( &rt);

				return ((pt.y - rt.bottom) / TRACK_CY) + m_ShowStartIndex;
			}
			break;
	}

	return -1;
}

INT32 i3TDKTimeEventCtrl::FindInsertPos( POINT pt)
{
	NC_HIT hit;
	INT32 idx = -1;
	CRect rt;

	hit = HitTest( pt);

	switch( hit)
	{
		case NC_HIT_TRACK_ENABLE :
		case NC_HIT_TRACK :
		case NC_HIT_SLIDE_HEAD :
		case NC_HIT_SLIDE_BODY :
		case NC_HIT_SLIDE_TAIL :
		case NC_HIT_SLIDE_EMPTY :
			{
				_GetRulerRect( &rt);

				idx = ((pt.y - rt.bottom) / TRACK_CY) + m_ShowStartIndex;
			}
			break;
	}

	if( idx == -1)
	{
		_GetTrackRect( m_ShowStartIndex, &rt);

		if( rt.top > pt.y)
			idx = m_ShowStartIndex;
		else
		{
			INT32 cnt = m_ClientHeight / rt.Height() + 1;

			idx = MIN( (INT32) m_pTimeEvent->getEventCount(), m_ShowStartIndex + cnt);
		}
	}

	return idx;
}

void	i3TDKTimeEventCtrl::OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx)
{
}

void	i3TDKTimeEventCtrl::OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx)
{
}

void	i3TDKTimeEventCtrl::OnChangeCurrentTime( REAL32 tm)
{
	if( m_pTimeEvent != nullptr)
	{
		m_pTimeEvent->OnChangeTime( tm);
	}
}

void	i3TDKTimeEventCtrl::OnChangeTimeSequence(void)
{
}

void	i3TDKTimeEventCtrl::OnChangeItemEnable( i3TimeEventInfo * pInfo)
{
}

void	i3TDKTimeEventCtrl::OnSelectAll(void)
{
	INT32 i;
	i3TimeEventInfo * pInfo;

	if( m_pTimeEvent == nullptr)
		return;

	for( i = 0; i < (INT32) m_pTimeEvent->getEventCount(); i++)
	{
		pInfo = m_pTimeEvent->getEvent( i);

		pInfo->addState( I3_TIMEEVENT_STATE_SELECTED);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo);

		if( pInfo->getAction() != nullptr)
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo->getAction());
		}
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeEvent);

	Invalidate();
}

void	i3TDKTimeEventCtrl::OnSelect( INT32 idx)
{
	i3TimeEventInfo * pInfo;

	pInfo = m_pTimeEvent->getEvent( idx);

	_SelectItem( idx);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo);

	if( pInfo->getAction() != nullptr)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo->getAction());
	}

	Invalidate();
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeEvent);
}

void	i3TDKTimeEventCtrl::OnUnselectAll(void)
{
	INT32 i;
	i3TimeEventInfo * pInfo;

	if( m_pTimeEvent == nullptr)
		return;

	for( i = 0; i < (INT32) m_pTimeEvent->getEventCount(); i++)
	{
		pInfo = m_pTimeEvent->getEvent( i);

		pInfo->removeState( I3_TIMEEVENT_STATE_SELECTED);
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeEvent);

	m_ED_Time.ShowWindow( SW_HIDE);

	Invalidate();
}

void	i3TDKTimeEventCtrl::OnDelete(void)
{
	UINT32 i;
	i3TimeEventInfo * pInfo;

	if( m_pTimeEvent == nullptr)
		return;

	for( i = 0; i < (INT32) m_pTimeEvent->getEventCount(); i++)
	{
		pInfo = m_pTimeEvent->getEvent( i);

		if( pInfo->getState() & I3_TIMEEVENT_STATE_SELECTED)
		{
			m_pTimeEvent->RemoveEvent( i);
			i--;

			m_ED_Time.ShowWindow( SW_HIDE);
		}
	}

	if( m_ShowStartIndex >= (INT32) m_pTimeEvent->getEventCount())
		m_ShowStartIndex = MAX( 0, (INT32)m_pTimeEvent->getEventCount() - 1);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeEvent);

	Invalidate();
}

void i3TDKTimeEventCtrl::OnCopy(void)
{
	if( m_pTimeEvent == nullptr)
		return;
}

void i3TDKTimeEventCtrl::OnPaste(void)
{
}

void i3TDKTimeEventCtrl::OnRename( INT32 idxSel)
{
	i3TDKDlgRename dlg;
	i3TimeEventInfo * pInfo;

	pInfo = m_pTimeEvent->getEvent( idxSel);

	if( pInfo->hasName())
	{
#ifdef _UNICODE
		i3::rc_wstring wstr;	i3::mb_to_utf16(pInfo->GetNameString(), wstr);
		dlg.setName(wstr);
#else
		dlg.setName( pInfo->GetNameString());
#endif
	}

	HINSTANCE hOld = AfxGetResourceHandle();

	AfxSetResourceHandle( g_hInstTDK);
	
	if( dlg.DoModal() == IDCANCEL)
	{
		AfxSetResourceHandle( hOld);
		return;
	}

	AfxSetResourceHandle( hOld);
#ifdef _UNICODE
	i3::rc_string dlgName;	i3::utf16_to_mb(dlg.getNameString(), dlgName);
#else
	i3::rc_string dlgName = dlg.getNameString();
#endif
	pInfo->SetName( dlgName );

	Invalidate();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pInfo);
}

void i3TDKTimeEventCtrl::OnNewEvent(void)
{
	i3TDKDlgSelectAction dlg;
	REAL32 tm;

	if( m_pTimeEvent == nullptr)
		return;

	{
		CPoint pos;
		CRect rect;

		GetCursorPos( &pos);

		ScreenToClient( &pos);

		_GetRulerRect( &rect);
		tm = _pix2tm( pos.x - rect.left);
		tm = MAX( tm, 0.0f);
	}

	HINSTANCE hOld = AfxGetResourceHandle();
	AfxSetResourceHandle( g_hInstTDK);
	
	if( dlg.DoModal() == IDCANCEL)
	{
		AfxSetResourceHandle( hOld);
		return;
	}

	AfxSetResourceHandle( hOld);

	{
		i3TimeEventInfo * pInfo = i3TimeEventInfo::new_object_ref();
		i3Action * pAction;

		{
			i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(dlg.getSelectedActionClassName() );
				// i3ClassMeta::FindClassMetaByName( dlg.getSelectedActionClassName());
			I3ASSERT( pMeta != nullptr);

			pAction = (i3Action *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
		}

		pInfo->setStartTime( tm);
		pInfo->setAction( pAction);

		{
			char szName[256];

			pAction->GetInfoString( szName, true, sizeof(szName) - 1);
			pInfo->SetName( szName);
		}

		m_pTimeEvent->AddEvent( pInfo);

		m_MaxDuration = m_pTimeEvent->getDuration();

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pInfo);
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeEvent);

		Invalidate();
	}
}

bool i3TDKTimeEventCtrl::_Rec_AnalyzeNode( i3Node * pNode, NODE_INFO * pInfo)
{
	return true;
}

void i3TDKTimeEventCtrl::_AnalyzeNode( i3Node * pNode, NODE_INFO * pInfo)
{
	pInfo->m_Type = NODE_TYPE_NORMAL;
	pInfo->m_Duration = 0.0f;

	_Rec_AnalyzeNode( pNode, pInfo);
}

void i3TDKTimeEventCtrl::_UpdateScrollInfo(void)
{
	SCROLLINFO si;

	si.cbSize	= sizeof(SCROLLINFO);
	si.fMask	=	SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin		= 0;
	
	// Vertical Scroll bar
	{
		if( m_pTimeEvent != nullptr)
			si.nMax = m_pTimeEvent->getEventCount();
		else
			si.nMax = 0;

		si.nPage = (m_ClientHeight - RULER_CY) / TRACK_CY;
		si.nPos = m_ShowStartIndex;

		SetScrollInfo( SB_VERT, &si);
	}

	// Horizontal Scroll Bar
	{
		si.nMax = (INT32) (m_MaxDuration * 500 * m_ZoomRate);
		si.nPage = (INT32) (m_ClientWidth - TRACK_CX);
		si.nPos = (INT32) (m_ShowStartTime * 500 * m_ZoomRate);

		SetScrollInfo( SB_HORZ, &si);
	}
}

void i3TDKTimeEventCtrl::EnsureVisible( REAL32 tm)
{
	CRect rect;
	INT32 x;

	_GetRulerRect( &rect);
	x = _tm2pix( tm);

	if((x < 0) || (x > rect.Width()))
	{
		// 화면에서 벗어났다..
		if( x < (m_ClientWidth >> 1))
		{
			// 왼쪽에 맞춘다.
			m_ShowStartTime = tm;
		}
		else
		{
			// 오른쪽에 맞춘다.
			m_ShowStartTime = _pix2tm( x - rect.Width());
		}

		_UpdateScrollInfo();
		Invalidate();
	}
}

BEGIN_MESSAGE_MAP(i3TDKTimeEventCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
ON_WM_MBUTTONDOWN()
ON_WM_MBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_MOUSEWHEEL()
ON_WM_TIMER()
ON_WM_LBUTTONDBLCLK()
ON_WM_MBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL i3TDKTimeEventCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	hCursor = ::LoadCursor( nullptr, IDC_ARROW);

	hbrBack = (HBRUSH) GetSysColorBrush( COLOR_3DFACE);

	lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS , hCursor, hbrBack, nullptr);
	ASSERT( lpszClass);

	dwStyle |= WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN;

	return CWnd::Create( lpszClass, nullptr, dwStyle, rect, pParentWnd, nID, nullptr);
}

// i3TDKTimeEventCtrl message handlers


BOOL i3TDKTimeEventCtrl::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

struct LOAD_BMP_INFO
{
	UINT32		m_ID;
	INT32		m_Width;
	INT32		m_Height;
} ;

static LOAD_BMP_INFO s_BMPTable[] =
{
	{	IDB_TIMESEQ_TRACK_LEFT,				15,	15	},	// TSQBMP_TRACK_NORMAL_LEFT,
	{	IDB_TIMESEQ_TRACK_BODY,				15,	15	},	// TSQBMP_TRACK_NORMAL_BODY,
	{	IDB_TIMESEQ_TRACK_RIGHT,			6,	15	},	// TSQBMP_TRACK_NORMAL_RIGHT,

	{	IDB_TIMESEQ_TRACK_SELECT_LEFT,		15,	15	},	// TSQBMP_TRACK_SELECT_LEFT,
	{	IDB_TIMESEQ_TRACK_SELECT_BODY,		15,	15	},	// TSQBMP_TRACK_SELECT_BODY,
	{	IDB_TIMESEQ_TRACK_SELECT_RIGHT,		6,	15	},	// TSQBMP_TRACK_SELECT_RIGHT,

	{	IDB_TIMESEQ_TRACK_DISABLE_LEFT,		15,	15	},	// TSQBMP_TRACK_DISABLE_LEFT,
	{	IDB_TIMESEQ_TRACK_DISABLE_BODY,		15,	15	},	// TSQBMP_TRACK_DISABLE_BODY,
	{	IDB_TIMESEQ_TRACK_DISABLE_RIGHT,	6,	15	},	// TSQBMP_TRACK_DISABLE_RIGHT,

	{	IDB_TIMEEVENT_SLIDE_NORMAL,			5,	15	},	// TSQBMP_SLIDE_NORMAL_LEFT,
	{	IDB_TIMESEQ_SLIDE_NORMAL_BODY,		2,	15	},	// TSQBMP_SLIDE_NORMAL_BODY,
	{	IDB_TIMESEQ_SLIDE_NORMAL_RIGHT,		5,	15	},	// TSQBMP_SLIDE_NORMAL_RIGHT,

	{	IDB_TIMEEVENT_SLIDE_SELECT,			5,	15	},	// TSQBMP_SLIDE_SELECT_LEFT,
	{	IDB_TIMESEQ_SLIDE_SELECT_BODY,		2,	15	},	// TSQBMP_SLIDE_SELECT_BODY,
	{	IDB_TIMESEQ_SLIDE_SELECT_RIGHT,		5,	15	},	// TSQBMP_SLIDE_SELECT_RIGHT,

	{	IDB_TIMEEVENT_SLIDE_DISABLE,		5,	15	},	// TSQBMP_SLIDE_DISABLE_LEFT,
	{	IDB_TIMESEQ_SLIDE_DISABLE_BODY,		2,	15	},	// TSQBMP_SLIDE_DISABLE_BODY,
	{	IDB_TIMESEQ_SLIDE_DISABLE_RIGHT,	5,	15	},	// TSQBMP_SLIDE_DISABLE_RIGHT,

	{	IDB_TIMESEQ_GUAGE,					9,	17	},	// TSQBMP_QUAGE,
	{	IDB_TIMESEQ_SLIDE_BACK,				32, 15	},	// TSQBMP_SLIDE_BACK,

	{	0,	0,	0	}
};

int i3TDKTimeEventCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rt;

		GetClientRect( &rt);
	}

	{
		HDC dc;

		dc = ::GetDC( m_hWnd);

		m_hdcDraw = CreateCompatibleDC( dc);

		m_hdcTemp = CreateCompatibleDC( dc);

		::ReleaseDC( m_hWnd, dc);
	}

	{
		m_hFont = i3TDK::CreateDefFont( 12, _T("Tahoma") );
	}

	{
		m_hpenBorder = ::CreatePen( PS_SOLID, 1, RGB( 46, 46, 46));
	}

	{
		m_hbrBack = ::CreateSolidBrush( RGB( 68, 68, 68));
	}

	{
		m_hbmpRulerBack = ::LoadBitmap( g_hInstTDK, MAKEINTRESOURCE( IDB_TIMESEQ_RULER_BACK));
		I3ASSERT( m_hbmpRulerBack != nullptr);
	}

	{
		INT32 i;

		for( i = 0; s_BMPTable[i].m_Width > 0; i++)
		{
			m_hbmpTable[i].m_hBitmap = ::LoadBitmap( g_hInstTDK, MAKEINTRESOURCE( s_BMPTable[i].m_ID));
			m_hbmpTable[i].m_Width = s_BMPTable[i].m_Width;
			m_hbmpTable[i].m_Height = s_BMPTable[i].m_Height;
		}
	}

	{
		m_Font.CreateFont( 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma") );
	}

	{
		CRect rect;

		rect.SetRect( 0, 0, 10, 10);

		m_ED_Time.setNotifyEnterKeyPress( true);

		m_ED_Time.Create( WS_CHILD | ES_AUTOHSCROLL | ES_LEFT, rect, this, 10022);
		m_ED_Time.SetFont( &m_Font);
	}

	return 0;
}

void i3TDKTimeEventCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( m_hdcDraw == nullptr)
		return;

	if((cx <= 0) || (cy <= 0))
		return;

	m_ClientWidth = cx;
	m_ClientHeight = cy;

	if( m_hbmpDraw != nullptr)
	{
		::SelectObject( m_hdcDraw, m_hbmpOld);
		::DeleteObject( m_hbmpDraw);

		m_hbmpDraw = nullptr;
	}

	if( m_hRegion != nullptr)
	{
		::DeleteObject( m_hRegion);
		m_hRegion = nullptr;
	}

	if( m_hRulerRgn != nullptr)
	{
		::DeleteObject( m_hRulerRgn);
		m_hRulerRgn = nullptr;
	}

	m_hbmpDraw = CreateBitmap( cx, cy, GetDeviceCaps( m_hdcDraw, PLANES), GetDeviceCaps( m_hdcDraw, BITSPIXEL), nullptr);
	I3ASSERT( m_hbmpDraw != nullptr);

	{
		CRect rt;

		_GetRulerRect( &rt);

		m_hRulerRgn = CreateRectRgn( rt.left, rt.top, rt.right, rt.bottom);
		I3ASSERT( m_hRulerRgn != nullptr);

		rt.left += 1;
		rt.right -= 1;
		rt.top = rt.bottom;
		rt.bottom = m_ClientHeight - 1;

		m_hRegion = CreateRectRgn( rt.left, rt.top, rt.right, rt.bottom);
		I3ASSERT( m_hRegion != nullptr);
	}

	m_hbmpOld = (HBITMAP) ::SelectObject( m_hdcDraw, m_hbmpDraw);
	

	_UpdateScrollInfo();
}

void i3TDKTimeEventCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	_Draw();

	::BitBlt( dc.m_hDC, 0, 0, m_ClientWidth, m_ClientHeight, m_hdcDraw, 0, 0, SRCCOPY);
}

void i3TDKTimeEventCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu, * pPopup;
	CPoint pt, cpt;
	UINT32 cmd;
	INT32 selIndex;
	NC_HIT hit;

	SetFocus();
	GetCursorPos( &pt);

	cpt = pt;
	ScreenToClient( &cpt);

	{
		selIndex = FindItem( cpt, &hit);
	}

	{
		HINSTANCE hOld = AfxGetResourceHandle();

		AfxSetResourceHandle( g_hInstTDK);

		menu.LoadMenu( IDR_POPUP_TIMEEVENT);

		AfxSetResourceHandle( hOld);
	}


	pPopup = menu.GetSubMenu( 0);

	OnPopupMenu( pPopup, hit, selIndex);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, nullptr);

	switch( cmd)
	{
		case ID_TIMEEVENT_SELECTALL :			OnSelectAll();	break;
		case ID_TIMEEVENT_UNSELECTALL :			OnUnselectAll();	break;
		case ID_TIMEEVENT_DELETE :				OnDelete();		break;
		case ID_TIMEEVENT_RENAME :				OnRename( selIndex);	break;
		case ID_TIMEEVENT_NEWEVENT :			OnNewEvent();	break;
		case ID_TIMEEVENT_TIMEEVENTSETTINGS :	OnTimeEventSetting();	break;
	}

	OnMenuSelected( cmd, hit, &cpt, selIndex);

	CWnd::OnRButtonDown(nFlags, point);
}

void i3TDKTimeEventCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect Rect, ClientRect;
	SCROLLINFO si;
	bool bFlag = false;
	INT32 iPos = (INT32) nPos;

	si.cbSize = sizeof(si);
	GetScrollInfo( SB_VERT, &si);

	switch( nSBCode)
	{
		case SB_TOP :
			iPos = 0;
			bFlag = true;
			break;

		case SB_BOTTOM :
			iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_ENDSCROLL :
			break;

		case SB_LINEDOWN :
			iPos = si.nPos + 1;
			if( (iPos + si.nPage) > (UINT)si.nMax)
				iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_LINEUP :
			iPos --;
			if( iPos < 0)
				iPos = 0;
			bFlag = true;
			break;

		case SB_PAGEDOWN :
			iPos += si.nPage;
			if( (iPos + si.nPage) > (UINT)si.nMax)
				iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_PAGEUP :
			iPos -= si.nPage;
			if( iPos < 0)
				iPos = 0;
			bFlag = true;
			break;

		case SB_THUMBPOSITION :
			bFlag = true;
			break;

		case SB_THUMBTRACK :
			bFlag = true;
			break;
	}

	if( bFlag)
	{
		m_ShowStartIndex = iPos;

		_UpdateScrollInfo();
		Invalidate();
	}

	CWnd::OnVScroll(nSBCode, iPos, pScrollBar);
}

void i3TDKTimeEventCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect Rect, ClientRect;
	SCROLLINFO si;
	bool bFlag = false;
	INT32 iPos = (INT32) nPos;

	si.cbSize = sizeof(si);
	GetScrollInfo( SB_HORZ, &si);

	switch( nSBCode)
	{
		case SB_TOP :
			iPos = 0;
			bFlag = true;
			break;

		case SB_BOTTOM :
			iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_ENDSCROLL :
			break;

		case SB_LINEDOWN :
			iPos = si.nPos + 1;
			if( (iPos + si.nPage) > (UINT)si.nMax)
				iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_LINEUP :
			iPos --;
			if( iPos < 0)
				iPos = 0;
			bFlag = true;
			break;

		case SB_PAGEDOWN :
			iPos += si.nPage;
			if( (iPos + si.nPage) > (UINT)si.nMax)
				iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_PAGEUP :
			iPos -= si.nPage;
			if( iPos < 0)
				iPos = 0;
			bFlag = true;
			break;

		case SB_THUMBPOSITION :
			bFlag = true;
			break;

		case SB_THUMBTRACK :
			bFlag = true;
			break;
	}

	if( bFlag)
	{
		m_ShowStartTime = (iPos * 0.002f) / m_ZoomRate;

		_UpdateScrollInfo();
		Invalidate();
	}

	CWnd::OnHScroll(nSBCode, iPos, pScrollBar);
}

static char s_HitName[][64] =
{
	"Ruler",
	"Guage",
	"Track",
	"Track (Enable)",
	"Slide (Head)",
	"Slide (Body)",
	"Slide (Tail)",
	"Slide (Empty)",
};

void i3TDKTimeEventCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	INT32 idx;
	NC_HIT	hit;

	m_DragStartPt = point;
	::SetCapture( m_hWnd);

	SetFocus();

	idx = FindItem( point, &hit);

	/*
	if( hit != NC_HIT_NONE)
	{
		I3TRACE( "%s\n", s_HitName[ hit]);
	}
	else
	{
		I3TRACE( "(NONE)\n");
	}
	*/
	m_CurrentNcHit = hit;
	switch( hit)
	{
		case NC_HIT_RULER :			// Guage의 이동
			{
				RECT rt;
				_GetTrackRect(0, &rt);

				INT32 nDeltaPosX = point.x - (rt.right - rt.left);

				m_timeGuage = _pix2tm(nDeltaPosX);
				Invalidate();
			}
			break;

		case NC_HIT_GUAGE :			// Guage의 Dragging
			{
				m_MouseMode = MOUSE_MODE_GUAGE_MOVE;

				m_SavedStartTime = m_timeGuage;
			}
			break;

		case NC_HIT_TRACK :			// 단순 Selection
			{
				I3ASSERT( idx != -1);

				if( !(nFlags & MK_CONTROL))
					OnUnselectAll();

				OnSelect( idx);

				m_MouseMode = MOUSE_MODE_TRACK_DRAG;
				m_DragStartIndex = idx;
			}
			break;

		case NC_HIT_SLIDE_EMPTY :
			{
				I3ASSERT( idx != -1);

				if( !(nFlags & MK_CONTROL))
					OnUnselectAll();

				OnSelect( idx);
			}
			break;

		case NC_HIT_TRACK_ENABLE :
			{
				I3ASSERT( idx != -1);
				i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( idx);

				if( pInfo->getState() & I3_TIMEEVENT_STATE_DISABLE)
				{
					pInfo->removeState( I3_TIMEEVENT_STATE_DISABLE);
				}
				else
				{
					pInfo->addState( I3_TIMEEVENT_STATE_DISABLE);
				}

				OnChangeItemEnable( pInfo);

				Invalidate();
			}
			break;

		case NC_HIT_SLIDE_HEAD :
		case NC_HIT_SLIDE_BODY :
		case NC_HIT_SLIDE_TAIL :
			{
				m_MouseMode = MOUSE_MODE_SLIDE_MOVE;

				I3ASSERT( idx != -1);
				OnUnselectAll();
				OnSelect( idx);

				{
					i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( idx);

					m_SavedStartTime = pInfo->getStartTime();
					m_SavedDurationTime = CalcDuration();
					m_SavedStartIndex = idx;
				}
			}
			break;

			/*
		case NC_HIT_SLIDE_TAIL :
			{
				m_MouseMode = MOUSE_MODE_SLIDE_RESIZE;

				I3ASSERT( idx != -1);
				OnUnselectAll();
				OnSelect( idx);
			}
			break;*/

		default :
			OnUnselectAll();
			break;
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKTimeEventCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	::ReleaseCapture();

//	if( m_MouseMode == MOUSE_MODE_SLIDE_MOVE)
//	{
//		m_pTimeEvent->SortEvents();					// 이 함수를 제거..(2012.05.21.수빈 내부에서 BinList로 관리되던것인데..정렬이유가 없음)
//	}

	m_MouseMode = MOUSE_MODE_NA;

	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKTimeEventCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_DragStartPt = point;
	::SetCapture( m_hWnd);

	SetFocus();

	m_SavedStartTime = m_ShowStartTime;
	m_SavedStartIndex = m_ShowStartIndex;
	m_MouseMode = MOUSE_MODE_SCROLL;

	CWnd::OnMButtonDown(nFlags, point);
}

void i3TDKTimeEventCtrl::OnMButtonUp(UINT nFlags, CPoint point)
{
	::ReleaseCapture();
	m_MouseMode = MOUSE_MODE_NA;

	CWnd::OnMButtonUp(nFlags, point);
}

void i3TDKTimeEventCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	switch( m_MouseMode)
	{
		case MOUSE_MODE_NA :
			break;

		case MOUSE_MODE_SCROLL :
			{
				REAL32 dt = _pix2tm_sz( m_DragStartPt.x - point.x);

				m_ShowStartTime = m_SavedStartTime + dt;
				if( m_ShowStartTime < 0.0f)
					m_ShowStartTime = 0.0f;

				if( m_ShowStartTime > m_MaxDuration)
					m_ShowStartTime = m_MaxDuration;

				Invalidate();
			}
			break;

		case MOUSE_MODE_TRACK_DRAG :
			{
				// 현재의 Mouse cursor position으로 삽입될 위치를 확인한다.
				/*
				INT32 idx;
				
				idx = FindInsertPos( point);

				if( idx != m_DragStartIndex)
				{
					// 새 위치에 삽입해 준다.
					i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( m_DragStartIndex);

					I3TRACE( "%d, %d\n", idx, m_DragStartIndex);

					pInfo->AddRef();

					m_pTimeEvent->RemoveSequence( pInfo);

					if( idx > m_DragStartIndex)
						idx--;

					m_DragStartIndex = m_pTimeEvent->InsertSequence( idx, pInfo);

					pInfo->Release();

					Invalidate();
				}
				*/
			}
			break;

		case MOUSE_MODE_SLIDE_MOVE :
			{
				REAL32 dt = _pix2tm_sz( m_DragStartPt.x - point.x);
				i3TimeEventInfo * pInfo;
				REAL32 tm = 0.0f, duration;

				pInfo = m_pTimeEvent->getEvent( m_SavedStartIndex);

				if (m_bUseSlideCustomSize)
				{
					if (m_CurrentNcHit == NC_HIT_SLIDE_HEAD)
					{
						if (m_SavedDurationTime + dt > 0.0f)
							pInfo->setDuration(m_SavedDurationTime + dt);
						else
							break;
					}
					else if (m_CurrentNcHit == NC_HIT_SLIDE_TAIL)
					{
						if (m_SavedDurationTime - dt > 0.0f)
							pInfo->setDuration(m_SavedDurationTime - dt);
						else
							break;
					}

					if (m_CurrentNcHit != NC_HIT_SLIDE_TAIL)
					{
						tm = m_SavedStartTime - dt;
						if (tm < 0.0f)
							tm = 0.0f;

						pInfo->setStartTime(tm);
					}
				}
				else
				{
					tm = m_SavedStartTime - dt;
					if (tm < 0.0f)
						tm = 0.0f;

					pInfo->setStartTime(tm);
				}

				duration = CalcDuration();
				if( m_MaxDuration != duration)
				{
					m_MaxDuration = duration;
					m_pTimeEvent->setDuration( m_MaxDuration);

					_UpdateScrollInfo();
				}

				EnsureVisible( tm);

				Invalidate();

				_UpdateTimeEdit( m_SavedStartIndex);
			}
			break;

		case MOUSE_MODE_SLIDE_RESIZE :
			break;

		case MOUSE_MODE_GUAGE_MOVE :
			{
				REAL32 dt = _pix2tm_sz( m_DragStartPt.x - point.x);

				m_timeGuage = m_SavedStartTime - dt;

				if( m_timeGuage < 0.0f)
					m_timeGuage = 0.0f;

				if( m_timeGuage > m_MaxDuration)
					m_timeGuage = m_MaxDuration;

				OnChangeCurrentTime( m_timeGuage);

				EnsureVisible( m_timeGuage);

				Invalidate();
			}
			break;
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL i3TDKTimeEventCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( nFlags & MK_CONTROL)
	{
		// Zoom
		m_ZoomRate += ((REAL32) zDelta * 0.001f);

		Invalidate();
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void i3TDKTimeEventCtrl::setAutoRefreshEnable( bool bFlag)
{
	m_bAutoRefresh = bFlag;

	if( m_bAutoRefresh)
	{
		::SetTimer( m_hWnd, 100, 16, nullptr);
	}
	else
	{
		::KillTimer( m_hWnd, 100);
	}
}



void i3TDKTimeEventCtrl::OnTimer(UINT nIDEvent)
{
	if( m_pTimeEvent != nullptr)
	{
		m_timeGuage = m_pTimeEvent->getLocalTime();

		CRect rect;

		_GetRulerRect( &rect);

		InvalidateRect( &rect, FALSE);
	}
}

void i3TDKTimeEventCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	INT32 idx;
	NC_HIT	hit;

	idx = FindItem( point, &hit);

	if( idx != -1)
	{
		i3TimeEventInfo * pInfo;

		pInfo = m_pTimeEvent->getEvent( idx);

		if( pInfo->getAction() != nullptr)
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_POPUP_SPEC, pInfo->getAction());
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void i3TDKTimeEventCtrl::_SelectItem( INT32 idx)
{
	i3TimeEventInfo * pInfo;

	m_idxSelected = idx;

	if( idx != -1)
	{
		pInfo = m_pTimeEvent->getEvent( idx);

		pInfo->addState( I3_TIMEEVENT_STATE_SELECTED);

		_UpdateTimeEdit( idx);	
	}
}

void i3TDKTimeEventCtrl::_UpdateTimeEdit( INT32 idx)
{
	TCHAR conv[128];
	INT32 x, y;
	CRect hrt, brt, trt;
	i3TimeEventInfo * pInfo;

	pInfo = m_pTimeEvent->getEvent( idx);

	i3::ftoa( pInfo->getStartTime(), conv);

	_GetSlideRect( idx, &hrt, &brt, &trt);

	x = hrt.left;
	y = hrt.bottom;

	{
		CRect crt;

		GetClientRect( &crt);

		if( (x + 32) > crt.right)
			x = crt.right - 32 - 2;

		if( (y + 15) > crt.bottom)
			y = hrt.top - 17;
	}

	m_ED_Time.SetWindowText( conv);
	m_ED_Time.SetWindowPos( nullptr, x, y, 32, 15, SWP_NOACTIVATE | SWP_NOZORDER);
	m_ED_Time.ShowWindow( SW_SHOW);
}


LRESULT i3TDKTimeEventCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_NOTIFY_ENTER)
	{
		OnEndEditTime();
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void i3TDKTimeEventCtrl::OnEndEditTime(void)
{
	TCHAR conv[256];
	REAL32 tm, duration;

	if( m_idxSelected == -1)
		return;

	i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( m_idxSelected);

	m_ED_Time.GetWindowText( conv, _countof(conv));

	tm = (REAL32) i3::atof( conv);

	if( tm < 0.0f)
		tm = 0.0f;

	pInfo->setStartTime( tm);

	duration = CalcDuration();
	if( duration != m_MaxDuration)
	{
		m_MaxDuration = duration;
		m_pTimeEvent->setDuration( m_MaxDuration);

		_UpdateScrollInfo();
	}

	EnsureVisible( pInfo->getStartTime());

	_UpdateTimeEdit( m_idxSelected);

	Invalidate();
}

void i3TDKTimeEventCtrl::OnTimeEventSetting(void)
{
	i3TDKDlgTimeEventSetting dlg;

	dlg.Execute( m_pTimeEvent);

	Invalidate();
}

REAL32 i3TDKTimeEventCtrl::CalcDuration(void)
{
	UINT32 i;
	REAL32 duration = 0.0f;

	for( i = 0; i < m_pTimeEvent->getEventCount(); i++)
	{
		i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( i);

		duration = MAX( duration, pInfo->getStartTime() + pInfo->getDuration());
	}

	return duration;
}
