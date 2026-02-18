#pragma once
#include "i3TDKImageViewCtrl.h"
#include "PaneBase.h"

class CPropertyPane : public CPaneBase
{
	DECLARE_DYNAMIC(CPropertyPane)
	DECLARE_MESSAGE_MAP()


	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedADDUV();
	afx_msg void OnBnClickedADDAnimation();
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);

	void AdjustLayout();

	CButton						m_btADDUV;
	CButton						m_btADDAnimation;
	CMFCPropertyGridCtrl		m_wndPropList;
	INT32						m_nCurControlIndex;
public:

	CPropertyPane();
	virtual ~CPropertyPane();

	void		Init();
	void		OnUpdateButton(CCmdUI *pCmdUI);
	void		Update(UVITEM* pItem);
	void		RemoveAllProperty(CMFCPropertyGridProperty* pProperty);
	void		RemoveAllProperty();
	void		UpdateUVItem(CMFCPropertyGridProperty* pProperty);
	INT32		GetPropertyValue(CMFCPropertyGridProperty* pProperty);
};

