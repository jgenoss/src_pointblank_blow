#pragma once
#include "PaneBase.h"
class CImageListPane : public CPaneBase
{
	DECLARE_DYNAMIC(CImageListPane)
	DECLARE_MESSAGE_MAP()
	CImageList			m_ImageList;
	CDialogBar			m_DlgBar;
public:
	CImageListPane();
	virtual ~CImageListPane();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMDblclkTreeResourcelist(NMHDR *pNMHDR, LRESULT *pResult);

	void Init();
	void AdjustLayout();
	void LoadImage();
	void ChildbyLoadImage(char* chDir, TVINSERTSTRUCT tvi, HTREEITEM hItem);
};

