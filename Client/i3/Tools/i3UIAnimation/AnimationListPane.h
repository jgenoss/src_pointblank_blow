#pragma once
#include "PaneBase.h"
class CAnimationListPane : public CPaneBase
{
	DECLARE_DYNAMIC(CAnimationListPane)
	DECLARE_MESSAGE_MAP()
	CDialogBar			m_DlgBar;
public:
	CAnimationListPane();
	virtual ~CAnimationListPane();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMDblclkListAnimation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListAnimation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);


	void AdjustLayout();
	void OnRemove(INT32 nIndex);
	void Update();

	afx_msg void OnNMRClickListAnimation(NMHDR *pNMHDR, LRESULT *pResult);
};

