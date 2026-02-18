// PanelFileInfo.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "PanelProperty.h"
#include "SingleFile.h"
#include ".\panelfileinfo.h"
#include "GlobalVar.h"
#include "i3Base/i3StringListBuffer.h"

// CPanelProperty dialog

IMPLEMENT_DYNAMIC(CPanelProperty, CDialog)
CPanelProperty::CPanelProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelProperty::IDD, pParent)
{
	m_pFile = NULL;
}

CPanelProperty::~CPanelProperty()
{
}

void CPanelProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_CHK_SECURITY, m_CHK_Security);
	DDX_Control(pDX, IDC_EDIT1, m_EdWorkDir);
	DDX_Control(pDX, IDC_EDIT2, m_EdLiveDir);
	DDX_Control(pDX, IDC_EDIT3, m_EdScriptDir);
}

void CPanelProperty::setFile( I3_PACK_FILE_REF * pFile)
{

}

BEGIN_MESSAGE_MAP(CPanelProperty, CDialog)
	//ON_BN_CLICKED(IDC_CHK_SECURITY, OnBnClickedChkSecurity)
	ON_BN_CLICKED(IDC_BUTTON1, &CPanelProperty::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPanelProperty::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPanelProperty::OnBnClickedButton3)
END_MESSAGE_MAP()


// CPanelProperty message handlers

BOOL CPanelProperty::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_EdWorkDir.SetWindowText( g_pOption->m_szWorkPath);
		m_EdLiveDir.SetWindowText( g_pOption->m_szLivePath);
		m_EdScriptDir.SetWindowText( g_pOption->m_szScriptPath);

		i3TDK::RegisterUpdate( m_hWnd, i3PackNode::static_meta(), I3_TDK_UPDATE_ALL);
		i3TDK::RegisterUpdate( m_hWnd, SingleFile::static_meta(), I3_TDK_UPDATE_ALL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelProperty::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	switch( pInfo->m_Event)
	{
		case I3_TDK_UPDATE_SCENE :
		case I3_TDK_UPDATE_UNSELECT :
			setFile( NULL);
			break;

		case I3_TDK_UPDATE_SELECT :
			if( i3::same_of<i3PackNode*>(pInfo->m_pMeta))
			{
				m_pPackNode = (i3PackNode *) pInfo->m_pObject;
			}
			else
			{
				setFile( (I3_PACK_FILE_REF *) pInfo->m_pObject);
			}
			break;
	}
}

LRESULT CPanelProperty::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPanelProperty::OnBnClickedChkSecurity()
{
	if( m_pFile == NULL)
		return;

	if( m_CHK_Security.GetCheck() == BST_CHECKED)
	{
		m_pFile->m_Attr |= I3_PACK_FILE_ATTR_SECURE;
	}
	else
	{
		m_pFile->m_Attr &= ~I3_PACK_FILE_ATTR_SECURE;
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, (i3ElementBase *) m_pFile, SingleFile::static_meta());
}

void CPanelProperty::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Workding Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szWorkPath) == TRUE)
	{
		m_EdWorkDir.SetWindowText( dlg.GetSelectedFolderPath());
	}

	m_EdWorkDir.GetWindowText( g_pOption->m_szWorkPath, sizeof(g_pOption->m_szWorkPath) - 1);
	i3ResourceFile::SetWorkingFolder( g_pOption->m_szWorkPath);
	SetCurrentDirectory( g_pOption->m_szWorkPath);

}

void CPanelProperty::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Live Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szLivePath) == TRUE)
	{
		m_EdLiveDir.SetWindowText( dlg.GetSelectedFolderPath());
	}
	m_EdLiveDir.GetWindowText( g_pOption->m_szLivePath, sizeof(g_pOption->m_szLivePath) - 1);

}

void CPanelProperty::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Script Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szScriptPath) == TRUE)
	{
		m_EdScriptDir.SetWindowText( dlg.GetSelectedFolderPath());
	}
	m_EdScriptDir.GetWindowText( g_pOption->m_szScriptPath, sizeof(g_pOption->m_szScriptPath) - 1);
}
