#if !defined ( __I3TDK_PROPERTY_SUBITEM_H)
#define __I3TDK_PROPERTY_SUBITEM_H

#define SUBITEM_MAX_STRING		1024
#define SUBITEM_MAX_WCHAR		256

#define SUBITEM_STYLE_BASE_MASK			0xFFFF0000
#define SUBITEM_STYLE_EXTEND_MASK		0x0000FFFF

#define SUBITEM_STYLE_LABELTOOLTIP		0x00010000		//	적용시 Label 텍스트가 툴팁으로 나옵니다.

#define SUBITEM_STYLE_TEXTALIGN_LEFT	0x00100000		//	텍스트 왼쪽 정렬
#define SUBITEM_STYLE_TEXTALIGN_RIGHT	0x00200000		//	텍스트 오른쪽 정렬
#define SUBITEM_STYLE_TEXTALIGN_CENTER	0x00300000		//	텍스트 중안 정렬

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// i3TDKPropertySubItem
struct SubItemUpdateInfo
{
	INT32	_nRowIdx = 0;
	INT32	_nColumnIdx = 0;

	const TCHAR*	_pszText = nullptr;

} ;

struct SubItemColorSet
{
	COLORREF _ColorBK = 0;
	COLORREF _ColorSelect = 0;
	COLORREF _Focused = 0;

} ;

struct SubItemInitStruct
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


class I3_EXPORT_TDK i3TDKPropertySubItem : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3TDKPropertySubItem, i3NamedElement);

protected:
	
	INT32			m_ID;				//	usable
	UINT32			m_Style;			//	style	
	UINT32			m_Type;				//	usable
	TCHAR			m_szLabel[ SUBITEM_MAX_STRING];		//	Label in control
	COLORREF		m_colorFont;
	
	i3::wstring		m_wstrToolTip;

	void *			m_pUserData;		//	data pointer

	INT32			m_nRowIdx;
	INT32			m_nColumnIdx;

public:

	i3TDKPropertySubItem( void);
	i3TDKPropertySubItem( SubItemInitStruct * pInitStruct);
	virtual ~i3TDKPropertySubItem( void);

	UINT32			GetStyle( void)			{	return m_Style;				}
	UINT32			GetID( void)			{	return m_ID;				}
	UINT32			GetType( void)			{	return m_Type;				}
	const TCHAR *	GetLabel( void)	const	{	return m_szLabel;			}
	const WCHAR16 *	GetToolTipText( void) const	{	return m_wstrToolTip.c_str();	}
	void *			GetUserData( void)		{	return m_pUserData;			}
	COLORREF		GetFontColor( void)		{	return m_colorFont;			}

	INT32			GetRowIndex( void)		{	return m_nRowIdx;			}
	INT32			GetColumnIndex( void)	{	return m_nColumnIdx;		}

	void			SetStyle( UINT32 style)			{	m_Style = style;					}
	void			AddStyle( UINT32 style)			{	m_Style |= style;					}
	void			RemoveStyle( UINT32 style)		{	m_Style &= ~style;					}
	void			SetID( INT32 id)				{	m_ID = id;							}
	void			SetType( UINT32 type)			{	m_Type = type;						}
	void			AddType( UINT32 type)			{	m_Type |= type;						}
	void			SetFontColor( COLORREF color)	{	m_colorFont = color;				}

	void			SetUserData( void * pData)		{	m_pUserData = pData;				}
	void			SetToolTipText( const TCHAR * pszText);

	void			SetRowIndex( INT32 idx)			{	m_nRowIdx = idx;					}
	void			SetColumnIndex( INT32 idx)		{	m_nColumnIdx = idx;					}
							
	BOOL			Create( SubItemInitStruct * pInitStruct);
	virtual void	Reset( void);
	virtual BOOL	IsActiveEdit( void)				{	return FALSE;						}
	virtual void	DestroyEdit( CWnd * pNewWnd)	{	(void) 0;							}
	virtual void	CopyTo( i3TDKPropertySubItem * pProperyItem);

	virtual void	SetLabel( const TCHAR* pLabel);					
	virtual void	SetLabel( const I3COLOR * pColor);					
	virtual void	SetLabel( const REAL32	val);						
	virtual void	SetLabel( const UINT32	val);					
	virtual void	SetLabel( const INT32	val);	

	virtual	void	OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet);
	virtual void	OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle);
	virtual void	OnUpdate( SubItemUpdateInfo * pInfo);
	virtual void	OnRelease( void);
	
};

#endif	//__I3TDK_PROPERTY_SUBITEM_H
