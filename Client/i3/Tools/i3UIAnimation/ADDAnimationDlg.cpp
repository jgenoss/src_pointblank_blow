#include "stdafx.h"
#include "ADDAnimationDlg.h"
#include "i3Base/string/ext/atof.h"
#include "GlobalInstance.h"
IMPLEMENT_DYNAMIC(CADDAnimationDlg, CDialog)
BEGIN_MESSAGE_MAP(CADDAnimationDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CADDAnimationDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CADDAnimationDlg::OnBnClickedButtonOk)
END_MESSAGE_MAP()

CADDAnimationDlg::CADDAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CADDAnimationDlg::IDD, pParent)
{
}


CADDAnimationDlg::~CADDAnimationDlg()
{
}

void CADDAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_UVLIST, m_Listbox);
}

BOOL CADDAnimationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char chTemp[MAX_PATH];
	for (INT32 i = 0; i < CGlobalInstance::getSingletonPtr()->GetUVCount(); i++)
	{
		UVITEM* pItem = CGlobalInstance::getSingletonPtr()->GetUVItem(i);

		sprintf_s(chTemp, "Rect : %d, %d, %d, %d Image : %s",
			(INT32)pItem->_pControl->getPos()->x,
			(INT32)pItem->_pControl->getPos()->y,
			(INT32)pItem->_pControl->getWidth(),
			(INT32)pItem->_pControl->getHeight(),
			pItem->_pTexture->GetName());
		m_Listbox.InsertString(i, chTemp);
	}
	


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CADDAnimationDlg::OnOK()
{
	CWnd* pWnd = GetDlgItem(IDC_EDIT_TITLE);
	pWnd->GetWindowText(m_chTitle, sizeof(m_chTitle) - 1);

	pWnd = GetDlgItem(IDC_EDIT_LIFETIME);
	char chLifeTime[MAX_PATH];
	pWnd->GetWindowText(chLifeTime, sizeof(chLifeTime) - 1);

	m_fLifeTime = i3::atof(chLifeTime);

	CDialog::OnOK();
}


void CADDAnimationDlg::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}


void CADDAnimationDlg::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}
