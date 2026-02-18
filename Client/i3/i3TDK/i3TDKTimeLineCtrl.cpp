#include "stdafx.h"
#include "i3TDKTimeLineCtrl.h"

#include "i3TDKUtilGDI.h"
#include "i3TDKGlobalVariable.h"
#include "resource.h"
#include "i3TDKUpdateSystem.h"
#include "i3TDKClipboard.h"
#include "i3TDKDlgRename.h"

#define		RULER_CY			19
#define		ITEM_CY				16
#define		TRACK_CX			100
#define		TRACK_CY			17
#define		GUAGE_CX			9
#define		ONERULERINTERVAL	5

#define		TRACK_SY			(1 + RULER_CY)

#define ROUNDING(x, dig)    ( floor((x) * pow(float(10), dig) + 0.5f) / pow(float(10), dig) )

IMPLEMENT_DYNAMIC(i3TDKTimeLineCtrl, CWnd)
BEGIN_MESSAGE_MAP(i3TDKTimeLineCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

i3TDKTimeLineCtrl::~i3TDKTimeLineCtrl()
{
	if (m_hdcDraw != nullptr)
	{
		DeleteDC(m_hdcDraw);
		m_hdcDraw = nullptr;
	}

	if (m_hdcTemp != nullptr)
	{
		::DeleteDC(m_hdcTemp);
		m_hdcTemp = nullptr;
	}

	if (m_hpenBorder != nullptr)
	{
		DeleteObject(m_hpenBorder);
		m_hpenBorder = nullptr;
	}

	if (m_hbrBack != nullptr)
	{
		::DeleteObject(m_hbrBack);
		m_hbrBack = nullptr;
	}

	if (m_hFont != nullptr)
	{
		::DeleteObject(m_hFont);
		m_hFont = nullptr;
	}

	if (m_hbmpDraw != nullptr)
	{
		::DeleteObject(m_hbmpDraw);
		m_hbmpDraw = nullptr;
	}

	if (m_hRegion != nullptr)
	{
		::DeleteObject(m_hRegion);
		m_hRegion = nullptr;
	}

	if (m_hRulerRgn != nullptr)
	{
		::DeleteObject(m_hRulerRgn);
		m_hRulerRgn = nullptr;
	}

	I3_SAFE_RELEASE(m_pTimeSeq);
}

void	i3TDKTimeLineCtrl::SetTimeSequence(i3TimeSequence * pTimeSeq)
{
	INT32 i;
	i3TimeSequenceInfo * pInfo;

	I3_REF_CHANGE(m_pTimeSeq, pTimeSeq);

	m_ShowStartTime = 0.0f;
	m_ShowStartIndex = 0;
	m_MaxDuration = 0.0f;
	m_timeGuage = 0.0f;

	if (pTimeSeq != nullptr)
	{
		for (i = 0; i < m_pTimeSeq->GetSequenceCount(); i++)
		{
			pInfo = m_pTimeSeq->GetSequence(i);

			UpdateSequence(pInfo);
		}
	}

	OnChangeTimeSequence();

	_UpdateScrollInfo();

	Invalidate();
}

void i3TDKTimeLineCtrl::UpdateSequence(i3TimeSequenceInfo * pInfo)
{
	NODE_INFO nodeInfo;

	if (pInfo->GetNode() != nullptr)
	{
		_AnalyzeNode(pInfo->GetNode(), &nodeInfo);

		pInfo->SetState(pInfo->GetState() | ((nodeInfo.m_Type << 16) & 0xFFFF0000));
		pInfo->SetDuration(nodeInfo.m_Duration);

		m_MaxDuration = MAX(m_MaxDuration, pInfo->GetStartTime() + pInfo->GetDuration());
	}
}

void i3TDKTimeLineCtrl::AddSequence(i3TimeSequenceInfo * pInfo)
{
	if (m_pTimeSeq == nullptr)
		return;

	m_pTimeSeq->AddSequence(pInfo);

	UpdateSequence(pInfo);

	_UpdateScrollInfo();

	Invalidate();
}

void	i3TDKTimeLineCtrl::_GetRulerRect(RECT * pRect)
{
	pRect->left = TRACK_CX;
	pRect->top = 1;
	pRect->right = m_ClientWidth;
	pRect->bottom = pRect->top + RULER_CY;
}

void	i3TDKTimeLineCtrl::_GetGuageRect(RECT * pRect)
{
	INT32 px;

	px = _tm2pix(m_timeGuage);

	pRect->left = TRACK_CX + px - (GUAGE_CX >> 1);
	pRect->top = 2;
	pRect->right = TRACK_CX + px + (GUAGE_CX >> 1);
	pRect->bottom = pRect->top + RULER_CY;
}

void	i3TDKTimeLineCtrl::_GetTrackRect(INT32 idx, RECT * pRect)
{
	idx -= m_ShowStartIndex;

	pRect->left = 1;
	pRect->top = TRACK_SY + (idx * TRACK_CY);
	pRect->right = TRACK_CX - 1;
	pRect->bottom = pRect->top + TRACK_CY;
}

void	i3TDKTimeLineCtrl::_GetTrackEnableRect(INT32 idx, RECT * pRect)
{
	_GetTrackRect(idx, pRect);

	pRect->left += 4;
	pRect->top += 4;
	pRect->right = pRect->left + 7;
	pRect->bottom = pRect->top + 7;
}

void	i3TDKTimeLineCtrl::_GetSlideRect(INT32 idx, RECT * pHeadRect, RECT * pBodyRect, RECT * pTailRect)
{
	INT32 startPix, endPix;
	i3TimeSequenceInfo * pInfo = m_pTimeSeq->GetSequence(idx);

	idx -= m_ShowStartIndex;

	startPix = _tm2pix(pInfo->GetStartTime());

// 	if (pInfo->GetDuration() == 0.0f)
// 	{
// 		// Infinite
// 		endPix = m_ClientWidth + 10;
// 	}
// 	else
// 	{
		endPix = _tm2pix(pInfo->GetStartTime() + pInfo->GetDuration());
	//}

	pHeadRect->left = startPix + TRACK_CX;
	pHeadRect->top = TRACK_SY + (idx * TRACK_CY);
	pHeadRect->right = pHeadRect->left + ONERULERINTERVAL;
	pHeadRect->bottom = pHeadRect->top + TRACK_CY;

	pBodyRect->left = pHeadRect->right;
	pBodyRect->top = pHeadRect->top;
	pBodyRect->right = endPix - ONERULERINTERVAL + TRACK_CX;
	pBodyRect->bottom = pHeadRect->bottom;

	pTailRect->left = pBodyRect->right;
	pTailRect->top = pBodyRect->top;
	pTailRect->right = endPix + TRACK_CX;
	pTailRect->bottom = pBodyRect->bottom;
}

void	i3TDKTimeLineCtrl::_GetSlideRect(INT32 idx, RECT * pRect)
{
	idx -= m_ShowStartIndex;

	pRect->left = TRACK_CX;
	pRect->top = TRACK_SY + (idx * TRACK_CY);
	pRect->right = m_ClientWidth;
	pRect->bottom = pRect->top + TRACK_CY;
}

void	i3TDKTimeLineCtrl::_DrawRuler(void)
{
	const int	_bmpCX = 24, _bmpCY = 17;
	INT32	i, count, _x, x, y;
	HBITMAP hbmpOld;
	TCHAR conv[256];

	{
		HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0));
		CRect rt;

		rt.SetRect(0, 0, m_ClientWidth + 1, m_ClientHeight + 1);

		::FillRect(m_hdcDraw, &rt, hbr);

		::DeleteObject(hbr);
	}

	// Border
	{
		::SelectObject(m_hdcDraw, g_pTDKGlobalRes->getPen(I3TDK_COLOR_FORE_3D_SHADOW));
		::MoveToEx(m_hdcDraw, m_ClientWidth, 0, nullptr);
		::LineTo(m_hdcDraw, 0, 0);
		::LineTo(m_hdcDraw, 0, m_ClientHeight);

		::SelectObject(m_hdcDraw, g_pTDKGlobalRes->getPen(I3TDK_COLOR_FORE_3D_LIT));
		::MoveToEx(m_hdcDraw, m_ClientWidth - 1, 1, nullptr);
		::LineTo(m_hdcDraw, m_ClientWidth - 1, m_ClientHeight - 1);
		::LineTo(m_hdcDraw, 0, m_ClientHeight - 1);
	}

	// 바닥
	{
		::SelectObject(m_hdcDraw, m_hpenBorder);
		::Rectangle(m_hdcDraw, 1, 1, m_ClientWidth - 1, 1 + RULER_CY);

		hbmpOld = (HBITMAP) ::SelectObject(m_hdcTemp, m_hbmpRulerBack);

		count = (m_ClientWidth - 2) / _bmpCX;
		_x = 2;

		// 배경 이미지를 바른다.
		for (i = 0; i < count; i++)
		{
			::BitBlt(m_hdcDraw, _x, 2, _bmpCX, _bmpCY, m_hdcTemp, 0, 0, SRCCOPY);
			_x += _bmpCX;
		}

		// 마지막 끝부분
		::BitBlt(m_hdcDraw, (m_ClientWidth - 1) - _bmpCX, 2, _bmpCX, _bmpCY, m_hdcTemp, 0, 0, SRCCOPY);

		::SelectObject(m_hdcTemp, hbmpOld);
	}

	// Time Line
	{
		CRect rt;

		::SetTextColor(m_hdcDraw, RGB(81, 81, 81));
		::SetBkMode(m_hdcDraw, TRANSPARENT);
		::SelectObject(m_hdcDraw, m_hFont);

		rt.SetRect(6, 3, 47, 15);
		::DrawText(m_hdcDraw, m_chTitle, -1, &rt, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
	}

	// 시간 단위 간격을 구한다.
	// Zoom 100%일 때, 2 ms가 1 Pixel에 해당한다. 즉, 1초는 500 Pixel 길이로 표현된다.
	REAL32	timeStep = 0.1f, tm, tmLength, accm;
	CRect rc, rc2;
	INT32 len;
	SIZE Sz;

	::SelectClipRgn(m_hdcDraw, m_hRulerRgn);

	rc.SetRect(100, 11, 100 + _tm2pix(m_MaxDuration), 19);

	::SelectObject(m_hdcDraw, GetStockObject(WHITE_BRUSH));
	::SelectObject(m_hdcDraw, GetStockObject(BLACK_PEN));
	::Rectangle(m_hdcDraw, rc.left, rc.top, rc.right, rc.bottom);

	rc.SetRect(100, 11, m_ClientWidth, 19);

	timeStep = 0.01f ;
	tmLength = _pix2tm(rc.Width());

	{
		INT32 div;

		div = (INT32)(m_ShowStartTime / timeStep);
		tm = timeStep * div;
	}

	y = RULER_CY - 1;

	SetTextColor(m_hdcDraw, RGB(51, 54, 51));

	REAL32 t = 0.f;
	REAL32 tLen = tmLength - tm;

	//for( ; tm < tmLength; )
	for (; t < tLen; )
	{
		x = rc.left + _tm2pix(tm);

		MoveToEx(m_hdcDraw, x, y, nullptr);


		{
			INT32 div = (INT32)(tm / 0.1f);
			accm = ((div + 1) * 0.1f);
			accm = i3Math::abs(accm - tm);
		}

		if (i3Math::IsSame(accm, timeStep) )
		{
			LineTo(m_hdcDraw, x, y - 6);

			i3::snprintf(conv, _countof(conv), _T("%.1f"), tm);

			len = i3::generic_string_size(conv);

			::GetTextExtentPoint32(m_hdcDraw, conv, len, &Sz);
			::TextOut(m_hdcDraw, x - (Sz.cx >> 1), 0, conv, len);
		}
		else
		{
			LineTo(m_hdcDraw, x, y - 4);
			accm += timeStep;
		}

		t += timeStep;
		tm += timeStep;
	}

	// Guage
	{
		_GetGuageRect(&rc);

		i3TDK::DrawButton(m_hdcDraw, &rc, m_hdcTemp, nullptr, &m_hbmpTable[TSQBMP_QUAGE], nullptr);
	}

	::SelectClipRgn(m_hdcDraw, nullptr);
}

void	i3TDKTimeLineCtrl::_DrawItem(RECT * pRect, i3TimeSequenceInfo * pInfo)
{
	CRect rt;
	INT32 idx;

	// Track Name
	{
		rt = *pRect;

		idx = TSQBMP_TRACK_NORMAL_LEFT;
		if (pInfo->GetState() & I3SG_TIMESEQ_STATE_SELECTED)
			idx = TSQBMP_TRACK_SELECT_LEFT;
		if (pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_DISABLE)
			idx = TSQBMP_TRACK_DISABLE_LEFT;

		rt.right = rt.left + TRACK_CX;
		rt.top += 1;
		rt.bottom -= 1;
		i3TDK::DrawButton(m_hdcDraw, &rt, m_hdcTemp, &m_hbmpTable[idx], &m_hbmpTable[idx + 1], &m_hbmpTable[idx + 2]);

		// Icon
		{
			switch ((pInfo->GetState() >> 16) & 0x0000FFFF)
			{
			case NODE_TYPE_SOUND:		idx = I3TDK_IMAGELIST_SOUND;	break;
				//case NODE_TYPE_PARTICLE :	idx = 
			default:					idx = I3TDK_IMAGELIST_GEOMETRY;	break;
			}

			if (pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_DISABLE)
				idx++;

			CImageList * pImageList = g_pTDKGlobalRes->m_pTDKImageList;

			{
				CDC * pDC;
				POINT pt;

				pDC = CDC::FromHandle(m_hdcDraw);

				pt.x = rt.left + 15;
				pt.y = rt.top + 0;

				pImageList->Draw(pDC, idx, pt, ILD_TRANSPARENT);
			}
		}

		// Name
		{
			i3Node * pNode = pInfo->GetNode();
			INT32 len;

			const TCHAR* pszName;
#ifdef _UNICODE
			i3::stack_wstring wstrName;
#endif

			if (pNode->hasName())
			{
#ifdef _UNICODE
				i3::mb_to_utf16(pNode->GetName(), wstrName);
				pszName = wstrName.c_str();
#else
				pszName = pNode->GetName();
#endif
			}
			else
			{
				pszName = _T("Nonamed");
			}

			len = i3::generic_string_size(pszName);

			rt.left += 33;
			rt.right -= 1;

			if (pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_DISABLE)
				SetTextColor(m_hdcDraw, RGB(150, 150, 150));
			else
				SetTextColor(m_hdcDraw, RGB(220, 223, 220));

			::DrawText(m_hdcDraw, pszName, len, &rt, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		}
	}

	// Track
	{
		rt = *pRect;

		rt.left += TRACK_CX;
		rt.top += 1;
		rt.bottom -= 1;
		i3TDK::DrawButton(m_hdcDraw, &rt, m_hdcTemp, nullptr, &m_hbmpTable[TSQBMP_SLIDE_BACK], nullptr);
	}

	// Slide
	{
		rt.left = (pRect->left + TRACK_CX) + _tm2pix(pInfo->GetStartTime());
		rt.right = rt.left + _tm2pix_sz(pInfo->GetDuration());
		rt.top = pRect->top;
		rt.bottom = pRect->bottom;

// 		if (pInfo->GetDuration() == 0.0f)
// 		{
// 			rt.right = m_ClientWidth + 10;
// 		}

		if (rt.right > m_ClientWidth + 10)
			rt.right = m_ClientWidth + 10;

		rt.right -= ONERULERINTERVAL * m_ZoomRate;

		idx = TSQBMP_SLIDE_NORMAL_LEFT;
		if (pInfo->GetState() & I3SG_TIMESEQ_STATE_SELECTED)
			idx = TSQBMP_SLIDE_SELECT_LEFT;
		if (pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_DISABLE)
			idx = TSQBMP_SLIDE_DISABLE_LEFT;

		SelectClipRgn(m_hdcDraw, m_hRegion);

		if (pInfo->GetDuration() > 0.0f)
		{
			i3TDK::DrawButton(m_hdcDraw, &rt, m_hdcTemp, &m_hbmpTable[idx], &m_hbmpTable[idx + 1], &m_hbmpTable[idx + 2]);
		}

		SelectClipRgn(m_hdcDraw, nullptr);
	}

	//눈금자
	{
		CRect rc;
		rc.SetRect(100, 11, m_ClientWidth, 19);
		REAL32 timeStep = 0.01f;
		REAL32 tmLength = _pix2tm(rc.Width());
		REAL32 tm;
		{
			INT32 div;
			div = (INT32)(m_ShowStartTime / timeStep);
			tm = timeStep * div;
		}
		REAL32 t = 0.f;
		REAL32 tLen = tmLength - tm;

		INT32  x, y;

		y = pRect->top + 3;
		::SelectObject(m_hdcDraw, g_pTDKGlobalRes->getPen(I3TDK_COLOR_TEXT_BLACK));

		CRect SelectRect;


		for (; t < tLen; )
		{
			x = rc.left + _tm2pix(tm);

			POINT pt;
			pt.x = rc.left + _tm2pix(tm + timeStep + m_ShowStartTime);
			pt.y = y;
			
			INT32 nRulerIndex = HitTrackIdx(m_SelectTrack, pt);

			MoveToEx(m_hdcDraw, x, y, nullptr);
			LineTo(m_hdcDraw, x, y + 11);

			SelectRect.left = x;
			SelectRect.right = x + ONERULERINTERVAL;
			SelectRect.top = y;
			SelectRect.bottom = y + 11;

			//선택된 눈금
			INT32 nNextSize = rc.left + _tm2pix(tm + timeStep);

			if (pInfo->GetState() & I3SG_TIMESEQ_STATE_SELECTED)
			{
				for (INT32 i = 0; i < (INT32)m_vecKeyFrame.size(); i++)
				{
					if (m_vecKeyFrame[i]._KeyIndex == nRulerIndex)
					{
						i3TDK::StretchBlt(m_hdcDraw, x, y, nNextSize - x, 11, m_hbmpTable[TSQBMP_TRACK_SELECT_BODY].m_hBitmap,
							0, 0, m_hbmpTable[TSQBMP_TRACK_SELECT_BODY].m_Width, m_hbmpTable[TSQBMP_TRACK_SELECT_BODY].m_Height, SRCCOPY);
					}
				}
			}

			t += timeStep;
			tm += timeStep;
		}
	}
}

void	i3TDKTimeLineCtrl::_DrawItems(void)
{
	INT32 i;
	CRect rt;

	rt.SetRect(1, RULER_CY + 1, m_ClientWidth - 1, m_ClientHeight - 1);

	::FillRect(m_hdcDraw, &rt, m_hbrBack);

	if (m_pTimeSeq != nullptr)
	{
		for (i = m_ShowStartIndex; i < m_pTimeSeq->GetSequenceCount(); i++)
		{
			i3TimeSequenceInfo * pInfo = m_pTimeSeq->GetSequence(i);

			rt.bottom = rt.top + ITEM_CY;

			// DrawItem
			_DrawItem(&rt, pInfo);

			if (rt.bottom > m_ClientHeight - 1)
				break;

			rt.top = rt.bottom + 1;
		}
	}
}

void	i3TDKTimeLineCtrl::_Draw(void)
{
	_DrawRuler();
	_DrawItems();
}

NC_HIT	i3TDKTimeLineCtrl::HitTest(POINT pt)
{
	CRect rt;
	INT32	count;


	_GetGuageRect(&rt);
	if (PtInRect(&rt, pt))
		return NC_HIT_GUAGE;

	_GetRulerRect(&rt);
	if (PtInRect(&rt, pt))
		return NC_HIT_RULER;

	if (m_pTimeSeq == nullptr)
		return NC_HIT_NONE;

	CRect rtHead, rtBody, rtTail;
	count = ((m_ClientHeight - rt.bottom) / TRACK_CY) + 1;

//  	for (INT32 i = m_ShowStartIndex; i < (m_ShowStartIndex + count); i++)
//  	{
//  		if (i >= m_pTimeSeq->GetSequenceCount())
//  			break;
//  
//  		_GetSlideRect(i, &rtHead, &rtBody, &rtTail);
//  		if (PtInRect(&rtHead, pt))
//  			return NC_HIT_SLIDE_HEAD;
//  
//  		if (PtInRect(&rtBody, pt))
//  			return NC_HIT_SLIDE_BODY;
//  
//  		if (PtInRect(&rtTail, pt))
//  			return NC_HIT_SLIDE_TAIL;
//  	}

	for (INT32 i = m_ShowStartIndex; i < (m_ShowStartIndex + count); i++)
	{
		if (i >= m_pTimeSeq->GetSequenceCount())
			break;

		_GetTrackEnableRect(i, &rt);
		if (PtInRect(&rt, pt))
			return NC_HIT_TRACK_ENABLE;

		_GetTrackRect(i, &rt);
		if (PtInRect(&rt, pt))
			return NC_HIT_TRACK;
	}

	for (INT32 i = m_ShowStartIndex; i < (m_ShowStartIndex + count); i++)
	{
		if (i >= m_pTimeSeq->GetSequenceCount())
			break;

		_GetSlideRect(i, &rt);
		if (PtInRect(&rt, pt))
			return NC_HIT_SLIDE_EMPTY;
	}

	return NC_HIT_NONE;
}

INT32 i3TDKTimeLineCtrl::FindItem(POINT pt, NC_HIT * pHitTest)
{
	NC_HIT hit;

	hit = HitTest(pt);

	if (pHitTest != nullptr)
	{
		*pHitTest = hit;
	}

	switch (hit)
	{
	case NC_HIT_TRACK_ENABLE:
	case NC_HIT_TRACK:
	case NC_HIT_SLIDE_HEAD:
	case NC_HIT_SLIDE_BODY:
	case NC_HIT_SLIDE_TAIL:
	case NC_HIT_SLIDE_EMPTY:
	{
		CRect rt;

		_GetRulerRect(&rt);

		return ((pt.y - rt.bottom) / TRACK_CY) + m_ShowStartIndex;
	}
	break;
	}

	return -1;
}

INT32 i3TDKTimeLineCtrl::FindInsertPos(POINT pt)
{
	NC_HIT hit;
	INT32 idx = -1;
	CRect rt;

	hit = HitTest(pt);

	switch (hit)
	{
	case NC_HIT_TRACK_ENABLE:
	case NC_HIT_TRACK:
	case NC_HIT_SLIDE_HEAD:
	case NC_HIT_SLIDE_BODY:
	case NC_HIT_SLIDE_TAIL:
	case NC_HIT_SLIDE_EMPTY:
	{
		_GetRulerRect(&rt);

		idx = ((pt.y - rt.bottom) / TRACK_CY) + m_ShowStartIndex;
	}
	break;
	}

	if (idx == -1)
	{
		_GetTrackRect(m_ShowStartIndex, &rt);

		if (rt.top > pt.y)
			idx = m_ShowStartIndex;
		else
		{
			INT32 cnt = m_ClientHeight / rt.Height() + 1;

			idx = MIN(m_pTimeSeq->GetSequenceCount(), m_ShowStartIndex + cnt);
		}
	}

	return idx;
}

void	i3TDKTimeLineCtrl::OnPopupMenu(CMenu * pPopup, NC_HIT hit, INT32 idx)
{
}

void	i3TDKTimeLineCtrl::OnMenuSelected(UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx)
{
}

void	i3TDKTimeLineCtrl::OnChangeCurrentTime(REAL32 tm)
{
	if (m_pTimeSeq != nullptr)
	{
		m_pTimeSeq->OnChangeTime(tm);
	}
}

void	i3TDKTimeLineCtrl::OnChangeTimeSequence(void)
{
}

void	i3TDKTimeLineCtrl::OnChangeItemEnable(i3TimeSequenceInfo * pInfo)
{
}

void	i3TDKTimeLineCtrl::OnSelectAll(void)
{
	INT32 i;
	i3TimeSequenceInfo * pInfo;

	if (m_pTimeSeq == nullptr)
		return;

	for (i = 0; i < m_pTimeSeq->GetSequenceCount(); i++)
	{
		pInfo = m_pTimeSeq->GetSequence(i);

		pInfo->AddState(I3SG_TIMESEQ_STATE_SELECTED);
	}

	i3TDK::Update(m_hWnd, I3_TDK_UPDATE_SELECT, nullptr, i3TimeSequenceInfo::static_meta());

	Invalidate();
}

void	i3TDKTimeLineCtrl::OnSelect(INT32 idx)
{
	i3TimeSequenceInfo * pInfo;

	pInfo = m_pTimeSeq->GetSequence(idx);

	pInfo->AddState(I3SG_TIMESEQ_STATE_SELECTED);

	Invalidate();
	i3TDK::Update(m_hWnd, I3_TDK_UPDATE_SELECT, pInfo);
}

void	i3TDKTimeLineCtrl::OnUnselectAll(void)
{
	INT32 i;
	i3TimeSequenceInfo * pInfo;

	if (m_pTimeSeq == nullptr)
		return;

	for (i = 0; i < m_pTimeSeq->GetSequenceCount(); i++)
	{
		pInfo = m_pTimeSeq->GetSequence(i);

		pInfo->RemoveState(I3SG_TIMESEQ_STATE_SELECTED);
	}

	i3TDK::Update(m_hWnd, I3_TDK_UPDATE_UNSELECT, nullptr, i3TimeSequenceInfo::static_meta());

	m_vecKeyFrame.clear();
	m_SelectTrack = 0;
	m_SelectCurKey = 0;

	Invalidate();
}

void	i3TDKTimeLineCtrl::OnDelete(void)
{
	INT32 i;
	i3TimeSequenceInfo * pInfo;

	if (m_pTimeSeq == nullptr)
		return;

	for (i = 0; i < m_pTimeSeq->GetSequenceCount(); i++)
	{
		pInfo = m_pTimeSeq->GetSequence(i);

		if (pInfo->GetState() & I3SG_TIMESEQ_STATE_SELECTED)
		{
			i3TDK::Update(m_hWnd, I3_TDK_UPDATE_REMOVE, pInfo);

			m_pTimeSeq->RemoveSequence(pInfo);
			i--;
		}
	}

	i3TDK::Update(m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeSeq);

	Invalidate();
}

void i3TDKTimeLineCtrl::OnCopy(void)
{
	INT32 i;
	i3TimeSequenceInfo * pInfo;
	i3TDKClipboard clip;

	if (m_pTimeSeq == nullptr)
		return;

	for (i = 0; i < m_pTimeSeq->GetSequenceCount(); i++)
	{
		pInfo = m_pTimeSeq->GetSequence(i);

		if (pInfo->GetState() & I3SG_TIMESEQ_STATE_SELECTED)
		{
			clip.Copy(m_hWnd, pInfo);
			return;
		}
	}
}

void i3TDKTimeLineCtrl::OnPaste(void)
{
	i3TDKClipboard clip;
	i3PersistantElement * pObj;
	i3TimeSequenceInfo * pInfo = nullptr;

	pObj = clip.Paste(m_hWnd);
	if (pObj == nullptr)
		return;

	if (i3::kind_of<i3Node*>(pObj))
	{
		pInfo = i3TimeSequenceInfo::new_object();

		pInfo->SetNode((i3Node *)pObj);
		pInfo->SetStartTime(0);
	}
	else if (i3::kind_of<i3TimeSequenceInfo* >(pObj))
	{
		pInfo = (i3TimeSequenceInfo *)pObj;
	}
	else
	{
		I3_MUST_RELEASE(pObj);
		return;
	}

	AddSequence(pInfo);

	I3_MUST_RELEASE(pInfo);
}

void i3TDKTimeLineCtrl::OnRename(INT32 idxSel)
{
	i3TDKDlgRename dlg;
	i3TimeSequenceInfo * pInfo;

	pInfo = m_pTimeSeq->GetSequence(idxSel);

	if (pInfo->GetNode() == nullptr)
		return;

	if (pInfo->GetNode()->hasName())
	{
#ifdef _UNICODE
		i3::rc_wstring wstrNodeName;	i3::mb_to_utf16(pInfo->GetNode()->GetNameString(), wstrNodeName);
		dlg.setName(wstrNodeName);
#else
		dlg.setName(pInfo->GetNode()->GetNameString());
#endif
	}

	HINSTANCE hOld = AfxGetResourceHandle();

	AfxSetResourceHandle(g_hInstTDK);


	if (dlg.DoModal() == IDCANCEL)
	{
		AfxSetResourceHandle(hOld);
		return;
	}

	AfxSetResourceHandle(hOld);

#ifdef _UNICODE
	i3::rc_string strNodeName;	i3::utf16_to_mb(dlg.getNameString(), strNodeName);
	pInfo->GetNode()->SetName(strNodeName);
#else
	pInfo->GetNode()->SetName(dlg.getNameString());
#endif

	Invalidate();

	i3TDK::Update(m_hWnd, I3_TDK_UPDATE_EDIT, pInfo);
	i3TDK::Update(m_hWnd, I3_TDK_UPDATE_EDIT, pInfo->GetNode());
}

bool i3TDKTimeLineCtrl::_Rec_AnalyzeNode(i3Node * pNode, NODE_INFO * pInfo)
{
	if (i3::same_of<i3Particle*>(pNode))
	{
		i3Particle * pParticle = (i3Particle *)pNode;

		pInfo->m_Type = NODE_TYPE_PARTICLE;

		if (pParticle->GetParticleInfo() != nullptr)
		{
			pInfo->m_Duration = MAX(pInfo->m_Duration, pParticle->GetParticleInfo()->GetLifeTime());
		}
	}
	else if (i3::kind_of<i3SoundNode*>(pNode))
	{
		i3SoundNode * pSoundNode = (i3SoundNode *)pNode;

		pInfo->m_Type = NODE_TYPE_SOUND;

		if (pSoundNode->getSoundPlayInfo() != nullptr)
		{
			i3SoundPlayInfo * pPlayInfo = pSoundNode->getSoundPlayInfo();

			if (pPlayInfo->getSound() != nullptr)
			{
				i3Sound * pSound = pPlayInfo->getSound();

				if (pPlayInfo->GetLoopMode() == I3SND_RT_LOOPMODE_ONESHOT || pSoundNode->getLoopCount() <= 0)
				{
					pInfo->m_Duration = MAX(pInfo->m_Duration, pSound->GetDuration());
				}
				else
				{
					pInfo->m_Duration = MAX(pInfo->m_Duration, pSound->GetDuration() * pSoundNode->getLoopCount());
				}
			}
		}
	}
	else if (i3::kind_of<i3Transform*>(pNode))
	{
		i3Transform * pTrans = (i3Transform *)pNode;
		INT32 i;

		for (i = 0; i < pTrans->getAnimationCount(); i++)
		{
			i3Animation * pAnim = pTrans->getAnimation(i);

			pInfo->m_Duration = MAX(pInfo->m_Duration, pAnim->GetDuration());
		}
	}

	return true;
}

void i3TDKTimeLineCtrl::_AnalyzeNode(i3Node * pNode, NODE_INFO * pInfo)
{
	pInfo->m_Type = NODE_TYPE_NORMAL;
	pInfo->m_Duration = 0.0f;

	_Rec_AnalyzeNode(pNode, pInfo);
}

void i3TDKTimeLineCtrl::_UpdateScrollInfo(void)
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin = 0;

	// Vertical Scroll bar
	{
		if (m_pTimeSeq != nullptr)
			si.nMax = m_pTimeSeq->GetSequenceCount();
		else
			si.nMax = 0;

		si.nPage = (m_ClientHeight - RULER_CY) / TRACK_CY;
		si.nPos = m_ShowStartIndex;

		SetScrollInfo(SB_VERT, &si);

	}

	// Horizontal Scroll Bar
	{
		si.nMax = (INT32)(m_MaxDuration * 500 * m_ZoomRate);
		si.nPage = (INT32)(m_ClientWidth - TRACK_CX);
		si.nPos = (INT32)(m_ShowStartTime * 500 * m_ZoomRate);

		SetScrollInfo(SB_HORZ, &si);
	}
}

void i3TDKTimeLineCtrl::EnsureVisible(REAL32 tm)
{
	CRect rect;
	INT32 x;

	_GetRulerRect(&rect);
	x = _tm2pix(tm);

	if ((x < 0) || (x > rect.Width()))
	{
		// 화면에서 벗어났다..
		if (x < (m_ClientWidth >> 1))
		{
			// 왼쪽에 맞춘다.
			m_ShowStartTime = tm;
		}
		else
		{
			// 오른쪽에 맞춘다.
			m_ShowStartTime = _pix2tm(x - rect.Width());
		}

		_UpdateScrollInfo();
		Invalidate();
	}
}



BOOL i3TDKTimeLineCtrl::Create(DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	hCursor = ::LoadCursor(nullptr, IDC_ARROW);

	hbrBack = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);

	lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, hCursor, hbrBack, nullptr);
	ASSERT(lpszClass);

	dwStyle |= WS_VSCROLL | WS_HSCROLL;

	return CWnd::Create(lpszClass, nullptr, dwStyle, rect, pParentWnd, nID, nullptr);
}


BOOL i3TDKTimeLineCtrl::OnEraseBkgnd(CDC* pDC)
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
	{ IDB_TIMESEQ_TRACK_LEFT,				15,	15 },	// TSQBMP_TRACK_NORMAL_LEFT,
	{ IDB_TIMESEQ_TRACK_BODY,				15,	15 },	// TSQBMP_TRACK_NORMAL_BODY,
	{ IDB_TIMESEQ_TRACK_RIGHT,			6,	15 },	// TSQBMP_TRACK_NORMAL_RIGHT,

	{ IDB_TIMESEQ_TRACK_SELECT_LEFT,		15,	15 },	// TSQBMP_TRACK_SELECT_LEFT,
	{ IDB_TIMESEQ_TRACK_SELECT_BODY,		15,	15 },	// TSQBMP_TRACK_SELECT_BODY,
	{ IDB_TIMESEQ_TRACK_SELECT_RIGHT,		6,	15 },	// TSQBMP_TRACK_SELECT_RIGHT,

	{ IDB_TIMESEQ_TRACK_DISABLE_LEFT,		15,	15 },	// TSQBMP_TRACK_DISABLE_LEFT,
	{ IDB_TIMESEQ_TRACK_DISABLE_BODY,		15,	15 },	// TSQBMP_TRACK_DISABLE_BODY,
	{ IDB_TIMESEQ_TRACK_DISABLE_RIGHT,	6,	15 },	// TSQBMP_TRACK_DISABLE_RIGHT,

	{ IDB_TIMESEQ_SLIDE_NORMAL_LEFT,		5,	15 },	// TSQBMP_SLIDE_NORMAL_LEFT,
	{ IDB_TIMESEQ_SLIDE_NORMAL_BODY,		2,	15 },	// TSQBMP_SLIDE_NORMAL_BODY,
	{ IDB_TIMESEQ_SLIDE_NORMAL_RIGHT,		5,	15 },	// TSQBMP_SLIDE_NORMAL_RIGHT,

	{ IDB_TIMESEQ_SLIDE_SELECT_LEFT,		5,	15 },	// TSQBMP_SLIDE_SELECT_LEFT,
	{ IDB_TIMESEQ_SLIDE_SELECT_BODY,		2,	15 },	// TSQBMP_SLIDE_SELECT_BODY,
	{ IDB_TIMESEQ_SLIDE_SELECT_RIGHT,		5,	15 },	// TSQBMP_SLIDE_SELECT_RIGHT,

	{ IDB_TIMESEQ_SLIDE_DISABLE_LEFT,		5,	15 },	// TSQBMP_SLIDE_DISABLE_LEFT,
	{ IDB_TIMESEQ_SLIDE_DISABLE_BODY,		2,	15 },	// TSQBMP_SLIDE_DISABLE_BODY,
	{ IDB_TIMESEQ_SLIDE_DISABLE_RIGHT,	5,	15 },	// TSQBMP_SLIDE_DISABLE_RIGHT,

	{ IDB_TIMESEQ_GUAGE,					9,	17 },	// TSQBMP_QUAGE,
	{ IDB_TIMESEQ_SLIDE_BACK,				32, 15 },	// TSQBMP_SLIDE_BACK,

	{ 0,	0,	0 }
};

int i3TDKTimeLineCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rt;

		GetClientRect(&rt);
	}

	{
		HDC dc;

		dc = ::GetDC(m_hWnd);

		m_hdcDraw = CreateCompatibleDC(dc);

		m_hdcTemp = CreateCompatibleDC(dc);

		::ReleaseDC(m_hWnd, dc);
	}

	{
		m_hFont = i3TDK::CreateDefFont(14, _T("Arial Narrow"));
	}

	{
		m_hpenBorder = ::CreatePen(PS_SOLID, 1, RGB(46, 46, 46));
	}

	{
		m_hbrBack = ::CreateSolidBrush(RGB(68, 68, 68));
	}

	{
		m_hbmpRulerBack = ::LoadBitmap(g_hInstTDK, MAKEINTRESOURCE(IDB_TIMESEQ_RULER_BACK));
		I3ASSERT(m_hbmpRulerBack != nullptr);
	}

	{
		INT32 i;

		for (i = 0; s_BMPTable[i].m_Width > 0; i++)
		{
			m_hbmpTable[i].m_hBitmap = ::LoadBitmap(g_hInstTDK, MAKEINTRESOURCE(s_BMPTable[i].m_ID));
			m_hbmpTable[i].m_Width = s_BMPTable[i].m_Width;
			m_hbmpTable[i].m_Height = s_BMPTable[i].m_Height;
		}
	}

	return 0;
}

void i3TDKTimeLineCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_hdcDraw == nullptr)
		return;

	if ((cx <= 0) || (cy <= 0))
		return;

	m_ClientWidth = cx;
	m_ClientHeight = cy;

	if (m_hbmpDraw != nullptr)
	{
		::SelectObject(m_hdcDraw, m_hbmpOld);
		::DeleteObject(m_hbmpDraw);

		m_hbmpDraw = nullptr;
	}

	if (m_hRegion != nullptr)
	{
		::DeleteObject(m_hRegion);
		m_hRegion = nullptr;
	}

	if (m_hRulerRgn != nullptr)
	{
		::DeleteObject(m_hRulerRgn);
		m_hRulerRgn = nullptr;
	}

	m_hbmpDraw = CreateBitmap(cx, cy, GetDeviceCaps(m_hdcDraw, PLANES), GetDeviceCaps(m_hdcDraw, BITSPIXEL), nullptr);
	I3ASSERT(m_hbmpDraw != nullptr);

	{
		CRect rt;

		_GetRulerRect(&rt);

		m_hRulerRgn = CreateRectRgn(rt.left, rt.top, rt.right, rt.bottom);
		I3ASSERT(m_hRulerRgn != nullptr);

		rt.left += 1;
		rt.right -= 1;
		rt.top = rt.bottom;
		rt.bottom = m_ClientHeight - 1;

		m_hRegion = CreateRectRgn(rt.left, rt.top, rt.right, rt.bottom);
		I3ASSERT(m_hRegion != nullptr);
	}

	m_hbmpOld = (HBITMAP) ::SelectObject(m_hdcDraw, m_hbmpDraw);


	_UpdateScrollInfo();
}

void i3TDKTimeLineCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	_Draw();

	::BitBlt(dc.m_hDC, 0, 0, m_ClientWidth, m_ClientHeight, m_hdcDraw, 0, 0, SRCCOPY);
}

void i3TDKTimeLineCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu, *pPopup;
	CPoint pt, cpt;
	UINT32 cmd;
	INT32 selIndex;
	NC_HIT hit;

	SetFocus();
	GetCursorPos(&pt);

	cpt = pt;
	ScreenToClient(&cpt);

	{
		selIndex = FindItem(cpt, &hit);
	}

	{
		HINSTANCE hOld = AfxGetResourceHandle();

		AfxSetResourceHandle(g_hInstTDK);

		menu.LoadMenu(IDR_POPUP_TIMESEQ);

		AfxSetResourceHandle(hOld);
	}


	pPopup = menu.GetSubMenu(0);

	OnPopupMenu(pPopup, hit, selIndex);

	{
		i3TDKClipboard clip;

		if (!clip.IsObject(i3Node::static_meta()) && !clip.IsObject(i3TimeSequenceInfo::static_meta()))
		{
			pPopup->EnableMenuItem(ID_TIMESEQCTRL_PASTE, MF_BYCOMMAND | MF_GRAYED);
		}
	}

	cmd = pPopup->TrackPopupMenuEx(TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd *) this, nullptr);

	switch (cmd)
	{
	case ID_TIMESEQCTRL_SELECTALL:		OnSelectAll();	break;
	case ID_TIMESEQCTRL_UNSELECTALL:	OnUnselectAll();	break;
	case ID_TIMESEQCTRL_DELETE:		OnDelete();		break;
	case ID_TIMESEQCTRL_COPY:			OnCopy();		break;
	case ID_TIMESEQCTRL_PASTE:			OnPaste();		break;
	case ID_TIMESEQCTRL_RENAME:		OnRename(selIndex);		break;
	}

	OnMenuSelected(cmd, hit, &cpt, selIndex);

	CWnd::OnRButtonDown(nFlags, point);
}

void i3TDKTimeLineCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect Rect, ClientRect;
	SCROLLINFO si;
	bool bFlag = false;
	INT32 iPos = (INT32)nPos;

	si.cbSize = sizeof(si);
	GetScrollInfo(SB_VERT, &si);

	switch (nSBCode)
	{
	case SB_TOP:
		iPos = 0;
		bFlag = true;
		break;

	case SB_BOTTOM:
		iPos = si.nMax - si.nPage;
		bFlag = true;
		break;

	case SB_ENDSCROLL:
		break;

	case SB_LINEDOWN:
		iPos = si.nPos + 1;
		if ((iPos + si.nPage) > (UINT)si.nMax)
			iPos = si.nMax - si.nPage;
		bFlag = true;
		break;

	case SB_LINEUP:
		iPos--;
		if (iPos < 0)
			iPos = 0;
		bFlag = true;
		break;

	case SB_PAGEDOWN:
		iPos += si.nPage;
		if ((iPos + si.nPage) >(UINT)si.nMax)
			iPos = si.nMax - si.nPage;
		bFlag = true;
		break;

	case SB_PAGEUP:
		iPos -= si.nPage;
		if (iPos < 0)
			iPos = 0;
		bFlag = true;
		break;

	case SB_THUMBPOSITION:
		bFlag = true;
		break;

	case SB_THUMBTRACK:
		bFlag = true;
		break;
	}

	if (bFlag)
	{
		m_ShowStartIndex = iPos;

		_UpdateScrollInfo();
		Invalidate();
	}

	CWnd::OnVScroll(nSBCode, iPos, pScrollBar);
}

void i3TDKTimeLineCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect Rect, ClientRect;
	SCROLLINFO si;
	bool bFlag = false;

	si.cbSize = sizeof(si);
	GetScrollInfo(SB_HORZ, &si);

	int curPos = si.nPos;

	switch (nSBCode)
	{
	case SB_TOP:
		curPos = 0;
		bFlag = true;
		break;

	case SB_BOTTOM:
		curPos = si.nMax - si.nPage;
		bFlag = true;
		break;

	case SB_ENDSCROLL:
		break;

	case SB_LINEDOWN:
		curPos = si.nPos + 1;
		if ((curPos + si.nPage) > (UINT)si.nMax)
			curPos = si.nMax - si.nPage;
		bFlag = true;
		break;

	case SB_LINEUP:
		if (curPos > 0)
			curPos--;
		bFlag = true;
		break;

	case SB_PAGEDOWN:
		curPos += si.nPage;
		if ((curPos + si.nPage) > (UINT)si.nMax)
			curPos = si.nMax - si.nPage;
		bFlag = true;
		break;

	case SB_PAGEUP:
		curPos -= si.nPage;
		if (curPos < 0)
			curPos = 0;
		bFlag = true;
		break;

	case SB_THUMBPOSITION:
		curPos = si.nTrackPos;
		bFlag = true;
		break;

	case SB_THUMBTRACK:
		bFlag = true;
		curPos = si.nTrackPos;
		break;
	}

	if (bFlag)
	{
		m_ShowStartTime = (curPos * 0.002f) / m_ZoomRate;
		m_ShowStartTime = ROUNDING(m_ShowStartTime, 2);
		_UpdateScrollInfo();
		Invalidate();
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
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

void i3TDKTimeLineCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	INT32 idx;
	NC_HIT	hit;

	m_DragStartPt = point;
	::SetCapture(m_hWnd);

	SetFocus();

	idx = FindItem(point, &hit);

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
	switch (hit)
	{
	case NC_HIT_RULER:			// Guage의 이동
	{
		m_timeGuage = HitTrackIdx(point);
		
		Invalidate();
	}
	break;

	case NC_HIT_GUAGE:			// Guage의 Dragging
	{
		m_MouseMode = MOUSE_MODE_GUAGE_MOVE;

		m_SavedStartTime = m_timeGuage;
	}
	break;

	case NC_HIT_TRACK:			// 단순 Selection
	{
		I3ASSERT(idx != -1);

		if (!(nFlags & MK_CONTROL))
			OnUnselectAll();

		OnSelect(idx);

		m_MouseMode = MOUSE_MODE_TRACK_DRAG;
		m_DragStartIndex = idx;
	}
	break;

	case NC_HIT_SLIDE_EMPTY:
	{
		I3ASSERT(idx != -1);

		if (!(nFlags & MK_CONTROL))
		{
			OnUnselectAll();
			m_vecKeyFrame.clear();
		}

		INT32 nIndex = HitTrackIdx(idx, point);

		SelectKey(nIndex);
		m_MouseMode = MOUSE_MODE_KEYFRAME;
		m_SelectCurKey = nIndex;
		m_SelectTrack = idx;
		OnSelect(idx);
	}
	break;

	case NC_HIT_TRACK_ENABLE:
	{
		I3ASSERT(idx != -1);
		i3TimeSequenceInfo * pInfo = m_pTimeSeq->GetSequence(idx);

		if (pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_DISABLE)
		{
			pInfo->SetStyle(pInfo->GetStyle() & ~I3SG_TIMESEQ_STYLE_DISABLE);

			if (pInfo->GetNode() != nullptr)
			{
				pInfo->GetNode()->RemoveFlag(I3_NODEFLAG_DISABLE);
			}
		}
		else
		{
			pInfo->SetStyle(pInfo->GetStyle() | I3SG_TIMESEQ_STYLE_DISABLE);

			if (pInfo->GetNode() != nullptr)
			{
				pInfo->GetNode()->AddFlag(I3_NODEFLAG_DISABLE);
			}
		}

		OnChangeItemEnable(pInfo);

		Invalidate();
	}
	break;

	case NC_HIT_SLIDE_HEAD:
	case NC_HIT_SLIDE_BODY:
	case NC_HIT_SLIDE_TAIL:
	{
		m_MouseMode = MOUSE_MODE_SLIDE_MOVE;

		I3ASSERT(idx != -1);
		OnUnselectAll();
		OnSelect(idx);

		{
			i3TimeSequenceInfo * pInfo = m_pTimeSeq->GetSequence(idx);

			m_SavedStartTime = pInfo->GetStartTime();
			m_SavedDurationTime = pInfo->GetDuration();
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

	default:
		OnUnselectAll();

		break;
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKTimeLineCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	::ReleaseCapture();
	m_MouseMode = MOUSE_MODE_NA;

	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKTimeLineCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_DragStartPt = point;
	::SetCapture(m_hWnd);

	SetFocus();

	m_SavedStartTime = m_ShowStartTime;
	m_SavedStartIndex = m_ShowStartIndex;
	m_MouseMode = MOUSE_MODE_SCROLL;

	CWnd::OnMButtonDown(nFlags, point);
}

void i3TDKTimeLineCtrl::OnMButtonUp(UINT nFlags, CPoint point)
{
	::ReleaseCapture();
	m_MouseMode = MOUSE_MODE_NA;

	CWnd::OnMButtonUp(nFlags, point);
}

void i3TDKTimeLineCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	switch (m_MouseMode)
	{
		case MOUSE_MODE_NA:
			break;

		case MOUSE_MODE_SCROLL:
		{
			REAL32 dt = _pix2tm_sz(m_DragStartPt.x - point.x);

			m_ShowStartTime = m_SavedStartTime + dt;
			if (m_ShowStartTime < 0.0f)
			{
				m_ShowStartTime = 0.0f;
			}

			if (m_ShowStartTime > m_MaxDuration)
				m_ShowStartTime = m_MaxDuration;

			Invalidate();
		}
		break;

		case MOUSE_MODE_TRACK_DRAG:
		{
			// 현재의 Mouse cursor position으로 삽입될 위치를 확인한다.
			INT32 idx;

			idx = FindInsertPos(point);

			if (idx != m_DragStartIndex)
			{
				// 새 위치에 삽입해 준다.
				i3TimeSequenceInfo * pInfo = m_pTimeSeq->GetSequence(m_DragStartIndex);

				I3TRACE("%d, %d\n", idx, m_DragStartIndex);

				I3_MUST_ADDREF(pInfo);

				m_pTimeSeq->RemoveSequence(pInfo);

				if (idx > m_DragStartIndex)
					idx--;

				m_DragStartIndex = m_pTimeSeq->InsertSequence(idx, pInfo);

				I3_MUST_RELEASE(pInfo);

				Invalidate();
			}
		}
		break;

		case MOUSE_MODE_SLIDE_MOVE:
		{
			REAL32 dt = _pix2tm_sz(m_DragStartPt.x - point.x);
			i3TimeSequenceInfo * pInfo;
			REAL32 tm = 0.0f;

			pInfo = m_pTimeSeq->GetSequence(m_SavedStartIndex);

			if (m_bUseSlideCustomSize)
			{
				if (m_CurrentNcHit == NC_HIT_SLIDE_HEAD)
				{
					if (m_SavedDurationTime + dt > 0.0f && m_SavedDurationTime + dt < m_MaxDuration)
						pInfo->SetDuration(m_SavedDurationTime + dt);
					else
						break;
				}
				else if (m_CurrentNcHit == NC_HIT_SLIDE_TAIL)
				{
					if (m_SavedDurationTime - dt > 0.0f && m_MaxDuration > m_SavedDurationTime - dt)
						pInfo->SetDuration(m_SavedDurationTime - dt);
					else
						break;
				}

				if (m_CurrentNcHit != NC_HIT_SLIDE_TAIL)
				{
					tm = m_SavedStartTime - dt;
					if (tm < 0.0f)
						tm = 0.0f;

					pInfo->SetStartTime(tm);
				}
			}
			else
			{
				tm = m_SavedStartTime - dt;
				if (tm < 0.0f)
					tm = 0.0f;

				pInfo->SetStartTime(tm);
			}

			if (pInfo->GetDuration() > 0.0f)
			{
				if ((tm + pInfo->GetDuration()) > m_MaxDuration)
				{
					//타임트랙의 맥스값은.. 애니메이션의 시간으로 넘어선 안될것 같다..
					//pInfo->SetStartTime(m_MaxDuration - pInfo->GetDuration());
					m_MaxDuration = (tm + pInfo->GetDuration());

					_UpdateScrollInfo();
				}
			}

			Invalidate();
		}
		break;

		case MOUSE_MODE_SLIDE_RESIZE:
			break;

		case MOUSE_MODE_GUAGE_MOVE:
		{
			//REAL32 dt = _pix2tm_sz(m_DragStartPt.x - point.x);
	
			//m_timeGuage = m_SavedStartTime - dt;
			m_timeGuage = HitTrackIdx(point);


			OnChangeCurrentTime(m_timeGuage);

			EnsureVisible(m_timeGuage);

			Invalidate();
		}
		break;
		case MOUSE_MODE_KEYFRAME:
		{
 			INT32 nIndex = HitTrackIdx(m_SelectTrack, point);
			m_vecKeyFrame.clear();
			if(m_SelectCurKey < nIndex)
				SelectKey(m_SelectCurKey, nIndex);
			else
				SelectKey(nIndex, m_SelectCurKey);
 			Invalidate();
		}
		break;
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL i3TDKTimeLineCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (nFlags & MK_CONTROL)
	{
		// Zoom
		if (zDelta > 0)
			m_ZoomRate += 1;
		else
			m_ZoomRate -= 1;
		
		if (m_ZoomRate == 0)
		{
			m_ZoomRate = 1;
		}

		Invalidate();
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

REAL32 i3TDKTimeLineCtrl::HitTrackIdx(POINT pt)
{
	RECT rt;
	_GetRulerRect(&rt);
	if (PtInRect(&rt, pt))
	{
		INT32 nValue = pt.x - TRACK_CX;
		INT32 nTemp = nValue % (ONERULERINTERVAL*m_ZoomRate);
		nValue -= nTemp;
		REAL32 ftimeGuage = _pix2tm(nValue);
		if (ftimeGuage < 0.0f)
			ftimeGuage = 0.0f;

		if (ftimeGuage > m_MaxDuration)
			ftimeGuage = m_MaxDuration;

		return ftimeGuage;
	}

	return 0.0f;
}

INT32 i3TDKTimeLineCtrl::HitTrackIdx(INT32 nIndex, POINT pt)
{
	CRect rt;

	rt.SetRect(1, RULER_CY + 1, m_ClientWidth - 1, m_ClientHeight - 1);

	for (INT32 i = m_ShowStartIndex; i < m_pTimeSeq->GetSequenceCount(); i++)
	{
		rt.bottom = rt.top + ITEM_CY;
		if (nIndex == i)
		{
			if (PtInRect(&rt, pt))
			{
				INT32 nValue = pt.x - TRACK_CX;

				//실수개산에 대한 오차범위가 넘어가는 이슈땜시 땜질,...
				if (nValue % (ONERULERINTERVAL*m_ZoomRate) == 0)
					nValue -= 1;

				//처음트랙은 1번임
				INT32 nTemp = (nValue / (ONERULERINTERVAL*m_ZoomRate)) + 1;

				return (INT32)nTemp;
			}
		}

		if (rt.bottom > m_ClientHeight - 1)
			break;

		rt.top = rt.bottom + 1;
	}

	return 0;
}

void i3TDKTimeLineCtrl::SelectKey(INT32 nIndex)
{
	INT32 idx = FindKey(nIndex);
	if (idx == -1)
	{
		KeyFrame key;
		key._KeyIndex = nIndex;
		m_vecKeyFrame.push_back(key);
	}
}

void i3TDKTimeLineCtrl::SelectKey(INT32 nBegin, INT32 nEnd)
{
	for (INT32 i = nBegin; i < nEnd; i++)
	{
		INT32 idx = FindKey(i);
		if (idx == -1)
		{
			KeyFrame key;
			key._KeyIndex = i;
			m_vecKeyFrame.push_back(key);
		}
	}
}

void i3TDKTimeLineCtrl::DeleteKey(INT32 nIndex)
{
	i3::vector<KeyFrame>::iterator it;

	for (it = m_vecKeyFrame.begin(); it != m_vecKeyFrame.end(); ++it)
	{
		if (it->_KeyIndex == nIndex)
		{
			m_vecKeyFrame.erase(it);
			break;
		}
	}
}

INT32 i3TDKTimeLineCtrl::FindKey(INT32 nIndex)
{
	for (INT32 i = 0; i < (INT32)m_vecKeyFrame.size(); i++)
	{
		if (m_vecKeyFrame[i]._KeyIndex == nIndex)
			return i;
	}

	return -1;
}
