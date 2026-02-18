// i3RegKeyCtrl.cpp : implementation file
//
#include "stdafx.h"
#include "i3TDKRegKeyCtrl.h"
#include <algorithm>

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/compare/generic_is_iless.h"

// i3RegKeyCtrl
static const TCHAR* szTypeString[] = 
{
	_T("정수형"),
	_T("실수형"),
	_T("문자열"),
	_T("벡터 2D"),
	_T("벡터 3D"),
	_T("벡터 4D"),
	_T("색상"),
	_T("매트릭스"),
};

IMPLEMENT_DYNAMIC(i3TDKRegKeyCtrl, CListCtrl)


i3TDKRegKeyCtrl::~i3TDKRegKeyCtrl()
{
	m_Font.DeleteObject();
	I3_SAFE_RELEASE( m_pKey);
}

BEGIN_MESSAGE_MAP(i3TDKRegKeyCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnLvnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &i3TDKRegKeyCtrl::OnLvnColumnclick)
END_MESSAGE_MAP()

// i3RegKeyCtrl message handlers
void i3TDKRegKeyCtrl::SetRegKey( i3RegKey * pKey)
{
	DeleteAllItems();

	I3_REF_CHANGE( m_pKey, pKey);

	if( m_pKey != nullptr)
	{
		_UpdateKey();
	}
}

void i3TDKRegKeyCtrl::_UpdateKey()
{
	if( m_pKey != nullptr)
	{
		INT32	nItemCount = m_pKey->GetDataCount();
		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegData * pData = m_pKey->GetData( i);
			I3TRACE("_UpdateKey() %d\n", i);
			_AddData( pData);
		}
	}
}

INT32 i3TDKRegKeyCtrl::_AddData( i3RegData * pData)
{
	I3ASSERT( pData != nullptr);

	INT32	nDataType = pData->getDataType();
	
	INT32 iCount = GetItemCount();
#ifdef _UNICODE
	i3::stack_wstring wstrDataName;		i3::mb_to_utf16(pData->GetNameString(), wstrDataName);
	INT32	nIndex = InsertItem(iCount , wstrDataName.c_str(), 0);
#else
	INT32	nIndex = InsertItem(iCount , pData->GetName(), 0);
#endif
	SetItemData( nIndex, (DWORD_PTR)pData);
	SetItemText( nIndex, 1, szTypeString[nDataType]);
	
#ifdef _UNICODE
	i3::stack_wstring conv( L"Not Initialized");
#else
	i3::stack_string conv( "Not Initialized" ) ;
#endif

	_Convert2String( conv, pData);

	const char* szName = pData->GetName();

	I3TRACE("i3TDKRegKeyCtrl::_AddData %s\n", szName);

	if ( strcmp( szName, "__Update Time" ) == 0 || strcmp( szName, "__Make Time" ) == 0 )
	{
		time_t osBinaryTime = _ttoi64( conv.c_str() );
		CTime ct(osBinaryTime);
#ifdef _UNICODE
		i3::stack_wstring strTimeStampString;
#else
		i3::stack_string strTimeStampString;
#endif
		int y,m,d,h,min,s;
	
		y=ct.GetYear();
		m=ct.GetMonth();
		d=ct.GetDay();
		h=ct.GetHour();
		min=ct.GetMinute();
		s=ct.GetSecond();

		i3::sprintf( strTimeStampString, _T("%04d-%02d-%02d %02d:%02d:%02d"), y, m, d, h, min, s );

		SetItemText( nIndex, 2, strTimeStampString.c_str() );
	}
	else
	{
		SetItemText( nIndex, 2, conv.c_str() );
	}

	return nIndex;
}

INT32 i3TDKRegKeyCtrl::_AddData( i3RegData * pData, const TCHAR* pszDefault )
{
	I3ASSERT( pData != nullptr);

	INT32	nDataType = pData->getDataType();
#ifdef _UNICODE
	i3::stack_wstring wstrDataName;		i3::mb_to_utf16(pData->GetName(), wstrDataName);
	INT32	nIndex = InsertItem( GetItemCount(), wstrDataName.c_str(), 0);
#else
	INT32	nIndex = InsertItem( GetItemCount(), pData->GetName(), 0);
#endif
	SetItemData( nIndex, (DWORD_PTR)pData);
	SetItemText( nIndex, 1, szTypeString[nDataType]);
		
	SetItemText( nIndex, 2, pszDefault);

	return nIndex;
}

void i3TDKRegKeyCtrl::_Convert2String( i3::stack_string& conv, i3RegData * pData)
{
	INT32	nDataType = pData->getDataType();

	switch( nDataType)
	{
		case	I3REG_TYPE_INT32:
			{
				i3::sprintf( conv, "%d", *IRTO_INT32( pData));
			}
			break;
		case	I3REG_TYPE_REAL32:
			{
				i3::sprintf( conv, "%f", *IRTO_REAL32( pData));
			}
			break;
		case	I3REG_TYPE_STRING:	
			{
				const i3::rc_wstring& wstr = IRTO_STRING( pData);
				i3::stack_string str;		i3::utf16_to_mb(wstr, str);
				conv = str;
			}
			break;
		case	I3REG_TYPE_VEC2D:
			{
				i3::sprintf( conv, "%f, %f", getU(IRTO_VEC2D( pData)), getV(IRTO_VEC2D( pData)));
			}
			break;
		case	I3REG_TYPE_VEC3D:
			{
				i3::sprintf( conv, "%f, %f, %f", getX(IRTO_VEC3D( pData)), 
											getY(IRTO_VEC3D( pData)),
											getZ(IRTO_VEC3D( pData)));
			}
			break;
		case	I3REG_TYPE_VEC4D:
			{
				i3::sprintf( conv, "%f, %f, %f, %f", getX(IRTO_VEC4D( pData)), 
											getY(IRTO_VEC4D( pData)),
											getZ(IRTO_VEC4D( pData)),
											getW(IRTO_VEC4D( pData)));
			}
			break;
		case	I3REG_TYPE_COLOR:
			{
				i3::sprintf( conv, "%d, %d, %d, %d", i3Color::GetR( IRTO_COLOR( pData)), 
					i3Color::GetG( IRTO_COLOR( pData)), 
					i3Color::GetB( IRTO_COLOR( pData)), 
					i3Color::GetA( IRTO_COLOR( pData))); 
			}
			break;
		case	I3REG_TYPE_MATRIX:
			{
				i3::sprintf( conv, "Click.....");
			}
			break;
		default:
			break;
	}
}

void i3TDKRegKeyCtrl::_Convert2String( i3::stack_wstring& conv, i3RegData * pData)
{
	INT32	nDataType = pData->getDataType();

	switch( nDataType)
	{
	case	I3REG_TYPE_INT32:
		{
			i3::sprintf( conv, L"%d", *IRTO_INT32( pData));
		}
		break;
	case	I3REG_TYPE_REAL32:
		{
			i3::sprintf( conv, L"%f", *IRTO_REAL32( pData));
		}
		break;
	case	I3REG_TYPE_STRING:	
		{
			const i3::rc_wstring& wstr = IRTO_STRING( pData);
			conv = wstr;
		}
		break;
	case	I3REG_TYPE_VEC2D:
		{
			i3::sprintf( conv, L"%f, %f", getU(IRTO_VEC2D( pData)), getV(IRTO_VEC2D( pData)));
		}
		break;
	case	I3REG_TYPE_VEC3D:
		{
			i3::sprintf( conv, L"%f, %f, %f", getX(IRTO_VEC3D( pData)), 
				getY(IRTO_VEC3D( pData)),
				getZ(IRTO_VEC3D( pData)));
		}
		break;
	case	I3REG_TYPE_VEC4D:
		{
			i3::sprintf( conv, L"%f, %f, %f, %f", getX(IRTO_VEC4D( pData)), 
				getY(IRTO_VEC4D( pData)),
				getZ(IRTO_VEC4D( pData)),
				getW(IRTO_VEC4D( pData)));
		}
		break;
	case	I3REG_TYPE_COLOR:
		{
			i3::sprintf( conv, L"%d, %d, %d, %d", i3Color::GetR( IRTO_COLOR( pData)), 
				i3Color::GetG( IRTO_COLOR( pData)), 
				i3Color::GetB( IRTO_COLOR( pData)), 
				i3Color::GetA( IRTO_COLOR( pData))); 
		}
		break;
	case	I3REG_TYPE_MATRIX:
		{
			i3::sprintf( conv, L"Click.....");
		}
		break;
	default:
		break;
	}
}


void i3TDKRegKeyCtrl::AddNewData( INT32 nType)
{
	i3RegData * pNewData = nullptr;

	switch( nType)
	{
	case	I3REG_TYPE_INT32:	pNewData = (i3RegData*)IR_INT32::new_object();	break;
	case	I3REG_TYPE_REAL32:	pNewData = (i3RegData*)IR_REAL32::new_object();	break;
	case	I3REG_TYPE_STRING:	pNewData = (i3RegData*)IR_STRING::new_object();	break;
	case	I3REG_TYPE_VEC2D:	pNewData = (i3RegData*)IR_VEC2D::new_object();	break;
	case	I3REG_TYPE_VEC3D:	pNewData = (i3RegData*)IR_VEC3D::new_object();	break;
	case	I3REG_TYPE_VEC4D:	pNewData = (i3RegData*)IR_VEC4D::new_object();	break;
	case	I3REG_TYPE_COLOR:	pNewData = (i3RegData*)IR_COLOR::new_object();	break;
	case	I3REG_TYPE_MATRIX:	pNewData = (i3RegData*)IR_MATRIX::new_object();	break;
	default:
		break;
	}

	if( pNewData != nullptr)
	{
		pNewData->SetName("New Data");
		m_pKey->AddData(pNewData);
		INT32 nItem = _AddData( pNewData);

		EnsureVisible( nItem, FALSE);
		SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void i3TDKRegKeyCtrl::AddTimeStampData( const TCHAR* pszName, const TCHAR* pszTime, const TCHAR* pszTimeString )
{
	i3RegString * pNewData = (i3RegString*)IR_STRING::new_object();

	if( pNewData != nullptr)
	{
#ifdef _UNICODE
		i3::rc_string strDataName;	i3::utf16_to_mb(pszName, strDataName);
		pNewData->SetName(strDataName);
#else
		pNewData->SetName( pszName );
#endif

#ifdef _UNICODE
		i3::rc_wstring utf16str = pszTime;
#else
		i3::wstring utf16str;	i3::mb_to_utf16(pszTime, utf16str);
#endif

		pNewData->setValue( utf16str );
		m_pKey->AddData(pNewData);
		INT32 nItem = _AddData( pNewData, pszTimeString );

		EnsureVisible( nItem, FALSE);
		//SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
	}
}

//+
void i3TDKRegKeyCtrl::UpdateTimeStampData( const TCHAR* pszName, const TCHAR* pszTime, const TCHAR* pszTimeString )
{
	LVFINDINFO info;
	int nIndex;

	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = pszName;

	nIndex = this->FindItem(&info);

	if ( nIndex != -1 )
	{
		i3RegString* pData = (i3RegString*)GetItemData( nIndex );
#ifdef _UNICODE
		i3::rc_wstring wstrTime = pszTime;
#else
		i3::rc_wstring wstrTime;	i3::mb_to_utf16(pszTime, wstrTime);		
#endif		
		pData->setValue( wstrTime );		

		// 보이는 것은 년월일시분초로 변경
		SetItemText( nIndex, 2, pszTimeString );
	}
}

void i3TDKRegKeyCtrl::PasteData( i3RegData * pData)
{
	if( pData == nullptr)	return;

	i3RegData * pNewData =	pData->create_instance();	//	(i3RegData*)CREATEINSTANCE( pData->GetMeta());

	if( pNewData != nullptr)
	{
		pNewData->SetName( pData->GetName());
		pData->CopyTo( pNewData, I3_COPY_INSTANCE);

		m_pKey->AddData(pNewData);
		INT32 nItem = _AddData( pNewData);

		EnsureVisible( nItem, FALSE);
		SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);

		I3_MUST_ADDREF( pNewData);
	}
}

void i3TDKRegKeyCtrl::DeleteData( INT32 nIdx)
{
	i3RegData * pData = (i3RegData*)GetItemData( nIdx);
	if( pData != nullptr)
	{
		m_pKey->RemoveData( pData);
		DeleteItem( nIdx);
	}
}

int i3TDKRegKeyCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add extra initialization here
	CRect rt;
	GetClientRect( &rt);

	INT32 nColWidth = rt.Width()/3;
	InsertColumn(0, _T("Data Name"), LVCFMT_LEFT, nColWidth);
	InsertColumn(1, _T("Data Type"), LVCFMT_LEFT, nColWidth);
	InsertColumn(2, _T("Data Value"), LVCFMT_LEFT, nColWidth);

	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT));
		logFont.lfHeight			= 13;
		logFont.lfWeight			= FW_DONTCARE;
		logFont.lfItalic			= FALSE;
		logFont.lfCharSet			= DEFAULT_CHARSET;
		logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
		logFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
		logFont.lfQuality			= DEFAULT_QUALITY;
		logFont.lfPitchAndFamily	= DEFAULT_PITCH;

		i3::generic_string_copy( logFont.lfFaceName, _T("굴림") );

		m_Font.CreateFontIndirect( &logFont);
		SetFont( &m_Font);
	}

	{
		INT32 nExStyle = GetExStyle();
		nExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;

		SetExtendedStyle( nExStyle);
	}
	return 0;
}

BOOL i3TDKRegKeyCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	 cs.style |= WS_CHILD | LVS_SHOWSELALWAYS | LVS_REPORT | LVS_EDITLABELS | LVS_SINGLESEL;  
	 cs.style &= ~LVS_SORTASCENDING;

	return CListCtrl::PreCreateWindow(cs);
}

void i3TDKRegKeyCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	INT32	nColWidth = (INT32)( cx * 0.2f);
	SetColumnWidth( 0, nColWidth*2);
	SetColumnWidth( 1, nColWidth);
	SetColumnWidth( 2, cx - nColWidth * 3);
}

void i3TDKRegKeyCtrl::UpdateItem( INT32 nRow)
{
	i3RegData * pData = (i3RegData*)GetItemData( nRow);

#ifdef _UNICODE
	i3::stack_wstring conv;
#else
	i3::stack_string conv;
#endif
	INT32	nDataType = pData->getDataType();
	
	SetItemText( nRow, 1, szTypeString[nDataType]);

	_Convert2String( conv, pData);

	SetItemText( nRow, 2, conv.c_str() );
}

void i3TDKRegKeyCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	// TODO: Add your control notification handler code here
	if( pDispInfo->item.pszText != nullptr)
	{		
		i3RegData * pData = (i3RegData*)pDispInfo->item.lParam;

#ifdef _UNICODE
		i3::rc_string strText;		i3::utf16_to_mb(pDispInfo->item.pszText, strText);
		pData->SetName(strText);
#else
		pData->SetName( pDispInfo->item.pszText);
#endif
		
		SetItemText( pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);
	}
	
	*pResult = 0;
}

void i3TDKRegKeyCtrl::OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	i3RegData * pData = (i3RegData*)pDispInfo->item.lParam;
	const char* szName = pData->GetName();

	if ( strcmp( szName, "__Update Time" ) == 0 || strcmp( szName, "__Make Time" ) == 0 )
		*pResult = 1;
	else
		*pResult = 0;

}

static bool	_RegDataValueCompProc( i3RegData * p1, i3RegData * p2)
{
	i3::wstring strTemp1, strTemp2;
	p1->GetValueByString(strTemp1);	p2->GetValueByString(strTemp2);

	return i3::generic_is_iless(strTemp1, strTemp2);
}

static bool	_RegDataNameCompProc( i3RegData * p1, i3RegData * p2)
{
	if(_strnicmp(p1->GetName(), p2->GetName(), 256) < 0)
	{
		return true;
	}
	else 
	{
		return false;
	}
	
}

void i3TDKRegKeyCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(pNMLV->iSubItem == 0) {
		std::sort(m_pKey->GetValueList()->begin(), m_pKey->GetValueList()->end(), _RegDataNameCompProc);
	}
	else if(pNMLV->iSubItem == 2) {
		std::sort(m_pKey->GetValueList()->begin(), m_pKey->GetValueList()->end(), _RegDataValueCompProc);
	}

	i3::vector<i3RegData*>::iterator itor = m_pKey->FindItorByName( "__Update Time" );
	if(itor != m_pKey->GetValueList()->end())
	{
		i3RegData* pData = (i3RegData*)*itor;
		m_pKey->GetValueList()->erase(itor);
	//	m_pKey->GetValueList()->push_front(pData);
		m_pKey->GetValueList()->insert(m_pKey->GetValueList()->begin(), pData);

	}
	itor = m_pKey->FindItorByName( "__Make Time" );
	if(itor != m_pKey->GetValueList()->end())
	{
		i3RegData* pData = (i3RegData*)*itor;
		m_pKey->GetValueList()->erase(itor);
	//	m_pKey->GetValueList()->push_front(pData);
		m_pKey->GetValueList()->insert(m_pKey->GetValueList()->begin(), pData);
	}

	/*i3RegString* pRegString = (i3RegString*)m_pKey->FindDataByName( "__Make Time" );
	m_pKey->getDataList()->Remove(pRegString);
	m_pKey->getDataList()->Insert(0, pRegString);
	pRegString = (i3RegString*)m_pKey->FindDataByName( "__Update Time" );
	m_pKey->getDataList()->Remove(pRegString);
	m_pKey->getDataList()->Insert(1, pRegString);*/

	/*if(pNMLV->iSubItem == 0) {
		m_pKey->getDataList()->Sort(_RegDataNameCompProc);
	}
	else if(pNMLV->iSubItem == 2) {
		m_pKey->getDataList()->Sort(_RegDataValueCompProc);
	}

	i3RegString* pRegString = (i3RegString*)m_pKey->FindDataByName( "__Make Time" );
	m_pKey->getDataList()->Remove(pRegString);
	m_pKey->getDataList()->Insert(0, pRegString);
	pRegString = (i3RegString*)m_pKey->FindDataByName( "__Update Time" );
	m_pKey->getDataList()->Remove(pRegString);
	m_pKey->getDataList()->Insert(1, pRegString);*/

	DeleteAllItems();
	_UpdateKey();
	
	*pResult = 0;
}




