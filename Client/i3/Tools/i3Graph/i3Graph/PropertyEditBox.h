#if !defined(AFX_PROPERTYEDITBOX_H__51CEDF81_3EF8_11D3_B5AD_0080C70D2C8F__INCLUDED_)
#define AFX_PROPERTYEDITBOX_H__51CEDF81_3EF8_11D3_B5AD_0080C70D2C8F__INCLUDED_

#pragma once
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyEditBox window

class CPropertyEditBox : public CEdit
{
// Construction
public:
	CPropertyEditBox();

// Attributes
public:
//	CProperty * CurProperty;

// Operations
public:
//	void SetProperty( CProperty *Property);
	void SetProperty(INT8* SetValue);
	void GetProperty(INT8* GetValue);
	void ClearSelect(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyEditBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyEditBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyEditBox)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYEDITBOX_H__51CEDF81_3EF8_11D3_B5AD_0080C70D2C8F__INCLUDED_)
