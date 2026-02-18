#pragma once
#include "afxcmn.h"


// CProp_LvRes_SceneGraph dialog
#include "ModifiedListCtrl.h"

class CProp_LvRes_SceneGraph : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvRes_SceneGraph)

protected:
	i3LevelResSceneGraph	*	m_pResSceneGraph;

public:
	CProp_LvRes_SceneGraph(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvRes_SceneGraph();

// Dialog Data
	enum { IDD = IDD_PROP_LVRES_SCENEGRAPH };

	virtual void	SetObject( i3ElementBase * pObj) override;

protected:
	void	_UpdateResInfo( void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CModifiedListCtrl m_listSceneGraphInfo;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
