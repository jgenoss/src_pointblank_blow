// UIShapeListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "resource.h"
#include "UIShapeListCtrl.h"
#include "UIEShapePropertyDlg.h"
#include "MainFrm.h"
#include "i3UIEditorView_Template.h"
#include "Dlg_SelectTexture.h"
#include "UIUVSelectorDlg.h"
#include "UITemplate_Editor.h"

#include "i3Framework/i3UI/i3VTFileFormat.h"

#define	_SPACE_			3
#define	_TEXT_SPACE		32

#define MIN_VIEWSIZE	48
#define	MAX_VIEWSIZE	256

// UIShapeListCtrl

IMPLEMENT_DYNAMIC(UIShapeListCtrl, CWnd)

UIShapeListCtrl::UIShapeListCtrl()
{
	m_pTemplate = NULL;
	m_BmpDC = NULL;

	m_ViewSize = 128;
	m_RowCount = 0;
	m_ColCount = 0;
	m_WndCX = 0;
	m_WndCY = 0;
	m_ScrollPos = 0;

	m_hFont = NULL;

	m_bModified = true;

	m_bNotifyUpdate = true;
	
	m_pEditor = NULL;
}

UIShapeListCtrl::~UIShapeListCtrl()
{
	RemoveAll();

	I3_SAFE_RELEASE( m_pTemplate);

	if( m_BmpDC != NULL)
	{
		::DeleteDC( m_BmpDC);
		m_BmpDC = NULL;
	}

	if( m_hFont != NULL)
	{
		::DeleteObject( m_hFont);
		m_hFont = NULL;
	}
}

BOOL UIShapeListCtrl::Create( LPCTSTR pszCaption, DWORD dwStyle, const CRect & rt, CWnd * pParent, UINT nID)
{
	HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW);

	LPCTSTR pszClass = AfxRegisterWndClass( CS_DBLCLKS, hCursor);

	dwStyle |= WS_VSCROLL;

	return CWnd::Create( pszClass, pszCaption, dwStyle, rt, pParent, nID);
}

void UIShapeListCtrl::SetEditor( CUITemplate_Editor * pEditor)
{
	m_pEditor = pEditor;
}

i3UIShape *	UIShapeListCtrl::getShape( INT32 idx)
{
	UISHAPE * pInfo = m_ShapeList[idx];

	return pInfo->m_pShape;
}

INT32 UIShapeListCtrl::getShapeIndex( i3UIShape * pShape)
{
	for( UINT32 i = 0; i < m_ShapeList.size(); i++)
	{
		UISHAPE * pInfo = m_ShapeList[i];

		if( pInfo->m_pShape == pShape)
			return i;
	}

	return -1;
}

INT32 UIShapeListCtrl::getSelected(void)
{
	UINT32 i;
	UISHAPE * pInfo;

	for( i = 0; i < m_ShapeList.size(); i++)
	{
		pInfo = m_ShapeList[i];

		if( pInfo->m_bSelected)
			return i;
	}

	return -1;
}

void UIShapeListCtrl::RemoveAll(void)
{
	size_t i;

	for( i = 0; i < m_ShapeList.size(); i++)
	{
		UISHAPE * pInfo = m_ShapeList[i];

		::DeleteObject( pInfo->m_hBitmap);

		delete pInfo;
	}

	m_ShapeList.clear();

	m_bModified = true;
}

void UIShapeListCtrl::setTemplate( i3UITemplate * pTemplate)
{
	I3_REF_CHANGE( m_pTemplate, pTemplate);

	UpdateAll();
}

void UIShapeListCtrl::UpdateAll(void)
{
	RemoveAll();

	// 각 Shape의 Bitmap을 생성해 둔다.
	if( m_pTemplate != NULL)
	{
		for( INT32 i = 0; i < m_pTemplate->getShapeCount(); i++)
		{
			i3UIShape * pShape = m_pTemplate->getShape( i);
			UISHAPE * pInfo = new UISHAPE;

			pInfo->m_bSelected = false;
			pInfo->m_idx = i;
			pInfo->m_pShape = pShape;
			pInfo->m_hBitmap = NULL;

			ExtractBitmap( pInfo);

			m_ShapeList.push_back( pInfo);
		}
	}

	AdjustView();

	Redraw();
}

void UIShapeListCtrl::Redraw(void)
{
	m_bModified = true;

	Invalidate();
}

INT32 UIShapeListCtrl::getItemCX(void) const
{
	return m_ViewSize + (_SPACE_ * 2);
}

INT32 UIShapeListCtrl::getItemCY(void) const
{
	return m_ViewSize + (_SPACE_ * 2) + _TEXT_SPACE;
}

void UIShapeListCtrl::getViewRange( INT32 * pStart, INT32 * pRowCount)
{
	INT32 itemCY = getItemCY();

	INT32 s = (m_ScrollPos / itemCY) * m_ColCount;

	*pStart = s;
	*pRowCount = (m_WndCY + itemCY - 1) / itemCY;
}

void UIShapeListCtrl::AdjustView(void)
{
	INT32 shapeCount = 0;
	INT32 itemCX = getItemCX();
	INT32 itemCY = getItemCY();

	if( m_pTemplate != NULL)
		shapeCount = m_pTemplate->getShapeCount();

	if( m_WndCX > 0)
		m_ColCount = (m_WndCX / itemCX);
	else
		m_ColCount = 0;

	if( m_ColCount > 0)
		m_RowCount = max( 0, (shapeCount + m_ColCount - 1) / m_ColCount);
	else
		m_RowCount = 0;

	// Scroll 영역 설정
	SCROLLINFO info;

	info.cbSize = sizeof( info);
	info.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	info.nMin = 0;
	info.nMax = (m_RowCount * itemCY);
	info.nPos = 0;
	info.nPage = m_WndCY;

	m_ScrollPos = 0;

	SetScrollInfo( SB_VERT, &info, TRUE);

	m_bModified = true;
}

void UIShapeListCtrl::Draw( void)
{
	CRect rt;
	HDC dc = m_DrawBoard.m_hDC;

	HFONT hOldFont = (HFONT) ::SelectObject( dc, m_hFont);

	// Background
	{
		rt.SetRect( 0, 0, m_WndCX, m_WndCY);

		::FillRect( dc, &rt, (HBRUSH) GetSysColorBrush( COLOR_3DSHADOW));
	}

	// Shapes
	if( m_pTemplate != NULL)
	{
		INT32 idxStart, rowCount;
		INT32 itemCX = getItemCX();
		INT32 itemCY = getItemCY();

		INT32 offset = -(m_ScrollPos % itemCY);

		getViewRange( &idxStart, &rowCount);

		for( INT32 i = 0, y = offset; i < rowCount; i++, y += itemCY)
		{
			for( INT32 j = 0, x = 0; j < m_ColCount; j++, x += itemCX)
			{
				INT32 idx = idxStart + (i * m_ColCount) + j;

				if( idx >= m_pTemplate->getShapeCount())
					break;

				UISHAPE * pInfo = m_ShapeList[idx];

				DrawShape( x, y, *pInfo);
			}
		}
	}

	::SelectObject( dc, hOldFont);
}

void UIShapeListCtrl::DrawShape( INT32 x, INT32 y, const UISHAPE & info)
{
	INT32 itemCX = getItemCX();
	INT32 itemCY = getItemCY();
	CRect rt;
	HDC dc = m_DrawBoard.m_hDC;
	HBRUSH hbrBack;

	::SetBkMode( dc, TRANSPARENT);

	if( info.m_bSelected)
	{
		// 선택 바탕 그리기.
		::SetTextColor( dc, GetSysColor( COLOR_HIGHLIGHTTEXT));
		hbrBack = (HBRUSH) ::GetSysColorBrush( COLOR_HIGHLIGHT);
	}
	else
	{
		::SetTextColor( dc, RGB( 0, 0, 0));

		hbrBack = (HBRUSH) ::GetSysColorBrush( COLOR_3DFACE);
	}

	rt.SetRect( x + _SPACE_, y + _SPACE_, x + itemCX - _SPACE_, y + itemCY - _SPACE_);
	::FillRect( dc, &rt, hbrBack);

	// Bitmap
	{
		HBITMAP hOldBmp = (HBITMAP) ::SelectObject( m_BmpDC, info.m_hBitmap);
		INT32 destX, destY;

		REAL32 aspect = (REAL32)info.m_bmpH / info.m_bmpW;

		INT32 viewBitmapCX = min( info.m_bmpW, m_ViewSize);
		INT32 viewBitmapCY = (INT32)(viewBitmapCX * aspect);

		destX = x + _SPACE_ + (m_ViewSize >> 1) - (viewBitmapCX >> 1);
		destY = y + _SPACE_ + (m_ViewSize >> 1) - (viewBitmapCY >> 1);

		BLENDFUNCTION blend;

		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.SourceConstantAlpha = 255;
		blend.AlphaFormat = AC_SRC_ALPHA;

		::AlphaBlend( dc, destX, destY, viewBitmapCX, viewBitmapCY, 
					m_BmpDC, 0, 0, info.m_bmpW, info.m_bmpH, blend);

		::SelectObject( m_BmpDC, hOldBmp);
	}

	// Texts
	{
		rt.left		= x + _SPACE_;
		rt.top		= y + _SPACE_ + m_ViewSize + _SPACE_;
		rt.right	= rt.left + m_ViewSize - _SPACE_;
		rt.bottom	= rt.top + _TEXT_SPACE;

		i3UIShape * pShape = m_pTemplate->getShape( info.m_idx);
		const char * pszName = m_pTemplate->getShapeInfoString( info.m_idx);

		char conv[256];

		INT32 shapeCX = (INT32) pShape->getWidth();
		INT32 shapeCY = (INT32) pShape->getHeight();

		if(( info.m_bmpW == shapeCX) && (info.m_bmpH == shapeCY))
		{
			sprintf_s( conv, _countof( conv), "%s\n(%dx%d)", pszName, info.m_bmpW, info.m_bmpH);
		}
		else
		{
			sprintf_s( conv, _countof( conv), "%s\n(%dx%d) (%dx%d)", pszName, info.m_bmpW, info.m_bmpH,
				shapeCX, shapeCY);
		}

		::DrawText( dc, conv, -1, &rt, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS);
	}
}

bool	UIShapeListCtrl::ExtractBitmap( UISHAPE * pInfo)
{
	i3UIShape * pShape = pInfo->m_pShape;
	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();

	INT32 x = (INT32)(pShape->getCoordU() + pShape->getTexCoordOffset()->x);
	INT32 y = (INT32)(pShape->getCoordV() + pShape->getTexCoordOffset()->y);
	INT32 w = (INT32)(pShape->getCoordW());
	INT32 h = (INT32)(pShape->getCoordH());

	if((w <= 0) || (h <= 0))
	{
		pInfo->m_hBitmap = NULL;
		pInfo->m_bmpW = 0;
		pInfo->m_bmpH = 0;
		return true;
	}

	if( pInfo->m_hBitmap != NULL)
	{
		::DeleteObject( pInfo->m_hBitmap);
		pInfo->m_hBitmap = NULL;
	}

	i3Texture * pTex = i3Texture::new_object();
	pTex->Create( w, h, 1, I3G_IMAGE_FORMAT_BGRA_8888, I3G_USAGE_FREQUENT_UPDATE);

	pVTex->ExtractTexture( pTex, x, y, w, h);

	pTex->Lock( 0);

	// 이미지 저장
	HDC dc = m_DrawBoard.m_hDC;

	HBITMAP hBitmap = ::CreateBitmap( w, h, GetDeviceCaps( dc, PLANES), GetDeviceCaps( dc, BITSPIXEL), pTex->GetLockedBuffer(0));
	I3ASSERT( hBitmap != NULL);

	pTex->Unlock(0);

	pTex->Release();

	pInfo->m_hBitmap = hBitmap;
	pInfo->m_bmpW = w;
	pInfo->m_bmpH = h;

	return true;
}

INT32 UIShapeListCtrl::getPickedShape( CPoint pt)
{
	INT32 itemCX = getItemCX();
	INT32 itemCY = getItemCY();

	INT32 y = m_ScrollPos + pt.y;

	INT32 idx = (y / itemCY) * m_ColCount;

	idx += (pt.x / itemCX);

	if( idx >= (INT32) m_ShapeList.size())
		idx = -1;

	return idx;
}

void UIShapeListCtrl::ExtractBitmapSelected(void)
{
	UINT32 i;

	for( i = 0; i < m_ShapeList.size(); i++)
	{
		UISHAPE * pInfo = m_ShapeList[i];

		if( pInfo->m_bSelected)
		{
			ExtractBitmap( pInfo);
		}
	}

	Redraw();
}

void UIShapeListCtrl::InvalidateShape( INT32 idx)
{
	I3_BOUNDCHK( (UINT32) idx, m_ShapeList.size());

	if( m_ColCount <= 0)
		return;

	INT32 itemCX = getItemCX();
	INT32 itemCY = getItemCY();

	INT32 _y = (idx / m_ColCount) * itemCY;
	INT32 _x = (idx % m_ColCount) * itemCX;

	CRect rt;

	rt.left = _x;
	rt.top = _y;
	rt.right = _x + itemCX;
	rt.bottom = _y + itemCY;

	InvalidateRect( &rt);
}

void UIShapeListCtrl::EnsureVisible( INT32 idx)
{
	I3_BOUNDCHK( (UINT32)idx, m_ShapeList.size());

	INT32 itemCY = getItemCY();
	INT32 y = (idx / m_ColCount) * itemCY;

	UpdateScrollBar( y);

	Redraw();
}

void UIShapeListCtrl::SelectShape( INT32 idx, bool bSelect)
{
	I3_BOUNDCHK( (UINT32) idx, m_ShapeList.size());

	UISHAPE * pInfo = m_ShapeList[idx];

	pInfo->m_bSelected = bSelect;

	m_bModified = true;
	InvalidateShape( idx);

	if( m_bNotifyUpdate)
	{
		if( bSelect)
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo->m_pShape);
		else
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pInfo->m_pShape);
	}
}

void UIShapeListCtrl::ToggleSelectShape( INT32 idx)
{
	I3_BOUNDCHK( (UINT32) idx, m_ShapeList.size());

	UISHAPE * pInfo = m_ShapeList[idx];

	pInfo->m_bSelected = !pInfo->m_bSelected;

	m_bModified = true;
	Invalidate();

	if( m_bNotifyUpdate)
	{
		if( pInfo->m_bSelected)
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo->m_pShape);
		else
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pInfo->m_pShape);
	}
}

void UIShapeListCtrl::UnselectAll(void)
{
	for( UINT32 i = 0; i < m_ShapeList.size(); i++)
	{
		UISHAPE * pInfo = m_ShapeList[i];

		pInfo->m_bSelected = false;
	}

	m_bModified = true;
	Invalidate();

	if( m_bNotifyUpdate)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, i3UIShape::static_meta());
	}
}

INT32 UIShapeListCtrl::GetShapeIndex( i3UIShape * pShape)
{
	for( UINT32 i = 0; i < m_ShapeList.size(); i++)
	{
		UISHAPE * pInfo = m_ShapeList[i];

		if( pInfo->m_pShape == pShape)
			return pInfo->m_idx;
	}

	return -1;
}

void UIShapeListCtrl::UpdateScrollBar( INT32 newPos)
{
	INT32 itemCY = getItemCY();
	INT32 maxPos = (m_RowCount - 1) * itemCY;

	newPos = max( 0, newPos);
	newPos = min( maxPos, newPos);

	m_ScrollPos = newPos;

	SetScrollPos( SB_VERT, newPos);
}

void UIShapeListCtrl::PopupMenu()
{
	if( m_pTemplate != NULL)
	{
		CMenu Menu, * pPopup;
		POINT pt;
		UINT32 cmd;

		GetCursorPos( &pt);

		{
			Menu.LoadMenu( IDR_MENU_SHAPELIST);
		}

		pPopup = Menu.GetSubMenu( 0);

		INT32 idx = getSelected();
		if( idx == -1)
		{
			pPopup->EnableMenuItem( ID_SHAPELIST_REMOVESHAPE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_EDITSHAPEINFO, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_MAKECLONE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);

			pPopup->EnableMenuItem( ID_SHAPELIST_SELECTTEXTURE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_SELECTUV, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}

		if( !i3::kind_of<i3UITemplate_User*>(m_pTemplate))
		{
			pPopup->EnableMenuItem( ID_SHAPELIST_REMOVESHAPE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_ADDSHAPE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_MAKECLONE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}

		cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
					pt.x, pt.y, (CWnd *) this, NULL);

		switch( cmd)
		{
			case ID_SHAPELIST_REMOVESHAPE:		RemoveShape( idx);	break;
			case ID_SHAPELIST_EDITSHAPEINFO:	EditShape( idx);	break;
			case ID_SHAPELIST_ADDSHAPE:			AddShape();	break;
			case ID_SHAPELIST_MAKECLONE:		MakeClone(idx);	break;

			case ID_SHAPELIST_SELECTTEXTURE :	SelectTexture();		break;
			case ID_SHAPELIST_SELECTUV :		SelectUV();			break;
		}
	}
}


INT32 UIShapeListCtrl::AddShape( void)
{
	I3ASSERT( m_pTemplate  != NULL);	// menu에 의해 호출 되는 부분이어서 절대로 template가 null이면 안된다.

	CUIEShapePropertyDlg dlg;

	REAL32 width = 100.f;
	REAL32 height = 100.f;

	if( m_pTemplate->getShapeCount() > 0)
	{
		i3UIShape * pShape = m_pTemplate->getShape( m_pTemplate->getShapeCount() -1);		

		width = pShape->getWidth();
		height = pShape->getHeight();
	}

	if( dlg.Execute( width, height) == true)
	{
		INT32 _addcount = dlg.GetShapeCount();
		i3UITemplate_User* pTempUser = (i3UITemplate_User*)m_pTemplate;
		

		i3Texture * pEmptyImg = UIGlobalRes::getTexture( UITEXTURE_EMPTY);
		I3ASSERT( pEmptyImg != NULL);

		i3VirtualTexture * pVTex = i3UI::getVirtualTexture();

		if( pVTex == NULL)
		{
			I3TRACE("failed virtual texture check\n");
			return false;
		}

		INT32 offX = 0, offY = 0;
		i3VTFileFormat* pFF = pVTex->GetFileFormat();

		pFF->PlaceTexture( pEmptyImg, &offX, &offY);
		pFF->Flush();
		pFF->SaveRefTextureInfo();

		for(INT32 i=0; i<_addcount; i++)
		{
			I3UI_SHAPE_INFO * pInfo = new I3UI_SHAPE_INFO;	//	(I3UI_SHAPE_INFO*)i3MemAlloc( sizeof( I3UI_SHAPE_INFO));

			pInfo->m_pShape = i3UIShape::new_object();

			dlg.GetShapeInfo( pInfo);

			i3::pack::RECT rt;
			rt.left = rt.top = 0.00001f;
			rt.right = rt.bottom = 127.0f;

			pInfo->m_pShape->setTextureResPath(pEmptyImg->getResourceLoadPath());
			pInfo->m_pShape->SetTextureCoord( &rt);
			pInfo->m_pShape->setTexcoordOffset( offX, offY);
			
			pTempUser->AddNewShape( pInfo);

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pInfo->m_pShape);
		}
		resortTemplateCount(pTempUser);
		UpdateAll();
	}

	return 1;
}

void UIShapeListCtrl::resortTemplateCount(i3UITemplate_User * pUser)
{
	INT32 _count = pUser->getShapeCount();
	for(INT32 i=0; i<_count; i++)
	{
		I3UI_SHAPE_INFO * pInfo = pUser->getShapeInfo(i);
		char conv[MAX_PATH] = {0,};
		::itoa(i+1, conv, 10);
		pInfo->m_strInfoString = conv;
	}
}

INT32 UIShapeListCtrl::RemoveShape( INT32 idx)
{
	I3ASSERT( m_pTemplate  != NULL);	// menu에 의해 호출 되는 부분이어서 절대로 template가 null이면 안된다.

	if( idx >= m_pTemplate->getShapeCount() || idx < 0)
		return -1;

	if( i3::kind_of<i3UITemplate_User*>( m_pTemplate))
	{
		i3UITemplate_User * pUserTemplate = (i3UITemplate_User *) m_pTemplate;

		I3UI_SHAPE_INFO * pInfo = ((i3UITemplate_User*)m_pTemplate)->getShapeInfo(idx);

		i3UIShape * pShape = pInfo->m_pShape;

		I3_SAFE_ADDREF( pShape);			// Update system이 Broadcast를 하는 동안 삭제되지 않도록...

		pUserTemplate->RemoveShape( pInfo);

		UpdateAll();

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REMOVE, pShape);

		I3_MUST_RELEASE( pShape);			// 삭제
	}

	return 1;
}

INT32 UIShapeListCtrl::EditShape( INT32 idx)
{
	I3ASSERT( m_pTemplate  != NULL);	// menu에 의해 호출 되는 부분이어서 절대로 template가 null이면 안된다.

	I3UI_SHAPE_INFO * pInfo = NULL;

	if( i3::kind_of<i3UITemplate_User*>(m_pTemplate)) //->IsTypeOf( i3UITemplate_User::static_meta()) )
	{
		pInfo = ((i3UITemplate_User*)m_pTemplate)->getShapeInfo(idx);
		if( pInfo != NULL)
		{
			CUIEShapePropertyDlg dlg;
			dlg.setShapeInfo( pInfo);
			if( dlg.DoModal() == IDOK)
			{
				// 버튼 이미지셋의 경우 데이타가 리셋되는 버그가 있어 예외 처리 추가
				if( i3::kind_of<i3UITemplate_ButtonImageSet*>(m_pTemplate)) //->IsTypeOf( i3UITemplate_ButtonImageSet::static_meta() ) )
				{
					CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
					CView* pView = frame->GetActiveView();

					if( pView )
					{
						if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)) )
						{
							m_pTemplate->AdjustLayout( pInfo->m_pShape->getWidth(), pInfo->m_pShape->getHeight() );
						}
					}	
				}

				UpdateAll();
				
				i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pInfo->m_pShape);
			}
		}
	}
	else
	{
		I3UI_SHAPE_INFO shapeInfo;
		shapeInfo.m_strInfoString.clear();
		shapeInfo.m_pShape			= m_pTemplate->getShape(idx);

		CUIEShapePropertyDlg dlg;
		dlg.setOnlyRectMode( true);
		dlg.setShapeInfo( &shapeInfo);
		if( dlg.DoModal() == IDOK)
		{
			UpdateAll();

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, shapeInfo.m_pShape);
		}
	}

	return 1;
}

INT32 UIShapeListCtrl::MakeClone( INT32 idx)
{
	I3_BOUNDCHK( idx, m_pTemplate->getShapeCount());

	I3UI_SHAPE_INFO	* pSrcInfo = ((i3UITemplate_User*)m_pTemplate)->getShapeInfo(idx);
	
	I3UI_SHAPE_INFO * pInfo = new I3UI_SHAPE_INFO; //(I3UI_SHAPE_INFO*)i3MemAlloc( sizeof( I3UI_SHAPE_INFO));
	pInfo->m_pShape = i3UIShape::new_object();

	{
		pSrcInfo->m_pShape->CopyTo( pInfo->m_pShape, I3_COPY_INSTANCE);
		pInfo->m_strInfoString = pSrcInfo->m_strInfoString;
	}

	INT32 ret = ((i3UITemplate_User*)m_pTemplate)->AddNewShape( pInfo);

	UpdateAll();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pInfo->m_pShape);

	return ret;
}

INT32 UIShapeListCtrl::MoveShape( INT32 nSel, INT32 nMove )
{
	return 0;
}

void UIShapeListCtrl::SelectTexture(void)
{
	m_pEditor->SelectTexture();

	ExtractBitmapSelected();
}

void UIShapeListCtrl::SelectUV( void)
{
	m_pEditor->SelectUV();

	ExtractBitmapSelected();
}

BEGIN_MESSAGE_MAP(UIShapeListCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()	
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// UIShapeListCtrl 메시지 처리기입니다.
int UIShapeListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		HDC dc = ::GetDC( m_hWnd);

		m_BmpDC = ::CreateCompatibleDC( dc);

		::ReleaseDC( m_hWnd, dc);
	}

	{
		m_hFont = i3TDK::CreateDefFont( -11, "Tahoma");
	}

	i3TDK::RegisterUpdate( m_hWnd, i3UIShape::static_meta(), I3_TDK_UPDATE_ALL);

	return 0;
}

void UIShapeListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rt = dc.m_ps.rcPaint;

	if( m_bModified)
	{
		Draw();

		m_bModified = false;
	}

	if( m_DrawBoard.m_hDC != NULL)
	{
		::BitBlt( dc.m_hDC, rt.left, rt.top, rt.Width(), rt.Height(), 
			m_DrawBoard.m_hDC, rt.left, rt.top, SRCCOPY);
	}
}

BOOL UIShapeListCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void UIShapeListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	INT32 idxSel = getPickedShape( point);

	if( nFlags & MK_CONTROL)
	{
		if( idxSel != -1)
			ToggleSelectShape( idxSel);
	}
	else
	{
		UnselectAll();

		if( idxSel != -1)
			SelectShape( idxSel);
	}

	SetFocus();

	CWnd::OnLButtonDown(nFlags, point);
}

void UIShapeListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
}

void UIShapeListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	INT32 idxSel = getPickedShape( point);

	if( idxSel != -1)
	{
		EditShape( idxSel);
	}
}

void UIShapeListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	PopupMenu();

	CWnd::OnRButtonDown(nFlags, point);
}

void UIShapeListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);
}

void UIShapeListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
}

BOOL UIShapeListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( nFlags & MK_CONTROL)
	{
		// Zoom
		m_ViewSize += (INT32)( zDelta * 0.1f);

		m_ViewSize = max( MIN_VIEWSIZE, m_ViewSize);
		m_ViewSize = min( MAX_VIEWSIZE, m_ViewSize);

		AdjustView();
		Redraw();
	}
	else
	{
		m_ScrollPos += (INT32)(-zDelta * 0.2f);

		UpdateScrollBar( m_ScrollPos);

		Redraw();
	}

	return TRUE;
}

void UIShapeListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	INT32 itemCY = getItemCY();
	INT32 maxPos = (m_RowCount - 1) * itemCY;
	INT32 page = (m_WndCY / itemCY) * itemCY;

	switch( nSBCode)
	{
		case SB_TOP :
			m_ScrollPos = 0;
			break;

		case SB_BOTTOM :
			m_ScrollPos = maxPos;
			break;

		case SB_LINEDOWN :
			m_ScrollPos = min( maxPos, m_ScrollPos + 1);
			break;

		case SB_LINEUP :
			m_ScrollPos = max( 0, m_ScrollPos - 1);
			break;

		case SB_PAGEDOWN :
			m_ScrollPos = min( maxPos, m_ScrollPos + page);
			break;

		case SB_PAGEUP :
			m_ScrollPos = max( 0, m_ScrollPos - page);
			break;

		case SB_THUMBPOSITION :
		case SB_THUMBTRACK :
			m_ScrollPos = nPos;
			break;
	}

	Redraw();

	SetScrollPos( SB_VERT, m_ScrollPos);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void UIShapeListCtrl::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	I3ASSERT( i3::kind_of<i3UIShape *>( pInfo->m_pMeta));

	// SelectShape() 등의 함수 내부에서 i3TDK::Update를 호출해서 중복적으로 Notification이 발생하는 것을 방지.
	m_bNotifyUpdate = false;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( pInfo->m_pObject != NULL)
		{
			INT32 idx = getShapeIndex( (i3UIShape *) pInfo->m_pObject);

			EnsureVisible( idx);
			SelectShape( idx, true);
		}
		else
		{
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_UNSELECT)
	{
		if( pInfo->m_pObject != NULL)
		{
			INT32 idx = getShapeIndex( (i3UIShape *) pInfo->m_pObject);

			SelectShape( idx, false);
		}
		else
		{
			UnselectAll();
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		if( pInfo->m_pObject != NULL)
		{
			INT32 idx = GetShapeIndex( (i3UIShape *) pInfo->m_pObject);

			if( idx != -1)
			{
				UISHAPE * pInfo2 = m_ShapeList[idx];

				ExtractBitmap( pInfo2);
			}
		}

		Draw();
		Invalidate();
	}

	m_bNotifyUpdate = true;
}

LRESULT UIShapeListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void UIShapeListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( (cx > 0) && (cy > 0))
	{
		m_WndCX = cx;
		m_WndCY = cy;

		m_DrawBoard.Create( m_hWnd, cx, cy);

		AdjustView();
	}
}


///////////////////////////////////////////////////////////////////////////////
void UISHAPE::SetUV( INT32 u, INT32 v, INT32 w, INT32 h)
{
	I3ASSERT( m_pShape != NULL);

	i3::pack::RECT rt;

	rt.left		= (REAL32) 0.0f;
	rt.top		= (REAL32) 0.0f;
	rt.right	= (REAL32) w;
	rt.bottom	= (REAL32) h;

	m_pShape->SetTextureCoord( &rt);

	m_pShape->setTexcoordOffset( u, v);

	m_bmpW = w;
	m_bmpH = h;
}
