// i3RegDataCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKRegDataCtrl.h"
#include "i3TDKListCtrl.h"
#include "i3TDKPropertySubItemFilePath.h"
#include "i3TDKPropertySubItemUserCtrl.h"
// #include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/atof.h"

#define	ID_FIRST	1000

enum META_INDEX
{
	PROP_STATIC,
	PROP_EDIT,
	PROP_COMBO,
	PROP_COLOR,
	PROP_FILEPATH,
	PROP_USERDEF,
	MAX_PROP,
};

// i3RegDataCtrl
static i3ClassMeta * s_ItemMeta[MAX_PROP] = 
{
	i3TDKPropertySubItemStatic::static_meta(),
	i3TDKPropertySubItemEdit::static_meta(),
	i3TDKPropertySubItemComboBox::static_meta(),
	i3TDKPropertySubItemColor::static_meta(),
	i3TDKPropertySubItemFilePath::static_meta(),
	i3TDKPropertySubItemUserCtrl::static_meta(),
};


void ReplaceString(char* strDesc, const char* strSrc,const char* strOld,const char* strNew)
{
	I3ASSERT(strDesc);
	I3ASSERT(strSrc);
	I3ASSERT(strOld);
	I3ASSERT(strNew);
	
    UINT32 srcPosition= 0;
	UINT32 oldPosition = 0;

	// 원본 끝까지
	while(*strSrc)
	{
		// 교체할 문자열 비교
		if (strSrc[srcPosition] == strOld[oldPosition])
		{
			srcPosition++;
			oldPosition++;

			// 모든 교체 문자열이 일치하면 새 문자열로 치환
			if (0 == strOld[oldPosition])
			{
				UINT32 i = 0;
				
				while(strNew[i])
				{
					*strDesc = strNew[i];
					strDesc++;
					i++;
				}
				
				strSrc += srcPosition;
				srcPosition = 0;
				oldPosition = 0;
			}
		}
		else
		{
			srcPosition++;
			oldPosition = 0;

			// 일치하지 않으면 일반 복사
			for(UINT32 i = 0; i < srcPosition; i++)
			{
				*strDesc = *strSrc;
				strDesc++;
				strSrc++;
			}

			srcPosition = 0;
		}
	}

	*strDesc = 0;
}

struct ColorItem : public SubItemInitStruct
{
	I3COLOR				_initColor = { 255, 255, 255, 255 };
};

struct ComboItem : public SubItemInitStruct
{
	UINT32			_WndStyle = CB_WNDSTYLE_DROPDOWNLIST;
};

struct PathItem : public SubItemInitStruct
{
	char *			_szRelativePath = nullptr;	//	상대 경로
	char *			_szDialogName = nullptr;		//	

	UINT32			_DialogStyle = 0;		//	style	
};

struct UserItem : public SubItemInitStruct
{
	CALL_DRAW		_pCallOnDraw = nullptr;		//	Call OnDraw Func
	CALL_HIT		_pCallOnHit = nullptr;		//	Call OnHit Func
	CALL_UPDATE		_pCallOnUpdate = nullptr;		//	Call OnUpdate Func
	CALL_RELEASE	_pCallOnRelease = nullptr;	//	Call OnRelease Func
};


IMPLEMENT_DYNAMIC(i3TDKRegDataCtrl, i3TDKListCtrl)

i3TDKRegDataCtrl::~i3TDKRegDataCtrl()
{
	I3_SAFE_RELEASE( m_pData);
}


BEGIN_MESSAGE_MAP(i3TDKRegDataCtrl, i3TDKListCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// i3RegDataCtrl message handlers
int i3TDKRegDataCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	ClearAll();

	i3TDK::RegisterUpdate( m_hWnd,  i3TDKPropertySubItem::static_meta(), I3_TDK_UPDATE_EDIT);

	return 0;
}

void	i3TDKRegDataCtrl::SetData( i3RegData * pData)
{
	if( m_pData == pData)	return;
	
	I3_REF_CHANGE( m_pData, pData);

	_InitData();
}

i3TDKPropertySubItem *	i3TDKRegDataCtrl::_CreateNewItem( INT32 nRow, INT32 nCol, i3ClassMeta * pMeta, const TCHAR* pszName, INT32 nID)
{
	SubItemInitStruct * pItemInit = nullptr;
	i3TDKPropertySubItem * pSubItem = (i3TDKPropertySubItem*)pMeta->create_instance();	

	if( i3::kind_of<i3TDKPropertySubItemStatic*>(pMeta)) 
	{
		pItemInit = new SubItemInitStruct;
		pItemInit->_style = SUBITEM_STYLE_LABELTOOLTIP;
	}
	else if( i3::kind_of<i3TDKPropertySubItemColor*>(pMeta)) 
	{
		pItemInit = new ColorItem;
	}
	else if( i3::kind_of<i3TDKPropertySubItemComboBox*>(pMeta)) 
	{
		pItemInit = new ComboItem;

		pItemInit->_style		= SUBITEM_STYLE_LABELTOOLTIP;
		((ComboItem*)pItemInit)->_WndStyle	= CB_WNDSTYLE_DROPDOWNLIST;
	}
	else if (i3::kind_of<i3TDKPropertySubItemEdit* >(pMeta)) 
	{
		pItemInit = new ComboItem;
		pItemInit->_style = SUBITEM_STYLE_LABELTOOLTIP;
		((ComboItem*)pItemInit)->_WndStyle = EDIT_WNDSTYLE_LEFT;
	}
	else if( i3::kind_of<i3TDKPropertySubItemFilePath* >(pMeta)) 
	{
		pItemInit = new PathItem;

		pItemInit->_style		= SUBITEM_STYLE_LABELTOOLTIP;
		((ComboItem*)pItemInit)->_WndStyle	= FILEPATH_STYLE_RELATIVE_PATH;
	}
	else if( i3::kind_of<i3TDKPropertySubItemUserCtrl* >(pMeta)) 
	{
		pItemInit = new UserItem;
	}
	
	if (pItemInit)
	{
		pItemInit->_row = nRow;
		pItemInit->_column = nCol;
		pItemInit->_id = nID;
		pItemInit->_label = pszName;

		pSubItem->Create(pItemInit);

		delete pItemInit;
	}
	else
	{
		I3ASSERT(pItemInit != nullptr);
	}

	return pSubItem;
}

void	i3TDKRegDataCtrl::_InitData( void)
{
	if( m_pData == nullptr)	return;

	INT32	nDataType = m_pData->getDataType() &I3REG_TYPE_MASK;

	switch( nDataType)
	{
		case I3REG_TYPE_INT32:		_InitINT32Data();	break;
		case I3REG_TYPE_REAL32:		_InitREAL32Data();	break;
		case I3REG_TYPE_STRING:		_InitSTRINGData();	break;
		case I3REG_TYPE_VEC2D:		_InitVEC2DData();	break;
		case I3REG_TYPE_VEC3D:		_InitVEC3DData();	break;
		case I3REG_TYPE_VEC4D:		_InitVEC4DData();	break;
		case I3REG_TYPE_COLOR:		_InitCOLORData();	break;
		case I3REG_TYPE_MATRIX:		_InitMATRIXData();	break;
		default:
			break;
	}
}

void	i3TDKRegDataCtrl::_InitINT32Data()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 20);
	AddColumnTitle( _T("Value"), 80);

	NewSubItem( 1);

	i3TDKPropertySubItem * pItem = nullptr;

	{//
#ifdef _UNICODE
		i3::stack_wstring wstrDataName;		i3::mb_to_utf16(m_pData->GetName(), wstrDataName);
		const wchar_t* szDataName = wstrDataName.c_str();
#else
		const char* szDataName = m_pData->GetName();
#endif
		pItem = _CreateNewItem( 0, 0, s_ItemMeta[PROP_STATIC],  szDataName, ID_FIRST);
		SetSubItem( 0, 0, pItem);

		pItem = _CreateNewItem( 0, 1, s_ItemMeta[PROP_EDIT],  _T(""), ID_FIRST+1);
		pItem->SetLabel( *IRTO_INT32( m_pData));

		SetSubItem( 0, 1, pItem);
	}

}
void	i3TDKRegDataCtrl::_InitREAL32Data()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 20);
	AddColumnTitle( _T("Value"), 80);

	NewSubItem( 1);

	i3TDKPropertySubItem * pItem = nullptr;
	{//
#ifdef _UNICODE
		i3::stack_wstring wstrDataName;		i3::mb_to_utf16(m_pData->GetName(), wstrDataName);
		const wchar_t* szDataName = wstrDataName.c_str();
#else
		const char* szDataName = m_pData->GetName();
#endif

		pItem = _CreateNewItem( 0, 0, s_ItemMeta[PROP_STATIC],  szDataName, ID_FIRST);
		SetSubItem( 0, 0, pItem);

		pItem = _CreateNewItem( 0, 1, s_ItemMeta[PROP_EDIT],  _T(""), ID_FIRST+1);
		pItem->SetLabel( *IRTO_REAL32( m_pData));
		SetSubItem( 0, 1, pItem);
	}
}

void	i3TDKRegDataCtrl::_InitSTRINGData()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 20);
	AddColumnTitle( _T("Value"), 80);

	NewSubItem( 1);

	i3TDKPropertySubItem * pItem = nullptr;

	{//
#ifdef _UNICODE
		i3::stack_wstring wstrDataName;		i3::mb_to_utf16(m_pData->GetName(), wstrDataName);
		const wchar_t* szDataName = wstrDataName.c_str();
#else
		const char* szDataName = m_pData->GetName();
#endif

		pItem = _CreateNewItem( 0, 0, s_ItemMeta[PROP_STATIC],  szDataName, ID_FIRST);
		SetSubItem( 0, 0, pItem);

		pItem = _CreateNewItem( 0, 1, s_ItemMeta[PROP_EDIT],  _T(""), ID_FIRST+1);

#ifdef _UNICODE
		i3::wstring wstr = IRTO_STRING( m_pData);
		i3::generic_string_replace_all(wstr, L"\n", L"\\n");
		pItem->SetLabel( wstr.c_str());
#else
		i3::string mbstr;		i3::utf16_to_mb( IRTO_STRING( m_pData), mbstr);
		i3::generic_string_replace_all(mbstr, "\n", "\\n");
		pItem->SetLabel( mbstr.c_str() );
#endif
		

		SetSubItem( 0, 1, pItem);
	}
}

// 안씀
void	i3TDKRegDataCtrl::_InitTimeData()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 20);
	AddColumnTitle( _T("Time"), 80);

	NewSubItem( 1);

	i3TDKPropertySubItem * pItem = nullptr;

	{//
#ifdef _UNICODE
		i3::stack_wstring wstrDataName;		i3::mb_to_utf16(m_pData->GetName(), wstrDataName);
		const wchar_t* szDataName = wstrDataName.c_str();
#else
		const char* szDataName = m_pData->GetName();
#endif

		pItem = _CreateNewItem( 0, 0, s_ItemMeta[PROP_STATIC],  szDataName, ID_FIRST);
		SetSubItem( 0, 0, pItem);

		pItem = _CreateNewItem( 0, 1, s_ItemMeta[PROP_EDIT],  _T(""), ID_FIRST+1);
		
		//char conv[1024], conv2[1024];
		//i3::string_ncopy_nullpad( conv, IRTO_STRING( m_pData), 1024 -1);
		//ReplaceString( conv2, conv, "\n", "\\n");

		pItem->SetLabel( _T("1900-00-00 00:00:00") );

		SetSubItem( 0, 1, pItem);
	}
}

void	i3TDKRegDataCtrl::_InitVEC2DData()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 20);
	AddColumnTitle( _T("U"), 40);
	AddColumnTitle( _T("V"), 40);

	NewSubItem( 1);

	i3TDKPropertySubItem * pItem = nullptr;
	{//Name
#ifdef _UNICODE
		i3::stack_wstring wstrDataName;		i3::mb_to_utf16(m_pData->GetName(), wstrDataName);
		const wchar_t* szDataName = wstrDataName.c_str();
#else
		const char* szDataName = m_pData->GetName();
#endif

		pItem = _CreateNewItem( 0, 0, s_ItemMeta[PROP_STATIC],  szDataName, ID_FIRST);
		SetSubItem( 0, 0, pItem);

		VEC2D * pVec = IRTO_VEC2D( m_pData);

		pItem = _CreateNewItem( 0, 1, s_ItemMeta[PROP_EDIT], _T(""), ID_FIRST+1);
		pItem->SetLabel( getU( pVec));
		SetSubItem( 0, 1, pItem);

		pItem = _CreateNewItem( 0, 2, s_ItemMeta[PROP_EDIT], _T(""), ID_FIRST+2);
		pItem->SetLabel( getV( pVec));
		SetSubItem( 0, 2, pItem);
	}
}

void	i3TDKRegDataCtrl::_InitVEC3DData()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 25);
	AddColumnTitle( _T("X"), 25);
	AddColumnTitle( _T("Y"), 25);
	AddColumnTitle( _T("Z"), 25);

	NewSubItem( 1);

	i3TDKPropertySubItem * pItem = nullptr;
	{//Name
#ifdef _UNICODE
		i3::stack_wstring wstrDataName;		i3::mb_to_utf16(m_pData->GetName(), wstrDataName);
		const wchar_t* szDataName = wstrDataName.c_str();
#else
		const char* szDataName = m_pData->GetName();
#endif

		pItem = _CreateNewItem( 0, 0, s_ItemMeta[PROP_STATIC],  szDataName, ID_FIRST);
		SetSubItem( 0, 0, pItem);

		VEC3D * pVec = IRTO_VEC3D( m_pData);

		pItem = _CreateNewItem( 0, 1, s_ItemMeta[PROP_EDIT], _T(""), ID_FIRST+1);
		pItem->SetLabel( getX( pVec));
		SetSubItem( 0, 1, pItem);

		pItem = _CreateNewItem( 0, 2, s_ItemMeta[PROP_EDIT], _T(""), ID_FIRST+2);
		pItem->SetLabel( getY( pVec));
		SetSubItem( 0, 2, pItem);

		pItem = _CreateNewItem( 0, 3, s_ItemMeta[PROP_EDIT], _T(""), ID_FIRST+3);
		pItem->SetLabel( getZ( pVec));
		SetSubItem( 0, 3, pItem);
	}
}

void	i3TDKRegDataCtrl::__InitVEC4D( const TCHAR * szTitle, INT32 nRow, INT32 nFirstID, VEC4D * pVec)
{
	i3TDKPropertySubItem * pItem = nullptr;
	{
		pItem = _CreateNewItem( nRow, 0, s_ItemMeta[PROP_STATIC],  szTitle, nFirstID);
		SetSubItem( nRow, 0, pItem);

		pItem = _CreateNewItem( nRow, 1, s_ItemMeta[PROP_EDIT], _T(""), nFirstID + 1);
		pItem->SetLabel( getX( pVec));
		SetSubItem( nRow, 1, pItem);

		pItem = _CreateNewItem( nRow, 2, s_ItemMeta[PROP_EDIT], _T(""), nFirstID + 2);
		pItem->SetLabel( getY( pVec));
		SetSubItem( nRow, 2, pItem);

		pItem = _CreateNewItem( nRow, 3, s_ItemMeta[PROP_EDIT], _T(""), nFirstID + 3);
		pItem->SetLabel( getZ( pVec));
		SetSubItem( nRow, 3, pItem);

		pItem = _CreateNewItem( nRow, 4, s_ItemMeta[PROP_EDIT], _T(""), nFirstID + 4);
		pItem->SetLabel( getW( pVec));
		SetSubItem( nRow, 4, pItem);
	}
}

void	i3TDKRegDataCtrl::_InitVEC4DData()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 20);
	AddColumnTitle( _T("X"), 20);
	AddColumnTitle( _T("Y"), 20);
	AddColumnTitle( _T("Z"), 20);
	AddColumnTitle( _T("W"), 20);

	NewSubItem( 1);

#ifdef _UNICODE
	i3::stack_wstring wstrDataName;		i3::mb_to_utf16(m_pData->GetName(), wstrDataName);
	const wchar_t* szDataName = wstrDataName.c_str();
#else
	const char* szDataName = m_pData->GetName();
#endif

	__InitVEC4D( szDataName, 0, ID_FIRST, IRTO_VEC4D( m_pData));
}

void	i3TDKRegDataCtrl::_InitCOLORData()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 20);
	AddColumnTitle( _T("Name"), 80);

	NewSubItem( 1);

	i3TDKPropertySubItem * pItem = nullptr;

	{//Name
		pItem = _CreateNewItem( 0, 0, s_ItemMeta[PROP_STATIC],  _T("DataName"), ID_FIRST);
		SetSubItem( 0, 0, pItem);

		pItem = _CreateNewItem( 0, 1, s_ItemMeta[PROP_COLOR],  _T(""), ID_FIRST+1);
		pItem->SetLabel( IRTO_COLOR( m_pData));
		SetSubItem( 0, 1, pItem);
	}
}

void	i3TDKRegDataCtrl::_InitMATRIXData()
{
	ClearAll();

	AddColumnTitle( _T("Name"), 20);
	AddColumnTitle( _T("X"), 20);
	AddColumnTitle( _T("Y"), 20);
	AddColumnTitle( _T("Z"), 20);
	AddColumnTitle( _T("W"), 20);

	NewSubItem( 4);

	MATRIX * mtx = IRTO_MATRIX( m_pData);

	__InitVEC4D( _T("Right"),	0, ID_FIRST,		(VEC4D*)i3Matrix::GetRight( mtx));
	__InitVEC4D( _T("Up"),		1, ID_FIRST + 5,	(VEC4D*)i3Matrix::GetUp( mtx));
	__InitVEC4D( _T("At"),		2, ID_FIRST + 10,	(VEC4D*)i3Matrix::GetAt( mtx));
	__InitVEC4D( _T("Pos"),		3, ID_FIRST + 15,	(VEC4D*)i3Matrix::GetPos( mtx));
}

void	i3TDKRegDataCtrl::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( i3::kind_of<i3TDKPropertySubItem*>(pInfo->m_pMeta))
	{
		i3TDKPropertySubItem * pItem = (i3TDKPropertySubItem * )pInfo->m_pObject;

		INT32	nDataType = m_pData->getDataType() &I3REG_TYPE_MASK;

		switch( nDataType)
		{
			case I3REG_TYPE_INT32:		_GetINT32Data(pItem);	break;
			case I3REG_TYPE_REAL32:		_GetREAL32Data(pItem);	break;
			case I3REG_TYPE_STRING:		_GetSTRINGData(pItem);	break;
			case I3REG_TYPE_VEC2D:		_GetVEC2DData(pItem);	break;
			case I3REG_TYPE_VEC3D:		_GetVEC3DData(pItem);	break;
			case I3REG_TYPE_VEC4D:		_GetVEC4DData(pItem);	break;
			case I3REG_TYPE_COLOR:		_GetCOLORData(pItem);	break;
			case I3REG_TYPE_MATRIX:		_GetMATRIXData(pItem);	break;
			default:
				break;
		}

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pData, i3RegData::static_meta());
	}
}

LRESULT i3TDKRegDataCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}
	
	return i3TDKListCtrl::WindowProc(message, wParam, lParam);
}

void	i3TDKRegDataCtrl::_GetINT32Data(i3TDKPropertySubItem * pItem)
{
	if( i3::same_of< i3TDKPropertySubItemEdit*>(pItem))
	{
		i3TDKPropertySubItemEdit * pEdit = (i3TDKPropertySubItemEdit*)pItem;
		if( pEdit->GetID() == ID_FIRST+1)
		{
			*((i3RegINT32*)m_pData) = (INT32)i3::atoi( pEdit->GetLabel());
		}
	}
}

void	i3TDKRegDataCtrl::_GetREAL32Data(i3TDKPropertySubItem * pItem)
{
	if( i3::same_of< i3TDKPropertySubItemEdit*>(pItem))
	{
		i3TDKPropertySubItemEdit * pEdit = (i3TDKPropertySubItemEdit*)pItem;
		if( pEdit->GetID() == ID_FIRST+1)
		{
			*((i3RegREAL32*)m_pData) = (REAL32)i3::atof( pEdit->GetLabel());
		}
	}
}

void	i3TDKRegDataCtrl::_GetSTRINGData(i3TDKPropertySubItem * pItem)
{
	if ( strcmp( m_pData->GetName(), "__Update Time" ) == 0 || strcmp( m_pData->GetName(), "__Make Time" ) == 0 )
	{
		return ;
	}

	if( i3::same_of<i3TDKPropertySubItemEdit*>(pItem))
	{
		i3TDKPropertySubItemEdit * pEdit = (i3TDKPropertySubItemEdit*)pItem;
		if( pEdit->GetID() == ID_FIRST+1)
		{

#ifdef _UNICODE
			i3::stack_wstring conv(pEdit->GetLabel() );
			i3::generic_string_replace_all(conv, L"\\n", L"\n");
			const i3::stack_wstring& utf16str = conv;
#else
			i3::stack_string	conv(pEdit->GetLabel());
			i3::generic_string_replace_all(conv, "\\n", "\n");
			i3::wstring utf16str;	i3::mb_to_utf16(conv, utf16str);
#endif
			((i3RegString*)m_pData)->setValue( utf16str );

		}
	}
}

void	i3TDKRegDataCtrl::_GetVEC2DData(i3TDKPropertySubItem * pItem)
{
	if( i3::same_of<i3TDKPropertySubItemEdit* >(pItem))
	{
		i3TDKPropertySubItemEdit * pEdit = (i3TDKPropertySubItemEdit*)pItem;
		VEC2D * pVec = IRTO_VEC2D( m_pData);

		if( pEdit->GetID() == ID_FIRST+1)
		{
			setU( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
		else if( pEdit->GetID() == ID_FIRST+2)
		{
			setV( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
	}
}

void	i3TDKRegDataCtrl::_GetVEC3DData(i3TDKPropertySubItem * pItem)
{
	if( i3::same_of<i3TDKPropertySubItemEdit* >(pItem))
	{
		i3TDKPropertySubItemEdit * pEdit = (i3TDKPropertySubItemEdit*)pItem;
		VEC3D * pVec = IRTO_VEC3D( m_pData);

		if( pEdit->GetID() == ID_FIRST+1)
		{
			setX( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
		else if( pEdit->GetID() == ID_FIRST+2)
		{
			setY( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
		else if( pEdit->GetID() == ID_FIRST+3)
		{
			setZ( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
	}
}

void	i3TDKRegDataCtrl::_GetVEC4DData(i3TDKPropertySubItem * pItem)
{
	if( i3::same_of<i3TDKPropertySubItemEdit*>(pItem))
	{
		i3TDKPropertySubItemEdit * pEdit = (i3TDKPropertySubItemEdit*)pItem;
		VEC4D * pVec = IRTO_VEC4D( m_pData);

		if( pEdit->GetID() == ID_FIRST+1)
		{
			setX( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
		else if( pEdit->GetID() == ID_FIRST+2)
		{
			setY( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
		else if( pEdit->GetID() == ID_FIRST+3)
		{
			setZ( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
		else if( pEdit->GetID() == ID_FIRST+4)
		{
			setW( pVec, (REAL32)i3::atof(pEdit->GetLabel()));
		}
	}
}

void	i3TDKRegDataCtrl::_GetCOLORData(i3TDKPropertySubItem * pItem)
{
	if( i3::same_of<i3TDKPropertySubItemColor* >(pItem))
	{
		i3TDKPropertySubItemColor * pColItem = (i3TDKPropertySubItemColor*)pItem;

		I3COLOR * pCol = IRTO_COLOR( m_pData);

		i3Color::Set( pCol, pColItem->GetColor());
	}
}

void	i3TDKRegDataCtrl::_GetMATRIXData(i3TDKPropertySubItem * pItem)
{
	if( i3::same_of<i3TDKPropertySubItemEdit* >(pItem))
	{
		i3TDKPropertySubItemEdit * pEdit = (i3TDKPropertySubItemEdit*)pItem;
		MATRIX * mtx = IRTO_MATRIX( m_pData);

		VEC4D * pVec = nullptr;
		INT32	nIdx = pItem->GetID() - ID_FIRST;
		I3TRACE( "%d\n", nIdx);

		if(( nIdx > 0) && ( nIdx < 5))
		{
			pVec = (VEC4D*)i3Matrix::GetRight( mtx);
			nIdx -= 1;
		}
		else if(( nIdx > 5) && ( nIdx < 10))
		{
			pVec = (VEC4D*)i3Matrix::GetUp( mtx);
			nIdx -= 6;
		}
		else if(( nIdx > 10) && ( nIdx < 15))
		{
			pVec = (VEC4D*)i3Matrix::GetAt( mtx);
			nIdx -= 11;
		}
		else if(( nIdx > 15) && ( nIdx < 20))
		{
			pVec = (VEC4D*)i3Matrix::GetPos( mtx);
			nIdx -= 16;
		}

		if( pVec != nullptr)
		{
			switch( nIdx)
			{
				case 0:	setX( pVec, (REAL32)i3::atof(pEdit->GetLabel()));	break;
				case 1:	setY( pVec, (REAL32)i3::atof(pEdit->GetLabel()));	break;
				case 2:	setZ( pVec, (REAL32)i3::atof(pEdit->GetLabel()));	break;
				case 3:	setW( pVec, (REAL32)i3::atof(pEdit->GetLabel()));	break;
			}
		}
	}
}