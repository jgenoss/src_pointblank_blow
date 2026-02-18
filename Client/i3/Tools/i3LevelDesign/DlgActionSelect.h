#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgActionSelect dialog

class CDlgActionSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgActionSelect)

protected:
	i3LevelResEvent	*	m_pSelEvent;
	CImageList			m_ImageList;

	i3::vector<i3LevelResEvent *>		m_EventList;

	void				UpdateAllEvents(void);
	void				UpdateEvent( INT32 idx, i3LevelResEvent * pEvent);
	void				RemoveAllEvents(void);
	static BOOL			EventProc( INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData);

public:
	CDlgActionSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgActionSelect();

// Dialog Data
	enum { IDD = IDD_DLG_ACTIONRESSEL };

public:
	bool					Execute( i3LevelResEvent * pEvent);
	i3LevelResEvent *		getSelectedEvent(void)		{ return m_pSelEvent; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_EventListCtrl;
	virtual BOOL OnInitDialog() override;
	CButton m_ctrlNewEvent;
	afx_msg void OnBnClickedBtnNewevent();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnOK() override;
public:
//	afx_msg void OnLvnItemchangedListActionSel(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_stActionPath;
	afx_msg void OnBnClickedBtnDeleteEvent();
	afx_msg void OnLvnEndlabeleditListActionSel(NMHDR *pNMHDR, LRESULT *pResult);
};
