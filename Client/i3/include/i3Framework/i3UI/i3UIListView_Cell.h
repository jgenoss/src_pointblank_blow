#ifndef __I3UI_LISTVIEW_CELL_H__
#define __I3UI_LISTVIEW_CELL_H__

#include "i3UICaptionControl.h"
#include "i3UIListView_Box.h"
#include "i3UIListView_Defines.h"

class I3_EXPORT_FRAMEWORK i3UIListView_Cell : public i3UICaptionControl	// i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIListView_Cell, i3UICaptionControl );

	friend class i3UIListView_Item;
	friend class i3UIListView_Box;

protected:
	ECELLTYPE			m_eType = ECELLTYPE_TEXT;
	i3UITemplate*		m_pChildTemplate = nullptr;
	i3Rect				m_ChildRect = { -1, -1, -1, -1 };	// Child가 Image/Template인 경우..

	virtual void		_UpdateShapeState(void) override;
	
public:
	i3UIListView_Cell();
	virtual ~i3UIListView_Cell(); 

	ECELLTYPE		getType( void)				{	return m_eType;		}
	void			setType( ECELLTYPE type)	{	m_eType = type;		}

	void			AddCellChild( i3UITemplate* pTemplate);
	void			SetCellChild( const TEMPLATE_INFO* Info);
	void			RemoveCellChild( void);
	void			ReplaceCellChild( i3UITemplate* pTemplate);
	i3UIControl*	getCellChild( void)							{	return (i3UIControl*)getFirstChild();	}

	void			setChildTemplate( i3UITemplate* pTemplate)	{	m_pChildTemplate = pTemplate;	}
	i3UITemplate*	getChildTemplate( void)						{	return m_pChildTemplate;	}

	void			setChildRect( INT32 x, INT32 y, INT32 w, INT32 h)	{	m_ChildRect.set( x, y, x+w, y+h);	}
	void			setChildRect( i3Rect* pRt)							{	if( pRt != nullptr)	setChildRect( pRt->left, pRt->top, pRt->getWidth(), pRt->getHeight());	}
	i3Rect*			getChildRect( void)									{	return &m_ChildRect;	}
	

	// =============
	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	virtual void	OnDraw( i3UIRenderer * pRenderer) override;

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse( void) override;

	virtual i3UIControl*	OnSelect( void) override;

	virtual bool	isOnMouse( I3UI_OVERLAP_INFO * pInfo) override;

	virtual void	SetSizeNoNotify( REAL32 cx, REAL32 cy) override;
	virtual void	SetSizeNoNotify( VEC2D * pVec) { SetSizeNoNotify( pVec->x, pVec->y); }

	//////////////////////////////////////////
	// Test Automation
public:
	virtual void	TA_GetPathID(char * pszFullPath, INT32 limit) override;
};

#endif //__I3UI_LISTVIEW_CELL_H__
