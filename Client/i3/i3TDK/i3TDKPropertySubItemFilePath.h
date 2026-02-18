#if !defined ( __I3TDK_PROPERTY_SUBITEM_FILEPATH_H)
#define __I3TDK_PROPERTY_SUBITEM_FILEPATH_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// i3TDKPropertySubItemFilePath

#include "i3TDKPropertySubItem.h"

#define FILEPATH_STYLE_ONLY_FOLDER		0x00000001		//	폴더 Dialog
#define FILEPATH_STYLE_RELATIVE_PATH	0x00000002		//	상대 경로 사용

//	File Dialog 만 가능
#define FILE_HIDEREADONLY		OFN_HIDEREADONLY
#define FILE_LONGNAMES			OFN_LONGNAMES
#define FILE_NONETWORK			OFN_NONETWORKBUTTON
#define FILE_NOCHANGEDIR		OFN_NOCHANGEDIR
#define FILE_NOTESTFILECREATE	OFN_NOTESTFILECREATE
#define FILE_FILEMUSTEXIST		OFN_FILEMUSTEXIST
#define FILE_PATHMUSTEXIST		OFN_PATHMUSTEXIST


struct SubItemFilePathInitStruct
{
	INT32			_id = -1;				//	usable
	UINT32			_style = 0;				//	style
	UINT32			_type = 0;				//	usable
	const TCHAR*	_label = nullptr;				//	Label in control
	COLORREF		_font_color = 0;		//	text color
	
	void *			_data = nullptr;				//	user data pointer
	
	INT32			_row = -1;
	INT32			_column = -1;

	const TCHAR*	_szRelativePath = nullptr;	//	상대 경로
	const TCHAR*	_szDialogName = nullptr;		//	

	UINT32			_DialogStyle = 0;		//	style	
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////

class I3_EXPORT_TDK i3TDKPropertySubItemFilePath : public i3TDKPropertySubItem
{
	I3_EXPORT_CLASS_DEFINE( i3TDKPropertySubItemFilePath, i3TDKPropertySubItem);

protected:
	TCHAR			m_szRelativePath[256];
	TCHAR			m_szDialogName[256];

	UINT32			m_DialogStyle;		//	Dialog style

public:

	i3TDKPropertySubItemFilePath( void);
	i3TDKPropertySubItemFilePath( SubItemFilePathInitStruct * pInitStruct);
	virtual ~i3TDKPropertySubItemFilePath(void) {}

	BOOL	Create( SubItemFilePathInitStruct * pInitStruct);
	void	SetRelativePath( const TCHAR * pszPath)	{	i3::safe_string_copy( m_szRelativePath, pszPath, 256);		}
	const TCHAR* GetRelativePath( void) const		{	return m_szRelativePath;						}

	void	SetDialogName( const TCHAR* pszTitle)	{	i3::safe_string_copy( m_szDialogName, pszTitle, 256);		}
	const TCHAR* GetDialogName( void) const			{	return m_szDialogName;							}
	
	virtual void	Reset( void);
	virtual BOOL	IsActiveEdit( void)				{	return FALSE;						}
	virtual void	DestroyEdit( CWnd * pNewWnd)	{	(void) 0;							}
	virtual void	CopyTo( i3TDKPropertySubItemFilePath * pSubItem);

	virtual	void	OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet);
	virtual void	OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle);
	virtual void	OnUpdate( SubItemUpdateInfo * pInfo);
	virtual void	OnRelease( void);
};

#endif //__I3TDK_PROPERTY_SUBITEM_FILEPATH_H
