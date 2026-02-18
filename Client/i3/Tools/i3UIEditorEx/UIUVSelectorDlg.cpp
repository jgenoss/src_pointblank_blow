// UIUVSelectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "UIUVSelectorDlg.h"
#include "GlobalVariables.h"
#include "MainFrm.h"

// CUIUVSelectorDlg dialog
#define BORDER_MARGIN 8
#define LEFT_MARGIN		96

IMPLEMENT_DYNAMIC(CUIUVSelectorDlg, CDialog)

CUIUVSelectorDlg::CUIUVSelectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUIUVSelectorDlg::IDD, pParent)
{
	m_pTexture	= NULL;
	
	m_bHasInitialValue = false;
	m_u = m_v = m_w = m_h = 0.0f;
	
	i3Color::Set( &m_ColorKey, 0.0f, 0.0f, 0.0f, 0.0f);

	m_MemDC			= NULL;
	m_hBitmap		= NULL;

	m_ColorChannel	= COLOR_CHANNEL_RGBA;

	m_Zoom			= 1.0f;

	m_posScroll.x	= 0;
	m_posScroll.y	= 0;

	m_bLButtonDown = false;
	m_RButtonDown	= FALSE;
	m_RDragStart.x	= 0 ; 
	m_RDragStart.y	= 0 ; 

	m_bColorKeySelecting = false;
	m_bSelected		= false;

	m_pGrip.Create( CUIGripObjHandler_RECT::static_meta());

	i3::string_ncopy_nullpad( m_szWindowCaption, "UVSelect", MAX_PATH);

	m_bLButtonDownOutSide = false;
	m_LDragStart.x = m_LDragStart.y = 0;
	m_PrevPt.x = m_PrevPt.y = 0;

	m_bDblClk = false;
}

CUIUVSelectorDlg::~CUIUVSelectorDlg()
{
	I3_SAFE_RELEASE( m_pTexture);

	for( size_t i = 0;i < m_listRect.size(); ++i)
	{
		RECT * pRect = m_listRect[i];

		delete pRect;
	}
	m_listRect.clear();

	_DeleteImage();

	for( size_t i=0; i<m_SelectedUVList.size(); i++)
	{
		i3::pack::RECT* pRect = m_SelectedUVList[i];
		delete pRect;
	}
	m_SelectedUVList.clear();
}

void CUIUVSelectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, ID_SELKEY, m_btnSelColorKey);
	DDX_Control(pDX, IDC_ST_COLORKEY, m_stColor);
	DDX_Control(pDX, IDC_UVLEFT, m_edUVLeft);
	DDX_Control(pDX, IDC_UVTOP, m_edUVTop);
	DDX_Control(pDX, IDC_UVRIGHT, m_edUVRight);
	DDX_Control(pDX, IDC_UVBOTTOM, m_edUVBottom);
}


BEGIN_MESSAGE_MAP(CUIUVSelectorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND_RANGE( ID_VIEWOPTION_A, ID_VIEWOPTION_RGBA, &CUIUVSelectorDlg::OnViewOption)
	ON_WM_INITMENU()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(ID_SELKEY, &CUIUVSelectorDlg::OnBnClickedSelkey)
	ON_EN_KILLFOCUS(IDC_UVLEFT, &CUIUVSelectorDlg::OnEnKillfocusUVEdit)
	ON_EN_KILLFOCUS(IDC_UVTOP, &CUIUVSelectorDlg::OnEnKillfocusUVEdit)
	ON_EN_KILLFOCUS(IDC_UVRIGHT, &CUIUVSelectorDlg::OnEnKillfocusUVEdit)
	ON_EN_KILLFOCUS(IDC_UVBOTTOM, &CUIUVSelectorDlg::OnEnKillfocusUVEdit)
END_MESSAGE_MAP()


// CUIUVSelectorDlg message handlers

BOOL CUIUVSelectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rt;

		m_stColor.GetWindowRect( &rt);

		ScreenToClient( &rt);

		_LoadColorKey();

		m_stColorCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
		m_stColorCtrl.setColor( &m_ColorKey);
	}

	m_ImageAreaSize.SetSize( m_pTexture->GetWidth(), m_pTexture->GetHeight());

	RECT rect;
	CRect scrRect;

	SystemParametersInfo( SPI_GETWORKAREA, 0, &scrRect, 0);

	rect.left	 = max(( scrRect.Width() - (INT32)m_pTexture->GetWidth())/2 - BORDER_MARGIN - LEFT_MARGIN, 0) ;
	rect.top	 = max(( scrRect.Height() - (INT32)m_pTexture->GetHeight())/2 - BORDER_MARGIN, 0) ;

	rect.right	= min( rect.left + (INT32)m_pTexture->GetWidth() + BORDER_MARGIN * 2 + LEFT_MARGIN, scrRect.Width());
	rect.bottom	= min( rect.top + (INT32)m_pTexture->GetHeight() + BORDER_MARGIN * 2, scrRect.Height());

	if (rect.bottom - rect.top < 330)
		rect.bottom = rect.top + 330;

	AdjustWindowRect( &rect, GetStyle(), FALSE);

	rect.left = max( 0, rect.left);
	rect.top = max( 0, rect.top);
	rect.right = min( rect.right - rect.left, scrRect.Width());
	rect.bottom= min( rect.bottom - rect.top, scrRect.Height());

	SetWindowPos( NULL, rect.left, rect.top, rect.right, rect.bottom - 50, SWP_NOACTIVATE | SWP_NOZORDER);

	SetWindowText( LPCTSTR(m_szWindowCaption));

	_SetImage();

	_AnalysisImage( m_pTexture);

	m_pGrip.Reset();
	m_pGrip.SetScrollPos( m_posScroll);
	m_pGrip.SetZoomRate( m_Zoom);

	SetScroll();

	POINT pt;
	pt.x = m_Rect.left;
	pt.y = m_Rect.top;

	m_pGrip.SetOffsetPos( pt);

	if( m_bHasInitialValue )
	{
		i3::pack::RECT rt;
		rt.left	= m_u;
		rt.top	= m_v;
		rt.right	= m_u + m_w;
		rt.bottom	= m_v + m_h;

		_OnSelectUV( &rt);		//m_pGrip.Add( &m_SelectedUV);
		UpdateUVValueToEdit();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUIUVSelectorDlg::_DeleteImage()
{
	if( m_MemDC != NULL)
	{
		if( m_hBitmap != NULL)
		{
			SelectObject( m_MemDC, m_hOldBitmap);

			DeleteObject( m_hBitmap);

			m_hBitmap = NULL;
		}
		DeleteDC( m_MemDC);

		m_MemDC = NULL;
	}
}

void	CUIUVSelectorDlg::_SetImage()
{
	if(m_pTexture == NULL)	return ;

	_DeleteImage();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Bitmap
	BITMAPINFOHEADER fi;
	PVOID pRaster;
	BITMAPINFO ih;

	//
	INT32				Height			= - (INT32)m_pTexture->GetHeight();					//이미지가 거꾸로 들어와있는상태라height 는 - 
	UINT16				DestBitCount	= 32;

	i3mem::FillZero( &fi,sizeof(BITMAPINFOHEADER));
	fi.biSize		= sizeof(BITMAPINFOHEADER);
	fi.biBitCount	= DestBitCount;

	fi.biHeight		= Height;
	fi.biWidth		= m_pTexture->GetWidth();
	fi.biPlanes		= 1;
	fi.biCompression= BI_RGB;

	memcpy( &ih.bmiHeader,&fi,sizeof(BITMAPINFOHEADER));

	m_hBitmap = CreateDIBSection(NULL,&ih,DIB_RGB_COLORS,&pRaster,NULL,0);

	if( m_MemDC == NULL)
	{
		HDC dc;

		dc = ::GetDC( m_hWnd);

		m_MemDC = CreateCompatibleDC( dc);
		m_hOldBitmap = (HBITMAP) ::SelectObject( m_MemDC, m_hBitmap);

		::ReleaseDC( m_hWnd, dc);
	}
	
	char * src		= m_pTexture->Lock(0);
	char * pDest	= (char*)pRaster;

	i3Image_Convert( m_pTexture->GetFormat(), src, m_pTexture->GetWidth(), m_pTexture->GetHeight(), 1, m_pTexture->GetClut(0),
					 I3G_IMAGE_FORMAT_BGRA_8888, pDest, 4, NULL);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Create DC
	//For Image

	//if have alphachannel
	if( m_ColorChannel != COLOR_CHANNEL_RGB)
	{
		UINT8 * pPixel = (UINT8 *) pRaster;
		UINT32 i, j, back;

		for( i = 0 ; i < m_pTexture->GetHeight(); i++)
		{
			for( j = 0; j < m_pTexture->GetWidth(); j++)
			{
				switch( m_ColorChannel)
				{
					case COLOR_CHANNEL_RGBA :
						back = ((((i >> 3) ^ (j >> 3)) & 0x01) << 6) + 128;

						pPixel[0] = (UINT8)((((UINT32)pPixel[0] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						pPixel[1] = (UINT8)((((UINT32)pPixel[1] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						pPixel[2] = (UINT8)((((UINT32)pPixel[2] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						break;

					case COLOR_CHANNEL_R :	*((UINT32 *) pPixel)  &= 0xFFFF0000;	break;
					case COLOR_CHANNEL_G :	*((UINT32 *) pPixel)  &= 0xFF00FF00;	break;
					case COLOR_CHANNEL_B :	*((UINT32 *) pPixel)  &= 0xFF0000FF;	break;
					case COLOR_CHANNEL_A :
						pPixel[0] = pPixel[3];
						pPixel[1] = pPixel[3];
						pPixel[2] = pPixel[3];
						break;
				}

				pPixel += 4;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pTexture->Unlock(0);
}

void CUIUVSelectorDlg::GetRects( CRect * pViewRect, CRect * pImageRect)
{
	//////////////////////////////////////////////////////////
	// source image의 위치와 destination의 zoom시 엇갈림을 줄이기위한 계산
	INT32 IX1 = (int)(m_posScroll.x / m_Zoom);
	INT32 IY1 = (int)(m_posScroll.y / m_Zoom);

	INT32 IW = (INT32)( m_Rect.Width() / m_Zoom);
	INT32 IH = (INT32)( m_Rect.Height() / m_Zoom);

	INT32 IX2 = min( m_ImageAreaSize.cx, IX1 + IW);
	INT32 IY2 = min( m_ImageAreaSize.cy, IY1 + IH);

	pImageRect->SetRect( IX1, IY1, IX2, IY2);

	///////////////////////////////////////////////////////////////////////
	// destination image의 뿌려질 시작위치에 대한 계산
	m_posStart.x = BORDER_MARGIN + LEFT_MARGIN;
	m_posStart.y = BORDER_MARGIN;

	pViewRect->left		= m_posStart.x;
	pViewRect->top		= m_posStart.y;
	pViewRect->right	= (INT32)( pViewRect->left	+ ((IX2 - IX1) * m_Zoom));
	pViewRect->bottom	= (INT32)( pViewRect->top	+ ((IY2 - IY1) * m_Zoom));
}

void CUIUVSelectorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rcView, rcImage;

	GetRects( &rcView, &rcImage);

	///////////////////////////////////////////////////////////////////////
	// 준비햇으면 그려..
	::StretchBlt(dc.m_hDC,
						rcView.left, rcView.top,
						rcView.Width(), rcView.Height(),
						m_MemDC,
						rcImage.left, rcImage.top,
						rcImage.Width(), rcImage.Height(),
						SRCCOPY);

	HGDIOBJ hOldBrush	= dc.SelectStockObject(NULL_BRUSH);
	HGDIOBJ hOldPen		= dc.SelectStockObject(WHITE_PEN);
	
	{
		//LButtonDown 처리
		//화면 깜빡임을 줄이기 위해 이 동안에는 shape의 흰색 테두리와 
		//선택되어있는 shape의 grip을 그리지 않습니다. 
		if( m_bLButtonDownOutSide)
		{
			//선택중인 영역 그리기
			HGDIOBJ prevPen = dc.SelectObject( UIGlobalRes::getPen( COLORSET_GRIP_NORMAL_BORDER));
			::Rectangle( dc.m_hDC, m_LDragStart.x, m_LDragStart.y, m_PrevPt.x, m_PrevPt.y);
			dc.SelectObject( prevPen);
		}
		else
		{

			POINT offSet;
			offSet.x = m_posStart.x - m_posScroll.x;
			offSet.y = m_posStart.y - m_posScroll.y;

			if( m_bSelected)
			{
				//선택되어있는 grip 그리기
				m_pGrip.SetOffsetPos( offSet);
				m_pGrip.Draw( dc.m_hDC);
			}
			else
			{
				//shape의 흰색 테두리
				
				for(size_t i = 0;i < m_listRect.size(); ++i)
				{
					RECT *pRt =m_listRect[i];

					::Rectangle( dc.m_hDC, offSet.x + (INT32)(pRt->left * m_Zoom),
											offSet.y + (INT32)(pRt->top * m_Zoom), 
											offSet.x + (INT32)(pRt->right * m_Zoom), 
											offSet.y + (INT32)(pRt->bottom * m_Zoom));
				}
			}
		}
	}

	// 바탕 그리기
	{
		CRect rt;
		HBRUSH hBrush = ::GetSysColorBrush( COLOR_3DFACE);

		// 왼쪽 Panel 영역
		rt.SetRect( 0, 0, LEFT_MARGIN, m_WndSize.cy);
		::FillRect( dc.m_hDC, &rt, hBrush);


		// 짙은 회색 테두리
		hBrush = (HBRUSH) ::GetStockObject( DKGRAY_BRUSH);
		
		rt.SetRect( LEFT_MARGIN, 0, rcView.left, m_WndSize.cy);		// 왼쪽
		::FillRect( dc.m_hDC, &rt, hBrush);

		rt.SetRect( LEFT_MARGIN, 0, m_WndSize.cx, rcView.top);		// 위쪽
		::FillRect( dc.m_hDC, &rt, hBrush);

		rt.SetRect( rcView.right, 0, m_WndSize.cx, m_WndSize.cy);		// 오른쪽
		::FillRect( dc.m_hDC, &rt, hBrush);

		rt.SetRect( LEFT_MARGIN, rcView.bottom, m_WndSize.cx, m_WndSize.cy);		// 아래쪽
		::FillRect( dc.m_hDC, &rt, hBrush);
	}

	dc.SelectObject( hOldPen);
	dc.SelectObject( hOldBrush);
}

void CUIUVSelectorDlg::setInitialTexCoord( REAL32 u, REAL32 v, REAL32 w, REAL32 h)
{
	m_bHasInitialValue = true;
	
	m_u = u;
	m_v = v;

	m_w = w;
	m_h = h;
}

void CUIUVSelectorDlg::SetTextureCoordinate( i3UIShape * pShape)
{
	if((m_bSelected) && (m_SelectedUVList.size() > 0))
	{
		//Last Item
		i3::pack::RECT* pRt = m_SelectedUVList[m_SelectedUVList.size()-1 ];

		pShape->setCoordU( pRt->left);
		pShape->setCoordV( pRt->top);

		// gdi 및 direct의 특성상 right와 bottom은 그려지지 않는 픽셀로 간주되기 때문에,
		// 폭과 높이 계산시 1을 더하지 않습니다.
		//pShape->setCoordW( m_SelectedUV.right - m_SelectedUV.left + 1);
		//pShape->setCoordH( m_SelectedUV.bottom - m_SelectedUV.top + 1);

		pShape->setCoordW( pRt->right - pRt->left);
		pShape->setCoordH( pRt->bottom - pRt->top);
	}
}

//CUIUVSelectorDlg::SetTextureCoordinate()의 변형
void CUIUVSelectorDlg::SetTextureCoordinateFromFirstUV( i3UIShape* pShape)
{
	if( m_SelectedUVList.size() > 0)
	{
		i3::pack::RECT* pRt = m_SelectedUVList[0];

		pShape->setCoordU( pRt->left);
		pShape->setCoordV( pRt->top);
		pShape->setCoordW( pRt->right - pRt->left);
		pShape->setCoordH( pRt->bottom - pRt->top);
	}
}

INT32 CUIUVSelectorDlg::_FindBefore( POINT *pt, i3::vector<RECT*>& listRect)
{
	if( pt->x < 0 || pt->y < 0)
		return -1;

	
	for(size_t i = 0;i < m_listRect.size(); ++i)
	{
		RECT * pRT = m_listRect[i];

		if( ::PtInRect( pRT, *pt))
			return i;
	}

	return -1;
}

RECT*	CUIUVSelectorDlg::MakeNewRect(POINT pos, i3::vector<RECT*>& listRect)
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

	return pNewRT;
}

void CUIUVSelectorDlg::RectExpend(RECT *rt, RECT_EXPANDTYPE type)
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

bool CUIUVSelectorDlg::_isSameColor( COLORREAL * pCol1, COLORREAL * pKey)
{
	if( i3Math::abs( pCol1->r - pKey->r) < 0.0001f &&
		i3Math::abs( pCol1->g - pKey->g) < 0.0001f &&
		i3Math::abs( pCol1->b - pKey->b) < 0.0001f &&
		i3Math::abs( pCol1->a - pKey->a) < 0.0001f)
		return true;

	return false;
}

void  CUIUVSelectorDlg::_AnalysisPickImage(i3Texture * pTexture)
{
	size_t i, j;

	COLORREAL Col;
	pTexture->Lock(0);

	i3::vector<RECT*> listRect;
	listRect.clear();

	//0,0픽셀이 colorkey sample이되게함
	//pTexture->GetPixel(0, 0, &Col);
	//i3Color::Set(&m_ColorKey, Col.r, Col.g, Col.b, Col.a);

	//@eunil. 줌, 스크롤 관련없이 포인트 변환
	INT32 StartX = (LONG)((m_LDragStart.x + m_posStart.x + m_posScroll.x) / m_Zoom);
	INT32 StartY = (LONG)((m_LDragStart.y + m_posStart.y + m_posScroll.y) / m_Zoom);

	INT32 EndX = (LONG)((m_LDragMove.x + m_posStart.x + m_posScroll.x) / m_Zoom);
	INT32 EndY = (LONG)((m_LDragMove.y + m_posStart.y + m_posScroll.y) / m_Zoom);

	StartX = MINMAX(0, StartX, (INT32)m_pTexture->GetWidth() - 1);
	StartY = MINMAX(0, StartY, (INT32)m_pTexture->GetHeight() - 1);		// signed/unsigned mismatch

	EndX = MINMAX(0, EndX, (INT32)m_pTexture->GetWidth() - 1);
	EndY = MINMAX(0, EndY, (INT32)m_pTexture->GetHeight() - 1);		// signed/unsigned mismatch

	for (i = StartY; i < EndY; i++)
	{
		for (j = StartX; j < EndX; j++)
		{
			pTexture->GetPixel(j, i, &Col);

			if (_isSameColor(&Col, &m_ColorKey) == false)
			{
				POINT pos;
				pos.x = j;
				pos.y = i;

				INT32 nIndex = _FindBefore(&pos, listRect);

				if (nIndex == -1)
				{
					RECT* pRect = MakeNewRect(pos, listRect);

					//최종적으로 listRect에 push할때 값을 줄이는 부분이 있어서
					//충돌체크하는 범위도 같이 맞춰준다 쩝... (예전 알고리즘에 맞추다보니...)
					pos.x++;
					pos.y++;
					if (_FindBefore(&pos, m_listRect) == -1)
					{
						bool bState = true;
						while (bState)
						{
							bState = CalculRect(pTexture, pRect);

							if (!bState)
								ResultRect(pTexture, pRect);
						}
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

	for (i = 0; i < listRect.size(); i++)
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

	for ( i = 0; i < listRect.size(); i++)
	{
		RECT* rt = listRect[i];
		if (rt) 
			delete rt;
	}

	RECT* rt = new RECT;
	rt->left = temp.left;
	rt->top = temp.top;
	rt->right = temp.right;
	rt->bottom = temp.bottom;

	m_listRect.push_back(rt);
}

void CUIUVSelectorDlg::_AnalysisImage( i3Texture * pTexture)
{
	//@eunil. 이미지 탐색 알고리즘 변환
	for (size_t i = 0; i < m_listRect.size(); ++i)
	{
		RECT * pRect = m_listRect[i];

		delete pRect;
	}
	m_listRect.clear();

	size_t i, j;

	COLORREAL Col;
	pTexture->Lock(0);


	//0,0픽셀이 colorkey sample이되게함
	//pTexture->GetPixel(0, 0, &Col);
	//i3Color::Set(&m_ColorKey, Col.r, Col.g, Col.b, Col.a);

	for (i = 0; i < pTexture->GetHeight(); i++)
	{
		for (j = 0; j < pTexture->GetWidth(); j++)
		{
			pTexture->GetPixel(j, i, &Col);

			if (_isSameColor(&Col, &m_ColorKey) == false)
			{
				POINT pos;
				pos.x = j;
				pos.y = i;

				INT32 nIndex = _FindBefore(&pos, m_listRect);

				if (nIndex == -1)
				{
					RECT* pRect = MakeNewRect(pos, m_listRect);

					//최종적으로 listRect에 push할때 값을 줄이는 부분이 있어서
					//충돌체크하는 범위도 같이 맞춰준다 쩝... (예전 알고리즘에 맞추다보니...)
					pos.x++;
					pos.y++;
					if (_FindBefore(&pos, m_listRect) == -1)
					{
						bool bState = true;
						while (bState)
						{
							bState = CalculRect(pTexture, pRect);

							if (!bState)
								ResultRect(pTexture, pRect);
						}
					}
				}
			}
		}
	}
	pTexture->Unlock(0);

}

void CUIUVSelectorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_WndSize.SetSize( cx, cy);

	m_Rect.left = BORDER_MARGIN + LEFT_MARGIN;
	m_Rect.top = BORDER_MARGIN;
	m_Rect.right = cx - BORDER_MARGIN;
	m_Rect.bottom = cy - BORDER_MARGIN;

	if(m_pTexture != NULL)
		SetScroll();
}

void CUIUVSelectorDlg::OnViewOption(UINT id)
{
	COLOR_CHANNEL old = m_ColorChannel;

	switch (id)
	{
	case ID_VIEWOPTION_A:		m_ColorChannel = COLOR_CHANNEL_A;	break;
	case ID_VIEWOPTION_R:		m_ColorChannel = COLOR_CHANNEL_R;	break;
	case ID_VIEWOPTION_G:		m_ColorChannel = COLOR_CHANNEL_G;	break;
	case ID_VIEWOPTION_B:		m_ColorChannel = COLOR_CHANNEL_B;	break;
	case ID_VIEWOPTION_RGB:		m_ColorChannel = COLOR_CHANNEL_RGB;	break;
	case ID_VIEWOPTION_RGBA:	m_ColorChannel = COLOR_CHANNEL_RGBA;break;
	default:
		m_ColorChannel = COLOR_CHANNEL_RGBA;
		break;
	}

	if( m_ColorChannel != old)
	{
		_SetImage();
		InvalidateRect( &m_Rect);
	}
}

void CUIUVSelectorDlg::OnInitMenu(CMenu* pMenu)
{
	CDialog::OnInitMenu(pMenu);

	// TODO: Add your message handler code here
	UINT32 i;
	for(i = ID_VIEWOPTION_A ; i <= ID_VIEWOPTION_RGBA ; ++i)
	{
		if(i == (UINT32)(m_ColorChannel + ID_VIEWOPTION_A))		pMenu->CheckMenuItem(i , MF_CHECKED );
		else													pMenu->CheckMenuItem(i , MF_UNCHECKED );
	}
}


//스크롤의 한계값을 결정하여 스크롤
bool CUIUVSelectorDlg::Scroll(int nBar , int ScrollAmount , LONG& pos)
{
	int Min , Max;
	GetScrollRange(nBar , &Min , &Max);

	LONG posOld = pos;

	if( ScrollAmount == 0)	return false;
	if(Max == 100)		return false;	//range가 max == min이되어 스크롤 바가 사라지면 min = 0 , max = 100의 기본값을가진 
										//원래상태의 스크롤 값을 가지게되는것같다.
	
	if	   (pos + ScrollAmount >= Max)	{	pos = Max;	}
	else if(pos + ScrollAmount <= Min)	{	pos = Min;	}
	else								{	pos += ScrollAmount;	}

	SetScrollPos(nBar , pos, TRUE);

	if( posOld != pos)
		return true;

	return false;
}

void	CUIUVSelectorDlg::ResetScroll(int nBar , LONG& pos)
{
	pos = 0;
	SetScrollRange	(nBar , 0,0,TRUE);
	SetScrollPos	(nBar , 0,TRUE);
}

void	CUIUVSelectorDlg::SetScroll()
{
	INT32 viewCX = (INT32)(m_Rect.Width());
	INT32 viewCY = (INT32)(m_Rect.Height());

	INT32 imgCX = (INT32)(m_ImageAreaSize.cx * m_Zoom);
	INT32 imgCY = (INT32)(m_ImageAreaSize.cy * m_Zoom);
	

	//현재 출력될 이미지의 크리보다 윈도의 크기가 작으면 범위와 위치 초기화(max == min , pos = 0)
	//현재 출력될 이미지의 크기보다 윈도의 크기가 작으면 범위와 위치 계산
	//SB_HORZ////////////////////////////////////////////////////////////////////////////////////
	if( viewCX >= imgCX)
	{
		ResetScroll( SB_HORZ , m_posScroll.x);
		m_posScroll.x = 0;
	}
	else
	{
		SCROLLINFO info;

		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		info.nMax = imgCX;
		info.nMin = 0;
		info.nPage = viewCX;
		info.nPos = m_posScroll.x;

		SetScrollInfo( SB_HORZ, &info);
	}

	//SB_VERT////////////////////////////////////////////////////////////////////////////////////
	if( viewCY >= imgCY)
	{
		ResetScroll( SB_VERT , m_posScroll.y);
		m_posScroll.y = 0;
	}
	else
	{
		SCROLLINFO info;

		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		info.nMax = imgCY;
		info.nMin = 0;
		info.nPage = viewCY;
		info.nPos = m_posScroll.y;

		SetScrollInfo( SB_VERT, &info);
	}

	m_pGrip.SetScrollPos( m_posScroll);

	Invalidate();
}

void CUIUVSelectorDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = TRUE;

	::SetCapture(m_hWnd);

	m_RDragStart.x = point .x;
	m_RDragStart.y = point .y;

	CWnd::OnRButtonDown(nFlags, point);
}

void CUIUVSelectorDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = FALSE;
	::ReleaseCapture();

	CWnd::OnRButtonUp(nFlags, point);
}

void CUIUVSelectorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_RButtonDown == false && m_bLButtonDown == false)
	{
		point.Offset( - m_posStart.x, - m_posStart.y);
		HCURSOR hCursor = m_pGrip.getCursor( point);
		::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);
		point.Offset( 0, 0);
	}

	CPoint origPt = CPoint( point);

	if (m_bLButtonDown)
	{
		//Rect 그리기

		if (m_pGrip.OnMouseMove(origPt, nFlags))
		{
			if (origPt != m_LDragMove)
			{
				m_pGrip.RecalcLayout();
			}
		}

		m_LDragMove= origPt;
	}

	if(m_RButtonDown)
	{
		//이동한양 계산
		int dX = point.x - m_RDragStart.x;
		int dY = point.y - m_RDragStart.y;

		//이동한양만큼 스크롤
		bool bRedraw1 = Scroll(SB_HORZ ,  - dX , m_posScroll.x);
		bool bRedraw2 = Scroll(SB_VERT ,  - dY , m_posScroll.y);

		if( bRedraw1 || bRedraw2)
		{
			m_pGrip.SetScrollPos( m_posScroll);
			InvalidateRect( &m_Rect);
		}

		//현재위치 저장
		m_RDragStart.x = point .x;
		m_RDragStart.y = point .y;
	}

	if( m_bSelected )
	{
		// rect가 정상적인 위치에 그려지도록 startposition을 더한만큼을 offset에 설정해두었기 때문에 
		//startposition을 제거한 값을 gripmanager에 보내줘야 합니다.
		point.Offset( - m_posStart.x, - m_posStart.y);
		if( m_pGrip.OnMouseMove( point, nFlags))
		{
			InvalidateRect( &m_Rect);
		}
		point.Offset( 0,0);
	}

	if( m_bLButtonDownOutSide)
	{
		m_bLDrag = true;

		IsSelectingRects( &origPt);
		InvalidateRect( &m_Rect);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CUIUVSelectorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int ScrollAmount = 0;
	// TODO: Add your message handler code here and/or call default
	switch(nSBCode)
	{
		case SB_LINELEFT:		ScrollAmount -= 1;		break;
		case SB_LINERIGHT:		ScrollAmount += 1;		break;
		case SB_PAGELEFT:		ScrollAmount -= 10;		break;
		case SB_PAGERIGHT:		ScrollAmount += 10;		break;
		case SB_THUMBTRACK:		ScrollAmount  =	nPos - m_posScroll.x;	break;
	}

	//ScrollAmount만큼 스크롤
	if( Scroll(SB_HORZ , ScrollAmount , m_posScroll.x))
	{
		m_pGrip.SetScrollPos( m_posScroll);

		InvalidateRect( &m_Rect);
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CUIUVSelectorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int ScrollAmount = 0;
	// TODO: Add your message handler code here and/or call default
	switch(nSBCode)
	{
		case SB_LINEUP:			ScrollAmount -= 1;		break;
		case SB_LINEDOWN:		ScrollAmount += 1;		break;
		case SB_PAGEUP:			ScrollAmount -= 10;		break;
		case SB_PAGEDOWN:		ScrollAmount += 10;		break;
		case SB_THUMBTRACK:		ScrollAmount  =	nPos - m_posScroll.y;	break;
	}

	//ScrollAmount만큼 스크롤
	if( Scroll(SB_VERT , ScrollAmount , m_posScroll.y))
	{
		m_pGrip.SetScrollPos( m_posScroll);

		Invalidate( false);
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CUIUVSelectorDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	bool bCtrl = (GetKeyState( VK_CONTROL) & 0x8000) != 0;
	bool bShift = (GetKeyState( VK_SHIFT) & 0x8000) != 0;
	REAL32 zoomRate = 1.0f;

	if( bCtrl)
	{

		REAL32 oldZoom = m_Zoom;

		//zoom의 크기 결정
		{
			if(zDelta >= 0)	m_Zoom  *= 2.0f;
			if(zDelta < 0)	m_Zoom  *= 0.5f;

			if(m_Zoom <= 0.25)	m_Zoom = 0.25f;
			if(m_Zoom >= 4.0f)	m_Zoom = 4.0f;
		}

		zoomRate = m_Zoom / oldZoom;
		if( m_Zoom != oldZoom)
		{
			SetScroll();

			m_pGrip.SetZoomRate( m_Zoom);
		}
	}
	else
	{
		INT32 ScrollAmount = zDelta > 0 ? -30 : 30;

		if( bShift)		//HScroll
		{
			if( Scroll(SB_HORZ , ScrollAmount , m_posScroll.x))
			{
				m_pGrip.SetScrollPos( m_posScroll);

				InvalidateRect( &m_Rect);
			}
		}
		else			//VScroll
		{
			if( Scroll(SB_VERT , ScrollAmount , m_posScroll.y))
			{
				m_pGrip.SetScrollPos( m_posScroll);

				Invalidate( false);
			}
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CUIUVSelectorDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;

	//return CDialog::OnEraseBkgnd(pDC);
}

void CUIUVSelectorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = true;

	CPoint origPt = CPoint( point);
	
	m_bLButtonDrag = true;
	m_LDragStart = m_LDragMove = point;

	if( m_bColorKeySelecting == true)
	{
		// Color Picking
		if( m_pTexture == NULL)
		{
			m_bColorKeySelecting = false;
			
			CDialog::OnLButtonDown(nFlags, point);
			
			return;
		}

		INT32 X	= (LONG)(point.x / m_Zoom) - m_posStart.x + m_posScroll.x;
		INT32 Y = (LONG)(point.y / m_Zoom) - m_posStart.y + m_posScroll.y;

		X = MINMAX(0, X, (INT32)m_pTexture->GetWidth() - 1);		// signed/unsigned mismatch
		Y = MINMAX(0, Y, (INT32)m_pTexture->GetHeight() - 1);		// signed/unsigned mismatch

		m_pTexture->Lock(0);
		m_pTexture->GetPixel(X,Y,&m_ColorKey);
		m_pTexture->Unlock(0);

		m_stColorCtrl.setColor( &m_ColorKey);

		_SaveColorKey();

		_AnalysisImage( m_pTexture);

		InvalidateRect( &m_Rect);

		CDialog::OnLButtonDown(nFlags, point);

		m_bColorKeySelecting = false;
		m_btnSelColorKey.SetState( FALSE);

		return;
	}

	m_bLButtonDownOutSide = true;

	if( m_bSelected )
	{
		// rect가 정상적인 위치에 그려지도록 startposition을 더한만큼을 offset에 설정해두었기 때문에 
		//startposition을 제거한 값을 gripmanager에 보내줘야 합니다.
		point.Offset( - m_posStart.x, - m_posStart.y);
		I3TRACE( "CUIUVSelectorDlg::OnLButtonDown: %d, %d, %d, %d\n", point.x, point.y, m_posStart.x, m_posStart.y);
		if( m_pGrip.OnLButtonDown( point))
		{
			I3TRACE("TRUE\n");
			m_bLButtonDownOutSide = false;
		}
		point.Offset( 0,0);
		I3TRACE( "CUIUVSelectorDlg::OnLButtonDown: %d, %d, %d, %d\n", point.x, point.y, m_posStart.x, m_posStart.y);
	}

	if( m_bLButtonDownOutSide)
	{
		WillSelectRects( &origPt);
		::SetCapture( m_hWnd);
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CUIUVSelectorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//I3TRACE("CUIUVSelectorDlg::OnLButtonDblClk\n");

	m_bDblClk = true;
	
	bool bCtrl = (GetKeyState( VK_CONTROL) & 0x8000) != 0;

	if( !bCtrl)		//single-select
		_ClearUVList();		

	RECT rt;
	for( size_t i = 0;i < m_listRect.size(); ++i)
	{
		RECT * pRect = m_listRect[i];

		rt.left = m_posStart.x - m_posScroll.x + (LONG)(pRect->left * m_Zoom);
		rt.top = m_posStart.y - m_posScroll.y + (LONG)(pRect->top * m_Zoom);
		rt.right = m_posStart.x - m_posScroll.x + (LONG)(pRect->right * m_Zoom);
		rt.bottom = m_posStart.y - m_posScroll.y + (LONG)(pRect->bottom * m_Zoom);

		if( ::PtInRect( &rt, point))
		{
			i3::pack::RECT rtSelected;
			rtSelected.left = (REAL32)pRect->left;
			rtSelected.top = (REAL32)pRect->top;
			rtSelected.right = (REAL32)pRect->right;
			rtSelected.bottom = (REAL32)pRect->bottom;
			
			_OnSelectUV( &rtSelected);	//multi-select
			
			InvalidateRect( &m_Rect);

			return;
		}
	}

	InvalidateRect( &m_Rect);

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CUIUVSelectorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bLButtonDown = false;
	CPoint origPt = CPoint( point);

	//DblClk에서 들어오는 LButtonUp 처리
	if( m_bDblClk)
	{
		m_bDblClk = false;
		
		UpdateUVValueToEdit();
		CDialog::OnLButtonUp(nFlags, point);

		return;
	}

	//I3TRACE("CUIUVSelectorDlg::OnLButtonUp\n");

	if( m_bSelected)
	{
		// rect가 정상적인 위치에 그려지도록 startposition을 더한만큼을 offset에 설정해두었기 때문에 
		//startposition을 제거한 값을 gripmanager에 보내줘야 합니다.
		point.Offset( - m_posStart.x, - m_posStart.y);
		m_pGrip.OnLButtonUp( point);
		point.Offset( 0,0);
	}

	if( m_bLButtonDownOutSide)
	{
		if( m_bLDrag)
			DidSelectRects( &origPt);
		
		::ReleaseCapture();
		m_bLButtonDownOutSide = false;
		m_bLDrag = false;
	}

// 	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
// 		{
// 			_AnalysisPickImage(m_pTexture);
// 		}

	UpdateUVValueToEdit();
	CDialog::OnLButtonUp(nFlags, point);
}

void CUIUVSelectorDlg::_SaveColorKey(void)
{
	INT32 R,G,B,A;
	R = (INT32)( i3Color::GetR( &m_ColorKey) * 255.0f);
	G = (INT32)( i3Color::GetG( &m_ColorKey) * 255.0f);
	B = (INT32)( i3Color::GetB( &m_ColorKey) * 255.0f);
	A = (INT32)( i3Color::GetA( &m_ColorKey) * 255.0f);

	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Color Key Red"), R);
	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Color Key Green"), G);
	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Color Key Blue"), B);
	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Color Key Alpha"), A);
}

void CUIUVSelectorDlg::_LoadColorKey(void)
{
	INT32 R,G,B,A;

	R = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Color Key Red"), 0);
	G = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Color Key Green"), 0);
	B = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Color Key Blue"), 0);
	A = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Color Key Alpha"), 0);

	i3Color::Set( &m_ColorKey, (UINT8)R, (UINT8)G, (UINT8)B, (UINT8)A);
}

LRESULT CUIUVSelectorDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch( message)
	{
		case WM_TDK_SETCOLOR :
			{
				i3Color::Set( &m_ColorKey, m_stColorCtrl.getColor());

				_SaveColorKey();

				_AnalysisImage( m_pTexture);

				InvalidateRect( &m_Rect);
			}
			break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CUIUVSelectorDlg::OnBnClickedSelkey()
{
	m_bColorKeySelecting = true;

	m_btnSelColorKey.SetState( TRUE);
}

void CUIUVSelectorDlg::UpdateUVValueToEdit( void)
{
	if( m_SelectedUVList.size() > 0)
	{
		//Get Last Item
		i3::pack::RECT* pRt = m_SelectedUVList[m_SelectedUVList.size()-1];
		
		char conv[32];

		i3::snprintf( conv, sizeof( conv), "%.3f", pRt->left / m_Zoom);
		m_edUVLeft.SetWindowText( conv);

		i3::snprintf( conv, sizeof( conv), "%.3f", pRt->top / m_Zoom);
		m_edUVTop.SetWindowText( conv);

		i3::snprintf( conv, sizeof( conv), "%.3f", pRt->right / m_Zoom);
		m_edUVRight.SetWindowText( conv);

		i3::snprintf( conv, sizeof( conv), "%.3f", pRt->bottom / m_Zoom);
		m_edUVBottom.SetWindowText( conv);
	}
	else
	{
		m_edUVLeft.SetWindowText( "");
		m_edUVTop.SetWindowText( "");
		m_edUVRight.SetWindowText( "");
		m_edUVBottom.SetWindowText( "");
	}
}

void CUIUVSelectorDlg::OnEnKillfocusUVEdit()
{
	// TODO: Add your control notification handler code here
	if( m_SelectedUVList.size() > 0)
	{
		//Get Last Item
		i3::pack::RECT* pRt = m_SelectedUVList[m_SelectedUVList.size()-1];

		char conv[32];
		
		m_edUVLeft.GetWindowText( conv, sizeof(conv));
		pRt->left = (REAL32)atof( conv) * m_Zoom;
		
		m_edUVTop.GetWindowText( conv, sizeof(conv));
		pRt->top = (REAL32)atof( conv) * m_Zoom;
		
		m_edUVRight.GetWindowText( conv, sizeof(conv));
		pRt->right = (REAL32)atof( conv) * m_Zoom;
		
		m_edUVBottom.GetWindowText( conv, sizeof(conv));
		pRt->bottom = (REAL32)atof( conv) * m_Zoom;

		InvalidateRect( &m_Rect);
	}
	else
	{
		m_edUVLeft.SetWindowText( "");
		m_edUVTop.SetWindowText( "");
		m_edUVRight.SetWindowText( "");
		m_edUVBottom.SetWindowText( "");
	}
}

void CUIUVSelectorDlg::WillSelectRects( CPoint* startPt)
{
	m_LDragStart.x = startPt->x;
	m_LDragStart.y = startPt->y;
	m_PrevPt.x = startPt->x;
	m_PrevPt.y = startPt->y;
}

void CUIUVSelectorDlg::IsSelectingRects( CPoint* curPt)
{
	m_PrevPt.x = curPt->x;
	m_PrevPt.y = curPt->y;
}

void CUIUVSelectorDlg::DidSelectRects( CPoint* endPt)
{
	if( (GetKeyState( VK_CONTROL) & 0x8000) == 0)
	{
		_ClearUVList();
	}

	RECT selRt;
	selRt.left = min( m_LDragStart.x, m_PrevPt.x);
	selRt.top = min( m_LDragStart.y, m_PrevPt.y);
	selRt.right = max( m_LDragStart.x, m_PrevPt.x);
	selRt.bottom = max( m_LDragStart.y, m_PrevPt.y);

	for( size_t i = 0; i < m_listRect.size(); i++)
	{
		RECT * pRect = m_listRect[i];

		RECT rt;
		rt.left = m_posStart.x - m_posScroll.x + (LONG)(pRect->left * m_Zoom);
		rt.top = m_posStart.y - m_posScroll.y + (LONG)(pRect->top * m_Zoom);
		rt.right = m_posStart.x - m_posScroll.x + (LONG)(pRect->right * m_Zoom);
		rt.bottom = m_posStart.y - m_posScroll.y + (LONG)(pRect->bottom * m_Zoom);

		if( rt.left > selRt.left && rt.right < selRt.right)
		{
			if( rt.top > selRt.top && rt.bottom < selRt.bottom)
			{
				i3::pack::RECT rtSelected;
				rtSelected.left = (REAL32)pRect->left;
				rtSelected.top = (REAL32)pRect->top;
				rtSelected.right = (REAL32)pRect->right;
				rtSelected.bottom = (REAL32)pRect->bottom;

				_OnSelectUV( &rtSelected);
			}
		}
	}

	m_LDragStart.x = endPt->x;
	m_LDragStart.y = endPt->y;
	m_PrevPt.x = endPt->x;
	m_PrevPt.y = endPt->y;

	::InvalidateRect( m_hWnd, NULL, TRUE);
}

BOOL CUIUVSelectorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd* wnd = GetFocus();

		if( wnd == &m_edUVLeft || wnd == &m_edUVTop || wnd == &m_edUVRight || wnd == &m_edUVBottom)
		{
			//EditBox에서 EnterKey를 누르면 KillFocus
			this->SetFocus();
		}
		else
		{
			PostMessage(IDOK);
		}

		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CUIUVSelectorDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

void CUIUVSelectorDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

bool CUIUVSelectorDlg::CalculRect(i3Texture * pTexture, RECT *rt)
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
			if (_isSameColor(&Col, &m_ColorKey) == false)
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
			if (_isSameColor(&Col, &m_ColorKey) == false)
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
			if (_isSameColor(&Col, &m_ColorKey) == false)
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
		if (pTexture->GetWidth() > tempRect.right)
		{
			pTexture->GetPixel(tempRect.right, i, &Col);
			if (_isSameColor(&Col, &m_ColorKey) == false)
			{
				RectExpend(rt, RECTEXPAND_RIGHT);
				bExpand = true;
				break;
			}
		}
	}

	return bExpand;
}

void CUIUVSelectorDlg::ResultRect(i3Texture * pTexture, RECT *rt)
{
	COLORREAL Col;
	bool bState = false;

	RECT tempRect = *rt;

	//걸리는 픽셀이있다면
	for (INT32 i = tempRect.left; i < tempRect.right; i++)
	{
		if (tempRect.top > 0)
		{
			pTexture->GetPixel(i, tempRect.top, &Col);
			if (_isSameColor(&Col, &m_ColorKey) == false)
			{
				bState = true;
				break;
			}
		}
	}

	//한픽셀 줄이기
	if (!bState)
		rt->top++;

	bState = false;

// 	for (INT32 i = tempRect.left; i < tempRect.right; i++)
// 	{
// 		if (pTexture->GetHeight() > tempRect.bottom)
// 		{
// 			pTexture->GetPixel(i, tempRect.bottom, &Col);
// 			if (_isSameColor(&Col, &m_ColorKey) == false)
// 			{
// 				bState = true;
// 				break;
// 			}
// 		}
// 	}
// 
// 	if (!bState)
// 		rt->bottom--;
// 
// 	bState = false;

	for (INT32 i = tempRect.top; i < tempRect.bottom; i++)
	{
		if (tempRect.left > 0)
		{
			pTexture->GetPixel(tempRect.left, i, &Col);
			if (_isSameColor(&Col, &m_ColorKey) == false)
			{
				bState = true;
				break;
			}
		}
	}

	if (!bState)
		rt->left++;

	//bState = false;

	//우확장
// 	for (INT32 i = tempRect.top; i < tempRect.bottom; i++)
// 	{
// 		if (pTexture->GetWidth() > tempRect.bottom)
// 		{
// 			pTexture->GetPixel(tempRect.right, i, &Col);
// 			if (_isSameColor(&Col, &m_ColorKey) == false)
// 			{
// 				bState = true;
// 				break;
// 			}
// 		}
// 	}
// 
// 	if (!bState)
// 		rt->right--;

	m_listRect.push_back(rt);
}

bool _IsSameRect( const i3::pack::RECT* pRt1, const i3::pack::RECT* pRt2)
{
	if( pRt1->left == pRt2->left && pRt1->top == pRt2->top
		&& pRt1->right == pRt2->right && pRt1->bottom == pRt2->bottom)
		return true;
	
	return false;
}

//For multi-select
//if pRt doesn't exist, create instance of i3::pack::RECT and add to m_SelectedUVList, return that instance
//if pRt exists, delete instance of i3::pack::RECT and return pRt
i3::pack::RECT* CUIUVSelectorDlg::_OnSelectUV( i3::pack::RECT* pRt)
{
	for( size_t i=0; i<m_SelectedUVList.size(); i++)
	{
		i3::pack::RECT* pCur = m_SelectedUVList[i];

		if( _IsSameRect( pRt, pCur))
		{
			m_pGrip.RemoveSelect( pCur);
			
			delete pCur;
			i3::vu::erase_index(m_SelectedUVList, i);

			m_bSelected = ( m_SelectedUVList.size() > 0) ? true : false;
						
			return pRt;
		}
	}

	i3::pack::RECT* newRect = new i3::pack::RECT;
	*newRect = *pRt;
	newRect->left = newRect->left ;
	newRect->right = newRect->right ;
	newRect->bottom = newRect->bottom ;
	newRect->top = newRect->top ;

	m_SelectedUVList.push_back( newRect);
	m_pGrip.AddSelect( newRect, 0, false);

	m_bSelected = true;

	return newRect;
}

void CUIUVSelectorDlg::_ClearUVList( void)
{
	for( size_t i=0; i<m_SelectedUVList.size(); i++)
	{
		i3::pack::RECT* pCur = m_SelectedUVList[i];
		m_pGrip.RemoveSelect( pCur);
		delete pCur;
	}
	m_SelectedUVList.clear();

	m_bSelected = false;
}

void CUIUVSelectorDlg::UnSelectFirstUV( void)
{
	if( m_SelectedUVList.size() > 0)
	{
		i3::pack::RECT* pCur = m_SelectedUVList[0];
		delete pCur;
		i3::vu::erase_index(m_SelectedUVList, 0);
	}
}