#pragma once

#include "UIPaneBase.h"


// CUIProperties
class CUIPaneProperties : public CUIPaneBase
{
	DECLARE_DYNAMIC(CUIPaneProperties)

protected:
	i3TDKPropertyGridCtrl m_wndPropList;

public:
	CUIPaneProperties();
	virtual ~CUIPaneProperties();

	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	DECLARE_MESSAGE_MAP()

	void InitPropList();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void DragTemplate(i3UITemplate* pTemplate);

	void		Update( void);
	void		SendMessagesToPropertyGrid( const i3::vector<PROPERTYMSG*>& msgList);
	
protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	
};


