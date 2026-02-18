#if !defined ( __I3TDK_PROPERTY_SUBITEM_COMBOBOX_H)
#define __I3TDK_PROPERTY_SUBITEM_COMBOBOX_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// i3TDKPropertySubItem

#include "i3TDKPropertySubItem.h"
#include "i3TDKListCtrlComboBox.h"

#define CB_WNDSTYLE_SIMPLE				CBS_SIMPLE
#define CB_WNDSTYLE_DROPDOWN			CBS_DROPDOWN
#define CB_WNDSTYLE_DROPDOWNLIST		CBS_DROPDOWNLIST
#define CB_WNDSTYLE_DISABLENOSCROLL		CBS_DISABLENOSCROLL

struct SubItemComboBoxInitStruct
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

class I3_EXPORT_TDK i3TDKPropertySubItemComboBox : public i3TDKPropertySubItem
{
	I3_EXPORT_CLASS_DEFINE( i3TDKPropertySubItemComboBox, i3TDKPropertySubItem);

protected:
	UINT32			m_WndStyle = 0;		//	wnd style

	INT32			m_nSelectIdx = 0;

#ifdef _UNICODE
	i3::vector<i3::rc_wstring>	m_ElementList;
#else
	i3::vector<i3::rc_string>	m_ElementList;
#endif

	i3TDKListCtrlComboBox	m_ComboBox;

public:

	i3TDKPropertySubItemComboBox(void) {}
	i3TDKPropertySubItemComboBox( SubItemComboBoxInitStruct * pInitStruct);
	virtual ~i3TDKPropertySubItemComboBox( void);

	BOOL	Create( SubItemComboBoxInitStruct * pInitStruct);	
	virtual void	Reset( void);
	virtual BOOL	IsActiveEdit( void)				{	return m_ComboBox.IsActive();					}
	virtual void	DestroyEdit( CWnd * pNewWnd)	{	m_ComboBox.SendMessage( WM_KILLFOCUS, (WPARAM) pNewWnd, 0);	}
	virtual void	CopyTo( i3TDKPropertySubItemComboBox * pSubItem);

	virtual	void	OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet);
	virtual void	OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle);
	virtual void	OnUpdate( SubItemUpdateInfo * pInfo);
	virtual void	OnRelease( void);

	void	SelectElement( INT32 idx)						  
			{	
				if( IsWindow( m_ComboBox.GetSafeHwnd()) )
					m_ComboBox.SelectElement( idx);	
				
				m_nSelectIdx = idx;
			}
	void	ClearElement( void)								
			{	
				if( IsWindow( m_ComboBox.GetSafeHwnd()) )
					m_ComboBox.ClearElement();

				m_ElementList.clear();	
				m_nSelectIdx = 0;
			}
	INT32	AddElement( const TCHAR* pszString)					{	INT32 idx = GetElementCount(); m_ElementList.push_back( pszString);	return idx; }

	INT32	InsertElement( INT32 idx, const TCHAR* pszString)		
	{	
#ifdef _UNICODE
		typedef i3::vector<i3::rc_wstring>::iterator iterator;
#else
		typedef i3::vector<i3::rc_string>::iterator iterator;
#endif
		iterator it = m_ElementList.insert(m_ElementList.begin() + idx, pszString);	
		return (INT32)(i3::distance(m_ElementList.begin(),it));
	}	

	INT32	DeleteElement( INT32 idx)						{	m_ElementList.erase(m_ElementList.begin() + idx);	return GetElementCount();	}
	INT32	FindElement( const TCHAR* pszString)			{	size_t idx = i3::vu::index_of(m_ElementList, pszString);	
																return (idx != m_ElementList.size()) ? INT32(idx) : -1;	}
	INT32	GetElementCount( void) const						{	return (INT32)m_ElementList.size();				}
	INT32	GetSelectIdx( void)								{	return m_nSelectIdx;							}
	const TCHAR * GetElement( INT32 idx) const				{	return m_ElementList[idx].c_str();	}	

#ifdef _UNICODE
	const i3::vector<i3::rc_wstring>& GetElementList( void)	const	{	return m_ElementList;	}
#else
	const i3::vector<i3::rc_string>& GetElementList( void) const	{	return m_ElementList;	}
#endif
};

#endif //__I3TDK_PROPERTY_SUBITEM_COMBOBOX_H


