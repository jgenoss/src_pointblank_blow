#include "stdafx.h"
#include "TextureViewer.h"
#include "Control/UIGripCtrl.h"
#include "Control/UIGripCtrlRect.h"
#include "MainFrm.h"
#include "i3UIAnimation.h"
#include "GlobalInstance.h"
IMPLEMENT_DYNAMIC(CTextureViewer, i3TDKImageViewCtrl)
BEGIN_MESSAGE_MAP(CTextureViewer, i3TDKImageViewCtrl)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()
CTextureViewer::CTextureViewer()
{
	m_pControllerMeta = CUIGripObjHandler_UIControl::static_meta();
	//LoadColorKey();

	m_bLBDown = false;
	m_ptOldPos.x = m_ptOldPos.y = 0;
}


CTextureViewer::~CTextureViewer()
{
	//SaveColorKey();

	for (size_t i = 0; i < m_listRect.size(); ++i)
	{
		RECT * pRect = m_listRect[i];

		delete pRect;
	}
	m_listRect.clear();
}

void CTextureViewer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKImageViewCtrl::OnSize(nType, cx, cy);
}


void CTextureViewer::DisplayTex(i3::string sItemPath)
{
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	i3NamedResourceManager* pTexManager = i3ResourceFile::GetResourceManager(i3Texture::static_meta());
	if (pTexManager)
	{
		i3Texture* tex = (i3Texture *)pTexManager->GetResource(sItemPath.c_str(), i3ResourceFile::GetWorkingFolder(), sItemPath.c_str());

		if (tex)
		{
			if (GetImage() != tex)
			{
				SetImage(tex);
				m_GripManager.SetZoomRate(m_Zoom);
			}

			for (INT32 i = 0; i < CGlobalInstance::getSingletonPtr()->GetUVCount(); i++)
			{
				UVITEM* pItem = CGlobalInstance::getSingletonPtr()->GetUVItem(i);
				if (pItem->_pTexture == tex)
				{
					m_GripManager.AddSelect(pItem->_pControl, m_pControllerMeta);
				}
				else
				{
					m_GripManager.RemoveSelect(pItem->_pControl);
				}
			}

			m_GripManager.SetDelta(CPoint(0, 0));
		}
		else
		{
			SetImage(NULL);
		}

		UpdateInvaliDate();
	}
}

void CTextureViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint	ptRel = _AbsToRel(point);
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	SetFocus();

	m_LDragStart = ptRel;

	if (m_GripManager.isOnGrip(ptRel) || m_GripManager.isSelected(ptRel))
	{
		if (m_GripManager.OnLButtonDown(ptRel))
		{
			if (m_GripManager.GetCurFocusShape())
			{
				INT32 nIndex = CGlobalInstance::getSingletonPtr()->GetUVItem((i3UIControl*)m_GripManager.GetCurFocusShape()->getObject());
				pMainWnd->GetPieceListPane()->SetFocusdItem(nIndex);
			}
		}
	}
	else
	{
		m_GripManager.AllPenColorNormal();
		pMainWnd->GetPieceListPane()->AllItemNonFocuse();
		m_bLBDown = true;
	}

	UpdateInvaliDate();
	i3TDKImageViewCtrl::OnLButtonDown(nFlags, point);
}


void CTextureViewer::OnDraw(HDC dc)
{
	CRect rect;
	::GetClientRect(m_hWnd, &rect);

	// 메모리 DC 선언
	CDC* pDC = GetDC();

	if (pDC)
	{
		CDC memDC;
		CBitmap *pOldBitmap, bitmap;

		char szTemp[MAX_PATH];

		// 화면 DC와 호환되는 메모리 DC 객체를 생성
		memDC.CreateCompatibleDC(pDC);

		// 마찬가지로 화면 DC와 호환되는 Bitmap 생성
		bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());

		pOldBitmap = memDC.SelectObject(&bitmap);

		{
			i3TDKImageViewCtrl::OnDraw(memDC.m_hDC);

			m_GripManager.Draw(memDC.m_hDC);

			HFONT hFont, hOldFont;
			hFont = CreateFont(12, 0, 0, 0, FW_HEAVY, 0, 0, 0, HANGUL_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림체");
			hOldFont = (HFONT)SelectObject(memDC.m_hDC, hFont);

			SetTextColor(memDC.m_hDC, RGB(255, 255, 0));
			SetBkMode(memDC.m_hDC, TRANSPARENT);

			INT32 posY = 15;

			sprintf_s(szTemp, "Mouse Pos : %d , %d ", (INT32)(abs(m_ptOldPos.x / m_Zoom)), (INT32)(abs((m_ptOldPos.y / m_Zoom))));
			TextOut(memDC.m_hDC, 0, 0, szTemp, MAX_PATH);

			if (m_bLBDown)
			{
				HGDIOBJ hOldBrush = (HGDIOBJ)::SelectObject(memDC.m_hDC, (HBRUSH) ::GetStockObject(NULL_BRUSH));
				HGDIOBJ hOldPen = (HGDIOBJ)::SelectObject(memDC.m_hDC, UIGlobalRes::getPen(COLORSET_GRIP_SELECT_BACK));

				CRect dragRect;
				dragRect.SetRect(m_LDragStart.x- m_GripManager.GetDelta().x, m_LDragStart.y- m_GripManager.GetDelta().y, 
					m_ptOldPos.x - m_GripManager.GetDelta().x, m_ptOldPos.y - m_GripManager.GetDelta().y);
				Rectangle(memDC.m_hDC, dragRect.left, dragRect.top, dragRect.right, dragRect.bottom);

				::SelectObject(memDC.m_hDC, hOldPen);
				::SelectObject(memDC.m_hDC, hOldBrush);

				sprintf_s(szTemp, "Drag Size : %d , %d ", (INT32)(abs((m_ptOldPos.x - m_LDragStart.x) / m_Zoom)), (INT32)(abs(((m_ptOldPos.y - m_LDragStart.y) / m_Zoom))));
				TextOut(memDC.m_hDC, 0, posY, szTemp, MAX_PATH);
			}
			
			{
				HGDIOBJ hOldBrush = (HGDIOBJ)::SelectObject(memDC.m_hDC, (HBRUSH) ::GetStockObject(NULL_BRUSH));
				HGDIOBJ hOldPen = (HGDIOBJ)::SelectObject(memDC.m_hDC, UIGlobalRes::getPen(COLORSET_GRIP_NORMAL_BACK));

				for (size_t i = 0; i < m_listRect.size(); ++i)
				{
					RECT *pRt = m_listRect[i];

					::Rectangle(memDC.m_hDC, (INT32)(pRt->left * m_Zoom),
						(INT32)(pRt->top * m_Zoom),
						(INT32)(pRt->right * m_Zoom),
						(INT32)(pRt->bottom * m_Zoom));
				}

				::SelectObject(memDC.m_hDC, hOldPen);
				::SelectObject(memDC.m_hDC, hOldBrush);
			}
			

			SelectObject(memDC.m_hDC, hOldFont);
			DeleteObject(hFont);
		}

		// 메모리 DC를 화면 DC에 고속 복사
		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBitmap);
		memDC.DeleteDC();
		bitmap.DeleteObject();
		ReleaseDC(pDC);
	}
}


void CTextureViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint	ptRel = _AbsToRel(point);

	HCURSOR hCursor = m_GripManager.getCursor(ptRel);

	::SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)hCursor);


	if (m_GripManager.OnMouseMove(ptRel, nFlags))
	{
		if (ptRel != m_ptOldPos)
		{
			m_GripManager.RecalcLayout();
		}
	}

	m_ptOldPos = ptRel;
	if( GetImage() )
		UpdateInvaliDate();

	i3TDKImageViewCtrl::OnMouseMove(nFlags, point);
}

void CTextureViewer::AddItem(UVITEM* pItem)
{
	m_GripManager.AddSelect(pItem->_pControl, m_pControllerMeta);

	UpdateInvaliDate();
}


void CTextureViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint	ptRel = _AbsToRel(point);
	
	m_bLBDown = false;
	UpdateInvaliDate();

	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	if (m_GripManager.GetFocusedShape())
	{
		i3UIControl* pControl = (i3UIControl*)m_GripManager.GetFocusedShape()->getObject();
		CGlobalInstance::getSingletonPtr()->UpdateUVItem(pControl);
	}
	else if (m_GripManager.getFocusedPoint())
	{
		i3UIControl* pControl = (i3UIControl*)m_GripManager.getFocusedPoint()->getOwner()->getHandler()->getObject();
		CGlobalInstance::getSingletonPtr()->UpdateUVItem(pControl);
	}
	else
	{
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			AnalysisPickImage();
		}
	}

	m_GripManager.OnLButtonUp(ptRel);

	i3TDKImageViewCtrl::OnLButtonUp(nFlags, point);
}

CPoint CTextureViewer::_AbsToRel(CPoint ptAbs)
{
	CPoint ptRel;

	ptRel.x = m_GripManager.GetDelta().x + ptAbs.x;
	ptRel.y = m_GripManager.GetDelta().y + ptAbs.y;

	return ptRel;
}


void CTextureViewer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateScroll(true, nSBCode, nPos, pScrollBar);

	i3TDKImageViewCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CTextureViewer::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateScroll(false, nSBCode, nPos, pScrollBar);

	i3TDKImageViewCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTextureViewer::UpdateScroll(bool bVert, UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_POS | SIF_TRACKPOS | SIF_PAGE | SIF_RANGE;
	INT32 nBar = bVert ? SB_VERT : SB_HORZ;		// bHorz or bVert
	::GetScrollInfo(m_hWnd, nBar, &info);

	INT32 nCurPos = info.nPos;

	switch (nSBCode)
	{
	case SB_TOP:			nCurPos = 0;			break;
	case SB_BOTTOM:		nCurPos = info.nMax;	break;
	case SB_LINEDOWN:		nCurPos++;				break;
	case SB_LINEUP:		nCurPos--;				break;
	case SB_PAGEDOWN:		nCurPos += 10;	break;
	case SB_PAGEUP:		nCurPos -= 10;	break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:	nCurPos = nPos;			break;
	}

	info.nPos = nCurPos;
	info.nTrackPos = nCurPos;
	::SetScrollInfo(m_hWnd, nBar, &info, TRUE);

	// min, max 가 반영된 값을 다시 얻어옴
	UpdateInvaliDate();
}

void CTextureViewer::AdjustShowXY()
{
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_POS;
	::GetScrollInfo(m_hWnd, SB_VERT, &info);
	INT32 y = (INT32)(info.nPos );

	::GetScrollInfo(m_hWnd, SB_HORZ, &info);
	INT32 x = (INT32)(info.nPos );

	if (x < 0)x = 0;
	if (y < 0)y = 0;
	m_GripManager.SetDelta(CPoint(x, y));
}

void CTextureViewer::SetFocusdItem(i3UIControl* pControl)
{
	m_GripManager.SetFocusdItem(pControl);

	UpdateInvaliDate();
}

void CTextureViewer::AnalysisPickImage()
{
	for (size_t i = 0; i < m_listRect.size(); ++i)
	{
		RECT * pRect = m_listRect[i];

		delete pRect;
	}
	m_listRect.clear();

	i3Texture* pTexture = GetImage();

	INT32 i, j, idxNext;

	COLORREAL Col;
	pTexture->Lock(0);

	
	//0,0픽셀이 colorkey sample이되게함
	pTexture->GetPixel(0, 0, &Col);
	i3Color::Set(&m_ColorKey, Col.r, Col.g, Col.b, Col.a);

	for (i = m_LDragStart.y / m_Zoom; i < m_ptOldPos.y / m_Zoom; i++)
	{
		for (j = m_LDragStart.x / m_Zoom; j < m_ptOldPos.x / m_Zoom; j++)
		{
			pTexture->GetPixel(j, i, &Col);

			if (isSameColor(&Col, &m_ColorKey) == false)
			{
				POINT pos;
				pos.x = j;
				pos.y = i;

				INT32 nIndex = FindBefore(&pos);

				if (nIndex == -1)
				{
					RECT* pRect = MakeNewRect(pos);

					bool bState = true;
					while (bState)
					{
						bState = CalculRect(pTexture, pRect);
					}
				}
			}
		}
	}
	pTexture->Unlock(0);

	RECT temp;
	ZeroMemory(&temp, sizeof(RECT));
	temp.left = pTexture->GetWidth();
	temp.top = pTexture->GetHeight();
	for (INT32 i = 0; i < m_listRect.size(); i++)
	{
		if (m_listRect[i]->left < temp.left)
			temp.left = m_listRect[i]->left;

		if (m_listRect[i]->right > temp.right)
			temp.right = m_listRect[i]->right;

		if (m_listRect[i]->top < temp.top)
			temp.top = m_listRect[i]->top;

		if (m_listRect[i]->bottom > temp.bottom)
			temp.bottom = m_listRect[i]->bottom;
	}

	for (size_t i = 0; i < m_listRect.size(); ++i)
	{
		RECT * pRect = m_listRect[i];

		delete pRect;
	}
	m_listRect.clear();
	
	CGlobalInstance::getSingletonPtr()->AddUVItem(GetImage(), CRect(temp.left, temp.top, temp.right - temp.left, temp.bottom - temp.top));
}

void CTextureViewer::AnalysisImage()
{
	for (size_t i = 0; i < m_listRect.size(); ++i)
	{
		RECT * pRect = m_listRect[i];

		delete pRect;
	}
	m_listRect.clear();

	i3Texture* pTexture = GetImage();

	INT32 i, j, idxNext;

	COLORREAL Col;
	pTexture->Lock(0);


	//0,0픽셀이 colorkey sample이되게함
	pTexture->GetPixel(0, 0, &Col);
	i3Color::Set(&m_ColorKey, Col.r, Col.g, Col.b, Col.a);

	for (i = 0; i < pTexture->GetHeight(); i++)
	{
		for (j = 0; j < pTexture->GetWidth(); j++)
		{
			pTexture->GetPixel(j, i, &Col);

			if (isSameColor(&Col, &m_ColorKey) == false)
			{
				POINT pos;
				pos.x = j;
				pos.y = i;

				INT32 nIndex = FindBefore(&pos);

				if (nIndex == -1)
				{
					RECT* pRect = MakeNewRect(pos);

					bool bState = true;
					while (bState)
					{
						bState = CalculRect(pTexture, pRect);
					}
				}
			}
		}
	}
	pTexture->Unlock(0);


	for (INT32 i = 0; i < m_listRect.size(); i++)
	{
		CGlobalInstance::getSingletonPtr()->AddUVItem(GetImage(), 
			CRect(m_listRect[i]->left, m_listRect[i]->top, m_listRect[i]->right - m_listRect[i]->left, m_listRect[i]->bottom - m_listRect[i]->top));
	}

	for (size_t i = 0; i < m_listRect.size(); ++i)
	{
		RECT * pRect = m_listRect[i];

		delete pRect;
	}

	m_listRect.clear();
}

INT32 CTextureViewer::FindBefore(POINT *pt)
{
	if (pt->x < 0 || pt->y < 0)
		return -1;


	for (size_t i = 0; i < m_listRect.size(); ++i)
	{
		RECT * pRT = m_listRect[i];

		if (::PtInRect(pRT, *pt))
			return i;
	}

	return -1;
}

RECT* CTextureViewer::MakeNewRect(POINT pos)
{
	RECT * pNewRT = new RECT;

	if (pos.x != 0)
		pNewRT->left = pos.x - 1;
	else
		pNewRT->left = pos.x;

	pNewRT->right = pos.x + 1;

	if (pos.y != 0)
		pNewRT->top = pos.y - 1;
	else
		pNewRT->top = pos.y;

	pNewRT->bottom = pos.y + 1;

	m_listRect.push_back(pNewRT);

	return pNewRT;
}

void CTextureViewer::RectExpend(RECT *rt, RECT_EXPANDTYPE type)
{
	if (type == RECTEXPAND_LEFT)
	{
		rt->left--;
	}
	else if (type == RECTEXPAND_RIGHT)
	{
		rt->right++;
	}
	else if (type == RECTEXPAND_TOP)
	{
		rt->top--;
	}
	else
	{
		rt->bottom++;
	}
}

INT32 CTextureViewer::ExpandRight(INT32 idx)
{
	I3_BOUNDCHK(idx, (INT32)m_listRect.size());

	RECT * pRT = m_listRect[idx];

	pRT->right++;

	return -1;
}


INT32 CTextureViewer::ExpandDown(INT32 idx)//위쪽 픽셀 관련 rect만 있다면 해당 rect의 bottom을 확장
{
	I3_BOUNDCHK(idx, (INT32)m_listRect.size());

	RECT * pRT = m_listRect[idx];

	pRT->bottom++;

	return pRT->right;
}

bool CTextureViewer::isSameColor(COLORREAL * pCol1, COLORREAL * pKey)
{
	if (i3Math::abs(pCol1->r - pKey->r) < 0.0001f &&
		i3Math::abs(pCol1->g - pKey->g) < 0.0001f &&
		i3Math::abs(pCol1->b - pKey->b) < 0.0001f &&
		i3Math::abs(pCol1->a - pKey->a) < 0.0001f)
		return true;

	return false;
}

void CTextureViewer::SaveColorKey()
{
	INT32 R, G, B, A;
	R = (INT32)(i3Color::GetR(&m_ColorKey) * 255.0f);
	G = (INT32)(i3Color::GetG(&m_ColorKey) * 255.0f);
	B = (INT32)(i3Color::GetB(&m_ColorKey) * 255.0f);
	A = (INT32)(i3Color::GetA(&m_ColorKey) * 255.0f);

	theApp.WriteProfileInt(LPCTSTR("Settings"), LPCTSTR("Color Key Red"), R);
	theApp.WriteProfileInt(LPCTSTR("Settings"), LPCTSTR("Color Key Green"), G);
	theApp.WriteProfileInt(LPCTSTR("Settings"), LPCTSTR("Color Key Blue"), B);
	theApp.WriteProfileInt(LPCTSTR("Settings"), LPCTSTR("Color Key Alpha"), A);
}

void CTextureViewer::LoadColorKey()
{
	INT32 R, G, B, A;

	R = theApp.GetProfileInt(LPCTSTR("Settings"), LPCTSTR("Color Key Red"), 0);
	G = theApp.GetProfileInt(LPCTSTR("Settings"), LPCTSTR("Color Key Green"), 0);
	B = theApp.GetProfileInt(LPCTSTR("Settings"), LPCTSTR("Color Key Blue"), 0);
	A = theApp.GetProfileInt(LPCTSTR("Settings"), LPCTSTR("Color Key Alpha"), 0);

	i3Color::Set(&m_ColorKey, (UINT8)R, (UINT8)G, (UINT8)B, (UINT8)A);
}

BOOL CTextureViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	

	i3TDKImageViewCtrl::OnMouseWheel(nFlags, zDelta, pt);

	
	m_GripManager.SetZoomRate(m_Zoom);
	AdjustShowXY();

	return TRUE;
}

void CTextureViewer::UpdateInvaliDate()
{
	InvalidateRect(NULL);
	AdjustShowXY();
}

void CTextureViewer::RemoveAllItem()
{
	m_GripManager.RemoveAllSelect();
}

bool CTextureViewer::CalculRect(i3Texture * pTexture, RECT *rt)
{
	COLORREAL Col;
	bool bExpand = false;

	RECT tempRect = *rt;

	//상확장
	for (INT32 i = tempRect.left; i < tempRect.right; i++)
	{
		if (tempRect.top > 0)
		{
			pTexture->GetPixel(i, tempRect.top, &Col);
			if (isSameColor(&Col, &m_ColorKey) == false)
			{
				RectExpend(rt, RECTEXPAND_TOP);
				bExpand = true;
				break;
			}
		}
	}

	//하확장
	for (INT32 i = tempRect.left; i < tempRect.right; i++)
	{
		if (pTexture->GetHeight() > tempRect.bottom)
		{
			pTexture->GetPixel(i, tempRect.bottom, &Col);
			if (isSameColor(&Col, &m_ColorKey) == false)
			{
				RectExpend(rt, RECTEXPAND_BOTTOM);
				bExpand = true;
				break;
			}
		}
	}

	//좌확장
	for (INT32 i = tempRect.top; i < tempRect.bottom; i++)
	{
		if (tempRect.left > 0)
		{
			pTexture->GetPixel(tempRect.left, i, &Col);
			if (isSameColor(&Col, &m_ColorKey) == false)
			{
				RectExpend(rt, RECTEXPAND_LEFT);
				bExpand = true;
				break;
			}
		}
	}

	//우확장
	for (INT32 i = tempRect.top; i < tempRect.bottom; i++)
	{
		if (pTexture->GetWidth() > tempRect.bottom)
		{
			pTexture->GetPixel(tempRect.right, i, &Col);
			if (isSameColor(&Col, &m_ColorKey) == false)
			{
				RectExpend(rt, RECTEXPAND_RIGHT);
				bExpand = true;
				break;
			}
		}
	}

	return bExpand;
}