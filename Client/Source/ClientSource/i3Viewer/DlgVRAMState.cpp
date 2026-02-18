// DebugPanel.cpp : implementation file
//

#include "stdafx.h"

#if defined( I3_DEBUG)
#include "DlgVRAMState.h"
#include "DlgTexInspector.h"
#include "DlgVBInspector.h"
#include "DlgIBInspector.h"
#include "i3Base/string/ext/integral_to_comma_str.h"

// CDlgVRAMState dialog

IMPLEMENT_DYNAMIC(CDlgVRAMState, CDialog)

CDlgVRAMState::CDlgVRAMState(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVRAMState::IDD, pParent)
{

}

CDlgVRAMState::~CDlgVRAMState()
{
}

void CDlgVRAMState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_TOTALVRAM, m_ED_TotalVRAM);
	DDX_Control(pDX, IDC_ED_RT_MEM, m_ED_RTSize);
	DDX_Control(pDX, IDC_ED_RT_COUNT, m_ED_RTCount);
	DDX_Control(pDX, IDC_ED_TEX_COUNT, m_ED_TexCount);
	DDX_Control(pDX, IDC_ED_TEX_MEM, m_ED_TexSize);
	DDX_Control(pDX, IDC_ED_VB_COUNT, m_ED_VBCount);
	DDX_Control(pDX, IDC_ED_VB_MEM, m_ED_VBSize);
	DDX_Control(pDX, IDC_ED_IB_COUNT, m_ED_IBCount);
	DDX_Control(pDX, IDC_ED_IB_MEM, m_ED_IBSize);
	DDX_Control(pDX, IDC_ED_SHADER_COUNT, m_ED_ShaderCount);
	DDX_Control(pDX, IDC_ED_SHADER_SIZE, m_ED_ShaderSize);
	DDX_Control(pDX, IDC_ED_EST_TOTAL, m_ED_EST_Total);
	DDX_Control(pDX, IDC_ED_EST_OVER, m_ED_EST_Gap);
}

void CDlgVRAMState::_Update(void)
{
	I3G_RES_STAT stat;
	char conv[256];

	i3GfxResource::GetStat( &stat);

	// Total
	{
		i3::integral_to_comma_str( stat.m_TotalVRAM, conv, sizeof(conv));

		m_ED_TotalVRAM.SetWindowText( conv);
	}

	// Texture
	{
		i3::snprintf( conv, sizeof(conv), "%d", stat.m_TexCount);
		m_ED_TexCount.SetWindowText( conv);


		i3::integral_to_comma_str( (INT32) stat.m_TexSize, conv, sizeof(conv));
		m_ED_TexSize.SetWindowText( conv);
	}


	// Render Target
	{
		i3::snprintf( conv, sizeof(conv), "%d", stat.m_RTCount);
		m_ED_RTCount.SetWindowText( conv);


		i3::integral_to_comma_str( (INT32) stat.m_RTSize, conv, sizeof(conv));
		m_ED_RTSize.SetWindowText( conv);
	}

	// Vertex Buffer
	{
		i3::snprintf( conv, sizeof(conv), "%d", stat.m_VBCount);
		m_ED_VBCount.SetWindowText( conv);


		i3::integral_to_comma_str( (INT32) stat.m_VBSize, conv, sizeof(conv));
		m_ED_VBSize.SetWindowText( conv);
	}

	// Index Buffer
	{
		i3::snprintf( conv, sizeof(conv), "%d", stat.m_IBCount);
		m_ED_IBCount.SetWindowText( conv);


		i3::integral_to_comma_str( (INT32) stat.m_IBSize, conv, sizeof(conv));
		m_ED_IBSize.SetWindowText( conv);
	}

	// Shader
	{
		i3::snprintf( conv, sizeof(conv), "%d", stat.m_ShaderCount);
		m_ED_ShaderCount.SetWindowText( conv);


		i3::integral_to_comma_str( stat.m_ShaderSize, conv, sizeof(conv));
		m_ED_ShaderSize.SetWindowText( conv);
	}

	// Estimated
	{
		UINT32 total = stat.m_RTSize + stat.m_TexSize + stat.m_IBSize + stat.m_VBSize + stat.m_ShaderSize;

		i3::integral_to_comma_str( total, conv, sizeof(conv));
		m_ED_EST_Total.SetWindowText( conv);

		i3::integral_to_comma_str( total - stat.m_TotalVRAM, conv, sizeof(conv));
		m_ED_EST_Gap.SetWindowText( conv);
	}
}

BEGIN_MESSAGE_MAP(CDlgVRAMState, CDialog)

	ON_BN_CLICKED(IDC_BTN_REFRESH, &CDlgVRAMState::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_RT, &CDlgVRAMState::OnBnClickedBtnRt)
	ON_BN_CLICKED(IDC_BTN_TEX, &CDlgVRAMState::OnBnClickedBtnTex)
	ON_BN_CLICKED(IDC_BTN_VB, &CDlgVRAMState::OnBnClickedBtnVb)
	ON_BN_CLICKED(IDC_BTN_IB, &CDlgVRAMState::OnBnClickedBtnIb)
END_MESSAGE_MAP()


// CDlgVRAMState message handlers


BOOL CDlgVRAMState::OnInitDialog()
{
	CDialog::OnInitDialog();

	_Update();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgVRAMState::OnBnClickedBtnRefresh()
{
	_Update();
}


void CDlgVRAMState::OnBnClickedBtnRt()
{
	// TODO: Add your control notification handler code here
}

void CDlgVRAMState::OnBnClickedBtnTex()
{
	CDlgTexInspector dlg;

	dlg.DoModal();
}

void CDlgVRAMState::OnBnClickedBtnVb()
{
	CDlgVBInspector dlg;

	dlg.DoModal();
}

void CDlgVRAMState::OnBnClickedBtnIb()
{
	CDlgIBInspector dlg;

	dlg.DoModal();
}

#endif
