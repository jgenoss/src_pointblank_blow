#pragma once
#include "afxwin.h"


// CPanelFileInfo dialog

class CPanelFileInfo : public CDialog
{
	DECLARE_DYNAMIC(CPanelFileInfo)
protected:
	i3PackNode *			m_pPackNode;
	I3_PACK_FILE_REF *		m_pFile;

protected:
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void		setFile( I3_PACK_FILE_REF * pFile);

public:
	CPanelFileInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelFileInfo();

// Dialog Data
	enum { IDD = IDD_PANEL_FILEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Security;
	virtual BOOL OnInitDialog() override;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnBnClickedChkSecurity();
};
