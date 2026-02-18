// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"

#include <sstream>
#include <set>

#include "i3TDKRegistryCtrl_Impl.h"
#include "i3TDKListCtrl_Impl.h"
#include "ListSubDialog.h"

#include "GlobalVariable.h"

#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/itl/algorithm/find_if.h"

UINT8 _toParseUINT8(const char* Src);
INT32 _toParseUINT32(const char* Src);
REAL32 _toParseREAL32(const char* Src);
VEC2D _toParseVEC2D(const char* Src);
VEC3D _toParseVEC3D(const char* Src);
VEC4D _toParseVEC4D(const char* Src);
I3COLOR _toParseCOLOR(const char* Src);
MATRIX _toParseMATRIX(const char* Src);

void _toString(UINT8 Src, char* Dest, size_t sizeOfDest);
void _toString(INT32 Src, char* Dest, size_t sizeOfDest);
void _toString(REAL32 Src, char* Dest, size_t sizeOfDest);
void _toString(const VEC2D & Src, char* Dest, size_t sizeOfDest);
void _toString(const VEC3D & Src, char* Dest, size_t sizeOfDest);
void _toString(const VEC4D & Src, char* Dest, size_t sizeOfDest);
void _toString(const I3COLOR & Src, char* Dest, size_t sizeOfDest);
void _toString(const MATRIX & Src, char* Dest, size_t sizeOfDest);


static char * szTypeString[] = 
{
	"정수형",
	"실수형",
	"문자열",
	"벡터 2D",
	"벡터 3D",
	"벡터 4D",
	"색상",
	"매트릭스",
};

static UINT ID_MENU_ALLSHOW_NATION = 6000;		//모든 나라를 보인다.
static UINT ID_MENU_ALLHIDE_NATION = 6001;		//모든 나라 안 보이게
static UINT ID_MENU_HIDE_NATION = 6002;		//현재 나라 안 보이게
static UINT ID_MENU_COL_WIDTH = 6003;		//셀 너비 수정
static UINT ID_MENU_SELECT_AUTOSIZE = 6004;		//오토 사이즈
static UINT ID_MENU_SELECT_NATION = 6005;		//나라 선택 창

static UINT ID_MENU_RESET_NATION = 6903;	//현재 나라에 데이터를 초기화

static UINT ID_MENU_DISPLAY = 6100;	//특정 나라 보이고 안 보이고
static UINT ID_MENU_CLONE_NATION = 6200;	//데이터 조작

int WM_OPEN_WIDTH_DIALOG = ::RegisterWindowMessage("WM_OPEN_WIDTH_DIALOG");


BEGIN_MESSAGE_MAP(i3TDKListCtrl_Impl, CListSubCtrl)

ON_REGISTERED_MESSAGE(WM_OPEN_WIDTH_DIALOG, &i3TDKListCtrl_Impl::OpenWidthDialog)

END_MESSAGE_MAP()


LRESULT i3TDKListCtrl_Impl::OpenWidthDialog(WPARAM wParam, LPARAM lParam)
{
	if(ID_MENU_COL_WIDTH ==wParam)
	{
		CListSubDialog Sub(this);

		if( Sub.DoModal() == IDOK )
		{
			CString ret = Sub.GetTextEdit();
			INT32 result = _ttoi(ret);
			CListCtrl::SetColumnWidth( lParam, result );
		}
	}
	else if(ID_MENU_SELECT_NATION ==wParam)
	{

	}

	return 0;
}


i3TDKListCtrl_Impl::i3TDKListCtrl_Impl()
{
	m_pKey	= NULL;
	m_PefFileType = PFT_SINGLE;
	m_bUseChildCellClick = false;
	m_bAutoSize = false;
//	m_bUseDefalutNationFile = true;
	m_hAccel = LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_ACCEL_EDITING));

	m_vecColumnList.clear();
}

i3TDKListCtrl_Impl::~i3TDKListCtrl_Impl()
{
	m_Font.DeleteObject();
	I3_SAFE_RELEASE( m_pKey);

	for (INT32 i = 0; i < m_vecColumnList.size(); i++)
	{
		I3_SAFE_DELETE(m_vecColumnList[i]);
	}
	m_vecColumnList.clear();
}

void i3TDKListCtrl_Impl::SetRegKey( i3RegKey * pKey, ActionType action )
{
	ResetDataPtr();
	Clear_AnalysisItem(DAT_COMPARE);
	Clear_AnalysisItem(DAT_MATCH);
	Clear_AnalysisItem(DAT_MODIFY);
	Clear_AnalysisItem(DAT_PASTE);
	m_bFocusDraw = false;

	I3_REF_CHANGE( m_pKey, pKey);

	UpdateRegFileType(action);

	if( m_pKey != NULL)
	{
		_UpdateKey();
	}
	
	Modify_UnSortedColumnText();
}

void i3TDKListCtrl_Impl::UpdateRegType(i3RegKey* pRegKey, ActionType action)
{
	if( pRegKey->GetDataCount() == 0 )
	{
		SetSingleNation();
// 		if (m_PefFileType != PFT_MULTI)
// 			SetSingleNation();
		return;
	}

	i3RegData* pRegData = pRegKey->GetData( 0 );
	if( pRegData->getDataType() != I3REG_TYPE_ARRAY )
	{
		//과거 Pef 헤더 리스트
		SetSingleNation();
	}
	else
	{
		i3RegArray* pArrayReg = (i3RegArray*)pRegData;
		size_t RegSize = pArrayReg->getSize();
		if( RegSize == 1 )
		{
			//통합 Pef 단일 헤더 리스트
			SetSingleNation();
		}
		else if( GetNationListRef()->size() == RegSize )
		{
			//통합 Pef 다중 헤더 리스트
			SetMultiNation(action);
		}
		else
		{
			// config/nation.nif
			LookForNationFile();

			// chunjong.song 2013.12.30
			// 프로퍼티에디터등에서 최초실행후 국가별데이터항목이있는것을 선택했을때 
			// config/nation.nlf파일 읽어들여 국가컬럼목록만읽어들이고 컬럼갱신을
			// 하지않는바람에 이상태에서 저장시 오류발생했던거 수정
			SetMultiNation(action);
		}
	}
}

void i3TDKListCtrl_Impl::UpdateRegFileType(ActionType action)
{
	//파일 중에 하나로된 파일이 있기에 여기서 해준다.
	//if( m_pKey  && (m_pKey ->getNodeStyle() == I3REG_STYLE_ROOT) )
	{
		//맨 처음 헤더 리스트
//		if( m_pKey->getChildCount() == 0 )
		{
			UpdateRegType( m_pKey, action);
			return;
		}

//		i3RegKey* RegKey = (i3RegKey*)m_pKey ->getChild( 0 );
//		if( RegKey != NULL)
//		{
//			UpdateRegType( RegKey, action);
//		}
	}
}

CString i3TDKListCtrl_Impl::GetNewDataName()
{
	UINT NewDataCount = 0;

	CString Ret;
	Ret.Format("New Data %d", NewDataCount);

	i3RegData* pData = i3Registry::FindData( m_pKey, Ret.GetString() );

	while( pData )
	{
		++NewDataCount;
		Ret.Format("New Data %d", NewDataCount);
		pData = i3Registry::FindData( m_pKey, Ret.GetString() );
	}

	return Ret;
}

i3RegData * i3TDKListCtrl_Impl::_AddNewData( i3RegKey* pRegKey, const CString & Name, INT32 nValue )
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();

	pNewArrayData->setElementType( I3REG_TYPE_INT32 );
	pNewArrayData->SetName( (const char*)Name );

	i3RegData* pData = NULL;

	int size = m_NationList.size();
	for(int i =0; i<size; i++)
	{
		pData = pNewArrayData->createRegData();
		((i3RegINT32*)pData)->setValue( nValue );

		pNewArrayData->addRegData( pData );
	}

	pRegKey->AddData(pNewArrayData);

	return pNewArrayData;
}

i3RegData * i3TDKListCtrl_Impl::AddNewData( INT32 nType, INT32 nValue )
{
	CString NewDataName = GetNewDataName();

	if( GetPefFileType() == PFT_MULTI ) 
	{
		i3RegArray * pNewArrayData = i3RegArray::new_object();

		pNewArrayData->setElementType( nType );
		pNewArrayData->SetName( (const char*)NewDataName );

		int size = m_NationList.size();
		for(int i =0; i<size; i++)
			pNewArrayData->addRegData( pNewArrayData->createRegData() );

		m_pKey->AddData(pNewArrayData);

		INT32 nItem = _AddData( pNewArrayData);
		EnsureVisible(nItem, FALSE);
		SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);

		return pNewArrayData;
	}
	else
	{
		i3RegData * pNewData = NULL;

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

		if( pNewData != NULL)
		{
			if( nValue != -1 && nType == I3REG_TYPE_INT32 )
			{
				((i3RegINT32*)pNewData)->setValue( nValue );
			}

			pNewData->SetName( (const char*)NewDataName );
			m_pKey->AddData(pNewData);
			INT32 nItem = _AddData( pNewData);

			EnsureVisible( nItem, FALSE);
			SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);

			return pNewData;
		}
	}

	return NULL;
}

void i3TDKListCtrl_Impl::UpdateTimeStampData( char* pszName, char* pszTime, char* pszTimeString )
{
	LVFINDINFO info;
	int nIndex;

	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = pszName;

	nIndex = this->FindItem(&info);

	if ( nIndex != -1 )
	{
		i3RegData* pData = (i3RegData*)GetDataPtr( nIndex );

		i3RegArray* pRegArray = NULL;

		INT32 nDataType = pData->getDataType();
		size_t size = 1;
		if( nDataType == I3REG_TYPE_ARRAY )
		{
			pRegArray = (i3RegArray*)pData;
			size = pRegArray->getSize();
		}
		
		i3::wstring wstrTime;
		i3::mb_to_utf16(pszTime, wstrTime);

		size_t colContextCount = (size_t)this->Get_ColumnContextCount();

		for(size_t i =0; i<size; i++)
		{
			if( pRegArray )
				pRegArray->replaceRegData( i, wstrTime );
			else
				((i3RegString*)pData)->setValue( wstrTime );

			// 보이는 것은 년월일시분초로 변경
			if ( i + 2 < colContextCount)
				Set_Cell( nIndex, i + 2, pszTimeString );
		}
	}
}

void i3TDKListCtrl_Impl::AddTimeStampData( i3RegKey* pRegKey, char* pszName, char* pszTime, char* pszTimeString )
{
	if( GetPefFileType() == PFT_MULTI ) 
	{
		i3RegArray * pNewArrayData = i3RegArray::new_object();

		pNewArrayData->setElementType( I3REG_TYPE_STRING );
		pNewArrayData->SetName( pszName );
		
		i3::wstring	wstrTime;
		i3::mb_to_utf16(pszTime, wstrTime);

		size_t size = m_NationList.size();
		for(size_t i =0; i<size; i++)
		{
			i3RegString * pNewData = (i3RegString *)pNewArrayData->createRegData();

			if( pNewData != NULL)
			{
				pNewData->setValue( wstrTime );
				pNewArrayData->addRegData( pNewData );
			}
		}

//		_AddData( pNewArrayData, pszTimeString );
		pRegKey->AddData(pNewArrayData);
	}
	else
	{
		i3RegString * pNewData = (i3RegString*)IR_STRING::new_object();
		
		i3::wstring	wstrTime;
		i3::mb_to_utf16(pszTime, wstrTime);

		if( pNewData != NULL)
		{
			pNewData->SetName( pszName );
			pNewData->setValue( wstrTime );
			pRegKey->AddData(pNewData);
//			 _AddData( pNewData, pszTimeString );
		}
	}
}

void i3TDKListCtrl_Impl::UpdateItem( INT32 nRow)
{
	i3RegData * pData = (i3RegData*)GetDataPtr( nRow);

	char conv[4096];
	INT32	nDataType = pData->getDataType();
	
	SetItemText( nRow, 1, szTypeString[nDataType]);

	_Convert2String( conv, 4096, pData);

	SetItemText( nRow, 2, conv);
}

void i3TDKListCtrl_Impl::_UpdateKey()
{
	DeleteAllItems();
	if( m_pKey != NULL)
	{
		INT32	nItemCount = m_pKey->GetDataCount();
		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegData * pData = m_pKey->GetData( i);
			//I3TRACE("_UpdateKey() %d\n", i);
			_AddData( pData);
		}
	}
}

INT32 i3TDKListCtrl_Impl::_AddData( i3RegData * pData)
{
	I3ASSERT( pData != NULL);

	i3RegArray* pRegArray = NULL;

	INT32 nIndex = GetItemCount();
	INT32 nDataType = pData->getDataType();
	size_t size = 2; // size가 0이면 weapon/itemlist들 (I3REG_TYPE_ARRAY이 아닌거)들이 안보임
	INT32 iDefSel = 2;
	if( nDataType == I3REG_TYPE_ARRAY )
	{
		pRegArray = (i3RegArray*)pData;
		nDataType = pRegArray->getElementType();
		size = pRegArray->getSize(); //+ iDefSel;
	}

	const char* szName = pData->GetName();
	//I3TRACE("i3TDKRegKeyCtrl::_AddData %s\n", szName);

	nIndex = InsertItem( nIndex , (LPTSTR)szName, 0);
	Set_Cell( nIndex, 1, szTypeString[nDataType]);

	SetItemData(nIndex, (DWORD_PTR)pData);
	AddDataPtr((void*)pData);

	size_t ColumnIndex = 0;
	for(size_t i = 0; i<size; i++)
	{
		char conv[4096] = "";
		strcpy( conv, "Not Initialized");
		if( (pRegArray != NULL) && ( i < pRegArray->getSize()))
		{
			_Convert2String( conv, 4092, (*pRegArray)[i] );
		}
		else
			_Convert2String( conv, 4092, pData );

		if ( strcmp( szName, "__Update Time" ) == 0 || strcmp( szName, "__Make Time" ) == 0 )
		{
			time_t osBinaryTime = _atoi64( conv );
			CTime ct(osBinaryTime);
			char szTimeStampString[64];
			int y,m,d,h,min,s;

			y=ct.GetYear();
			m=ct.GetMonth();
			d=ct.GetDay();
			h=ct.GetHour();
			min=ct.GetMinute();
			s=ct.GetSecond();

			sprintf(szTimeStampString, "0");
			//sprintf( szTimeStampString, "%04d-%02d-%02d %02d:%02d:%02d", y, m, d, h, min, s );

			if( Is_HeaderShow( i+iDefSel) )
			{
				Set_Cell( nIndex, ColumnIndex + iDefSel, szTimeStampString );
				++ColumnIndex;
			}
		}
		else
		{
			if( Is_HeaderShow( i+iDefSel) )
			{
				Set_Cell( nIndex, ColumnIndex + iDefSel, conv );
				++ColumnIndex;
			}				
		}

		if( m_bAutoSize == true)
			CListCtrl::SetColumnWidth( i, LVSCW_AUTOSIZE_USEHEADER );
	}

	return nIndex;
}
 
INT32 i3TDKListCtrl_Impl::_AddData( i3RegData * pData, char* pszDefault )
{
	I3ASSERT( pData != NULL);

	i3RegArray* pRegArray = NULL;

	INT32 nIndex = GetItemCount();
	INT32 nDataType = pData->getDataType();
	size_t size = 1;
	if( nDataType == I3REG_TYPE_ARRAY )
	{
		pRegArray = (i3RegArray*)pData;
		nDataType = pRegArray->getElementType();
		size = pRegArray->getSize();
	}

	nIndex = InsertItem( nIndex , (LPTSTR)pData->GetName(), 0);
	Set_Cell( nIndex, 1, szTypeString[nDataType]);

	SetItemData(nIndex, (DWORD_PTR)pData);
	AddDataPtr( (void*)pData);

	size_t ColumnIndex = 0;
	for(size_t i = 0; i<size; i++)
	{
		if( Is_HeaderShow( i+2) )
		{
			Set_Cell( nIndex, ColumnIndex + 2, pszDefault);
			++ColumnIndex;
		}
	}
	
	return nIndex;
}

void i3TDKListCtrl_Impl::_Convert2String( char * conv, size_t sizeOfString, i3RegData * pData)
{
	switch( pData->getDataType() )
	{
		case	I3REG_TYPE_INT32:
			{
				INT32 Value;
				GET_REG_SINGLE_DATA(pData, &Value);
				_toString(Value, conv, sizeOfString );		
			}
			break;
		case	I3REG_TYPE_REAL32:
			{
				REAL32 Value;
				GET_REG_SINGLE_DATA(pData, &Value);
				_toString(Value, conv, sizeOfString );		
			}
			break;
		case	I3REG_TYPE_STRING:	
			{
				i3::rc_wstring wstr;
				GET_REG_SINGLE_DATA( pData, wstr );
				
				i3::string strValue;		i3::utf16_to_mb(wstr, strValue);
				sprintf_s( conv, sizeOfString, "%s", strValue.c_str() );
			}
			break;
		case	I3REG_TYPE_VEC2D:
			{
				VEC2D Value;
				GET_REG_SINGLE_DATA(pData, &Value);
				_toString(Value, conv, sizeOfString );		
			}
			break;
		case	I3REG_TYPE_VEC3D:
			{
				VEC3D Value;
				GET_REG_SINGLE_DATA(pData, &Value);
				_toString(Value, conv, sizeOfString );		
			}
			break;
		case	I3REG_TYPE_VEC4D:
			{
				VEC4D Value;
				GET_REG_SINGLE_DATA(pData, &Value);
				_toString(Value, conv, sizeOfString );		
			}
			break;
		case	I3REG_TYPE_COLOR:
			{
				I3COLOR Value;
				GET_REG_SINGLE_DATA(pData, &Value);
				_toString(Value, conv, sizeOfString );		
			}
			break;
		case	I3REG_TYPE_MATRIX:
			{
				MATRIX Value;
				GET_REG_SINGLE_DATA(pData, &Value);
				_toString(Value, conv, sizeOfString );
			}
			break;
		default:
			break;
	}
}

void i3TDKListCtrl_Impl::PasteData( i3RegData * pData)
{
	if( pData == NULL)	return;

	i3RegData * pNewData = pData->create_instance(); // (i3RegData*)CREATEINSTANCE( pData->GetMeta());

	if( pNewData != NULL)
	{
		pNewData->SetName( pData->GetName());
		pData->CopyTo( pNewData, I3_COPY_INSTANCE);

		m_pKey->AddData(pNewData);
		INT32 nItem = _AddData( pNewData);

		EnsureVisible( nItem, FALSE);
	//	SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);

		pNewData->AddRef();
	}	
}

void i3TDKListCtrl_Impl::DeleteData( INT32 nIdx)
{
	i3RegData * pData = (i3RegData*)GetDataPtr( nIdx);
	if( pData != NULL)
	{
		m_pKey->RemoveData( pData);
		DeleteItem( nIdx);
		DelDataPtr( nIdx );
	}
}

//헤더 초기값 변경시에 사용된다.
void i3TDKListCtrl_Impl::NotifyHeaderColumn(ActionType action)
{
	if( m_NationList.empty() )
		MediaPathNationFile();

	switch(action)
	{
	case CListSubCtrl::ACT_CHANGEKEY :
		{
			CListSubCtrl::NotifyHeaderColumn(CListSubCtrl::ACT_CHANGEKEY);

			NationBank::iterator It = m_DisplayNationList.begin();
			NationBank::iterator End = m_DisplayNationList.end();

			for(int i = 0; It != End; ++It, i++ )
				Set_Column( (LPTSTR)It->c_str(), DEFAULT_COLUMNSIZE, CST_NONE);

			UpdateColumnList();

			CListSubCtrl::ClearLastColumnSize();
		}
		break;
	case CListSubCtrl::ACT_COPYCELL:
		{
			CListSubCtrl::NotifyHeaderColumn(CListSubCtrl::ACT_COPYCELL);

			NationBank::iterator It = m_DisplayNationList.begin();
			NationBank::iterator End = m_DisplayNationList.end();

			for(int i = 2; It != End; ++It, i++ )
			{
				int size = CListSubCtrl::GetLastColumnSize(i);
				Set_Column( (LPTSTR)It->c_str(), DEFAULT_COLUMNSIZE, CST_NONE);
			}

			UpdateColumnList();

			CListSubCtrl::ClearLastColumnSize();
		}
		break;
	}

}

void i3TDKListCtrl_Impl::NotifyRegFileNation()
{
	NotifyHeaderColumn(ACT_CHANGEKEY);
}

void i3TDKListCtrl_Impl::LoadNationFile(const CString & Path)
{
	CStdioFile FileStream;
	if( !FileStream.Open(Path, CFile::modeRead) )
	{
		I3PRINTLOG(I3LOG_FATAL, "Stream IO Error! %s", Path.GetString() );
	}

	m_NationList.clear();

	CString Buffer;
	while( FileStream.ReadString(Buffer) )
	{
		std::string NationString(Buffer.GetString());

		size_t Index;
		//주석 제거
		if( !NationString.empty() )
		{
			Index = NationString.rfind(";");
			if( Index != std::string::npos )
				NationString.erase( Index );
		}
		else
			continue;
		//앞쪽 Trim
		if( !NationString.empty() )
		{
			Index = NationString.find_first_not_of(" \t");
			if( Index != std::string::npos )
				NationString.erase(0, Index );
		}
		else
			continue;

		//뒷쪽 Trim
		if( !NationString.empty() )
		{
			Index = NationString.find_last_not_of(" \t") +1;
			if( Index != std::string::npos )
				NationString.erase( Index );
		}
		else
			continue;

		if( !NationString.empty() )
		{
			m_NationList.push_back( NationString );
		}
	}

	FileStream.Close();
}

BOOL i3TDKListCtrl_Impl::PreTranslateMessage(MSG* pMsg)
{
	if(WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		if(GetFocus() == this && m_hAccel && ::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
			return TRUE;
	}

	return CListSubCtrl::PreTranslateMessage(pMsg);
}

bool i3TDKListCtrl_Impl::MediaPathNationFile()
{
	CString str;

	if (g_pProfile)
	{
		str = g_pProfile->m_WorkDirectoryPath;
		str = str + "\\config\\Nation.nlf";
		

	}
	
	CFileFind cFileFinder;
	if (cFileFinder.FindFile( str ) )
	{
		LoadNationFile( str );
		return true;
	}

	return false;
}

bool i3TDKListCtrl_Impl::CurrnetPathNationFile()
{
	CString str;

	str = g_pProfile->m_CurrentFilePath;
	str.Delete( str.ReverseFind( '\\' ), str.GetLength() );
	str = str + "\\Nation.nlf";

	CFileFind cFileFinder;
	if (cFileFinder.FindFile( str ) )
	{
		LoadNationFile( str );
		return true;
	}

	return false;
}

void i3TDKListCtrl_Impl::LookForPathNationFile() 
{
	CString FileExt = "nlf Files (*.nlf) | *.nlf |";

	CFileDialog Dlg(TRUE, "Look For Nation List Files!", 
		"*.nlf", 
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_ENABLESIZING, 
		FileExt, this);

	char CuPath[MAX_PATH] = "";
	::GetCurrentDirectory(MAX_PATH, CuPath);

	Dlg.GetOFN().lpstrInitialDir = CuPath;

	if( Dlg.DoModal() == IDOK )
	{
		LoadNationFile( Dlg.GetPathName() );
	}
}

void i3TDKListCtrl_Impl::LookForNationFile()
{
	if( !CurrnetPathNationFile() )
	{
		if( !MediaPathNationFile() )
		{

		}
	}
}



UINT i3TDKListCtrl_Impl::GetPefFileType() const
{
	if( m_Name == "List2" )
		return PFT_MULTI;
	
	return m_PefFileType;	
}

void i3TDKListCtrl_Impl::SetSingleNation()
{
	m_DisplayNationList.clear();
	m_DisplayNationList.push_back( "Data Value" );

	NotifyHeaderColumn(ACT_CHANGEKEY);

	m_PefFileType = PFT_SINGLE;
}

void i3TDKListCtrl_Impl::SetMultiNation(ActionType action)
{
	m_DisplayNationList = m_NationList;

	NotifyHeaderColumn(action);

	m_PefFileType = PFT_MULTI;
}

bool i3TDKListCtrl_Impl::IsStampText(const CString& Str)
{
	if( Str.Find("__") == 0 && Str.Find( "Time" ) == Str.GetLength() - 4 )
		return true;

	return false;
}

UINT i3TDKListCtrl_Impl::GetAbleWeaponNumber()
{
	if( m_Name != "List1" )
		return (UINT)-1;

	if( GetPefFileType() == PFT_MULTI )
		return (UINT)-1;

	INT32 Value;

	i3RegData* pData = NULL;
	int Count = m_pKey->GetDataCount();
	std::set< int > NumberRoom;

	for(int i = 0; i < Count; ++i )
	{
		pData = m_pKey->GetData(i);

		if( IsStampText( pData->GetName() ) )
			continue;

		pData->GetValue( &Value );
		NumberRoom.insert( Value );
	}

	std::set< int >::iterator It = NumberRoom.begin();
	std::set< int >::iterator End = NumberRoom.end();

	int Ret = 1;

	for( ;It != End; ++It )
	{
		if( *It != Ret )
			break;

		++Ret;
	}

	return Ret;
}

//bool _Compare(const CString & ShortStr, const CString & LongStr, int ShortStringSize)
//{
//	for(int i=0;i<ShortStringSize;++i)
//	{
//		if( ShortStr[i] < LongStr[i] )
//			return true;
//
//		if( ShortStr[i] > LongStr[i] )
//			return false;
//	}
//
//	return true;
//}
//
//bool _Compare(const CString & str1, const CString & str2)
//{
//	int size1 = str1.GetLength();
//	int size2 = str2.GetLength();
//
//	if( size1 < size2 )
//		return _Compare(str1, str2, size1);
//
//	return _Compare(str2, str1, size2);
//}


bool i3TDKListCtrl_Impl::Sort_Compare(const CString& Str1, const CString& Str2, BOOL bAscending, int Row1, int Row2)
{
	bool Stamp1, Stamp2, StampHeader1, StampHeader2;

	CString String1(Str1), String2(Str2);
	CString StringHeader1 = Get_CellText(Row1, 0);
	CString StringHeader2 = Get_CellText(Row2, 0);

	Stamp1 = IsStampText(String1);
	Stamp2 = IsStampText(String2);
	StampHeader1 = IsStampText(StringHeader1);
	StampHeader2 = IsStampText(StringHeader2);

	if( !Stamp1 && !Stamp2 && !StampHeader1 && !StampHeader2 )
	{
//		return _Compare(String1, String2);
		return String1 < String2;
	}

	if( bAscending )
	{
		if( (Stamp1 && Stamp2) || (StampHeader1 && StampHeader2)  )
		{
			if( (String2 == "__Make Time") || (StringHeader2 == "__Make Time") )
				return true;

			return false;
		}

		if( Stamp2 || StampHeader2  )
			return true;

		return false;
	}
	else
	{
		if( (Stamp1 && Stamp2) || (StampHeader1 && StampHeader2)  )
		{
			if( String1 == "__Make Time" || (StringHeader1 == "__Make Time") )
				return true;

			return false;
		}

		if( Stamp1 || StampHeader1  )
			return true;

		return false;
	}

	return true;
}

void i3TDKListCtrl_Impl::Sort_Swap(int low, int high)
{
	CListSubCtrl::Sort_Swap(low, high);

	i3::vector<i3RegData*>* List = m_pKey->GetValueList();
	i3::swap( List->at( low ), List->at( high ) );
}

void i3TDKListCtrl_Impl::ColumnRClickMenu(int nRow, POINT pt)
{
	CString HeaderName = Get_ColumnText(nRow);

	CMenu MainMenu;	//최상위 메뉴
	CMenu DisplayMenu;	//보이고 안 보이고
	CMenu CopyMenu;	//복사, 초기화, 옮기기
	
	MainMenu.CreatePopupMenu();
	DisplayMenu.CreateMenu();
	CopyMenu.CreatePopupMenu();

	MainMenu.AppendMenu( MF_POPUP, (INT_PTR)DisplayMenu.m_hMenu, "Display");	//보이고 안 보이고
	if( nRow > 1 )
	{
		MainMenu.AppendMenu( MF_STRING, ID_MENU_HIDE_NATION, "Hide");	//그냥 안 보이고
	}
	MainMenu.AppendMenu( MF_STRING, ID_MENU_ALLSHOW_NATION, "All Show");	//모두 보이고
	MainMenu.AppendMenu( MF_STRING, ID_MENU_ALLHIDE_NATION, "All Hide");	//모두 안 보이게
	MainMenu.AppendMenu( MF_STRING, ID_MENU_COL_WIDTH, "Width");			//폭 조절

	if( getAutoSize() == true)
		MainMenu.AppendMenu( MF_STRING | MF_ENABLED | MF_CHECKED, ID_MENU_SELECT_AUTOSIZE, "Auto Size &A");
	else
		MainMenu.AppendMenu( MF_STRING | MF_ENABLED | MF_UNCHECKED, ID_MENU_SELECT_AUTOSIZE, "Auto Size &A");
	
	MainMenu.AppendMenu( MF_STRING, ID_MENU_SELECT_NATION, "SelectNation");	//나라 선택 
	
	
	if( nRow > 1 )
	{
		MainMenu.AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");
		MainMenu.AppendMenu( MF_POPUP, (INT_PTR)CopyMenu.m_hMenu, "Clone");	//데이터 복사
		MainMenu.AppendMenu( MF_STRING, ID_MENU_RESET_NATION, "Reset");	//데이터 초기화
	}

	CString MenuString;
	int DisplaySize = m_NationList.size();
	for(int i=0; i<DisplaySize; ++i)
	{
		MenuString.Format( _T("%s"), m_NationList[i].c_str() );
		if( mColumnContexts[i+2]->Show == true)
			DisplayMenu.AppendMenu( MF_STRING | MF_ENABLED | MF_CHECKED, ID_MENU_DISPLAY+i, MenuString );
		else
			DisplayMenu.AppendMenu( MF_STRING | MF_ENABLED | MF_UNCHECKED, ID_MENU_DISPLAY+i, MenuString );
	}

	int CloneSize = 0;
	for(size_t i=0; i < m_NationList.size();++i)
	{
		if( mColumnContexts[i+2]->Show == true)
		{
			MenuString.Format( _T("%s"), m_NationList[i].c_str() );
			CopyMenu.AppendMenu( MF_STRING, ID_MENU_CLONE_NATION+i, MenuString );
		}
		CloneSize++;
	}

	UINT32 cmd = MainMenu.TrackPopupMenuEx( 
		TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd*)this, NULL);

	if( ID_MENU_HIDE_NATION == cmd )
	{
		HideColumn(i3::rc_string(HeaderName));
		//Hide_Column( HeaderName );
		//m_DisplayNationList.erase( m_DisplayNationList.begin() + (nRow-2) );
	}
	else if( ID_MENU_ALLSHOW_NATION == cmd )
	{

		Show_AllColumns();
		_UpdateKey();
		Modify_UnSortedColumnText();
	}
	else if( ID_MENU_ALLHIDE_NATION == cmd )
	{
		size_t size = m_NationList.size();
		for(size_t i=0;i<size;++i)
		{
			HideColumn(m_NationList[i].c_str());
			//Hide_Column( m_NationList[ i ].c_str() );
		}
	}
	else if(( ID_MENU_COL_WIDTH == cmd ) || ( ID_MENU_SELECT_NATION == cmd ))
	{		
		this->PostMessage(WM_OPEN_WIDTH_DIALOG, cmd, nRow);		
	}
	else if( ID_MENU_SELECT_AUTOSIZE == cmd )
	{
		ChangeAutoSize();
	}
	else if( ID_MENU_DISPLAY <= cmd && ID_MENU_DISPLAY + DisplaySize>= cmd )
	{
		int Index = cmd - ID_MENU_DISPLAY;

		if (mColumnContexts[Index + 2]->Show == true)
			HideColumn(m_NationList[Index].c_str());//Hide_Column( m_NationList[ Index ].c_str() );
		else
			ShowColumn( m_NationList[ Index ].c_str() );
			

		_UpdateKey();
		Modify_UnSortedColumnText();
	}
	else if( ID_MENU_RESET_NATION == cmd )
	{
		i3TDKRegistryCtrl_Impl* pParent = (i3TDKRegistryCtrl_Impl*)GetParent();
		pParent->ResetRegNation( HeaderName.GetString(), false );
		SetRegKey(m_pKey, ACT_CHANGEKEY);
	}
	else if( ID_MENU_CLONE_NATION <= cmd && ID_MENU_CLONE_NATION + CloneSize>= cmd )
	{
		UINT FromNationIndex = 
			Get_ColumnContextIndex( (cmd - ID_MENU_CLONE_NATION)+2 ) - 2;
		UINT ToNationIndex = 
			Get_ColumnContextIndex(HeaderName) - 2;

		i3TDKRegistryCtrl_Impl* pParent = (i3TDKRegistryCtrl_Impl*)GetParent();

		pParent->CopyRegNation( FromNationIndex, ToNationIndex, false );

		SetRegKey(m_pKey, ACT_CHANGEKEY);
	}

	MainMenu.DestroyMenu();
	DisplayMenu.DestroyMenu();
	CopyMenu.DestroyMenu();
}

void i3TDKListCtrl_Impl::OnLvnColumnRClickList(int nRow, POINT pt)
{
	if( GetPefFileType() == PFT_MULTI )
		ColumnRClickMenu(nRow, pt);
}

void i3TDKListCtrl_Impl::OnLvnCellLClickList(int nItem, int nSubItem, POINT pt) 
{
	if( !m_bUseChildCellClick )
		return;

	if( nItem < 0 || nSubItem < 0 )
		return;

	i3TDKRegistryCtrl_Impl* pParent = (i3TDKRegistryCtrl_Impl*)GetParent();
	if( pParent->GetItemWeaponPef() )
	{
		CString TitleString = CListSubCtrl::Get_CellText(nItem, 0);
		if( IsStampText( TitleString ) )
			return;

		CString NumberString = CListSubCtrl::Get_CellText(nItem, 2);

		size_t Number  = i3::atoi( NumberString );

		if( Number == 0 )
		{
			I3PRINTLOG(I3LOG_FATAL, "The Weapon Number[ %d ] is not a valid number !", Number );
		}

		pParent->DisplayRegistryWeaponDictionary( Number );
		pParent->SelectSubTreeFirstItem();
	}
}

void i3TDKListCtrl_Impl::OnLvnCellRClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	i3TDKRegistryCtrl_Impl* pParent = (i3TDKRegistryCtrl_Impl*)GetParent();

	pParent->OnNMRClickListCtrl(pNMHDR, pResult);	
}

bool i3TDKListCtrl_Impl::PreKillFocus()
{
	if( m_Name == "List1" )
	{
		m_bFocusDrawLine = true;
		return true;
	}

	return false;
}

void i3TDKListCtrl_Impl::Clear_AnalysisItem(AnalysisType atype)
{
	CListSubCtrl::Clear_CustomDraw(atype);
}
void i3TDKListCtrl_Impl::Add_CompareItem(int nItem, int nSubItem)
{
	CListSubCtrl::Add_CustomDraw(nItem, nSubItem, DAT_COMPARE , RGB(237,28,36));
}
void i3TDKListCtrl_Impl::Add_MatchItem(int nItem, int nSubItem)
{
	CListSubCtrl::Add_CustomDraw(nItem, nSubItem, DAT_MATCH , RGB(10, 128,128));
}
void i3TDKListCtrl_Impl::Add_Modify(int nItem, int nSubItem)
{
	CListSubCtrl::Add_CustomDraw(nItem, nSubItem, DAT_MODIFY , RGB(237,28,36));
}

void i3TDKListCtrl_Impl::_AddPasteRowToCustomDraw(INT32 numRowToAdd)
{
	INT32 numItem = GetItemCount();
	INT32 numColumn = Get_ColumnCount();
	INT32 startRow = numItem - numRowToAdd;

	if(startRow < 0)
		return;

	for(INT32 m=0; m<numRowToAdd; m++)
	{
		for(INT32 k=0; k<numColumn; k++)
			Add_CustomDraw(startRow, k,  DAT_PASTE, RGB(0,100,0));

		startRow++;
	}

}

typedef std::map<std::string, std::vector<int>*> ItemResult;

void i3TDKListCtrl_Impl::AnalysisItem(int nItem, int nSubItem, AnalysisType Type)
{
	ItemResult ItemResultBank;

	//모든 아이템을 검색 한다.
	int Count = Get_ColumnCount() -2;
	for(int i=0; i<Count; ++i)
	{
		//위치에 아이템을 얻어 온다.
		CString Item = Get_CellText(nItem, i+2);

		ItemResult::iterator FindIt = ItemResultBank.find( Item.GetString() );

		//새로운 ItemResult를 추가한다.
		if( FindIt == ItemResultBank.end() )
		{
			std::vector<int>* pItemResult = new std::vector<int>;
			pItemResult->push_back(i+2);
			ItemResultBank.insert( ItemResult::value_type( Item.GetString(), pItemResult ) );
		}
		else
		{
			FindIt->second->push_back(i+2);
		}
	}

	Clear_AnalysisItem( Type);
	
	if( Type == DAT_COMPARE )	//젤 많은 Item을 표시한다.
	{
		ItemResult::iterator it = ItemResultBank.begin();
		ItemResult::iterator end = ItemResultBank.end();

		ItemResult::iterator maxIt;
		size_t maxSize = 0;

		for(; it != end; ++it)
		{
			if( it->second->size() > maxSize )
			{
				maxSize = it->second->size();
				maxIt = it;
			}
		}

		size_t size = maxIt->second->size();
		for(size_t i=0;i<size;++i)
		{
			Add_CompareItem( nItem, (*maxIt->second)[i]);
		}
	}
	else if( Type == DAT_MATCH )	//나와 같은 Item을 표시한다.
	{
		CString Item = Get_CellText(nItem, nSubItem);

		ItemResult::iterator FindIt = ItemResultBank.find( Item.GetString() );
		if( FindIt != ItemResultBank.end() )
		{
			size_t size = FindIt->second->size();
			for(size_t i=0;i<size;++i)
			{
				Add_MatchItem( nItem, (*FindIt->second)[i] );
			}
		}
	}

	ItemResult::iterator it = ItemResultBank.begin();
	ItemResult::iterator end = ItemResultBank.end();

	for(; it != end; ++it)
		delete it->second;

	ItemResultBank.clear();

	RedrawItems(0, GetItemCount() );
}



i3TDKListCtrl_Impl::CellSubType i3TDKListCtrl_Impl::PreSubControlType(int Row, int Col)
{
	if( Col == 1 )	return CST_NONE;

	CString Title = Get_CellText(Row, 0);

	if( IsStampText(Title) )
		return CST_NONE;

	if( Col == 0 )	return CST_EDIT;

	i3RegData * pData = (i3RegData *)GetDataPtr(Row);

	UINT32 Type = 0;
	if( GetPefFileType() == PFT_MULTI )
	{
		i3RegArray * pArrayData = (i3RegArray*)pData;
		Type = pArrayData->getElementType();
	}
	else
	{
		Type = pData->getDataType();
	}

	switch( Type )
	{
	case I3REG_TYPE_VEC2D:	case I3REG_TYPE_VEC3D:	case I3REG_TYPE_VEC4D:
	case I3REG_TYPE_COLOR:	case I3REG_TYPE_MATRIX:
		return CST_DIALOG;
		break;
	}

	return CST_EDIT;
}


void i3TDKListCtrl_Impl::PreSubDialogControl(int Row, int Col, 
											   CListSubDialog * pDialog, const CRect& rect, const CString & CellText)
{
	pDialog->SetTitle( Get_ColumnText(Col), Get_CellText(Row, 0) );

	i3RegData * pData = (i3RegData *)GetDataPtr(Row);

	UINT32 Type = 0;
	if( GetPefFileType() == PFT_MULTI ) 
	{
		i3RegArray * pArrayData = (i3RegArray*)pData;
		Type = pArrayData->getElementType();
	}
	else
	{
		Type = pData->getDataType();
	}

	CString Name;

	switch( Type )
	{
	case I3REG_TYPE_VEC2D:	Name = _T("X, Y");		break;
	case I3REG_TYPE_VEC3D:	Name = _T("X, Y, Z");		break;
	case I3REG_TYPE_VEC4D:	Name = _T("X, Y, Z, W");		break;
	case I3REG_TYPE_COLOR:	Name = _T("R, G, B, A");		break;
	case I3REG_TYPE_MATRIX:	Name = _T("Mat, rix");		break;
	}

	pDialog->InitDialogText( Name, CellText );
}

bool i3TDKListCtrl_Impl::isSameSubControl(int Row, int Col, LPTSTR String)
{
	TCHAR DisplayText[1024] = _T("");

	i3RegData* pData = (i3RegData*)GetDataPtr( Row );

	//Data Name 변경
	if( Col == 0 )
	{
		_tcscpy_s(DisplayText, 1024, String);
		pData->SetName( String );

		CListSubCtrl::EndSubControl(Row, Col, String);

		return false;
	}

	if( GetPefFileType() == PFT_MULTI ) 
	{
		i3RegArray* pRegArray = (i3RegArray*)pData;

		switch( pRegArray->getElementType() )
		{
		case	I3REG_TYPE_INT32:	
			{
				INT32 val = _toParseUINT32(String);
				_toString(val, DisplayText, 1024);
				return pRegArray->isSameRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_REAL32:	
			{
				REAL32 val = _toParseREAL32(String);
				_toString(val, DisplayText, 1024);
				return pRegArray->isSameRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_STRING:	
			{
				_tcscpy_s(DisplayText, 1024, String);
				return pRegArray->isSameRegData( Col - 2, String );
			}
			break;
		case	I3REG_TYPE_VEC2D:	
			{
				VEC2D val = _toParseVEC2D(String);
				_toString(val, DisplayText, 1024);
				return pRegArray->isSameRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_VEC3D:
			{
				VEC3D val = _toParseVEC3D(String);
				_toString(val, DisplayText, 1024);
				return pRegArray->isSameRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_VEC4D:	
			{
				VEC4D val = _toParseVEC4D(String);
				_toString(val, DisplayText, 1024);
				return pRegArray->isSameRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_COLOR:	
			{
				I3COLOR val = _toParseCOLOR(String);
				_toString(val, DisplayText, 1024);
				return pRegArray->isSameRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_MATRIX:	
			break;
		default:
			break;
		}
	}
	return false;
}

void i3TDKListCtrl_Impl::EndSubControl(int Row, int Col, LPTSTR String)
{
	TCHAR DisplayText[1024] = _T("");

	i3RegData* pData = (i3RegData*)GetDataPtr( Row );

	//Data Name 변경
	if( Col == 0 )
	{
		_tcscpy_s(DisplayText, 1024, String);
		pData->SetName( String );

		CListSubCtrl::EndSubControl(Row, Col, String);

		return;
	}

	if( GetPefFileType() == PFT_MULTI ) 
	{
		i3RegArray* pRegArray = (i3RegArray*)pData;

		switch( pRegArray->getElementType() )
		{
		case	I3REG_TYPE_INT32:	
			{
				INT32 val = _toParseUINT32(String);
				_toString(val, DisplayText, 1024);
				pRegArray->replaceRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_REAL32:	
			{
				REAL32 val = _toParseREAL32(String);
				_toString(val, DisplayText, 1024);
				pRegArray->replaceRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_STRING:	
			{
				_tcscpy_s(DisplayText, 1024, String);

				i3::wstring wstr;		i3::mb_to_utf16(String, wstr);

				pRegArray->replaceRegData( Col - 2, wstr );
			}
			break;
		case	I3REG_TYPE_VEC2D:	
			{
				VEC2D val = _toParseVEC2D(String);
				_toString(val, DisplayText, 1024);
				pRegArray->replaceRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_VEC3D:
			{
				VEC3D val = _toParseVEC3D(String);
				_toString(val, DisplayText, 1024);
				pRegArray->replaceRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_VEC4D:	
			{
				VEC4D val = _toParseVEC4D(String);
				_toString(val, DisplayText, 1024);
				pRegArray->replaceRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_COLOR:	
			{
				I3COLOR val = _toParseCOLOR(String);
				_toString(val, DisplayText, 1024);
				pRegArray->replaceRegData( Col - 2, val );
			}
			break;
		case	I3REG_TYPE_MATRIX:	
			break;
		default:
			break;
		}
	}
	else
	{
		switch( pData->getDataType() )
		{
		case	I3REG_TYPE_INT32:	
			{
				INT32 val = _toParseUINT32(String);
				_toString(val, DisplayText, 1024);

				(*(i3RegINT32*)pData) = val;
			}
			break;
		case	I3REG_TYPE_REAL32:	
			{
				REAL32 val = _toParseREAL32(String);
				_toString(val, DisplayText, 1024);
				(*(i3RegREAL32*)pData) = val;
			}
			break;
		case	I3REG_TYPE_STRING:	
			{
				_tcscpy_s(DisplayText, 1024, String);

				i3::wstring wstrValue;	i3::mb_to_utf16(String, wstrValue);
				(*(i3RegString*)pData) = wstrValue;
			}
			break;
		case	I3REG_TYPE_VEC2D:	
			{
				VEC2D val = _toParseVEC2D(String);
				_toString(val, DisplayText, 1024);
				(*(i3RegVEC2D*)pData) = val;
			}
			break;
		case	I3REG_TYPE_VEC3D:
			{
				VEC3D val = _toParseVEC3D(String);
				_toString(val, DisplayText, 1024);
				(*(i3RegVEC3D*)pData) = val;
			}
			break;
		case	I3REG_TYPE_VEC4D:	
			{
				VEC4D val = _toParseVEC4D(String);
				_toString(val, DisplayText, 1024);
				(*(i3RegVEC4D*)pData) = val;
			}
			break;
		case	I3REG_TYPE_COLOR:	
			{
				I3COLOR val = _toParseCOLOR(String);
				_toString(val, DisplayText, 1024);
				(*(i3RegCOLOR*)pData) = val;
			}
			break;
		case	I3REG_TYPE_MATRIX:	
			break;
		default:
			break;
		}
	}

	CListSubCtrl::EndSubControl(Row, Col, DisplayText);
}

BOOL i3TDKListCtrl_Impl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(HIWORD(wParam) == 1) // by AccelatorKey
	{
		WORD accelKeyID = LOWORD(wParam);
		LVHITTESTINFO* lvh = NULL;

		UINT32 cmd = 0;

		if(GetFocus() && GetFocus()->GetSafeHwnd() == GetSafeHwnd())
		{
			switch(accelKeyID)
			{
			case ID_ACCEL_EDITING_VAL_COPY  : if(GetSelectedCount()) cmd = ID_MENU_COPYDATA; break;
			case ID_ACCEL_EDITING_VAL_PASTE : cmd = ID_MENU_PASTEDATA; break;
			case ID_ACCEL_EDITING_VAL_DEL	: if(GetSelectedCount()) cmd = ID_MENU_DELETEDATA; break;
			default: break;
			}
		}

		if(cmd)
		{
			i3TDKRegistryCtrl_Impl* pParent = (i3TDKRegistryCtrl_Impl*)GetParent();
			pParent->DirectCallCommandRClickList(this, cmd, lvh);
			return TRUE;
		}
	}

	return CListSubCtrl::OnCommand(wParam, lParam);
}

namespace {
	struct _find_columnname_fn {
		_find_columnname_fn(const i3::rc_string& name) : name(name) {}
		inline bool operator()(const COLUMELIST* pcolumelist) const
		{
			return i3::generic_is_equal(pcolumelist->_Name, name);
		}

		const i3::rc_string& name;
	};
}

void i3TDKListCtrl_Impl::HideColumn(const i3::rc_string& sColume)
{
	Hide_Column(sColume.c_str());
	AddColumnStorage(false, sColume);
}

void i3TDKListCtrl_Impl::ShowColumn(const i3::rc_string& sColume)
{
	Show_Column(sColume.c_str());
	AddColumnStorage(true, sColume);
}

void i3TDKListCtrl_Impl::AddColumnStorage(bool bState, const i3::rc_string& sColume)
{
	i3::vector<COLUMELIST*>::iterator It = i3::find_if(m_vecColumnList.begin(), m_vecColumnList.end(), _find_columnname_fn(sColume));

	if (It == m_vecColumnList.end())
	{
		COLUMELIST* pColumn = new COLUMELIST;
		pColumn->_State = bState;
		i3::generic_copy_range(pColumn->_Name, sColume);

		m_vecColumnList.push_back(pColumn);
	}
	else
	{
		(*It)->_State = bState;
	}
}

void i3TDKListCtrl_Impl::UpdateColumnList()
{
	for (INT32 i = 0; i < m_DisplayNationList.size(); i++)
	{
		i3::vector<COLUMELIST*>::iterator It = i3::find_if(m_vecColumnList.begin(), m_vecColumnList.end(), _find_columnname_fn(m_DisplayNationList[i]));
		if (It != m_vecColumnList.end())
		{
			if ((*It)->_State)
				Show_Column(m_DisplayNationList[i].c_str());
			else
				Hide_Column(m_DisplayNationList[i].c_str());
		}
	}

}

std::vector< std::string > _Split(const std::string& src, const std::string & delims, unsigned int maxSplits = 0)
{
	std::vector< std::string > strBank;

	// Pre-allocate some space for performance
	strBank.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

	unsigned int numSplits = 0;

	// Use STL methods 
	size_t start, pos;
	start = 0;
	do 
	{
		pos = src.find_first_of(delims, start);
		if (pos == start)
		{
			// Do nothing
			start = pos + 1;
		}
		else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits))
		{
			// Copy the rest of the string
			strBank.push_back( src.substr(start) );
			break;
		}
		else
		{
			// Copy up to delimiter
			strBank.push_back( src.substr(start, pos - start) );
			start = pos + 1;
		}
		// parse up to next real data
		start = src.find_first_not_of(delims, start);
		++numSplits;

	} while (pos != std::string::npos);

	return strBank;
}

UINT8 _toParseUINT8(const char* Src)
{
	INT32 ret = _toParseUINT32(Src);

	ret = max( 0x0, min( 0xFF, ret) );

	return (UINT8)ret;
}
INT32 _toParseUINT32(const char* Src)
{
	std::istringstream str(Src);
	INT32	ret = 0;
	str >> ret;
	return ret;
}
REAL32 _toParseREAL32(const char* Src)
{
	std::istringstream str(Src);
	REAL32	ret = 0;
	str >> ret;
	return ret;
}
VEC2D _toParseVEC2D(const char* Src)
{
	std::vector< std::string > List = _Split( Src, ", ");

	I3ASSERT( List.size() == 2 );

	VEC2D ret;
	if( List.size() == 2 )
	{
		ret.x = _toParseREAL32( List[0].c_str() );
		ret.y = _toParseREAL32( List[1].c_str() );
	}
	else
	{
		ret.x = 0;		ret.y = 0;
	}

	return ret;
}
VEC3D _toParseVEC3D(const char* Src)
{
	std::vector< std::string > List = _Split( Src, ", ");

	I3ASSERT( List.size() == 3 );

	VEC3D ret;
	if( List.size() == 3 )
	{
		ret.x = _toParseREAL32( List[0].c_str() );
		ret.y = _toParseREAL32( List[1].c_str() );
		ret.z = _toParseREAL32( List[2].c_str() );
	}
	else
	{
		ret.x = 0;		ret.y = 0;		ret.z = 0;
	}

	return ret;

}
VEC4D _toParseVEC4D(const char* Src)
{
	std::vector< std::string > List = _Split( Src, ", ");

	I3ASSERT( List.size() == 4 );

	VEC4D ret;
	if( List.size() == 4 )
	{
		ret.x = _toParseREAL32( List[0].c_str() );
		ret.y = _toParseREAL32( List[1].c_str() );
		ret.z = _toParseREAL32( List[2].c_str() );
		ret.w = _toParseREAL32( List[3].c_str() );
	}
	else
	{
		ret.x = 0;		ret.y = 0;		ret.z = 0;	ret.w = 0;
	}

	return ret;
}
I3COLOR _toParseCOLOR(const char* Src)
{
	std::vector< std::string > List = _Split( Src, ", ");

	I3ASSERT( List.size() == 4 );

	I3COLOR ret;
	if( List.size() == 4 )
	{
		ret.r = _toParseUINT8( List[0].c_str() );
		ret.g = _toParseUINT8( List[1].c_str() );
		ret.b = _toParseUINT8( List[2].c_str() );
		ret.a = _toParseUINT8( List[3].c_str() );
	}
	else
	{
		ret.r = 0;		ret.g = 0;		ret.b = 0;	ret.a = 0;
	}
	return ret;
}
MATRIX _toParseMATRIX(const char* Src)
{
	MATRIX ret;
	ret.m[0][0] = 0;
	return ret;
}


void _toString(UINT8 Src, char* Dest, size_t sizeOfDest)
{
	_toString( (INT32)Src, Dest, sizeOfDest );
}
void _toString(INT32 Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%d", Src );
}
void _toString(REAL32 Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%f", Src );
}
void _toString(const VEC2D & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%f, %f", Src.x, Src.y );
}
void _toString(const VEC3D & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%f, %f, %f", Src.x, Src.y, Src.z );
}
void _toString(const VEC4D & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%f, %f, %f, %f", Src.x, Src.y, Src.z, Src.w );
}
void _toString(const I3COLOR & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%d, %d, %d, %d", Src.r, Src.g, Src.b, Src.a );
}
void _toString(const MATRIX & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "Click.....");
}