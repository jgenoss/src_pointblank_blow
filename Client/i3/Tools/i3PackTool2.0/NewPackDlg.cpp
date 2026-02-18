// NewPackDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "i3PackTool2.0.h"
#include "NewPackDlg.h"
#include "PTTreeworkLive.h"
#include "PTStorage.h"
#include "PTFile.h"
#include "PTTreeworkMediator.h"

// CNewPackDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNewPackDlg, CDialog)

CNewPackDlg::CNewPackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewPackDlg::IDD, pParent)
{

}

CNewPackDlg::~CNewPackDlg()
{
}

void CNewPackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NEW_PACK_NAME, nameEditBox);
}


BEGIN_MESSAGE_MAP(CNewPackDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CNewPackDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewPackDlg 메시지 처리기입니다.

void CNewPackDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
	nameEditBox.GetWindowText(m_PackName);

	TheMediator().GetTreeworkLive()->MakeNewPack(m_PackName);
}
