#if !defined ( __I3TDK_PROPERTY_SUBITEM_EDIT_H)
#define __I3TDK_PROPERTY_SUBITEM_EDIT_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// i3TDKPropertySubItem

#include "i3TDKPropertySubItem.h"
#include "i3TDKListCtrlEdit.h"

#define EDIT_WNDSTYLE_LEFT		ES_LEFT
#define	EDIT_WNDSTYLE_RIGHT		ES_RIGHT
#define	EDIT_WNDSTYLE_CENTER	ES_CENTER
#define	EDIT_WNDSTYLE_NUMBER	ES_NUMBER

#define EDIT_STYLE_FLOATING_POINT	0x00000001
#define EDIT_STYLE_INTEGER			0x00000002

struct SubItemEditInitStruct
{
	INT32			_id = -1;				//	usable
	UINT32			_style = 0;				//	style	
	UINT32			_type = 0;				//	usable
	const TCHAR*	_label = nullptr;				//	Label in control
	COLORREF		_font_color = 0;		//	text color
	
	void *			_data = nullptr;				//	user data pointer

	INT32			_row = -1;
	INT32			_column = -1;

	UINT32			_WndStyle = 0;			//	style	
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////

class I3_EXPORT_TDK i3TDKPropertySubItemEdit : public i3TDKPropertySubItem
{
	I3_EXPORT_CLASS_DEFINE( i3TDKPropertySubItemEdit, i3TDKPropertySubItem);  

protected:
	UINT32				m_WndStyle = 0;		//	wnd style

	i3TDKListCtrlEdit	m_Edit;

public:

	i3TDKPropertySubItemEdit(void) {}
	i3TDKPropertySubItemEdit( SubItemEditInitStruct * pInitStruct);
	virtual ~i3TDKPropertySubItemEdit(void) {}

	BOOL	Create( SubItemEditInitStruct * pInitStruct);
	virtual void	Reset( void);
	virtual BOOL	IsActiveEdit( void)				{	return m_Edit.IsActive();						}
	virtual void	DestroyEdit( CWnd * pNewWnd)	{	m_Edit.SendMessage( WM_KILLFOCUS, (WPARAM) pNewWnd, 0);	}
	virtual void	CopyTo( i3TDKPropertySubItemEdit * pSubItem);

	virtual	void	OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet);
	virtual void	OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle);
	virtual void	OnUpdate( SubItemUpdateInfo * pInfo);
	virtual void	OnRelease( void);
};

#endif //__I3TDK_PROPERTY_SUBITEM_EDIT_H
