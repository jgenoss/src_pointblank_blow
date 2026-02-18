// i3TDKSpec_Node.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Node.h"
#include "i3TDKUtilCommon.h"
#include "i3TDKDlg_NodeFlag.h"
#include "i3TDKDlg_NodeBound.h"

// i3TDKSpec_Node dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Node, i3TDKDialogBase)

i3TDKSpec_Node::~i3TDKSpec_Node()
{
	I3_SAFE_RELEASE( m_pNode);
}

void i3TDKSpec_Node::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_NAME, m_ED_Name);
	DDX_Control(pDX, IDC_TDK_ED_FLAGS, m_ED_Flags);
	DDX_Control(pDX, IDC_TDK_ED_BOUND, m_ED_Bound);
	DDX_Control(pDX, IDC_TDK_BTN_FLAGS, m_BTN_Flags);
	DDX_Control(pDX, IDC_TDK_BTN_BOUND, m_BTN_Bound);
}

void i3TDKSpec_Node::SetObject( i3ElementBase * pObj)
{
	TCHAR conv[256];

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pNode, ((i3Node *)pObj));

	if( pObj != nullptr)
	{
		// Name
#ifdef _UNICODE
		i3::stack_wstring wstrNodeName;		i3::mb_to_utf16(m_pNode->GetName(), wstrNodeName);
		const wchar_t* szNodeName = wstrNodeName.c_str();
#else
		const char* szNodeName = m_pNode->GetName();
#endif
		m_ED_Name.SetWindowText( szNodeName );

		// Flags
		i3TDK::GetNodeFlagString( m_pNode, conv, _countof(conv));
		m_ED_Flags.SetWindowText( conv);

		// Bounds
		if( m_pNode->GetBound() != nullptr)
		{
			VEC3D * pMin, * pMax;

			pMin = m_pNode->GetBound()->GetMin();
			pMax = m_pNode->GetBound()->GetMax();

			i3::snprintf( conv, _countof(conv), _T("(%.2f %.2f %.2f) - (%.2f %.2f %.2f)"), pMin->x, pMin->y, pMin->z, pMax->x, pMax->y, pMax->z);
			m_ED_Bound.SetWindowText( conv);
		}
		else
		{
			m_ED_Bound.SetWindowText( _T("(None)") );
		}

	}
	else
	{
		m_ED_Name.SetWindowText( _T("") );
		m_ED_Flags.SetWindowText( _T("") );
	}

	m_ED_Name.SetModify( FALSE);
	m_ED_Flags.SetModify( FALSE);
	m_ED_Bound.SetModify( FALSE);

	m_ED_Name.EnableWindow( pObj != nullptr);
	m_ED_Flags.EnableWindow( pObj != nullptr);
	m_ED_Bound.EnableWindow( pObj != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Node, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_TDK_ED_NAME, OnEnKillfocusTdkEdName)
	ON_BN_CLICKED(IDC_TDK_BTN_FLAGS, OnBnClickedTdkBtnFlags)
	ON_EN_KILLFOCUS(IDC_TDK_ED_BOUND, OnEnKillfocusTdkEdBound)
	ON_EN_CHANGE(IDC_TDK_ED_NAME, &i3TDKSpec_Node::OnEnChangeTdkEdName)
	ON_BN_CLICKED(IDC_TDK_BTN_BOUND, &i3TDKSpec_Node::OnBnClickedTdkBtnBound)
END_MESSAGE_MAP()


// i3TDKSpec_Node message handlers

BOOL i3TDKSpec_Node::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Node::OnOK()
{
}

void i3TDKSpec_Node::OnCancel()
{
}

void i3TDKSpec_Node::OnEnKillfocusTdkEdName()
{
	if( m_pNode == nullptr)
		return;

	TCHAR conv[256] = {0,};

	m_ED_Name.GetWindowText( conv, _countof(conv));

#ifdef _UNICODE
	i3::stack_string strNodeName;	i3::utf16_to_mb(conv, strNodeName);
	const char* szNodeName = strNodeName.c_str();
#else
	const char* szNodeName = conv;
#endif

	m_pNode->SetName( szNodeName);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pNode);

}

void i3TDKSpec_Node::OnBnClickedTdkBtnFlags()
{
	i3TDKDlg_NodeFlag dlg;

	if( dlg.Execute( m_pNode))
	{
		TCHAR conv[256];

		i3TDK::GetNodeFlagString( m_pNode, conv, _countof(conv));
		m_ED_Flags.SetWindowText( conv);
	}
}

void i3TDKSpec_Node::OnEnKillfocusTdkEdBound()
{

	
}

void i3TDKSpec_Node::OnEnChangeTdkEdName()
{
	if( m_pNode == nullptr)
		return;

	TCHAR conv[256] = {0,};

	m_ED_Name.GetWindowText( conv, _countof(conv));

#ifdef _UNICODE
	i3::stack_string strNodeName;		i3::utf16_to_mb(conv, strNodeName);
	const char* szNodeName = strNodeName.c_str();
#else
	const char* szNodeName = conv;
#endif

	m_pNode->SetName( szNodeName );

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pNode);
}

void i3TDKSpec_Node::OnBnClickedTdkBtnBound()
{
	if( m_pNode && m_pNode->GetBound() != nullptr)
	{
		i3TDKDlg_NodeBound dlg;

		if( dlg.Execute( m_pNode))
		{
			TCHAR conv[256] = {0,};

			VEC3D * pMin, * pMax;

			pMin = m_pNode->GetBound()->GetMin();
			pMax = m_pNode->GetBound()->GetMax();

			i3::snprintf( conv, _countof(conv), _T("(%.2f %.2f %.2f) - (%.2f %.2f %.2f)"), pMin->x, pMin->y, pMin->z, pMax->x, pMax->y, pMax->z);

			m_ED_Bound.SetWindowText( conv);

			if(m_pCB_ResetBoundMat != nullptr)
				m_pCB_ResetBoundMat(m_pNode);
		}
	}
}
