#pragma once
#include "afxcmn.h"


// CPanelFileList dialog

class CPanelFileList : public CDialog
{
	DECLARE_DYNAMIC(CPanelFileList)

protected:
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void		UpdateAll( i3PackNode * pNode);
	void		UpdateItem( INT32 idx, I3_PACK_FILE_REF * pFile);
	INT32		_getSelectedItem(void);
	INT32		_findItemByFile( I3_PACK_FILE_REF * pFile);

public:

public:
	CPanelFileList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelFileList();

// Dialog Data
	enum { IDD = IDD_PANEL_FILELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_FileListCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnNMClickFileList(NMHDR *pNMHDR, LRESULT *pResult);
};
