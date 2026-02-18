// PanelFileInfo.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "PanelFileInfo.h"
#include "SingleFile.h"
#include ".\panelfileinfo.h"


// CPanelFileInfo dialog

IMPLEMENT_DYNAMIC(CPanelFileInfo, CDialog)
CPanelFileInfo::CPanelFileInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelFileInfo::IDD, pParent)
{
	m_pFile = NULL;
}

CPanelFileInfo::~CPanelFileInfo()
{
}

void CPanelFileInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_SECURITY, m_CHK_Security);
}

void CPanelFileInfo::setFile( I3_PACK_FILE_REF * pFile)
{
	m_pFile = pFile;

	if( m_pFile == NULL)
	{
		m_pPackNode = NULL;

		m_CHK_Security.SetCheck( BST_UNCHECKED);
	}
	else
	{
		// Security
		if( m_pFile->m_Attr & I3_PACK_FILE_ATTR_SECURE)
			m_CHK_Security.SetCheck( BST_CHECKED);
		else
			m_CHK_Security.SetCheck( BST_UNCHECKED);
	}

	m_CHK_Security.EnableWindow( (m_pFile != NULL));
}

BEGIN_MESSAGE_MAP(CPanelFileInfo, CDialog)
	ON_BN_CLICKED(IDC_CHK_SECURITY, OnBnClickedChkSecurity)
END_MESSAGE_MAP()


// CPanelFileInfo message handlers

BOOL CPanelFileInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		i3TDK::RegisterUpdate( m_hWnd, i3PackNode::static_meta(), I3_TDK_UPDATE_ALL);
		i3TDK::RegisterUpdate( m_hWnd, SingleFile::static_meta(), I3_TDK_UPDATE_ALL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelFileInfo::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	switch( pInfo->m_Event)
	{
		case I3_TDK_UPDATE_SCENE :
		case I3_TDK_UPDATE_UNSELECT :
			setFile( NULL);
			break;

		case I3_TDK_UPDATE_SELECT :
			if( i3::same_of<i3PackNode*>(pInfo->m_pMeta)) //->IsExactTypeOf( i3PackNode::static_meta()))
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

LRESULT CPanelFileInfo::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPanelFileInfo::OnBnClickedChkSecurity()
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
