// PreprocessItemDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "PreprocessItemDlg.h"


// PreprocessItemDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PreprocessItemDlg, CDialog)

PreprocessItemDlg::PreprocessItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PreprocessItemDlg::IDD, pParent)
{

}

PreprocessItemDlg::~PreprocessItemDlg()
{
}

void PreprocessItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PREPROCESS_TEXT, m_Text);
}


BEGIN_MESSAGE_MAP(PreprocessItemDlg, CDialog)
END_MESSAGE_MAP()


// PreprocessItemDlg 메시지 처리기입니다.
void	PreprocessItemDlg::SetText(const wchar_t* str)
{
	m_Text.SetWindowText(str);
}

