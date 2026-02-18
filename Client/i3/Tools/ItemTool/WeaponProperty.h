#pragma once

class cMyMFCPropertyGridCtrl : public CMFCPropertyGridCtrl
{
	
	DECLARE_DYNAMIC(cMyMFCPropertyGridCtrl)
public:
	//cMyMFCPropertyGridCtrl();
	virtual ~cMyMFCPropertyGridCtrl(){};
	//virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
};

class cMyMFCPropertyGridProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(cMyMFCPropertyGridProperty)

public:
	cMyMFCPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE);
	cMyMFCPropertyGridProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL);
	virtual ~cMyMFCPropertyGridProperty();

};

enum eSubPropItem
{
	eSubAnimName,
	eSubClassName,
	eSubAnimList,
	eSubAnimFileName,
	eSubBlendTime,
	eSubBlendLoop,
	eSubIsAnimEnd,
	eSubIsTimeLimit,
	eSubSetTimeLimit,
};
// cWeaponProperty 폼 뷰입니다.

class cWeaponProperty : public CDialog
{
	DECLARE_DYNCREATE(cWeaponProperty)

public:
	cWeaponProperty();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~cWeaponProperty();

public:
	void Init();
	void InitPropList();
	void SetPropListFont();
	void AdjustLayout();

	void UpdateAnimList();
	void UpdateAll(int nIdx);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	
	void SetPropMain();
	void SetPropAnimation();
	void SetPropBlending();
	void SetPropConditions();
	void SetPropNextState();
	//void SetPropScript();

private:
	i3AIState*	m_pCurrAiState;
	CFont m_fntPropList;

	cMyMFCPropertyGridCtrl m_wndPropList;

	typedef std::vector<CString> vStrPropCon;
	
	//main
	CString m_strAniName;
	CString m_strFileName;
	
	//animation
	vStrPropCon m_vAnimationFilter;
	bool		m_bDisplayPath;
	bool		m_bOnlyName;
	vStrPropCon m_vAnimationList;
	CString		m_strSelectedAnimFileName;
	CString		m_strFilterName;

	//Blend
	float		m_fBlendTime;
	bool		m_bLoop;

	//condition
	bool		m_bIsAnimationEndtoBlend;
	bool		m_bIsBlendTimeLimit;
	float		m_fBlendtimeLimit;

	//nextstate
	vStrPropCon m_vNextAnimation;

	int m_nSelectedState;
	DECLARE_MESSAGE_MAP()
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);



	
};


