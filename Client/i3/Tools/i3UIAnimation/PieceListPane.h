#pragma once
#include "ImageListCtrl.h"
#include "PaneBase.h"
class CPieceListPane : public CPaneBase
{
	DECLARE_DYNAMIC(CPieceListPane)
	DECLARE_MESSAGE_MAP()


	UVITEM*					m_pSelectItem;
public:
	CImageListCtrl			m_UVListCtrl;
	CPieceListPane();
	virtual ~CPieceListPane();

	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);

	void			AdjustLayout();
	void			AddItem(UVITEM* pUVItem, LVITEM* pControlItem);
	void			RemoveAllItem();
	void			UpdateUVItem(UVITEM* pUVItem, INT32 nIndex);
	void			SetFocusdItem(INT32 nIndex);
	void			AllItemNonFocuse();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};

