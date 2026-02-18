#pragma once
#include "afxcmn.h"
#include "afxwin.h"
//#include "LayerListCtrl.h"
//#include "LayerView.h"

// CDlgLayer dialog

class CDlgLayer : public CDialog
{
	DECLARE_DYNAMIC(CDlgLayer)

public:
	CDlgLayer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLayer();

// Dialog Data
	enum { IDD = IDD_DLG_LAYER };

protected:
	CImageList		m_ImageList;
	CImageList		m_ElmImageList;
	CImageList *	m_pDragImage;
	bool			m_bDrag;

	HCURSOR			m_hCursorArrow;
	HCURSOR			m_hCursorNo;

	CToolTipCtrl	m_Tooltip;

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	void	SetCurLayer( INT32 idx);
	void	ToggleShow( INT32 idx);
	void	ToggleFreeze( INT32 idx);

	INT32	getSelectedLayer(void);
	INT32	_getElmIcon( i3LevelElement3D * pElm);

	void	_getSelectedElement( i3::vector<i3LevelElement3D *>& List);
	void	DropElm( CPoint pt);

	void	OnDeleteElement(void);
	void	OnSelectFit(void);
	void	getSelectedElements( i3::vector< INT32> * pIdxList);

public:

	void		UpdateAllLayer(void);
	void		UpdateLayer( INT32 idx, i3LevelLayer * pLayer);
	void		_PlaceControl( INT32 cx, INT32 cy);

	void		UpdateAllElements(void);
	void		UpdateElement( INT32 idx, i3LevelElement3D * pElm);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_BTN_New;
	CButton m_BTN_Delete;
	afx_msg void OnBnClickedBtnNew();
	afx_msg void OnBnClickedBtnDelete();
	CListCtrl m_LayerListCtrl;
	CListCtrl m_ElmListCtrl;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLvnEndlabeleditLayerList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkLayerList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickLayerList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragElmList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	CButton m_BTN_Select;
	afx_msg void OnBnClickedBtnSelectElm();
	afx_msg void OnNMSetfocusElmList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnNMRClickElmList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkElmList(NMHDR *pNMHDR, LRESULT *pResult);
};
