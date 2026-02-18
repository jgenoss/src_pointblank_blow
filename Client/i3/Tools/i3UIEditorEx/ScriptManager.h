#ifndef __SCRIPT_MANAGER_H__
#define __SCRIPT_MANAGER_H__

#include "ScriptPane.h"

extern const char*	BLOCKTAG_START;
extern const char*	BLOCKTAG_END;
extern const char*	BLOCKFMT_START;
extern const char*	BLOCKFMT_END;
extern const char*	BLOCKFMT;
extern const int	BLOCKLEN_START;
extern const int	BLOCKLEN_END;

struct LUAMEMBER_INFO;
struct LUABLOCK_INFO;

namespace ScriptManager
{
	bool	_CreateIntellisenseFile( i3XMLElement* pHeader, const char* pszLuafile, const char* pszClass);

	// 리스트(pLuaList)에 있는 모든 루아 파일들을 파싱하여 멤버함수들의 signature를 구하고 
	// xml에 저장한다. (BuildLuaMemberTable에서 input으로 사용)
	bool	CreateIntellisenseFile( const i3::vector<i3::rc_string>&  LuaList);

	void	BuildIntellisense( void);

	void	SetScene( i3UIScene* pScene);
	void	SetLibrary( i3UILibrary* pLibrary);
	void	SetLuaRoot( const char* pszLuaRoot);
	void	SetScrScript( const char* pszScriptForScreen);
	void	SetLibScript( const char* pszScriptForLibrary);

	// 파일(pszFile)을 열고 텍스트(pszText)의 시작(pStart)과 끝(pEnd)의 인덱스를 찾는다.
	//bool	FindText( const char* pszFile, const char* pszText, INT32* pStart = NULL, INT32* pEnd = NULL);
	
	// 파일(pszFile)을 열고 텍스트(pszText)를 특정 위치(nInsertAt)에 삽입한다.
	//void	InsertText( const char* pszFile, const char* pszText, INT32 nInsertAt);
	
	// s_pScene(UIPaneScreen의 트리와 같음)으로부터 Root의 모든 자식컨트롤을 구한다.
	void	GetLuaControls( i3::vector<i3GameNode*>& Out);

	// s_pLibrary(UIPaneTemplate의 트리와 같음)로부터 Root의 모든 자식템플릿을 구한다.
	void	GetLuaTemplates( i3::vector<i3UITemplate*>& Out);
	
	// 	s_LuaMemberTable로부터 루아에 정의된 컨트롤들의 멤버함수 정보를 얻는다. 
	void			GetLuaMemberFunctions( i3::vector<i3::rc_string>& Out, i3UIControl* pCtrl);
	void			GetLuaMemberDescription( const char* pszMemberFunction, char* pOut);
	void			GetLuaMemberParams( const char* pszMemberFunction, char* pOut);

	// i3UIDefines.h의 s_EventInfoTable로부터 event handler와 event code, event description 정보를 얻는다. 
	void			GetLuaEventHandlers( i3::vector<i3::rc_string>& Out, i3UIControl* pCtrl);
	const char*		GetLuaEventCode( const char* pszEventHandler);
	const char*		GetLuaEventDescription( const char* pszEventHandler);

	// pszRootPath 하위에 있는 모든 루아 파일들을 pOut에 담는다. 
	// uiUtil.lua, uiControl.lua 등의 클래스 정의 파일들이 들어있는 폴더를 pszRootPath로 지정해주어야 한다. 
	void			GetLuaClassFiles( const char* pszRootPath, i3::vector<i3::rc_string>& Out, bool bRecursive = true);

	
	// intellisense용으로 생성된 xml파일을 읽어들여 LUAMEMBER_INFO*를 원소로 갖는 테이블을 만든다.
	bool			BuildLuaMemberTable( void);
	void			ReleaseLuaMemberTable( void);
	const i3::vector<LUAMEMBER_INFO*>&	GetLuaMemberTable( void) ;

	
	// pTable의 원소는 LUABLOCK_INFO* 타입
	void			_MakeBlockTable( char* pBuf, i3::vector<LUABLOCK_INFO*>& Table);

	//block이 아니라 file이 열린 ScriptWnd에 대하여 
	//현재 포지션(nCurPos)이 BLOCKTAG_START와 BLOCKTAG_END의 사이에 위치하고 있는지 확인한다.
	bool			isInsideBlock( char* pBuf, INT32 nCurPos, char* pOut);

	//LuaBlockTable 안에 블락(pszBlockname)이 있는지 검사한다. 
	// 블락이 없으면 return -1, 블락이 있으면 블락의 start index를 return. 
	INT32			HasBlock( char* pBuf, const char* pszBlockname);

	//Find Event Handler
	INT32			HasEventHandler( CScriptWnd* pWnd, const char* pszHandlerName);

	//Insert Use("classname") block
	//returns false if the block already exists
	bool			InsertUseBlock( CScriptWnd* pWnd, const char* pszBlockname = NULL);

	// Insert Event Handler Dialog를 열고
	// OK 버튼이 눌리면 pInsertTo Pane에 Block과 EventHandler를 삽입한다. (이미 있다면 그 위치로 이동)
	// pInsertTo == NULL 이면 코드를 삽입할 Pane을 찾는다. 
	void			InsertEventHandler( const char* pszBlockname, bool bScreen, CScriptPane* pInsertTo = NULL);
	
};

#endif // __SCRIPT_MANAGER_H__