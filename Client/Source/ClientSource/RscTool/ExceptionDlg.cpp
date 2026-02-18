// ExceptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "ExceptionDlg.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/itl/range/algorithm/replace.h"
#include "i3Base/string/ext/strtok_fast.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "PackScriptContents.h"

// ExceptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(ExceptionDlg, CDialog)

ExceptionDlg::ExceptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExceptionDlg::IDD, pParent)
{

}

ExceptionDlg::~ExceptionDlg()
{
}

void ExceptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_EXCEPTION, m_edit_exception);
	DDX_Control(pDX, IDC_EDIT_FORCE_COPY_TO_QAPATH, m_edit_force_copy_to_qapath);
}


BEGIN_MESSAGE_MAP(ExceptionDlg, CDialog)
END_MESSAGE_MAP()


// ExceptionDlg 메시지 처리기입니다.
namespace
{
	
	bool ConvertFileListStringToCEdit(const i3::string& input, CEdit& inout)
	{
		if (!inout.GetSafeHwnd())  
			return false;

		if (!input.empty())
		{		
			i3::wstring wstr;		i3::mb_to_utf16(input, wstr);
			i3::range::replace(wstr, L';', L' ');
			inout.SetWindowText(wstr.c_str());
		}
		else
		{
			inout.SetWindowText(L"");
		}

		return true;
	}
	
	bool ConvertCEditToFileListString(const CEdit& input, i3::string& out)
	{
		if (!input.GetSafeHwnd())
			return false;

		CString mfcstr;	input.GetWindowText(mfcstr);

		i3::wstring wstr(LPCTSTR(mfcstr), mfcstr.GetLength());

		if (!wstr.empty() )
		{
			i3::vector<i3::wstring> token_list;
			i3::strtok_fast(token_list, wstr, L" \n\r;", true);

			wstr.clear();
			const size_t num_token = token_list.size();
			if ( num_token > 0 )
			{
				size_t i = 0;
				for (; i < num_token - 1 ; ++i )
				{
					wstr += token_list[i];
					wstr += L';';
				}

				wstr += token_list[i];
			}
		}	

		i3::utf16_to_mb(wstr, out);
		return true;
	}


}



BOOL ExceptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ConvertFileListStringToCEdit(m_exception_copied_string.str_exception, m_edit_exception);
	ConvertFileListStringToCEdit(m_exception_copied_string.str_force_copy, m_edit_force_copy_to_qapath);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void ExceptionDlg::SetExceptionFileString(const PackScriptExceptionString& str)
{
	m_exception_copied_string = str;

	ConvertFileListStringToCEdit(m_exception_copied_string.str_exception, m_edit_exception);
	ConvertFileListStringToCEdit(m_exception_copied_string.str_force_copy, m_edit_force_copy_to_qapath);
}


void ExceptionDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	
	ConvertCEditToFileListString(m_edit_exception, m_exception_copied_string.str_exception);
	ConvertCEditToFileListString(m_edit_force_copy_to_qapath, m_exception_copied_string.str_force_copy);
	
	CDialog::OnOK();
}
