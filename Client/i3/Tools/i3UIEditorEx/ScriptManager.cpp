#include "stdafx.h"
#include "ScriptManager.h"
#include "i3UIDefines.h"
#include "Dlg_InsertEventHandler.h"	
#include "MainFrm.h"
#include "i3UIEditorDoc.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/compare/generic_is_nequal.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/contain_char.h"
#include "i3Base/string/ext/generic_string_cat.h"

const char* BLOCKTAG_START = "--[BLOCK_START:";
const char* BLOCKTAG_END = "--[BLOCK_END:";
const char* BLOCKFMT_START = "--[BLOCK_START:%s]";
const char* BLOCKFMT_END = "--[BLOCK_END:%s]";
const char* BLOCKFMT = "---------------------------------------\n--[BLOCK_START:%s]\n\n--[BLOCK_END:%s]\n";
const int	BLOCKLEN_START = 15;
const int	BLOCKLEN_END = 13;

struct LUAMEMBER_INFO
{
	//ex: uiControl:OnNotify( evt, ... )
	char	m_szClass[64] = { 0 };		//uiControl
	char	m_cType = 0;			//:
	char	m_szName[128] = { 0 };		//OnNotify
	char	m_szParams[128] = { 0 };	//evt, ...
};


struct LUABLOCK_INFO
{
	char	m_szBlockName[MAX_PATH] = { 0 };
	INT32	m_nStart = 0;
	INT32	m_nEnd = 0;
};

/*static*/
i3UIScene*		s_pScene = NULL;
i3UILibrary*	s_pLibrary = NULL;

char			s_szLuaRoot[MAX_PATH] = {0};			// Root Path( ex: D:/PointBlank/Media/Script/uiLua)
char			s_szLuaIntellisense[MAX_PATH] = {0};		// XML Path( ex: D:/PointBlank/Media/Script/uiLua/intellisense.xml)
char			s_szScrScript[MAX_PATH] = {0};
char			s_szLibScript[MAX_PATH] = {0};

i3::vector<LUAMEMBER_INFO*>	s_LuaMemberTable;		// LUAMEMBER_INFO*

// BlockInfo는 Table로 관리할 필요없이 그때그때 buffer를 parsing한다.
//List			s_LuaBlockTable_Scr;	// LUABLOCK_INFO*
//List			s_LuaBlockTable_Lib;	// LUABLOCK_INFO*


void ScriptManager::SetScene( i3UIScene* pScene)
{
	s_pScene = pScene;
}

void ScriptManager::SetLibrary( i3UILibrary* pLibrary)
{
	s_pLibrary = pLibrary;
}

void ScriptManager::SetLuaRoot( const char* pszLuaRoot)
{
	if( pszLuaRoot != NULL)
		i3::string_ncopy_nullpad( s_szLuaRoot, pszLuaRoot, MAX_PATH);
	else
		i3::string_ncopy_nullpad( s_szLuaRoot, "", MAX_PATH);

	i3::snprintf( s_szLuaIntellisense, MAX_PATH, "%s/%s", s_szLuaRoot, "intellisense.xml");
}

void ScriptManager::SetScrScript( const char* pszScriptForScreen)
{
	if( pszScriptForScreen != NULL)
		i3::string_ncopy_nullpad( s_szScrScript, pszScriptForScreen, MAX_PATH);
	else
		i3::string_ncopy_nullpad( s_szScrScript, "", MAX_PATH);
}

void ScriptManager::SetLibScript( const char* pszScriptForLibrary)
{
	if( pszScriptForLibrary != NULL)
		i3::string_ncopy_nullpad( s_szLibScript, pszScriptForLibrary, MAX_PATH);
	else
		i3::string_ncopy_nullpad( s_szLibScript, "", MAX_PATH);
}

bool _isWhiteSpace( char c)
{
	switch( c)
	{
	case ' ':
	case '\t':
	case '\n':
	case '\r':
	case 0:
		return true;
	}

	return false;
}

//--로 시작하는 pToken에 대하여 
//주석의 시작부분과 끝부분토큰을 구한다. 
//Ex) pCmtStart="--", pCmtEnd=""
//Ex) pCmtStart="--[==[", pCmtEnd="]==]"
void _GetCommentTokens( char* pToken, char* pCmtStart, char* pCmtEnd)
{
	// considering cases : --[[, --[===[
	// otherwise return with pCmtEnd[0] = 0 and indicates single-line comment
	
	INT32 i = 0; 
	pCmtEnd[0] = 0;

	while( i < 3)
	{
		pCmtStart[i] = pToken[i];
		i++;
	}

	if( pToken[2] != '[')
		return;

	while( pToken[i] == '=')
		i++;

	pCmtStart[i+1] = 0;

	while( i >= 3)
	{
		if( pToken[i] != '[' && pToken[i] != '=')
			return;

		pCmtStart[i] = pToken[i];
		i--;
	}

	i = 2;
	INT32 j = 0;

	while( pCmtStart[i] != 0)
	{
		switch( pCmtStart[i])
		{
		case '=':	pCmtEnd[j++] = '='; break;
		case '[':	pCmtEnd[j++] = ']'; break;
		default: break;
		}
		
		i++;
	}

	pCmtEnd[j] = 0;
}

// White Space를 기준으로 한 Token을 구한다.
bool _GetNextToken( char* pBuf, INT32 &i, char* pToken, INT32 &nTokenStart, char* pSep)
{
	//initial & final state: i points to non-whitespace character or '\0'
	//pToken: sequence of non-whitespace characters
	//pSep: sequence of whitespace characters

	//Token
	nTokenStart = i;

	if( pBuf[i] == 0)
		return false;
	
	INT32 nToken = 0;
	while( !_isWhiteSpace( pBuf[i]))
	{
		if( nToken == 0)
			nTokenStart = i;

		pToken[nToken++] = pBuf[i++];
		
		if( nToken >= MAX_PATH-1)
			break;	
	}

	pToken[nToken] = 0;

	//Sep
	INT32 nSep = 0;
	while( pBuf[i] != 0 && _isWhiteSpace( pBuf[i]))
	{
		pSep[nSep++] = pBuf[i++];

		if( nSep >= MAX_PATH-1)
			break;	
	}
	
	pSep[nSep] = 0;
	
	return true;
}

// White Space나 Lua Comment가 아닌 의미있는 토큰을 구한다. 
// . : ( ) = 는 별개의 토큰이 될 수 있도록 앞,뒤에서 끊어준다. 

char _special_token[] = { '.', ':', '=', '(', ')', 0};

bool _GetNextLuaToken( char* pBuf, INT32 &i, char* pToken)
{
	INT32 previ = i;	//_GetNextToken 전에 previ 를 기억
	char pSep[MAX_PATH];
	INT32 nTokenStart = -1;

	bool bFoundToken = false;
	
	while( !bFoundToken && _GetNextToken( pBuf, i, pToken, nTokenStart, pSep))
	{
		INT32 idx = i3::contain_string( pToken, "--");
		
		if( idx < 0)
		{
			if( pToken[0] == 0)
				continue;		
			else
				bFoundToken = true;		//no comment
		}
		else if( idx == 0)
		{
			char pCmtStart[MAX_PATH];
			char pCmtEnd[MAX_PATH];
			_GetCommentTokens( pToken, pCmtStart, pCmtEnd);

			if( pCmtEnd[0] == 0)	//single-line
			{
				while( i3::contain_string( pSep, "\n") == -1 && _GetNextToken( pBuf, i, pToken, nTokenStart, pSep))
				{
				}
			}
			else					//multi-line
			{
				while( i3::contain_string( pToken, pCmtEnd) == -1 && _GetNextToken( pBuf, i, pToken, nTokenStart, pSep))
				{
				}

				// ]] 바로 뒤에 붙어있는 토큰 처리
				// Ex: ']]function', ']==]function' 
				while( pBuf[i-1] != ']')
					i--;
			}
		}
		else
		{
			//주석이 토큰의 중간 혹은 맨뒤에 오는 경우
			//Ex: function--[[comment]]uiControl:func()

			INT32 j = 0;
			while( pToken[j] != '-')
				j++;
			
			pToken[j] = 0;
			i = previ + j;			// 다음 검색은 여기부터
			
			bFoundToken = true;	
		}

		previ = i;
	}

	if( bFoundToken)
	{
		INT32 idx=0;
		while( pToken[idx] != 0)
		{
			INT32 j = 0;
			while( _special_token[j] != 0)
			{
				if( pToken[idx] == _special_token[j])
				{
					if( idx == 0)
					{
						pToken[idx+1] = 0;
						i = nTokenStart + 1;
					}
					else
					{
						pToken[idx] = 0;
						i = nTokenStart + idx;
					}
					
					return true;
				}
				
				j++;
			}
			
			idx++;
		}
	}

	return bFoundToken;
}

void _ConvertToLuaMemberInfo( const char* pszSig, LUAMEMBER_INFO* pInfo)
{
	INT32 i=0;
	INT32 j=0;

	while( pszSig[i] != '.' && pszSig[i] != ':')	// (pszSig[i] != 0) check? 
	{
		pInfo->m_szClass[j++] = pszSig[i++];
	}
	
	pInfo->m_szClass[j] = 0;
	
	pInfo->m_cType = pszSig[i++];

	j=0;
	while( pszSig[i] != '(')
	{
		pInfo->m_szName[j++] = pszSig[i++];
	}

	pInfo->m_szName[j] = 0;

	i++;

	j=0;
	while( pszSig[i] != ')')
	{
		pInfo->m_szParams[j++] = pszSig[i++];
	}

	pInfo->m_szParams[j] = 0;
}

bool _IsValidMember( LUAMEMBER_INFO* pInfo)
{
	if( pInfo->m_szName[0] == '_')
		return false;	// assume this function is protected

	for( INT32 i=1; i< I3UI_EVT_MAXCOUNT; i++)
	{
		if( i3::generic_is_iequal( pInfo->m_szName, s_EventInfoTable[i].m_pszEvent) )
			return false;	// this function is 'Event Handler', and will show up in 'Event Handler' section 
	}

	return true;
}

bool _SaveTo( const char* pszClass, const char* pSignature, i3XMLElement* pHeader)
{
	LUAMEMBER_INFO info;

	_ConvertToLuaMemberInfo( pSignature, &info);

	//protected function이나 event hanlder는 걸러낸다. 
	if( _IsValidMember( &info))
	{
		i3XMLElement* pNode = new i3XMLElement( pszClass);

		pNode->SetAttribute( "class", info.m_szClass);

		if( info.m_cType == ':')
			pNode->SetAttribute( "type", ":");
		else if( info.m_cType == '.')
			pNode->SetAttribute( "type", ".");
		else
			I3ASSERT( 0);

		pNode->SetAttribute( "name", info.m_szName);
		pNode->SetAttribute( "params", info.m_szParams);

		pHeader->LinkEndChild( pNode);

		return true;
	}

	return false;
}

bool ScriptManager::_CreateIntellisenseFile( i3XMLElement* pHeader, const char* pszLuafile, const char* pszClass)
{
	CFile file;
	
	if( !file.Open( pszLuafile, CFile::modeRead))	// Open
		return false;

	ULONGLONG filelen = file.GetLength();

	INT32 nBufSize = (INT32)filelen + 1;		//conversion ok?
	char* pBuf = new char[nBufSize];	
	
	file.Read( pBuf, nBufSize);		// returns the number of bytes transferred to the buffer
	pBuf[nBufSize-1] = 0;

	file.Close();

	// Parsing
	INT32 i=0;
	char pToken[MAX_PATH];

	while( _GetNextLuaToken( pBuf, i, pToken))
	{
		//I3TRACE("%s\n", pToken);

		if( i3::generic_is_iequal( "function", pToken) )
		{
			//function block
			char pSignature[MAX_PATH];

			i3::string_ncopy_nullpad( pSignature, "", MAX_PATH);
			
			while( _GetNextLuaToken( pBuf, i, pToken) && i3::contain_char( pToken, ')') == -1)
			{
				i3::generic_string_cat( pSignature, pToken);
			}

			i3::generic_string_cat( pSignature, pToken);

			if( i3::contain_string( pSignature, pszClass) > -1 &&
				(i3::contain_char( pSignature, '.') > -1 || i3::contain_char( pSignature, ':') > -1) &&
				i3::contain_char( pSignature, '(') > -1 && 
				i3::contain_char( pSignature, ')') > -1)
			{

				_SaveTo( pszClass, pSignature, pHeader);
			}
			
		}
		else if( i3::generic_is_iequal( pToken, pszClass) )
		{
			INT32 previ = i;
			bool bFound = false;
						
			_GetNextLuaToken( pBuf, i, pToken);
			if( i3::generic_is_equal( pToken, ".") )
			{
				_GetNextLuaToken( pBuf, i, pToken);

				char szName[MAX_PATH] = {0,};
				i3::string_ncopy_nullpad( szName, pToken, MAX_PATH);
				
				_GetNextLuaToken( pBuf, i, pToken);
				if( i3::generic_is_equal( pToken, "=") )
				{
					_GetNextLuaToken( pBuf, i, pToken);
					if( i3::generic_is_iequal( pToken, "function") )
					{
						char szParams[MAX_PATH] = {0,};
						while( _GetNextLuaToken( pBuf, i, pToken) && i3::contain_char( pToken, ')') == -1)
						{
							i3::generic_string_cat( szParams, pToken);
						}
						i3::generic_string_cat( szParams, pToken);

						char pSignature[MAX_PATH] = {0,};

						if( i3::contain_string( szParams, "self") == 1)
						{
							char szNewParams[MAX_PATH] = {0,};
							i3::snprintf( szNewParams, MAX_PATH, "(%s", &szParams[6]);
							i3::snprintf( pSignature, MAX_PATH, "%s:%s%s", pszClass, szName, szNewParams);
						}
						else
						{
							i3::snprintf( pSignature, MAX_PATH, "%s.%s%s", pszClass, szName, szParams);
						}
						
						//I3TRACE("Function signature Found!!\n");
						bFound = true;

						_SaveTo( pszClass, pSignature, pHeader);
					}
				}
			}
			
			if( !bFound)
				i = previ;
		}
		
	}

	delete [] pBuf;

	return true;
}

bool ScriptManager::CreateIntellisenseFile( const i3::vector<i3::rc_string>& LuaList)
{
	i3XMLDocument* pDoc = new TiXmlDocument( s_szLuaIntellisense);

	TiXmlDeclaration decl("1.0", "euc-kr", "yes");
	pDoc->InsertEndChild( decl);

	i3XMLElement* pRoot = new i3XMLElement( "i3UIEditor_LuaIntellisense");	// no delete? 
	pDoc->LinkEndChild( pRoot);


	for( size_t i=0; i< LuaList.size(); i++)
	{
		const char* pszLuaFile = LuaList[i].c_str();
		char pBuf[MAX_PATH];

//		i3String::SplitFileName( pszLuaFile, pBuf, FALSE);
		i3::extract_filetitle( pszLuaFile, pBuf);

		i3XMLElement* pHeader = new i3XMLElement( pBuf);

		pRoot->LinkEndChild( pHeader);

		bool bRes = _CreateIntellisenseFile( pHeader, pszLuaFile, pBuf);

		if( bRes)
			I3TRACE("Successful: %s\n", pszLuaFile);
	}

	bool bResult = pDoc->SaveFile();

	if( !bResult)
		I3PRINTLOG(I3LOG_WARN, "Failed to create %s\n", s_szLuaIntellisense);

	delete pDoc;
	pDoc = NULL;

	return bResult;
}

void ScriptManager::BuildIntellisense( void)
{
	i3::vector<i3::rc_string> luaList;

	GetLuaClassFiles( s_szLuaRoot, luaList);
	
	CreateIntellisenseFile( luaList);

	BuildLuaMemberTable();
}


//bool ScriptManager::FindText( const char* pszFile, const char* pszText, INT32* pStart, INT32* pEnd)
//{
//	CFile file;
//
//	if( !file.Open( pszFile, CFile::modeRead))	// Open
//		return false;
//	
//	ULONGLONG filelen = file.GetLength();
//
//	INT32 nBufSize = (INT32)filelen + 1;		//conversion ok?
//	char* pBuf = new char[nBufSize];	
//	
//	file.Read( pBuf, nBufSize);		// returns the number of bytes transferred to the buffer
//	pBuf[nBufSize-1] = 0;
//
//	file.Close();
//
//	// File Block
//	INT32 nStart = (INT32)(strstr( pBuf, pszText) - pBuf);	//INT32 nStart = i3String::Contain( pBuf, pszBlockStart);
//	INT32 nEnd = nStart + i3::generic_string_size( pszText);
//
//	delete [] pBuf;
//	
//	if( nStart > -1)
//	{
//		if( pStart != NULL)
//			*pStart = nStart;
//		if( pEnd != NULL)
//			*pEnd = nEnd;
//
//		return true;
//	}
//	
//	return false;
//}
//	
//void ScriptManager::InsertText( const char* pszFile, const char* pszText, INT32 nInsertAt)
//{
//	CFile file;
//
//	if( !file.Open( pszFile, CFile::modeRead))	// Open
//		return;
//	
//	ULONGLONG filelen = file.GetLength();
//
//	INT32 nBufSize = (INT32)filelen + 1;		//conversion ok?
//	char* pBuf = new char[nBufSize];	
//
//	file.Read( pBuf, nBufSize);		// returns the number of bytes transferred to the buffer
//	pBuf[nBufSize-1] = 0;
//
//	file.Close();
//
//	if( !file.Open( pszFile, CFile::modeWrite))
//	{
//		delete [] pBuf;
//		return;
//	}
//
//	file.Seek( nInsertAt, CFile::begin);
//
//	//MINMAX
//
//	file.Write( pszText, i3::generic_string_size( pszText));
//	file.Write( &pBuf[nInsertAt], nBufSize - nInsertAt);		// 'NUL' character
//
//
//
//	file.Close();
//}

void	_Rec_GetAllChild( i3GameNode* pRoot, i3::vector<i3GameNode*>& Out)
{
	if(( pRoot->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
	{
		//if( !i3::same_of<i3UIControl*>(pRoot)) //->IsExactTypeOf( i3UIControl::static_meta()) == 0)
		i3UIControl* pCtrl = (i3UIControl*)pRoot;

		if(( pCtrl->getStyle() & I3UI_STYLE_SLAVE) == 0)
		{
			Out.push_back( pRoot);
		}
	}

	i3GameNode* pChild = pRoot->getFirstChild();

	while( pChild != NULL)
	{
		_Rec_GetAllChild( pChild, Out);	//Recursive

		pChild = pChild->getNext();
	}
}

void ScriptManager::GetLuaControls( i3::vector<i3GameNode*>& Out)
{
	if( s_pScene != NULL)
	{
		_Rec_GetAllChild( s_pScene, Out);
		
		i3::vu::remove(Out, s_pScene);
	}
}

void ScriptManager::GetLuaTemplates( i3::vector<i3UITemplate*>& Out)
{
	if( s_pLibrary != NULL)
	{
		INT32 nCnt = s_pLibrary->getElementCount();

		for( INT32 i=0; i< nCnt; i++)
		{
			Out.push_back( s_pLibrary->getElement( i));
		}
	}
}
	
void ScriptManager::GetLuaEventHandlers( i3::vector<i3::rc_string>& Out, i3UIControl* pCtrl)
{
	//i3Lua
	//i3UIDefines.h
	//s_EventInfoTable

	// i starts with 1
	for( INT32 i=1; i< I3UI_EVT_MAXCOUNT; i++)
	{
		Out.push_back( s_EventInfoTable[i].m_pszEvent);		
	}
}

const char* ScriptManager::GetLuaEventCode( const char* pszEventHandler)
{
	for( INT32 i=1; i< I3UI_EVT_MAXCOUNT; i++)
	{
		if(i3::generic_is_iequal( s_EventInfoTable[i].m_pszEvent, pszEventHandler) )
		{
			return s_EventInfoTable[i].m_pszCode;
		}
	}

	//I3ASSERT(0);
	return "Not Found";
}

const char* ScriptManager::GetLuaEventDescription( const char* pszEventHandler)
{
	//temp
	return GetLuaEventCode( pszEventHandler);
}

struct _classmatching_
{
	const char*		m_pszLuaclass;
	i3ClassMeta*	m_pMeta;
};

void ScriptManager::GetLuaMemberFunctions( i3::vector<i3::rc_string>& Out, i3UIControl* pCtrl)
{
	//----------------------------------------------------------------------------------
	//test
	static _classmatching_ s_info[] = {
		{	"uiControl",			i3UIControl::static_meta()	},
		{	"uiCaption",			i3UICaptionControl::static_meta()	},
		{	"uiFrameWnd",			i3UIFrameWnd::static_meta()	},
		{	"uiButton",				i3UIButton::static_meta()	},
		{	"uiStaticText",			i3UIStaticText::static_meta()	},
		{	"uiCheckBox",			i3UICheckBox::static_meta()	},
		{	"uiComboBox",			i3UIComboBox::static_meta()	},
		{	"uiEditBox",			i3UIEditBox::static_meta()	},
		{	"uiHBar",				i3UIHBar::static_meta()	},
		{	"uiImageBox",			i3UIImageBox::static_meta()	},
		{	"uiImageBoxEx",			i3UIImageBoxEx::static_meta()	},
		{	"uiListBox",			i3UIListBox::static_meta()	},
		{	"uiProgressBar",		i3UIProgressBar::static_meta()	},
		{	"uiProgressBarImage",	i3UIProgressBarImage::static_meta()	},
		{	"uiProgressBarSplit",	i3UIProgressBarSplit::static_meta()	},
		{	"uiRadio",				i3UIRadio::static_meta()	},
		{	"uiScrollBar",			i3UIScrollBar::static_meta()	},
		{	"uiSocketBox",			i3UISocketBox::static_meta()	},
		{	"uiSpin",				i3UISpin::static_meta()	},
		{	"uiTab",				i3UITab::static_meta()	},
		{	"uiToolTip",			i3UIToolTip::static_meta()	},
		{	"uiListView_Box",		i3UIListView_Box::static_meta()	},
		{	"uiUser",				i3UIUserControl::static_meta()	},
		{	"uiButtonComposed3",	i3UIButtonComposed3::static_meta()	},
		{	"uiButtonComboBox",		i3UIButtonComboBox::static_meta()	},
		{   "uiComboBox_NoEditBox", i3UIComboBox_NoEditBox::static_meta() },
		{	NULL,	NULL	}
	};

	//일단 pCtrl이 lua에서 정의된 클래스인지 확인
	i3ClassMeta* pMeta = pCtrl->meta();
	bool bDefined = false;

	for( INT32 i=0; s_info[i].m_pMeta; i++)
	{
		if( pMeta->same_of( s_info[i].m_pMeta))
		{
			bDefined = true;
			break;
		}
	}

	if( bDefined == false)
		return;
	
	char szClass[MAX_PATH];
		
	for( INT32 i=0; s_info[i].m_pMeta; i++)
	{
		if( pMeta->kind_of( s_info[i].m_pMeta))
		{
			i3::string_ncopy_nullpad( szClass, s_info[i].m_pszLuaclass, MAX_PATH);	
			
			for( size_t j=0; j< s_LuaMemberTable.size(); j++)
			{
				LUAMEMBER_INFO* pInfo = s_LuaMemberTable[j];

				if( i3::generic_is_iequal( pInfo->m_szClass, szClass) )
				{
					//char pBuf[MAX_PATH];
					//i3::snprintf( pBuf, MAX_PATH, "%s%c%s", pInfo->m_szClass, pInfo->m_cType, pInfo->m_szName);
					//pOut->Add( pBuf);

					Out.push_back( pInfo->m_szName);
				}
			}
		}
	}

	//----------------------------------------------------------------------------------
	//Lua에서 읽어오려고 했으나 class들이 table이 아니라 userdata인 관계로..
	////i3Lua
	//i3UIManager* pManager = i3UI::getManager();
	//I3ASSERT( pManager != NULL);

	//lua_State* L = pManager->getLuaContext();
	//bool bClose = false;

	//if( L == NULL)
	//{
	//	bClose = true;
	//	pManager->OpenLuaContext();
	//	L = pManager->getLuaContext();

	//	I3ASSERT( L != NULL);
	//}
	//
	//
	////// ref: http://www.lua.org/manual/5.1/manual.html#lua_next
	////// ref: http://stackoverflow.com/questions/966568/what-is-the-c-interface-to-lua-for-accessing-a-tables-key-value-pairs
	////// table traversal => for k,v in pairs(t) do body end

	////// Test
	////i3Lua::GetField(L, LUA_GLOBALSINDEX, "uiListView_Box");	
	//////i3Lua::GetField(L, LUA_GLOBALSINDEX, "global_bUpdateUI");	
	//////i3Lua::GetField(L, LUA_GLOBALSINDEX, "controltable");	
	//////i3Lua::GetField(L, LUA_GLOBALSINDEX, "lv_0");
	//////i3Lua::GetField(L, LUA_GLOBALSINDEX, "imgList_Rank");
	/////* table is in the stack at index 't' */
	////i3Lua::PushNil(L);  /* first key */
	////
	//////INT32 t = i3Lua::StackGetTop( L);
	////
	////INT32 t = 1;

	/////*for( INT32 i=1; i <= t; i++)
	////{
	////	INT32 type = lua_type( L, i);
	////	switch( type)
	////	{
	////	case LUA_TSTRING:
	////		break;
	////	case LUA_TBOOLEAN:
	////		break;
	////	case LUA_TNUMBER:
	////		break;
	////	case LUA_TTABLE:
	////		break;
	////	case LUA_TFUNCTION:
	////		break;
	////	default:
	////		break;
	////	}
	////}*/
	////	
	////while( i3Lua::StackNext(L, t) != 0) 
	////{
	////   /* uses 'key' (at index -2) and 'value' (at index -1) */
	////   I3TRACE("%s - %s\n",
	////		  lua_typename(L, lua_type(L, -2)),
	////		  lua_typename(L, lua_type(L, -1)));
	////   /* removes 'value'; keeps 'key' for next iteration */
	////   lua_pop(L, 1);
	////}

	//if( bClose)
	//	pManager->CloseLuaContext();
}

void ScriptManager::GetLuaMemberDescription( const char* pszMemberFunction, char* pOut)
{
	for( size_t i=0; i< s_LuaMemberTable.size(); i++)
	{
		LUAMEMBER_INFO* pInfo = s_LuaMemberTable[i];

		if( i3::generic_is_iequal( pInfo->m_szName, pszMemberFunction) )
		{
			i3::snprintf( pOut, MAX_PATH, "%s%c%s(%s)", pInfo->m_szClass, pInfo->m_cType, pInfo->m_szName, pInfo->m_szParams);
			return;
		}
	}

	i3::string_ncopy_nullpad( pOut, "Not Found", MAX_PATH);
}

void ScriptManager::GetLuaMemberParams( const char* pszMemberFunction, char* pOut)
{
	for( size_t i=0; i< s_LuaMemberTable.size(); i++)
	{
		LUAMEMBER_INFO* pInfo = s_LuaMemberTable[i];
		if( i3::generic_is_iequal( pInfo->m_szName, pszMemberFunction) )
		{
			i3::string_ncopy_nullpad( pOut, pInfo->m_szParams, MAX_PATH);
			return;
		}
	}

	i3::string_ncopy_nullpad( pOut, "Not Found", MAX_PATH);
}	

void ScriptManager::GetLuaClassFiles( const char* pszRootPath, i3::vector<i3::rc_string>& Out, bool bRecursive)
{
	//http://www.codeguru.com/forum/showthread.php?t=312461

	CString strToFind;
	strToFind.Format(_T("%s\\*.lua"), pszRootPath);

	CFileFind ff;
	BOOL bFound = ff.FindFile( strToFind);	//Call this member function to open a file search.

	while( bFound)
	{
		bFound = ff.FindNextFile();
		
		if( ff.IsDirectory() && !ff.IsDots())
		{
			if(true == bRecursive)
			{
				CString strRootPath = ff.GetFilePath();
				GetLuaClassFiles( strRootPath, Out, bRecursive);
			}
		}
		else if(!ff.IsDots() && !ff.IsDirectory())
		{
			CString strFilePath = ff.GetFilePath();
			Out.push_back(	i3::rc_string( strFilePath.GetString(), strFilePath.GetLength() ) );	// (char*)strFilePath.GetString());
		}
	}
}


bool ScriptManager::BuildLuaMemberTable( void)
{
	bool bResult = false;
	
	// Open XML
	i3XMLDocument* pDoc = new TiXmlDocument( s_szLuaIntellisense);

	if( pDoc->LoadFile() == false)
	{
		I3ERRLOG( i3Error::_READFAIL);
		goto ExitPart;
	}

	i3XMLElement* pRoot = (i3XMLElement *) pDoc->LastChild();
	
	if( pRoot == NULL)
	{
		I3ERRLOG( i3Error::_INVALIDFORMAT);
		goto ExitPart;
	}
	
	if( i3::generic_is_iequal( pRoot->Value(), "i3UIEditor_LuaIntellisense") == false)
	{
		I3ERRLOG( i3Error::_INVALIDFORMAT);
		goto ExitPart;
	}

	//Reset contents
	for( size_t i=s_LuaMemberTable.size(); i>0 ; i--)
	{
		LUAMEMBER_INFO* pInfo = s_LuaMemberTable[i - 1];
		
		delete pInfo;
		pInfo = NULL;

//		s_LuaMemberTable.Delete( i);
	}
	s_LuaMemberTable.clear();


	//Traverse
	i3XMLElement* pClassNode = (i3XMLElement*)pRoot->FirstChild();

	while( pClassNode != NULL)
	{
		i3XMLElement* pSigNode = (i3XMLElement*)pClassNode->FirstChild();

		while( pSigNode != NULL)
		{
			LUAMEMBER_INFO* pInfo = new LUAMEMBER_INFO();

			i3::string_ncopy_nullpad( pInfo->m_szClass, pSigNode->getAttr( "class"), MAX_PATH);
			
			if( i3::generic_is_nequal( pSigNode->getAttr( "type"), ":", 1) )
				pInfo->m_cType = ':';
			else if( i3::generic_is_nequal( pSigNode->getAttr( "type"), ".", 1) )
				pInfo->m_cType = '.';

			i3::string_ncopy_nullpad( pInfo->m_szName, pSigNode->getAttr( "name"), MAX_PATH);
			i3::string_ncopy_nullpad( pInfo->m_szParams, pSigNode->getAttr( "params"), MAX_PATH);

			s_LuaMemberTable.push_back( pInfo);
			
			pSigNode = (i3XMLElement*)pSigNode->NextSibling();
		}
		
		pClassNode = (i3XMLElement*)pClassNode->NextSibling();
	}
	
	bResult = true;

ExitPart:
	if( pDoc != NULL)
	{
		delete pDoc;
		pDoc = NULL;
	}

	return bResult;
}

void ScriptManager::ReleaseLuaMemberTable( void)
{
	
	LUAMEMBER_INFO * pInfo;

	for(size_t i = 0; i < s_LuaMemberTable.size(); i++)
	{
		pInfo =s_LuaMemberTable[ i];
		delete pInfo;
	}

	s_LuaMemberTable.clear();
}

const i3::vector<::LUAMEMBER_INFO*>& ScriptManager::GetLuaMemberTable( void) 
{
	return s_LuaMemberTable;
}

//void ScriptManager::BuildBlockTable( bool bScreen)
//{
//	CFile Src;
//	BOOL bRes = FALSE;
//
//	if( bScreen)
//		bRes = Src.Open( s_szScrScript, CFile::modeRead);
//	else
//		bRes = Src.Open( s_szLibScript, CFile::modeRead);
//		
//	if( !bRes)	return;
//	
//	ULONGLONG SrcLen = Src.GetLength();
//
//	INT32 nBufSize = (INT32)SrcLen + 1;		
//	char* pBuf = new char[nBufSize];				
//
//	Src.Read( pBuf, nBufSize);		
//	pBuf[nBufSize-1] = 0;
//
//	Src.Close();
//
//	// Build
//	_BuildBlockTable( bScreen, pBuf);
//	
//	delete[] pBuf;
//
//}


void ScriptManager::_MakeBlockTable( char* pBuf, i3::vector<LUABLOCK_INFO*>& Table)		// const char*? 
{
	char* pCur = pBuf;

	while( *pCur != 0)
	{
		// -------- Find a block -------- 
		// Start of Block
		char* pFound = strstr( pCur, BLOCKTAG_START);
		
		if( pFound == NULL)	
			break;	// pszBlkStart does not appear in pCur

		INT32 nStart = (INT32)(pFound - pBuf);

		pFound += BLOCKLEN_START;	

		char szNameS[MAX_PATH];
		INT32 i=0;
		
		while( *pFound != 0 && *pFound != ']')
		{
			szNameS[i++] = *(pFound++);
		}

		if( *pFound == 0)
			break;

		szNameS[i] = 0;
		
		// End of Block
		pFound = strstr( pFound, BLOCKTAG_END);

		if( pFound == NULL)
			break;	// incomplete block
		
		pFound += BLOCKLEN_END;	

		char szNameE[MAX_PATH];
		INT32 j=0; 

		while( *pFound != 0 && *pFound != ']')
		{
			szNameE[j++] = *(pFound++);
		}

		if( *pFound == 0)
			break;

		szNameE[j] = 0;

		INT32 nEnd = (INT32)(pFound - pBuf) + 1;	// ']'

		if( i3::generic_is_iequal( szNameS, szNameE) == false)
			break;

		if( nStart < 0 || nEnd < 0 || nEnd < nStart)
			break;

		// -------- Add to list -------- 
		LUABLOCK_INFO* pInfo = new LUABLOCK_INFO();

		i3::string_ncopy_nullpad( pInfo->m_szBlockName, szNameS, MAX_PATH);
		pInfo->m_nStart = nStart;
		pInfo->m_nEnd = nEnd;

		I3TRACE("%s, %d, %d\n", pInfo->m_szBlockName, pInfo->m_nStart, pInfo->m_nEnd);

		Table.push_back( pInfo);

		pCur = pFound;
	}
}

bool ScriptManager::isInsideBlock( char* pBuf, INT32 nCurPos, char* pOut)
{
	i3::vector<LUABLOCK_INFO*> list;

	_MakeBlockTable( pBuf, list);

	for( size_t i=0; i< list.size(); i++)
	{
		LUABLOCK_INFO* pInfo = list[i];
		
		if( nCurPos >= pInfo->m_nStart && nCurPos < pInfo->m_nEnd)
		{
			i3::string_ncopy_nullpad( pOut, pInfo->m_szBlockName, MAX_PATH);
			return true;
		}
	}

	pOut[0] = 0;
	return false;
}

//pBuf 안에 pszBlockname의 블록이 있는지 확인
INT32 ScriptManager::HasBlock( char* pBuf, const char* pszBlockname)
{
	i3::vector<LUABLOCK_INFO*> list;

	_MakeBlockTable( pBuf, list);
	
	for( size_t i=0; i< list.size(); i++)
	{
		LUABLOCK_INFO* pInfo = list[i];
		
		if( i3::generic_is_iequal( pInfo->m_szBlockName, pszBlockname) )
		{
			return pInfo->m_nStart;
		}
	}

	return -1;
}

INT32 ScriptManager::HasEventHandler( CScriptWnd* pWnd, const char* pszHandlerName)
{
	INT32 nLen = pWnd->getTextLen();
	char* pBuf = new char[nLen+1];		
	pWnd->getTextBuf( pBuf, nLen+1);

	char szBlock[MAX_PATH];
	pWnd->getBlockname( szBlock, MAX_PATH);

	char szStringToFind[MAX_PATH];

	i3::snprintf( szStringToFind, sizeof( szStringToFind), "%s:%s", szBlock, pszHandlerName);

	char* pFind = strstr( pBuf, szStringToFind);

	if( pFind == NULL)
		return -1;

	delete[] pBuf;

	return (INT32)( pFind - pBuf);
}

bool ScriptManager::InsertUseBlock( CScriptWnd* pWnd, const char* pszBlockname)
{
	INT32 nLen = pWnd->getTextLen();
	char* pBuf = new char[nLen+1];		//너무 빨리 할당하는거아닌가.. 쓰기 직전에 할당. 쓰자마자 해제. 
	pWnd->getTextBuf( pBuf, nLen+1);

	char szBlock[MAX_PATH];

	if( pszBlockname != NULL)
		i3::string_ncopy_nullpad( szBlock, pszBlockname, MAX_PATH);
	else
		pWnd->getBlockname( szBlock, sizeof( szBlock));

	INT32 i = 0;
	INT32 nStep = 0;
	char pToken[MAX_PATH] = {0,};
	bool bUse = false;
	while( 	_GetNextLuaToken( pBuf, i, pToken))
	{
		switch( nStep)
		{
		case 0:
			{
				if( i3::generic_is_iequal( pToken, "Use" ) )
					nStep++;
				else
					nStep=0;
			}
			break;
		case 1:
			{
				if( i3::generic_is_equal( pToken, "(") )
					nStep++;
				else 
					nStep=0;
			}
			break;
		case 2:
			{
				char quotedstring[MAX_PATH] = {0,};
				i3::snprintf( quotedstring, MAX_PATH, "\"%s\"", szBlock);
				if( i3::generic_is_iequal( pToken, quotedstring) )
					nStep++;
				else
					nStep=0;
			}
			break;
		case 3:
			{
				if( i3::generic_is_equal( pToken, ")") )
					bUse = true;
				else
					nStep=0;
			}
			break;
		default:
			break;
		}

		if( bUse)
			break;
	}

	if( !bUse)
	{
		char szUse[MAX_PATH] = {0,};
		i3::snprintf( szUse, sizeof( szUse), "Use(\"%s\")\n\n", szBlock);
		pWnd->InsertText( szUse, 0);

		return true;
	}

	delete[] pBuf;

	return false;
}

void ScriptManager::InsertEventHandler( const char* pszBlockname, bool bScreen, CScriptPane* pInsertTo)
{
	// Event Handler들을 로딩하기 위한 준비
	if( bScreen)
	{
		if( s_pScene == NULL || s_szScrScript[0] == 0)
		{	
			I3PRINTLOG(I3LOG_NOTICE, "스크립트 또는 스크린이 준비되지 않았습니다.");
			//MessageBox( NULL, "스크립트 또는 스크린이 준비되지 않았습니다.", MB_OK);
			return;
		}
	}
	else
	{
		if( s_pLibrary == NULL || s_szLibScript[0] == 0)
		{
			I3PRINTLOG(I3LOG_NOTICE, "스크립트 또는 라이브러리가 준비되지 않았습니다.");
			//MessageBox( NULL, "스크립트 또는 라이브러리가 준비되지 않았습니다.", MB_OK);
			return;
		}
	}

	// Lua Member Function들을 로딩하기 위한 준비
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if( pMainFrm->IsLuaChanged())
	{
		//파일이 변경되었다면 Intellisense를 업데이트 한다. 
		BuildIntellisense();
		pMainFrm->SetLuaChanged( false);
	}

	//Dialog settings
	CDlg_InsertEventHandler dlg;
	dlg.SetScreen( bScreen);

	if( pszBlockname[0] != 0)	dlg.SetBlock( pszBlockname);

	//Open dialog
	if( dlg.DoModal() == IDOK)
	{
		char szBlock[MAX_PATH];
		dlg.GetBlockName( szBlock, MAX_PATH);

		// 코드를 삽입할 ScriptPane을 구한다. 
		CScriptPane* pPane = (pInsertTo != NULL) ? pInsertTo : pMainFrm->OnEditScript( szBlock, false, bScreen);
		CScriptWnd* pWnd = pPane->getScriptWnd();

		// check if Use("ctrlname") exists
		InsertUseBlock( pWnd, szBlock);

		if( dlg.isEventHandler())	//event handler
		{
			// check if event handler exists
			char szHandler[MAX_PATH];
			dlg.GetFunctionName( szHandler, MAX_PATH);

			INT32 nHandlerPos = HasEventHandler( pWnd, szHandler);

			if( nHandlerPos > -1)
			{
				pWnd->SetCurrentPosTo( nHandlerPos);
			}
			else
			{
				char szGenCode[MAX_PATH];
				dlg.GetGeneratedCodeBlock( szGenCode, MAX_PATH);
				pWnd->AppendText( szGenCode);
				pWnd->SetCurrentPosTo( pWnd->getTextLen());
			}
		}
		else	//member function
		{
			char szGenCode[MAX_PATH];
			dlg.GetGeneratedCodeBlock( szGenCode, MAX_PATH);

			if( pInsertTo != NULL)
			{
				pWnd->InsertText( szGenCode, pWnd->getCurrentPos());
			}
			else
			{
				pWnd->AppendText( szGenCode);
			}
		}

		::SetFocus( pWnd->m_hWnd);
	}
}

