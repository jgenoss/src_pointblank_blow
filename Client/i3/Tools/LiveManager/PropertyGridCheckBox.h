#pragma once
class CPropertyGridCtrlEx;
class CPropertyGridCheckBox : public CMFCPropertyGridProperty
{
public:

	CPropertyGridCheckBox(CPropertyGridCtrlEx* pPropertyList, const CString& strName, _variant_t& varValue, const CString strParant, 
		DWORD dwFontDialogFlags = CF_EFFECTS | CF_SCREENFONTS, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0, COLORREF color = (COLORREF)-1);
	virtual ~CPropertyGridCheckBox(void);

	virtual BOOL OnEdit(LPPOINT lptClick) override;
	virtual void OnDestroyWindow() override;
	virtual void OnDrawValue(CDC* pDC, CRect rect) override;
	virtual BOOL PushChar(UINT nChar) override;

private:
	bool m_bStaticCheckState;
	bool m_bCheckState;
	CRect m_rectClient;
	CPropertyGridCtrlEx *m_pwndPropList;
	CMFCPropertyGridProperty* m_pProperty;
	CString m_strParantName;

public:
	CMFCPropertyGridProperty* GetNewProperty(){ return m_pProperty; }
	DWORD GetFlags() { return m_dwFlags ;}
	CString GetParantName(){ return m_strParantName; }
};

class CPropertyGridCtrlEx : public CMFCPropertyGridCtrl
{
	friend class CMFCPropertyGridProperty;
	DECLARE_DYNAMIC(CPropertyGridCtrlEx)
	DECLARE_MESSAGE_MAP()

public:
	CPropertyGridCtrlEx();
	virtual void AdjustLayout() override;
	

public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	
};

class CPropertyGridPropertyWeapon : public CMFCPropertyGridProperty
{
	bool m_bPauseEdit;

public:

	CPropertyGridPropertyWeapon(const CString& strName, _variant_t& varValue, bool bPauseState = false, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CPropertyGridPropertyWeapon(void);

	virtual BOOL OnEdit(LPPOINT lptClick) override;
	virtual BOOL PushChar(UINT nChar) override;
	virtual void OnDrawButton(CDC* pDC, CRect rect) override;

	bool GetPauseState(){ return m_bPauseEdit; }
	void SetPauseState(bool bState){ m_bPauseEdit = bState; }
};

