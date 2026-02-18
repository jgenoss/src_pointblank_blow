#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CProp_LvRes_Texture dialog
#include "ModifiedListCtrl.h"

class CProp_LvRes_Texture : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvRes_Texture)

protected:
	i3LevelResTexture * m_pLvResTexture;

public:
	CProp_LvRes_Texture(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvRes_Texture();

// Dialog Data
	enum { IDD = IDD_PROP_LVRES_TEXTURE };

	virtual void	SetObject( i3ElementBase * pObj) override;

protected:
	void	_UpdateResInfo( void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	CModifiedListCtrl m_listTextureInfo;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() override;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
