#pragma once
#include "afxcmn.h"


// CDlgTextureSelect dialog

class CDlgTextureSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgTextureSelect)

protected:
	i3LevelResTexture *		m_pSelTex;
	CImageList				m_ImageList;
	HDC						m_hDC;

public:
	bool					Execute( i3LevelResTexture * pTex);
	i3LevelResTexture *		getSelectedTexture(void)		{ return m_pSelTex; }

public:
	CDlgTextureSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTextureSelect();

// Dialog Data
	enum { IDD = IDD_DLG_TEXTURE_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_TexListCtrl;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDestroy();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
protected:
	virtual void OnOK() override;
};
