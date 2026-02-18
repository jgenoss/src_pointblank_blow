#if !defined ( __I3TDK_PROPERTY_SUBITEM_COLOR_H)
#define __I3TDK_PROPERTY_SUBITEM_COLOR_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// i3TDKPropertySubItem

#include "i3TDKPropertySubItem.h"

struct SubItemColorInitStruct
{
	INT32			_id = -1;				//	usable
	UINT32			_style = 0;				//	style
	UINT32			_type = 0;				//	usable
	const TCHAR*	_label = nullptr;				//	Label in control
	
	void *			_data = nullptr;				//	user data pointer
	
	INT32			_row = -1;
	INT32			_column = -1;

	I3COLOR			_initColor = { 0, 0, 0, 0 };
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////

class I3_EXPORT_TDK i3TDKPropertySubItemColor : public i3TDKPropertySubItem
{
	I3_EXPORT_CLASS_DEFINE( i3TDKPropertySubItemColor, i3TDKPropertySubItem);

protected:
	I3COLOR			m_Color;

public:

	i3TDKPropertySubItemColor( void);
	i3TDKPropertySubItemColor( SubItemColorInitStruct * pInitStruct);
	virtual ~i3TDKPropertySubItemColor( void);

	BOOL	Create( SubItemColorInitStruct * pInitStruct);
	virtual void	Reset( void);
	virtual BOOL	IsActiveEdit( void)				{	return FALSE;						}
	virtual void	DestroyEdit( CWnd * pNewWnd)	{	(void) 0;							}
	virtual void	CopyTo( i3TDKPropertySubItemColor * pSubItem);

	virtual	void	OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet);
	virtual void	OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle);
	virtual void	OnUpdate( SubItemUpdateInfo * pInfo);
	virtual void	OnRelease( void);

	virtual void	SetLabel( const I3COLOR * pColor);

	I3COLOR	*		GetColor( void)				{	return &m_Color;					}
	void			SetColor( I3COLOR * color)	{	m_Color = *color;					}   
};

#endif //__I3TDK_PROPERTY_SUBITEM_COLOR_H
