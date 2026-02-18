#if !defined( __I3_TDK_PROPERTY_EDITBOX_CTRL_H)
#define __I3_TDK_PROPERTY_EDITBOX_CTRL_H

#pragma once

#include "i3TDKProperty.h"

/////////////////////////////////////////////////////////////////////////////
// i3TDKPropertyEditBoxCtrl window

class I3_EXPORT_TDK i3TDKPropertyEditBoxCtrl : public CEdit
{
// Construction
public:
	i3TDKPropertyEditBoxCtrl();

// Attributes
public:
	i3TDKProperty * CurProperty = nullptr;

// Operations
public:
	void SetProperty( i3TDKProperty *Property);
	void ClearSelect(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(i3TDKPropertyEditBoxCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~i3TDKPropertyEditBoxCtrl() {}

	// Generated message map functions
protected:
	//{{AFX_MSG(i3TDKPropertyEditBoxCtrl)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYEDITBOX_H__51CEDF81_3EF8_11D3_B5AD_0080C70D2C8F__INCLUDED_)
