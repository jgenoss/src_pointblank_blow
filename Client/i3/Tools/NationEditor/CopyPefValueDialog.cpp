// CopyPefValueDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NationEditor.h"
#include "CopyPefValueDialog.h"

#include "NationEditorDlg.h"

// CClonePefValueDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CClonePefValueDialog, CDialog)

CClonePefValueDialog::CClonePefValueDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CClonePefValueDialog::IDD, pParent)
{

}

CClonePefValueDialog::~CClonePefValueDialog()
{
}

void CClonePefValueDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PEF, m_PefList);
	DDX_Control(pDX, IDC_LIST_SRC_NATION, m_SrcList);
	DDX_Control(pDX, IDC_LIST_DEST_NATION, m_DestList);
	DDX_Control(pDX, IDC_LIST_COPY_ITEM, m_CloneItemList);
	DDX_Control(pDX, IDC_EDIT_COPY_ITEM, m_CloneItem);
}


BEGIN_MESSAGE_MAP(CClonePefValueDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CClonePefValueDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CClonePefValueDialog 메시지 처리기입니다.

BOOL CClonePefValueDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitPefList();
	InitNationList();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CClonePefValueDialog::InitPefList()
{
	size_t size = m_pPefContextBank->size();

	for(size_t i = 0; i <size; ++i)
	{
		m_PefList.AddString( (*m_pPefContextBank)[i]->Name.c_str() );
	}
}

void CClonePefValueDialog::InitNationList()
{
	size_t size = m_pNationBank->size();

	for(size_t i = 0; i <size; ++i)
	{
		m_SrcList.AddString( (*m_pNationBank)[i].c_str() );
		m_DestList.AddString( (*m_pNationBank)[i].c_str() );
	}
}

BOOL CClonePefValueDialog::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		switch( pMsg->wParam )
		{
		case VK_RETURN:
			if( GetFocus() == & m_CloneItem )
			{
				CString str;
				m_CloneItem.GetWindowText(str);
				m_CloneItemList.AddString( str );
				m_CloneItem.SetWindowText("");
			}
			return TRUE;
			break;
		case VK_ESCAPE:
			return TRUE;
			break;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CClonePefValueDialog::OnBnClickedOk()
{
	CNationEditorDlg* pParentDlg = (CNationEditorDlg*)this->m_pParentWnd;

	pParentDlg->SetCloneData(
		m_PefList.GetCurrentSel(),
		m_SrcList.GetCurrentSel(),
		m_DestList.GetCurrentSel(),
		m_CloneItemList.GetAllStrings() );

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}
