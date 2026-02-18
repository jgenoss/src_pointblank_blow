#pragma once

#include "AI.h"

// CAIGraphView

class CAIGraphView : public i3GICDiagramView
{
	DECLARE_DYNAMIC(CAIGraphView)
protected:
	AI *			m_pAI;

	i3GICDeviceContext *		m_pGDC;

public:
	CAIGraphView();
	virtual ~CAIGraphView();

	AI *			getAI(void)					{ return m_pAI; }
	void			SetAI( AI * pAI);

	i3GICDeviceContext *		getGDC( void)	{ return m_pGDC; }

	int OnCreate( LPCREATESTRUCT lpCreateStruct);

	virtual void	OnDraw( void);

	virtual bool	OnCreateShape( i3GICShape * pShape);
	virtual void	OnSelectShape( i3GICShape * pShape, GIC_MOUSEMODE mode);

	virtual void	OnBeforePasteFromClipboard( i3::vector<i3PersistantElement*>& List);

	virtual void	OnRButtonUpProcess( UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()
};


