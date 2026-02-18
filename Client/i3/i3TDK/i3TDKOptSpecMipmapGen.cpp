// i3TDKOptSpecMipmapGen.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKOptSpecMipmapGen.h"
#include ".\i3tdkoptspecmipmapgen.h"
#include "i3Base/string/ext/atoi.h"

// i3TDKOptSpecMipmapGen dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecMipmapGen, i3TDKOptSpecBase)

void i3TDKOptSpecMipmapGen::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_LIMITSIZE, m_CHK_LimitSize);
	DDX_Control(pDX, IDC_ED_LIMITCX, m_ED_MinX);
	DDX_Control(pDX, IDC_ED_LIMITCY, m_ED_MinY);
	DDX_Control(pDX, IDC_CHK_LIMITLEVEL, m_CHK_LimitLevel);
	DDX_Control(pDX, IDC_ED_MAXLEVEL, m_ED_MaxLevel);
	DDX_Control(pDX, IDC_TDK_CB_FILTER, m_CB_Filter);
	DDX_Control(pDX, IDC_TDK_CHK_DITHER, m_CHK_Dither);
	DDX_Control(pDX, IDC_TDK_CHK_SRGB_IN, m_CHK_SRGBIn);
	DDX_Control(pDX, IDC_TDK_CHK_SRGB_OUT, m_CHK_SRGBOut);
	DDX_Control(pDX, IDC_TDK_CHK_MIRROR_U, m_CHK_MirrorU);
	DDX_Control(pDX, IDC_TDK_CHK_MIRROR_V, m_CHK_MirrorV);
	DDX_Control(pDX, IDC_TDK_CHK_MIRROR_W, m_CHK_MirrorW);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpecMipmapGen, i3TDKOptSpecBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_FILTER, OnCbnSelchangeCbFilter)
	ON_BN_CLICKED(IDC_CHK_LIMITSIZE, OnBnClickedChkLimitsize)
	ON_BN_CLICKED(IDC_CHK_LIMITLEVEL, OnBnClickedChkLimitlevel)
END_MESSAGE_MAP()


// i3TDKOptSpecMipmapGen message handlers

inline void setchk( bool bState, CButton * pButton)
{
	if( bState)
		pButton->SetCheck( BST_CHECKED);
	else
		pButton->SetCheck( BST_UNCHECKED);
}

inline bool getchk( CButton * pButton)
{
	return pButton->GetCheck() == BST_CHECKED;
}

BOOL i3TDKOptSpecMipmapGen::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if( m_pOpt != nullptr)
	{
		TCHAR conv[256];

		i3OptMipmapGen * pOpt = (i3OptMipmapGen *) m_pOpt;

		// Limit Size
		setchk( pOpt->getLimitSizeState(), &m_CHK_LimitSize );

		// MinX
		i3::snprintf( conv, 256, _T("%d"), pOpt->getMinWidth());
		m_ED_MinX.SetWindowText( conv);
		m_ED_MinX.EnableWindow( pOpt->getLimitSizeState() == true);

		// MinY
		i3::snprintf( conv, 256, _T("%d"), pOpt->getMinHeight());
		m_ED_MinY.SetWindowText( conv);
		m_ED_MinY.EnableWindow( pOpt->getLimitSizeState() == true);

		// Limit Level
		setchk( pOpt->getLimitLevelState(), &m_CHK_LimitLevel);

		// Max Level
		i3::snprintf( conv, 256, _T("%d"), pOpt->getLimitMaxLevel());
		m_ED_MaxLevel.SetWindowText( conv);
		m_ED_MaxLevel.EnableWindow( pOpt->getLimitLevelState() == true);

		// Filter 항목 추가
		{
			// i3OptMipmapGen클래스는 TCHAR에 종속되선 안되므로 그냥 char/wchar_t구현을 별도 처리한다.


			for(INT32 i = 0; i < pOpt->getFilterCount(); i++)
			{
#ifdef _UNICODE
				i3::stack_wstring wstrFilter;	i3::mb_to_utf16(pOpt->getFilterName(i), wstrFilter);
				const wchar_t* strFilter = wstrFilter.c_str();
#else
				const char* strFilter = pOpt->getFilterName(i);
#endif
				m_CB_Filter.AddString( strFilter );
			}


		}

		// Filter 선택
		m_CB_Filter.SetCurSel( pOpt->getFilter());

		BOOL bD3DX = (pOpt->getFilter() >= i3OptMipmapGen::FILTER_D3DX_POINT) && 
					(pOpt->getFilter() <= i3OptMipmapGen::FILTER_D3DX_BOX);

		// Dither
		setchk( pOpt->getDitherState(), &m_CHK_Dither);
		m_CHK_Dither.EnableWindow( bD3DX);

		// SRGB In
		setchk( pOpt->getSRGBInState(), &m_CHK_SRGBIn);
		m_CHK_SRGBIn.EnableWindow( bD3DX);

		// SRGB Out
		setchk( pOpt->getSRGBOutState(), &m_CHK_SRGBOut);
		m_CHK_SRGBOut.EnableWindow( bD3DX);

		// Mirror U
		setchk( pOpt->getMirrorUState(), &m_CHK_MirrorU);
		m_CHK_MirrorU.EnableWindow( bD3DX);

		// Mirror V
		setchk( pOpt->getMirrorVState(), &m_CHK_MirrorV);
		m_CHK_MirrorV.EnableWindow( bD3DX);

		// Mirror W
		setchk( pOpt->getMirrorWState(), &m_CHK_MirrorW);
		m_CHK_MirrorW.EnableWindow( bD3DX);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecMipmapGen::OnOK()
{
	if( m_pOpt != nullptr)
	{
		TCHAR conv[256];

		i3OptMipmapGen * pOpt = (i3OptMipmapGen *) m_pOpt;

		// Limit Size
		pOpt->setLimitSizeState( getchk( &m_CHK_LimitSize));

		// MinX
		m_ED_MinX.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR));
		pOpt->setMinWidth( i3::atoi( conv));

		// MinY
		m_ED_MinY.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR));
		pOpt->setMinHeight( i3::atoi( conv));

		// Limit Level
		pOpt->setLimitLevelState( getchk( &m_CHK_LimitLevel));

		// Max Level
		m_ED_MaxLevel.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR));
		pOpt->setLimitMaxLevel( i3::atoi( conv));

		// Filter 선택
		pOpt->setFilter( (i3OptMipmapGen::FILTER) m_CB_Filter.GetCurSel());

		// Dither
		pOpt->setDitherState( getchk( &m_CHK_Dither));

		// SRGB In
		pOpt->setSRGBInState( getchk( &m_CHK_SRGBIn));

		// SRGB Out
		pOpt->setSRGBOutState( getchk( &m_CHK_SRGBOut));

		// Mirror U
		pOpt->setMirrorUState( getchk( &m_CHK_MirrorU));

		// Mirror V
		pOpt->setMirrorVState( getchk( &m_CHK_MirrorV));

		// Mirror W
		pOpt->setMirrorWState( getchk( &m_CHK_MirrorW));
	}

	i3TDKOptSpecBase::OnOK();
}

void i3TDKOptSpecMipmapGen::OnCbnSelchangeCbFilter()
{
	INT32 filter = m_CB_Filter.GetCurSel();

	BOOL bD3DX = (filter >= i3OptMipmapGen::FILTER_D3DX_POINT) && 
					(filter <= i3OptMipmapGen::FILTER_D3DX_BOX);

	m_CHK_Dither.EnableWindow( bD3DX);
	m_CHK_SRGBIn.EnableWindow( bD3DX);
	m_CHK_SRGBOut.EnableWindow( bD3DX);
	m_CHK_MirrorU.EnableWindow( bD3DX);
	m_CHK_MirrorV.EnableWindow( bD3DX);
	m_CHK_MirrorW.EnableWindow( bD3DX);
}

void i3TDKOptSpecMipmapGen::OnBnClickedChkLimitsize()
{
	BOOL bState = getchk( &m_CHK_LimitSize) == true;

	m_ED_MinX.EnableWindow( bState);
	m_ED_MinY.EnableWindow( bState);
}

void i3TDKOptSpecMipmapGen::OnBnClickedChkLimitlevel()
{
	BOOL bState = getchk( &m_CHK_LimitLevel) == true;

	m_ED_MaxLevel.EnableWindow( bState);
}
