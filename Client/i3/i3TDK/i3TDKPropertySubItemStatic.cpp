#include "stdafx.h"
#include "i3TDKPropertySubItemStatic.h"

I3_CLASS_INSTANCE( i3TDKPropertySubItemStatic);

i3TDKPropertySubItemStatic::i3TDKPropertySubItemStatic( SubItemStaticInitStruct * pInitStruct)
{
	Create( pInitStruct);
}

BOOL i3TDKPropertySubItemStatic::Create( SubItemStaticInitStruct * pInitStruct)
{
	Reset();

	SubItemInitStruct init;
	init._id = pInitStruct->_id;
	init._row = pInitStruct->_row;
	init._column = pInitStruct->_column;
	init._style = pInitStruct->_style;   
	init._label = pInitStruct->_label;
	init._data = pInitStruct->_data;

	return i3TDKPropertySubItem::Create( &init);
}

void i3TDKPropertySubItemStatic::OnRelease( void)
{
}

void i3TDKPropertySubItemStatic::OnUpdate( SubItemUpdateInfo * pInfo)
{	
	i3TDKPropertySubItem::OnUpdate( pInfo);
}

void i3TDKPropertySubItemStatic::OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet)
{
	i3TDKPropertySubItem::OnDraw( lpDrawItemStruct, pRect, pColorSet);
}

void i3TDKPropertySubItemStatic::OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle)
{
}

