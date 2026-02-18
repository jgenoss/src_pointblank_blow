#pragma once


// PefTxtDiffDlg 대화 상자입니다.
#include "i3MFCListCtrl.h"
#include "i3MFCButton.h"
#include "ExecuteSVN.h"
#include "PathMode.h"

#include "PefTxtDiffInputInfo.h"

enum PefTxtDiffFileFormat
{
	PEFTXT_DIFF_FILE_FORMAT_PEF,
	PEFTXT_DIFF_FILE_FORMAT_TXT,
};


class PefTxtDiffDlg : public CDialog
{
	DECLARE_DYNAMIC(PefTxtDiffDlg)
public:
	
	PefTxtDiffDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PefTxtDiffDlg();

	void InitParam(PathMode pm, PefTxtDiffFileFormat fmt, const i3::rc_wstring& filename);

	void UpdateFileList_MediaQAPath(const i3::wstring& base_path, int l_r);
	void UpdateFileList_LivePath(const i3::wstring& base_path, int l_r);
	void UpdateLogList(int file_list_index, int l_r);
	void UpdateLogText(int log_list_index, int l_r);

	void UpdateNextLogList(int l_r);

	const PefTxtDiffInputInfo& GetResult() const { return m_result; }

private:
		
	i3MFCListCtrl	m_PathCtrl[2];
	i3MFCListCtrl	m_FileListCtrl[2];
	i3MFCListCtrl	m_LogListCtrl[2];
	CEdit			m_LogDataCtrl[2];
	
	i3MFCButton		m_button_NextLog;
	
	i3::vector<i3::rc_string>	m_FileList[2];
	svn::LogInfoList			m_LogInfoList[2];

	PefTxtDiffFileFormat		m_curr_file_format;
	PathMode					m_init_path_mode;
	i3::rc_wstring				m_selected_filename;
	
	PefTxtDiffInputInfo			m_result;

private:

	void			InitDialog_PathCtrls();
	void			InitDialog_FileListCtrls();
	void			InitDialog_LogListCtrls();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PEFTXT_DIFF_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
