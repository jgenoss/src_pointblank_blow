#pragma once
#include "GraphEditCtrl.h"

// CPropertyKeyframe dialog

class CPropertyKeyframe : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyKeyframe)

protected:

public:
	CGraphEditCtrl		m_GraphEditor;

public:
	CPropertyKeyframe();
	virtual ~CPropertyKeyframe();

	void	Edit( i3KeyframeTable * pTable, INT32 type);

// Dialog Data
	enum { IDD = IDD_PROPERTY_KEYFRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
};
