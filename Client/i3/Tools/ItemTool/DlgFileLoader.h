#pragma once

#include "FileData.h"
// cDlgFileLoader 대화 상자입니다.

static void ReadThreadProc(LPVOID lpParameter);

class cDlgFileLoader : public CDialog
{
	friend static void ReadThreadProc(LPVOID lpParameter);

	DECLARE_DYNAMIC(cDlgFileLoader)

	HANDLE m_handle;
	CFileData* m_pFileData;
	char* m_szRootPath;
	bool m_bLoaded;

public:
	cDlgFileLoader(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~cDlgFileLoader();

	mFileNameType m_mFileNameWeapon;
	mFileNameType m_mFileNameChara;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_FILE_LOADER };

	void OnStartThread();
	void OnEndThread();
	LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	CFileData*	GetFileData() const { return m_pFileData; }
	char*		GetRootPath() const { return m_szRootPath; }

	bool		IsLoaded() const { return m_bLoaded; }
	void		SetLoaded(const bool& bLoaded) {m_bLoaded = bLoaded;}
	
	void		SetFileDir(const std::string& strName, const std::string& strFileName);
	void		SetFileRootName(const std::string& strName, const std::string& strRootName);
	void		SetFileFolderName(const std::string& strName, const std::string& strFolderName);
	std::string		GetFileDir(const std::string& strName);
	std::string		GetFileRootName(const std::string& strName);
	std::string		GetFileFolderName(const std::string& strName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	
	DECLARE_MESSAGE_MAP()
public:
	CString m_str_res_name;
	virtual BOOL OnInitDialog();
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL DestroyWindow();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual void OnOK();
};
