// i3TDKColorPicker.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKColorPicker.h"
#include "i3TDKGlobalRes.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKUtilGDI.h"
#include ".\i3tdkcolorpicker.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/atof.h"
// i3TDKColorPicker dialog

IMPLEMENT_DYNAMIC(i3TDKColorPicker, CDialog)

void i3TDKColorPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_R, m_RCtrl);
	DDX_Control(pDX, IDC_TDK_ED_R2, m_R2Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_G, m_GCtrl);
	DDX_Control(pDX, IDC_TDK_ED_G2, m_G2Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_B, m_BCtrl);
	DDX_Control(pDX, IDC_TDK_ED_B2, m_B2Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_A, m_ACtrl);
	DDX_Control(pDX, IDC_TDK_ED_A2, m_A2Ctrl);
}

void i3TDKColorPicker::_FindBaseColor(void)
{
	m_PrimaryColorPickCtrl.setColor( &m_Color);
	m_PrimaryColorPickCtrl.getPrimaryColor( &m_BaseColor);

	m_RGBPickCtrl.setBaseColor( &m_BaseColor);
	m_RGBPickCtrl.setColor( &m_Color);

	m_AlphaPickCtrl.setColor( &m_Color);
}

bool i3TDKColorPicker::Execute( COLORREAL * pColor)
{
	i3Color::Set( &m_Color, pColor);
	i3Color::Set( &m_OrgColor, pColor);

	int rv;

	i3TDK::SetResInstance();

	rv = DoModal();

	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL)
	{
		return false;
	}

	return true;
}

void i3TDKColorPicker::_SelectColor( CWnd * pFromWnd, COLORREAL * pColor)
{
	TCHAR conv[128];

	i3Color::Set( &m_Color, pColor);

	if( pFromWnd != &m_RGBPickCtrl)
	{
		m_RGBPickCtrl.setColor( pColor);
	}

	if( pFromWnd != &m_RCtrl)
	{
		i3::snprintf( conv, 128, _T("%d"), i3TDK::intColor( pColor->r, false));
		m_RCtrl.SetWindowText( conv);

		i3::ftoa( pColor->r, conv);
		m_R2Ctrl.SetWindowText( conv);
	}

	if( pFromWnd != &m_GCtrl)
	{
		i3::snprintf( conv, 128, _T("%d"), i3TDK::intColor( pColor->g, false));
		m_GCtrl.SetWindowText( conv);

		i3::ftoa( pColor->g, conv);
		m_G2Ctrl.SetWindowText( conv);
	}

	if( pFromWnd != &m_BCtrl)
	{
		i3::snprintf( conv, 128, _T("%d"), i3TDK::intColor( pColor->b, false));
		m_BCtrl.SetWindowText( conv);

		i3::ftoa( pColor->b, conv);
		m_B2Ctrl.SetWindowText( conv);
	}

	if( pFromWnd != &m_ACtrl)
	{
		i3::snprintf( conv, 128, _T("%d"), i3TDK::intColor( pColor->a, false));
		m_ACtrl.SetWindowText( conv);

		i3::ftoa( pColor->a, conv);
		m_A2Ctrl.SetWindowText( conv);
	}

	if( pFromWnd != &m_AlphaPickCtrl)
	{
		m_AlphaPickCtrl.setColor( pColor);
	}

	if( pFromWnd != &m_ColorPreviewCtrl)
	{
		m_ColorPreviewCtrl.setColor( pColor);
	}
}

void i3TDKColorPicker::_SelectAlpha( CWnd * pFromWnd, COLORREAL * pColor)
{
	TCHAR conv[128];

	i3Color::Set( &m_Color, pColor);

	if( pFromWnd != &m_ACtrl)
	{
		i3::snprintf( conv, 128, _T("%d"), i3TDK::intColor( pColor->a));
		m_ACtrl.SetWindowText( conv);

		i3::ftoa( pColor->a, conv);
		m_A2Ctrl.SetWindowText( conv);
	}

	if( pFromWnd != &m_AlphaPickCtrl)
	{
		m_AlphaPickCtrl.setColor( pColor);
	}

	if( pFromWnd != &m_ColorPreviewCtrl)
	{
		m_ColorPreviewCtrl.setColor( pColor);
	}
}

BEGIN_MESSAGE_MAP(i3TDKColorPicker, CDialog)
	ON_EN_KILLFOCUS(IDC_TDK_ED_R2, OnEnKillfocusTdkEdR2)
	ON_EN_KILLFOCUS(IDC_TDK_ED_G2, OnEnKillfocusTdkEdG2)
	ON_EN_KILLFOCUS(IDC_TDK_ED_B2, OnEnKillfocusTdkEdB2)
	ON_EN_KILLFOCUS(IDC_TDK_ED_A2, OnEnKillfocusTdkEdA2)
	ON_EN_KILLFOCUS(IDC_TDK_ED_R, OnEnKillfocusTdkEdR)
	ON_EN_KILLFOCUS(IDC_TDK_ED_G, OnEnKillfocusTdkEdG)
	ON_EN_KILLFOCUS(IDC_TDK_ED_B, OnEnKillfocusTdkEdB)
	ON_EN_KILLFOCUS(IDC_TDK_ED_A, OnEnKillfocusTdkEdA)
END_MESSAGE_MAP()


// i3TDKColorPicker message handlers

BOOL i3TDKColorPicker::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( g_pTDKGlobalRes == nullptr)
	{
		i3TDKGlobalRes::Init();
	}

	{
		CRect rect;

		rect.left = 3;
		rect.top = 24;
		rect.right = rect.left + 256 + 2;
		rect.bottom = rect.top + 256 + 2;

		// RGB Pick Control
		m_RGBPickCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);

		// Primary Color Pick Control
		rect.left = rect.right + 4;
		rect.top  = rect.top - 1 - 4;
		rect.right = rect.left + 41;
		rect.bottom = rect.top + 270;

		m_PrimaryColorPickCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 101);

		rect.left = rect.right + 12;
		rect.right = rect.left + 41;

		m_AlphaPickCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 102);

		rect.left = rect.right + 12;
		rect.right = rect.left + 64;
		rect.bottom = rect.top + 64;

		m_ColorPreviewCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 103);
	}

	_FindBaseColor();

	_SelectColor( nullptr, &m_Color);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return false
}

void i3TDKColorPicker::OnOK()
{
	i3Color::Set( &m_Color, m_RGBPickCtrl.getColor());
	m_Color.a = m_AlphaPickCtrl.getAlpha();

	CDialog::OnOK();
}

void i3TDKColorPicker::OnCancel()
{
	i3Color::Set( &m_Color, &m_OrgColor);

	CDialog::OnCancel();
}


LRESULT i3TDKColorPicker::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_TDK_UPDATEPRIMARYCOLOR :
			{
				COLORREAL col;

				m_PrimaryColorPickCtrl.getPrimaryColor( &col);
				m_RGBPickCtrl.setBaseColor( &col);

				i3Color::Set( &col, m_RGBPickCtrl.getColor());
				m_AlphaPickCtrl.setColor( &col);
			}
			break;

		case WM_TDK_UPDATEALPHA :
			{
				COLORREAL col;

				i3Color::Set( &col, m_RGBPickCtrl.getColor());
				col.a = m_AlphaPickCtrl.getAlpha();

				_SelectAlpha( &m_AlphaPickCtrl, &col);
			}
			break;

		case WM_TDK_SELECTCOLOR:
			// 색상 변경
			{
				_SelectColor( &m_RGBPickCtrl, m_RGBPickCtrl.getColor());
			}
			break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void i3TDKColorPicker::OnEnKillfocusTdkEdR2()
{
	TCHAR conv[64];

	m_R2Ctrl.GetWindowText( conv, _countof(conv));

	m_Color.r = (REAL32) i3::atof( conv);

	_SelectColor( nullptr, &m_Color);
}

void i3TDKColorPicker::OnEnKillfocusTdkEdR()
{
	TCHAR conv[64];

	m_RCtrl.GetWindowText( conv, _countof(conv));

	m_Color.r = (REAL32) (i3::atoi( conv) / 255.0f);

	_SelectColor( nullptr, &m_Color);
}

void i3TDKColorPicker::OnEnKillfocusTdkEdG2()
{
	TCHAR conv[64];

	m_G2Ctrl.GetWindowText( conv, _countof(conv));

	m_Color.g = (REAL32) i3::atof( conv);

	_SelectColor( nullptr, &m_Color);
}

void i3TDKColorPicker::OnEnKillfocusTdkEdG()
{
	TCHAR conv[64];

	m_GCtrl.GetWindowText( conv, _countof(conv));

	m_Color.g = (REAL32) (i3::atoi( conv) / 255.0f);

	_SelectColor( nullptr, &m_Color);
}

void i3TDKColorPicker::OnEnKillfocusTdkEdB2()
{
	TCHAR conv[64];

	m_B2Ctrl.GetWindowText( conv, _countof(conv));

	m_Color.b = (REAL32) i3::atof( conv);

	_SelectColor( nullptr, &m_Color);
}


void i3TDKColorPicker::OnEnKillfocusTdkEdB()
{
	TCHAR conv[64];

	m_BCtrl.GetWindowText( conv, _countof(conv));

	m_Color.b = (REAL32) (i3::atoi( conv) / 255.0f);

	_SelectColor( nullptr, &m_Color);
}


void i3TDKColorPicker::OnEnKillfocusTdkEdA2()
{
	TCHAR conv[64];

	m_A2Ctrl.GetWindowText( conv, _countof(conv));

	m_Color.a = (REAL32) i3::atof( conv);

	_SelectColor( nullptr, &m_Color);
}

void i3TDKColorPicker::OnEnKillfocusTdkEdA()
{
	TCHAR conv[64];

	m_ACtrl.GetWindowText( conv, _countof(conv));

	m_Color.a = (REAL32) (i3::atoi( conv) / 255.0f);

	_SelectColor( nullptr, &m_Color);
}
