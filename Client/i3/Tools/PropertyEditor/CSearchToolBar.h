
#ifndef _CSEARCHTOOLBAR_H_
#define _CSEARCHTOOLBAR_H_


#include "CSearchToolBar_I.h"

class CSearchComboBox : public CComboBoxEx
{
	DECLARE_DYNAMIC(CSearchComboBox)

public:
	enum ECUST_NOTIFY
	{
		ECUST_NOTIFY_BASE = 0xfe00,
		ECUST_NOTIFY_RETURN_KEY,
	};

	CSearchComboBox();
	virtual ~CSearchComboBox();

	void SetResID(INT id) { m_resID = id; }
	INT GetResID() { return m_resID; }
	void SetNumMaxDropDownListItems(INT num) { m_numMaxDropDownListItems = num; }

	void SetSortCandidate(bool sorting) { m_sortCandidate = sorting; }
	void SetCallback(CSearchComboBoxCallBack* callback) { m_callback = callback; }

	void ClearCandidateItems();

	void FitDropRectToItems();
	void RefreshSearchTextCandidate(bool atOnce=false);

	bool IsShowDropDownList();
	void DoWithSearchText();
	bool OpenDropDownList();

	DWORD GetSearchOptFlag();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual BOOL OnChildNotify(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT* rst) override;

	void AddStringAsItem(const char* text, void* arg);

	void OnComboEditTextChange();
	void OnSelectItem();

	void _RefreshCandidate();

protected:
	DECLARE_MESSAGE_MAP()

private:
	INT								m_resID;
	CSearchComboBoxCallBack*		m_callback;		

	bool							m_sortCandidate;
	INT								m_numMaxDropDownListItems;

public:
	bool							m_searchPrefixOnly;
};


class CSearchToolBar : public CToolBar
{
	DECLARE_DYNAMIC(CSearchToolBar)

public:
		// CToolBar Override
	virtual BOOL Create(CWnd* pParentWnd,DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_TOOLBAR) override;
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, CRect rcBorders = CRect(0, 0, 0, 0),
		UINT nID = AFX_IDW_TOOLBAR) override;

	virtual void GetItemRect(int nIndex, LPRECT lpRect) const;
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz) override;
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode) override;

public:
	CSearchToolBar();
	virtual ~CSearchToolBar();

	// LoadToolBar 이후에 호출해주세요
	BOOL CreateAdditional(INT comboResID, INT comboWidth=300);
	void SetCallback(CSearchComboBoxCallBack* callback);

	void SetSortCandidate(bool sorting);
	void OnSelectItem();
	bool IsPefSave();

protected:
	void CreateSearchCombo(INT placeIndexInToolBar, INT resID, const CSize siz);
	void UpdateToolBarBtn();

	// afx_msg
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void UpdateUIToolBarBtn(CCmdUI* pCmdUI);
	afx_msg void OnSearchPrefixOnlyCkBtn();

protected:
	BOOL								m_isAdditionalCreated;

	CSearchComboBox						m_searchCombo;
	INT									m_searchComboBoxWidth;
	DWORD								m_searchComboStyle;
	CRect								m_rectSearchCombo;
	INT									m_searchComboPlaceIndexInToolBar;
	CSearchComboBoxCallBack*			m_callbackSearchCombo;

	INT									m_searchPrefixOnlyCkBtnWidth;
	CButton								m_searchPrefixOnlyCkBtn;
	CRect								m_rectSearchPrefixOnlyCkBtn;

	INT									m_PEFBtnWidth;
	CButton								m_PEFBtn;
	CRect								m_PEFBtnRect;
};

#endif
