#if !defined( __I3_TDK_PROPERTY_SUBITEM_STATIC_H)
#define __I3_TDK_PROPERTY_SUBITEM_STATIC_H

#include "i3TDKPropertySubItem.h"

struct SubItemStaticInitStruct
{
	INT32			_id = -1;				//	usable
	UINT32			_style = 0;				//	Edit style
	UINT32			_type = 0;				//	usable
	const TCHAR*	_label = nullptr;				//	Label in control
	COLORREF		_font_color = 0;		//	text color
	
	void *			_data = nullptr;				//	user data pointer

	INT32			_row = -1;
	INT32			_column = -1;
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////

class I3_EXPORT_TDK i3TDKPropertySubItemStatic : public i3TDKPropertySubItem
{
	I3_EXPORT_CLASS_DEFINE( i3TDKPropertySubItemStatic, i3TDKPropertySubItem);

public:
	i3TDKPropertySubItemStatic(void) {}
	i3TDKPropertySubItemStatic( SubItemStaticInitStruct * pInitStruct);
	virtual ~i3TDKPropertySubItemStatic(void) {}

	BOOL	Create( SubItemStaticInitStruct * pInitStruct);

	virtual	void	OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet);
	virtual void	OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle);
	virtual void	OnUpdate( SubItemUpdateInfo * pInfo);
	virtual void	OnRelease( void);
};

#endif //__I3_TDK_PROPERTY_SUBITEM_STATIC_H