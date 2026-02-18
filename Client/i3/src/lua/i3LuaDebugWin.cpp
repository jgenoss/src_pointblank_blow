// WinConsole.cpp: implementation of the i3LuaDebugWin class.
//
//////////////////////////////////////////////////////////////////////

#include "i3LuaPCH.h"
#include "i3LuaDebugWin.h"

#define		DEBUGWIN_CAPTION		"LUA DebugWin"

#define		FONT_HEIGHT		9

LuaFuncReg DebugGlue[];

i3LuaDebugWin * g_pLuaDebugWin = nullptr;

volatile bool i3LuaDebugWin::m_bWinIsActive = true;
volatile HWND i3LuaDebugWin::m_hWnd = nullptr;
//volatile HWND i3LuaDebugWin::m_hEditControl = nullptr;

char i3LuaDebugWin::m_CommandBuffer[ COMMAND_BUFFER_SIZE];

//WNDPROC lpfnInputEdit;  // Storage for subclassed edit control 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//I3_CLASS_INSTANCE( i3LuaDebugWin, i3NamedElement);
I3_CLASS_INSTANCE( i3LuaDebugWin);

/*
HWND i3LuaDebugWin::StartConsole( HINSTANCE hInstance, LuaState * pScriptContext)
{
	if( !g_pLuaDebugWin)
		g_pLuaDebugWin = i3LuaDebugWin::new_object();

	if(!g_pLuaDebugWin->m_hWnd)
		g_pLuaDebugWin->Init( hInstance);

	g_pLuaDebugWin->m_pLuaState = pScriptContext;

	// init the glue functions required for the debug window
	for(int i=0; DebugGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( g_pLuaDebugWin->m_pLuaState, DebugGlue[i].name, DebugGlue[i].func);
	}

	i3Lua::DoString( g_pLuaDebugWin->m_pLuaState, "print( string.format( \"Debug starting from %s!\", _VERSION))");

	return ( g_pLuaDebugWin->m_hWnd);
}

void i3LuaDebugWin::StopConsole( void)
{
	I3_SAFE_RELEASE( g_pLuaDebugWin);
}*/

i3LuaDebugWin::i3LuaDebugWin()
{
	m_hWnd = nullptr;
	m_pLuaState = nullptr;
	memset(m_CommandBuffer, 0, COMMAND_BUFFER_SIZE);
	m_ScrollyPos = 0;
	m_textAreaHeight = 0;
	m_hInstance = nullptr;
}

i3LuaDebugWin::~i3LuaDebugWin()
{
	if(m_hWnd != nullptr)
	{
		CloseWindow(m_hWnd);		
	}	

	SendMessage( m_hWnd, WM_DESTROY, 0, 0);
}

void i3LuaDebugWin::AdjustScrollBar(void)
{
	SCROLLINFO si;

	si.cbSize = sizeof(si); 
	si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin   = 0; 
	si.nMax   = (int) m_stringList.size(); 
	si.nPage  = m_textAreaHeight; 
	si.nPos   = (int) m_stringList.size() - m_ScrollyPos; 
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}

void i3LuaDebugWin::ResizeControls(void)
{
	RECT r;

	GetClientRect(m_hWnd, &r);
	m_textAreaHeight = (r.bottom - r.top) / FONT_HEIGHT;

	//SetWindowPos(m_hEditControl, HWND_TOP, r.left + 2, r.bottom - 18, r.right - r.left - 4, FONT_HEIGHT, SWP_NOZORDER);

	AdjustScrollBar();
	::InvalidateRect(m_hWnd, NULL, TRUE);
}

void i3LuaDebugWin::Paint(HDC hDC)
{
	if( !g_pLuaDebugWin) return;

	SetTextColor(hDC, RGB(255,255,255));  
	SetBkColor(hDC, RGB(0,0,0));

	RECT r;

	GetClientRect(m_hWnd, &r);

	int x = 2;  
	//int y = r.bottom - 40;
	int y = r.bottom - FONT_HEIGHT;

	std::list<std::string>::iterator it = m_stringList.begin();
	int skip = m_ScrollyPos;
	while(skip)
	{
		++it;
		--skip;
	}

	while(it != m_stringList.end())
	{
		TextOut(hDC, x, y, (*it).c_str(), strlen((*it).c_str()));
		y -= FONT_HEIGHT; 
		++it;
	}
}


LRESULT WINAPI i3LuaDebugWin::MsgProc( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
    {
		case WM_ACTIVATEAPP:
			m_bWinIsActive = (wParam != 0);
			return 0L;

		case WM_ACTIVATE:
			// Pause if minimized or not the top window
			m_bWinIsActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
			return 0L;

		case WM_DESTROY:
			m_bWinIsActive = false;
			m_hWnd = nullptr;
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_pLuaDebugWin->Paint(hdc);
			EndPaint(hWnd, &ps);
			break;

		case WM_CHAR:
			break;

		case WM_LBUTTONDOWN:
			break;

		case WM_RBUTTONDOWN:
			break;

		case WM_LBUTTONUP:
			break;

		case WM_RBUTTONUP:
			break;

		case WM_MOUSEMOVE:
			break;

		case WM_COMMAND:
			break;

		case WM_KEYDOWN:
			break;

		case WM_SIZING:
		case WM_SIZE:
			g_pLuaDebugWin->ResizeControls();
			break;

		case WM_SETCURSOR:
			::SetCursor(LoadCursor(NULL, IDC_ARROW));
			::ShowCursor(TRUE);
			break;

		case WM_VSCROLL: 
		{		
			switch( LOWORD (wParam)) 
			{ 
				// User clicked the shaft above the scroll box. 
	 
				case SB_PAGEUP: 
 					g_pLuaDebugWin->m_ScrollyPos = min((int) (g_pLuaDebugWin->m_ScrollyPos+g_pLuaDebugWin->m_textAreaHeight), (int) (g_pLuaDebugWin->m_stringList.size() - g_pLuaDebugWin->m_textAreaHeight)+1);
					//yInc = min(-1, -yClient / yChar); 
					break; 
	 
				// User clicked the shaft below the scroll box. 
	 
				case SB_PAGEDOWN: 
					// yInc = max(1, yClient / yChar); 
					break; 
	 
				// User clicked the top arrow. 
	 
				case SB_LINEUP: 
 					g_pLuaDebugWin->m_ScrollyPos = min(g_pLuaDebugWin->m_ScrollyPos+1, (int) (g_pLuaDebugWin->m_stringList.size() - g_pLuaDebugWin->m_textAreaHeight)+1);
					break; 
	 
				// User clicked the bottom arrow. 
	 
				case SB_LINEDOWN: 
					//yInc = 1; 
 					g_pLuaDebugWin->m_ScrollyPos = max(g_pLuaDebugWin->m_ScrollyPos-1, 0);
					break; 
	 
				// User dragged the scroll box. 
				case SB_THUMBPOSITION:
					
				case SB_THUMBTRACK: 
					g_pLuaDebugWin->m_ScrollyPos = (int) (g_pLuaDebugWin->m_stringList.size() - g_pLuaDebugWin->m_textAreaHeight)+1 - HIWORD(wParam);
					//yInc = HIWORD(wParam) - yPos; 
					break; 
	 
				default: 
					//yInc = 0;
					break;
	 
			} 
			{
				SCROLLINFO si;
				si.cbSize = sizeof(si); 
				si.fMask  = SIF_POS; 
				si.nPos   = (int) g_pLuaDebugWin->m_stringList.size() - g_pLuaDebugWin->m_textAreaHeight - g_pLuaDebugWin->m_ScrollyPos; 
				::SetScrollInfo(hWnd, SB_VERT, &si, TRUE); 
			}
			::InvalidateRect(m_hWnd, NULL, TRUE);
		}
		break;

		case WM_USER:
			// command ready from edit control
			// string should be in m_CommandBuffer
			Write(m_CommandBuffer);
			if (0 != i3Lua::LoadBuffer( g_pLuaDebugWin->m_pLuaState, m_CommandBuffer, strlen(m_CommandBuffer), nullptr))
			{
				Write("Error loading Command\n");
			}
			if (0 != i3Lua::PCall( g_pLuaDebugWin->m_pLuaState, 0, LUA_MULTRET, 0))
			{
				Write("Error in Command\n");
				Write( i3Lua::CheckStringArg( g_pLuaDebugWin->m_pLuaState, -1));
			}
			// clear buffer when done processing
			memset(m_CommandBuffer, 0, COMMAND_BUFFER_SIZE);
			break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


void i3LuaDebugWin::Write(const char *pString)
{
	if(g_pLuaDebugWin && m_hWnd)
	{
		//remove any linefeed chars (look goofy in log)
		char *buf = new char[strlen(pString)+1];

		int indx=0;
		for(int i=0; i<(int) strlen(pString); i++)
		{
			if(pString[i] != 10)
			{
				buf[indx++] = pString[i];
			}
			else
			{
				buf[indx] = 0;
				g_pLuaDebugWin->m_stringList.push_front(buf);
				indx = 0;
			}


		}
		if(indx > 0)
		{
			buf[indx] = 0;
			g_pLuaDebugWin->m_stringList.push_front(buf);
		}
		::InvalidateRect(m_hWnd, NULL, TRUE);
		delete [] buf;

		g_pLuaDebugWin->AdjustScrollBar();
	}
}

/*
LRESULT CALLBACK i3LuaDebugWin::SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{     
	switch(message)     
	{         
	case WM_CHAR:              // Found a RETURN keystroke!             
		if ((TCHAR) wParam == VK_RETURN)             
		{ 
			// get the command string
			long lSizeofString;                  
			// Get the size of the string
			lSizeofString = (long) SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);

            // Get the string                 
			SendMessage(hWnd, WM_GETTEXT, lSizeofString + 1, (LPARAM) m_CommandBuffer); 

			// send message to parent that command was entered
			SendMessage(m_hWnd, WM_USER, 0, lSizeofString);

			// clear the edit string
			SendMessage(hWnd, WM_SETTEXT, 0, (long) ""); 
			return 1;
		}
	}
    return CallWindowProc(lpfnInputEdit, hWnd, message, wParam, lParam); 
}*/

void i3LuaDebugWin::Init( HINSTANCE hInstance, DebugWinInitInfo * pInfo)
{
 	m_hInstance = hInstance;
	m_ScrollyPos = 0;
   // create application handler and link to our WindowProc
    WNDCLASS wc;

	const char * pClassName = i3LuaDebugWin::static_meta()->class_name().c_str();

    // Set up and register window class
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC) MsgProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(DWORD);
    wc.hInstance = m_hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr; 
	wc.lpszClassName = pClassName;
#if 0
    if (!RegisterClass(&wc))
        ThrowError();
#else
	RegisterClass(&wc);
#endif

	INT32 x = 0, y = 0, w = 640, h = 480;

	if( pInfo)
	{
		x = pInfo->_x;
		y = pInfo->_y;
		w = pInfo->_width;
		h = pInfo->_height;
	}

    m_hWnd = CreateWindow(pClassName, // class
                        DEBUGWIN_CAPTION, // caption
                        WS_OVERLAPPEDWINDOW, // style 
                        x, // left
                        y, // top
                        w, // width
                        h, // height
                        NULL, // parent window
                        NULL, // menu 
                        m_hInstance, // instance
                        NULL); // parms

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);
    SetFocus(m_hWnd);

	/*m_hEditControl = CreateWindow("EDIT", // class
                        "", // caption
                        ES_LEFT | WS_CHILD, // style 
                        2, // left
                        404, // top
                        228, // width
                        16, // height
                        m_hWnd, // parent window
                        (HMENU)0xa7, // menu 
                        m_hInstance, // instance
                        NULL); // parms

    ShowWindow(m_hEditControl, SW_SHOW);
    UpdateWindow(m_hEditControl);
    SetFocus(m_hEditControl);*/
	m_ScrollyPos = 0;

	//lpfnInputEdit = (WNDPROC) SetWindowLong(m_hEditControl, GWL_WNDPROC, (long) SubclassInputEditProc); 
	g_pLuaDebugWin->ResizeControls();


}
/*
static INT32 Debug_Print(lua_State *L) 
{
#ifdef I3_DEBUG
  INT32 n = i3Lua::StackGetTop(L);  // number of arguments
  INT32 i;
  i3Lua::GetGlobal(L, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
	i3Lua::StackPushValue(L, -1);  // function to be called
    i3Lua::StackPushValue(L, i);   // value to print 
	i3Lua::Call(L, 1, 1);
	s = i3Lua::StackToString(L, -1);  // get result 
    if (s == nullptr)
		return i3Lua::SetErrorMsg(L, "`tostring' must return a string to `print'");
    if (i>1) i3LuaDebugWin::Write("\t");
    i3LuaDebugWin::Write(s);
	i3Lua::StackPop(L, 1);  // pop result 
  }
  i3LuaDebugWin::Write("\n");
#endif
  return 0;
}

LuaFuncReg DebugGlue[] = 
{
  {"Print",					Debug_Print},
  {"print",					Debug_Print},

  {nullptr, nullptr}
};
*/