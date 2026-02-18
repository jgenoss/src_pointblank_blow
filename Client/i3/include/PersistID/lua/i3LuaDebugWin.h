// WinConsole.h: interface for the i3LuaDebugWin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined( I3_WINDOWS)
#error "i3LuaDebugWin only can support WIN32"
#endif

#ifndef __I3LUA_DEBUG_WIN_H__
#define __I3LUA_DEBUG_WIN_H__

#include "i3Lua.h"

#include "list"

#define COMMAND_BUFFER_SIZE			1024//4096

class i3LuaDebugWin : public i3NamedElement
{
	I3_CLASS_DEFINE( i3LuaDebugWin);

public:
	i3LuaDebugWin( void);
	virtual ~i3LuaDebugWin( void);

	//HWND StartConsole( HINSTANCE hInstance, LuaState * pScriptContext);
	//void StopConsole();
	static void Write(const char *pString);

	static LRESULT WINAPI MsgProc( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);
	//static LRESULT CALLBACK SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	BOOL	DebugReady(void)	{	return m_hWnd!= NULL;}

	void	Init( HINSTANCE hInstance, DebugWinInitInfo * pInfo);
	void	ResizeControls(void);
	void	AdjustScrollBar(void);
	void	Paint(HDC hDC);

	HWND		GetHwnd()			{	return m_hWnd;						}
	LuaState *	GetLuaState()		{	return m_pLuaState;					}
	void		SetLuaState( LuaState * pState)		{	m_pLuaState = pState;			}

private:
	static volatile BOOL	m_bWinIsActive;
	static volatile HWND	m_hWnd;
	//static volatile HWND m_hEditControl;
	static char m_CommandBuffer[ COMMAND_BUFFER_SIZE];
	
	std::list<std::string> m_stringList;
	HINSTANCE m_hInstance;

	LuaState * m_pLuaState;

	int m_ScrollyPos;
	int m_textAreaHeight;
};


extern i3LuaDebugWin * g_pLuaDebugWin;	//	디버그 윈도우 전역


#endif // __I3LUA_DEBUG_WIN_H__
