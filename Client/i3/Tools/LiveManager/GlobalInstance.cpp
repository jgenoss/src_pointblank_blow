#include "stdafx.h"
#include "GlobalInstance.h"
#include "MainFrm.h"
#include "LiveManager.h"
CGlobalInstance::CGlobalInstance()
{
	//m_pConsole_Log = NULL;
	m_nPrograssbar = 0;
	m_CurFocusWindow = NULL;
	m_bSave = false;
}
CGlobalInstance::~CGlobalInstance()
{
	std::map<int, CWeapon_Pane*>::iterator it;
	for(it=m_mapPane.begin(); it!=m_mapPane.end(); ++it)
	{
		delete it->second;
	}

	if( m_pItemListPane )
	{
		delete m_pItemListPane;
		m_pItemListPane = NULL;
	}
// 	if( m_pConsole_Log )
// 	{
// 		delete m_pConsole_Log;
// 		m_pConsole_Log = NULL;
// 	}
}
void CGlobalInstance::Create(CMainFrame *pMainFrame)
{
// 	m_pConsole_Log = new CConsoleLoggerEx;
// 	m_pConsole_Log->Create("this one");
// 	m_pConsole_Log->cls(CConsoleLoggerEx::COLOR_BACKGROUND_RED);
// 	m_pConsole_Log->cprintf("여기에 로그남기자\n");

	DWORD dwStyle;

	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI;


	CWeapon_Pane* pWeaponPane = new CWeapon_Pane;
	pWeaponPane->Create( L"Weapon", pMainFrame, CRect( 0, 0, 200, 200), TRUE, ID_WEAPON_PANE, dwStyle );
	pWeaponPane->EnableDocking(CBRS_ALIGN_ANY);
	pMainFrame->DockPane(pWeaponPane);

	m_mapPane[ID_WEAPON_PANE] = pWeaponPane;

	m_pItemListPane = new CItemList_Pane;
	m_pItemListPane->Create( L"ItemList", pMainFrame, CRect( 0, 0, 200, 200), TRUE, ID_ITEMLIST_PANE, dwStyle );
	m_pItemListPane->EnableDocking(CBRS_ALIGN_ANY);
	pMainFrame->DockPane(m_pItemListPane);

	pWeaponPane = new CEquip_Pane;
	pWeaponPane->Create( L"Equip", pMainFrame, CRect( 0, 0, 200, 200), TRUE, ID_EQUIP_PANE, dwStyle );
	pWeaponPane->EnableDocking(CBRS_ALIGN_ANY);
	pMainFrame->DockPane(pWeaponPane);

	m_mapPane[ID_EQUIP_PANE] = pWeaponPane;
}

void CGlobalInstance::Update()
{
	std::map<int, CWeapon_Pane*>::iterator it;
	for(it=m_mapPane.begin(); it!=m_mapPane.end(); ++it)
	{
		it->second->Update();
	}

	if( !m_deqStatusBarMsg.empty() )
	{
		((CMainFrame*)theApp.GetMainWnd())->SetStatusBarText(CString((char*)m_deqStatusBarMsg[0].c_str()));
		m_deqStatusBarMsg.pop_back();
	}

	((CMainFrame*)theApp.GetMainWnd())->SetPrograssBar(m_nPrograssbar);
}

CWeapon_Pane* CGlobalInstance::GetWeaponPane(int nPropIndex) 
{ 
	std::map<int, CWeapon_Pane*>::iterator it = m_mapPane.find(nPropIndex);
	
	if( it != m_mapPane.end() )
	{
		return it->second;
	}

	return NULL;
}

void CGlobalInstance::Save()
{
	std::map<int, CWeapon_Pane*>::iterator it;
	for(it=m_mapPane.begin(); it!=m_mapPane.end(); ++it)
	{
		if( m_CurFocusWindow == it->second )
			it->second->Save();
	}
}


void CGlobalInstance::AddRegistry()
{
	std::map<int, CWeapon_Pane*>::iterator it;
	for(it=m_mapPane.begin(); it!=m_mapPane.end(); ++it)
	{
		if( m_CurFocusWindow == it->second )
			it->second->AddRegistry();
	}
}

void CGlobalInstance::DeleteRegistry()
{
	std::map<int, CWeapon_Pane*>::iterator it;
	for(it=m_mapPane.begin(); it!=m_mapPane.end(); ++it)
	{
		if( m_CurFocusWindow == it->second )
			it->second->DeleteRegistry();
	}
}

void CGlobalInstance::StartSave()
{
	std::map<int, CWeapon_Pane*>::iterator it;
	for(it=m_mapPane.begin(); it!=m_mapPane.end(); ++it)
	{
		if( m_CurFocusWindow == it->second )
			it->second->StartSave();
	}
}

int CGlobalInstance::GetCurFocusWindow()
{
	std::map<int, CWeapon_Pane*>::iterator it;
	for(it=m_mapPane.begin(); it!=m_mapPane.end(); ++it)
	{
		if( m_CurFocusWindow == it->second )
			return it->first;
	}

	return -1;
}

void CGlobalInstance::_CompressString(std::string &str)
{
	while (true)
	{
		std::string::size_type pos = str.find("-");
		if( pos == std::string::npos )
		{
			break;
		}
		else
		{
			str.erase(str.begin()+pos, str.begin()+pos+1);
		}
	}

	while (true)
	{
		std::string::size_type pos = str.find("_");
		if( pos == std::string::npos )
		{
			break;
		}
		else
		{
			str.erase(str.begin()+pos, str.begin()+pos+1);
		}
	}
}

void CGlobalInstance::_ReplaceString(std::string &str, std::string findString, std::string replaceString)
{
	while (true)
	{
		std::string::size_type pos = str.find(findString);
		if( pos == std::string::npos )
		{
			break;
		}
		else
		{
			str.replace(pos, findString.size(), replaceString);
		}
	}
}

char* CGlobalInstance::StringToChar(CString& _strSource, char* _szResult, int nSize)
{
	ZeroMemory(_szResult, nSize);
	if( WideCharToMultiByte(CP_ACP, 0, _strSource, -1, _szResult, _strSource.GetLength(), NULL, NULL) == 0 )
		return NULL;
	else
		return _szResult;
}
std::vector< std::string > CGlobalInstance::_Split(const std::string& src, const std::string & delims, unsigned int maxSplits)
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

UINT8 CGlobalInstance::_toParseUINT8(const char* Src)
{
	INT32 ret = _toParseUINT32(Src);

	ret = max( 0x0, min( 0xFF, ret) );

	return (UINT8)ret;
}
INT32 CGlobalInstance::_toParseUINT32(const char* Src)
{
	std::istringstream str(Src);
	INT32	ret = 0;
	str >> ret;
	return ret;
}
REAL32 CGlobalInstance::_toParseREAL32(const char* Src)
{
	std::istringstream str(Src);
	REAL32	ret = 0;
	str >> ret;
	return ret;
}
VEC2D CGlobalInstance::_toParseVEC2D(const char* Src)
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
VEC3D CGlobalInstance::_toParseVEC3D(const char* Src)
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
VEC4D CGlobalInstance::_toParseVEC4D(const char* Src)
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
I3COLOR CGlobalInstance::_toParseCOLOR(const char* Src)
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
MATRIX CGlobalInstance::_toParseMATRIX(const char* Src)
{
	MATRIX ret;
	ret.m[0][0] = 0;
	return ret;
}

void CGlobalInstance::_toString(UINT8 Src, char* Dest, size_t sizeOfDest)
{
	_toString( (INT32)Src, Dest, sizeOfDest );
}
void CGlobalInstance::_toString(INT32 Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%d", Src );
}
void CGlobalInstance::_toString(REAL32 Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%f", Src );
}
void CGlobalInstance::_toString(const VEC2D & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%f, %f", Src.x, Src.y );
}
void CGlobalInstance::_toString(const VEC3D & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%f, %f, %f", Src.x, Src.y, Src.z );
}
void CGlobalInstance::_toString(const VEC4D & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%f, %f, %f, %f", Src.x, Src.y, Src.z, Src.w );
}
void CGlobalInstance::_toString(const I3COLOR & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "%d, %d, %d, %d", Src.r, Src.g, Src.b, Src.a );
}
void CGlobalInstance::_toString(const MATRIX & Src, char* Dest, size_t sizeOfDest)
{
	sprintf_s( Dest, sizeOfDest, "Click.....");
}



//value값을 문자열로 변환
void CGlobalInstance::_Convert2String(char * conv, size_t sizeOfString, i3RegData * pData)
{
	if (!pData) return; 
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

void CGlobalInstance::_String2Convert(char * conv, size_t sizeOfString, i3RegData * pData)
{
	if (!pData) return; 
	switch( pData->getDataType() )
	{
	case	I3REG_TYPE_INT32:
		{
			((i3RegINT32*)pData)->setValue(atoi(conv));
		}
		break;
	case	I3REG_TYPE_REAL32:
		{
			((i3RegREAL32*)pData)->setValue((REAL32)atof(conv));
		}
		break;
	case	I3REG_TYPE_STRING:	
		{
			i3::rc_wstring wstr;
			i3::mb_to_utf16(conv, wstr);

			((i3RegString*)pData)->setValue(wstr);
		}
		break;
	case	I3REG_TYPE_VEC2D:
		{
			VEC2D vec2d = _toParseVEC2D(conv);
			((i3RegVEC2D*)this)->setValue(&vec2d);
		}
		break;
	case	I3REG_TYPE_VEC3D:
		{
			VEC3D vec3d = _toParseVEC3D(conv);
			((i3RegVEC3D*)this)->setValue(&vec3d);
		}
		break;
	case	I3REG_TYPE_VEC4D:
		{
			VEC4D vec4d = _toParseVEC4D(conv);
			((i3RegVEC4D*)this)->setValue(&vec4d);
		}
		break;
	case	I3REG_TYPE_COLOR:
		{
			I3COLOR color = _toParseCOLOR(conv);
			((i3RegCOLOR*)this)->setValue(&color);
		}
		break;
	case	I3REG_TYPE_MATRIX:
		{
			// 			MATRIX Value;
			// 			GET_REG_SINGLE_DATA(pData, &Value);
		}
		break;
	default:
		break;
	}
}

void CGlobalInstance::cprintf(const char *format,...)
{

	int ret;
	char tmp[1024];

	va_list argList;
	va_start(argList, format);
#ifdef WIN32
	ret = _vsnprintf(tmp,sizeof(tmp)-1,format,argList);
#else
	ret = vsnprintf(tmp,sizeof(tmp)-1,format,argList);
#endif
	tmp[ret]=0;


	va_end(argList);

	((CMainFrame*)theApp.GetMainWnd())->cprintf(CString(tmp));
}

void CGlobalInstance::SetPrograssBar(INT32 nPos)
{
	m_nPrograssbar = nPos;
}