// PathDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "PathDlg.h"

#include "i3MFCFolderDialog.h"

#include "PackScriptContents.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

// PathDlg 대화 상자입니다.
namespace
{
	struct ButtonCB : i3MFCButtonCallback
	{
		PathDlg* m_owner;
		CEdit* m_edit;
		ButtonCB(PathDlg* owner, CEdit* edit) : m_owner(owner), m_edit(edit) {}
		virtual void OnClick(i3MFCButton* ctrl) 
		{
			i3MFCFolderDialog dlg;
			
			CString str;
			m_edit->GetWindowText(str);
			
			i3::wstring wstrPath(LPCWSTR(str), str.GetLength());
			dlg.SelectFolder( wstrPath );
						
			if (dlg.DoModal()  == IDOK)
			{
				m_edit->SetWindowText(dlg.GetResultPath().c_str());
				//
				m_owner->UpdateNationCombo_IfMediaPathChange(m_edit, dlg.GetResultPath());
				//
			}
		}
	};

}



IMPLEMENT_DYNAMIC(PathDlg, CDialog)

PathDlg::PathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PathDlg::IDD, pParent)
{
	m_button_PackScriptPath.SetCallback(new ButtonCB(this, &m_editPackScriptPath));
	m_button_MediaPath.SetCallback(new ButtonCB(this, &m_editMediaPath));
	m_button_QAPath.SetCallback(new ButtonCB(this, &m_editQAPath));
	m_button_LivePath.SetCallback(new ButtonCB(this, &m_editLivePath));
}

PathDlg::~PathDlg()
{
	delete m_button_LivePath.GetCallback();
	delete m_button_QAPath.GetCallback();
	delete m_button_MediaPath.GetCallback();
	delete m_button_PackScriptPath.GetCallback();
}

void PathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_EDIT_PATH_NAME, m_editPathName);
	DDX_Control(pDX, IDC_EDIT_PACKSCRIPT_PATH, m_editPackScriptPath);
	DDX_Control(pDX, IDC_EDIT_MEDIA_PATH, m_editMediaPath);
	DDX_Control(pDX, IDC_EDIT_QA_PATH, m_editQAPath);
	DDX_Control(pDX, IDC_EDIT_LIVE_PATH, m_editLivePath);

	DDX_Control(pDX, IDC_BUTTON_PACKSCRIPT_PATH, m_button_PackScriptPath);
	DDX_Control(pDX, IDC_BUTTON_MEDIA_PATH, m_button_MediaPath);
	DDX_Control(pDX, IDC_BUTTON_QA_PATH, m_button_QAPath);
	DDX_Control(pDX, IDC_BUTTON_LIVE_PATH, m_button_LivePath);

	DDX_Control(pDX, IDC_COMBO_NATION, m_combo_Nation);
}


BEGIN_MESSAGE_MAP(PathDlg, CDialog)

	ON_EN_KILLFOCUS(IDC_EDIT_MEDIA_PATH, &PathDlg::OnEnKillfocusEditMediaPath)
END_MESSAGE_MAP()


// PathDlg 메시지 처리기입니다.

void PathDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CString pathName; 

	m_editPathName.GetWindowText(pathName);

	if (pathName.IsEmpty())
	{
		MessageBox(L"You have to fill your path name.", L"Requirement");
	}
//	else if
//	{
//		이 부분에는 기존 리스트의 경로이름과 이름이 중복되지 않는 체크를 심어야한다..
//	}
	else
	{
		CString packscriptPath, mediaPath, QAPath, LivePath;
		m_editPackScriptPath.GetWindowText(packscriptPath);
		m_editMediaPath.GetWindowText(mediaPath);
		m_editQAPath.GetWindowText(QAPath);
		m_editLivePath.GetWindowText(LivePath);

		m_unit.PathName.assign(LPCWSTR(pathName), pathName.GetLength());
		m_unit.PackScriptPath.assign(LPCWSTR(packscriptPath), packscriptPath.GetLength());
		m_unit.MediaPath.assign(LPCWSTR(mediaPath), mediaPath.GetLength());
		m_unit.QAPath.assign(LPCWSTR(QAPath), QAPath.GetLength());
		m_unit.LivePath.assign(LPCWSTR(LivePath), LivePath.GetLength());
		
	//	m_unit.Nation
		
		if (m_combo_Nation.IsWindowEnabled() && m_combo_Nation.GetCount() > 0)
		{
			int idx = m_combo_Nation.GetCurSel();
			if (idx == -1 ) 
				idx = 0;

			CString strNation;		m_combo_Nation.GetLBText(idx, strNation);
			m_unit.Nation.assign( LPCWSTR(strNation), strNation.GetLength());
		}
		else
		{
			m_unit.Nation.clear();
		}

		CDialog::OnOK();
	}
}

void	PathDlg::SetUnit(const PackPathUnit& unit)
{
	m_unit = unit;

	PathDlg::UpdateEditControls();
}

BOOL PathDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	PathDlg::UpdateEditControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void	PathDlg::UpdateEditControls()
{
	if (this->GetSafeHwnd())
	{
		m_editPathName.SetWindowText(m_unit.PathName.c_str());

		m_editPackScriptPath.SetWindowText(m_unit.PackScriptPath.c_str());
		m_editMediaPath.SetWindowText(m_unit.MediaPath.c_str());
		m_editQAPath.SetWindowText(m_unit.QAPath.c_str());
		m_editLivePath.SetWindowText(m_unit.LivePath.c_str());
		
		PathDlg::UpdateNationCombo(m_unit.MediaPath);
		
	}
}

void	PathDlg::UpdateNationCombo(const i3::wstring& mediaPath)
{
	if (!this->GetSafeHwnd())
		return;

	// nation.nlf 점검..

	m_combo_Nation.ResetContent();

	i3::stack_wstring wstrPath = mediaPath; //m_unit.MediaPath;
	wstrPath += L"/config/Nation.nlf";

	NationList nl;
	if ( LoadNationList(wstrPath, nl ) )
	{
		m_combo_Nation.EnableWindow(TRUE);		
		
//		m_combo_Nation.GetEditCtrl()->SetReadOnly(TRUE);

		const size_t num_nl = nl.list.size();
		i3::wstring wstrTemp;

		for (size_t i = 0 ; i < num_nl ; ++i)
		{
			i3::mb_to_utf16(nl.list[i], wstrTemp);
			m_combo_Nation.InsertString(i, wstrTemp.c_str());
		}

		if (!m_unit.Nation.empty())
		{
			i3::string curr_nation;
			i3::utf16_to_mb( m_unit.Nation, curr_nation);
			int idx = i3::vu::int_index_of(nl.list, curr_nation);

			if (idx != -1)
				m_combo_Nation.SetCurSel(idx);
		}

		if (m_combo_Nation.GetCurSel() == -1)
			m_combo_Nation.SetCurSel(0);
	}
	else
	{
		m_combo_Nation.EnableWindow(FALSE);				
	}
	
}

void	PathDlg::UpdateNationCombo_IfMediaPathChange(CEdit* edit, const i3::wstring& mediaPath)
{
	if (edit != &m_editMediaPath)
		return;

	PathDlg::UpdateNationCombo(mediaPath);
}

void PathDlg::OnEnKillfocusEditMediaPath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString mediaPath;
	m_editMediaPath.GetWindowText(mediaPath);

	i3::wstring wstrMediaPath( LPCWSTR(mediaPath), mediaPath.GetLength() );

	PathDlg::UpdateNationCombo(wstrMediaPath);
}
