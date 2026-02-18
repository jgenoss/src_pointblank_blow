#pragma once

#include "SceneGraphTreeCtrl.h"


// Panel_SceneGraph

class Panel_SceneGraph : public CDockablePane
{
	DECLARE_DYNAMIC(Panel_SceneGraph)
protected:
	CSceneGraphTreeCtrl		m_SgCtrl;

protected:
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	Panel_SceneGraph();
	virtual ~Panel_SceneGraph();

	CSceneGraphTreeCtrl * getSceneGraphTreeCtrl()	{return &m_SgCtrl;}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};


