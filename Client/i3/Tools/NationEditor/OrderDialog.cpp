// OrderDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NationEditor.h"
#include "OrderDialog.h"

#include "NationEditorDlg.h"

// COrderDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(COrderDialog, CDialog)

COrderDialog::COrderDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COrderDialog::IDD, pParent)
{
	m_pParent = static_cast< CNationEditorDlg* >(pParent);		// dynamic_cast-->static_cast로 변경..(2013.05.20.수빈)
}

COrderDialog::~COrderDialog()
{
}

void COrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST2, m_ListBox_Before);
	DDX_Control(pDX, IDC_LIST3, m_ListBox_After);
}


BEGIN_MESSAGE_MAP(COrderDialog, CDialog)

END_MESSAGE_MAP()


// COrderDialog 메시지 처리기입니다.

BOOL COrderDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListBox_After.EnableSwap();

	const StringVector* pList = m_pParent->GetNationListPtr();

	size_t size = pList->size();

	for(size_t i = 0; i < size; ++i)
	{
		m_ListBox_Before.AddString( (*pList)[i].c_str() );
		m_ListBox_After.AddString( (*pList)[i].c_str() );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

