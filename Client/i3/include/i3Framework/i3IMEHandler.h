#if !defined( __I3IME_HANDLER_H_)
#define __I3IME_HANDLER_H_

typedef struct Ime_CreateContext
{
	HWND _hWnd;
	HINSTANCE _hInst;
	INT32 _nTextLimitCount;
	UINT32 _StyleIME;
		
	Ime_CreateContext(void) : _hWnd(NULL), _hInst(NULL), _nTextLimitCount(0), _StyleIME(0) {}

}IME_CREATE_CONTEXT;

class I3_EXPORT_FRAMEWORK i3IMEHandler : public i3ElementBase
{
	I3_CLASS_DEFINE( i3IMEHandler);

public:
	i3EventReceiver	*	m_pOwner;
	HWND				m_hParentHwnd;
	HWND				m_hWnd;
	WCHAR*				m_pTextBuf;
	i3InputMethodEditor*m_pIME;
	INT32				m_nTextMaxCount;
	BOOL				m_bFocus;
	BOOL				m_bEnableMultiLine;

	EDIT_STATE			m_nState;

protected:
	void				_ClearBuffer(void);

public:
	i3IMEHandler(void* pVoid = NULL);
	virtual ~i3IMEHandler(void);

	void				CreateIME( IME_CREATE_CONTEXT* pStruct);
	
	void				setOwner( i3EventReceiver * pRecv)			{ m_pOwner = pRecv;}
	i3EventReceiver	*	getOwner( void)								{ return m_pOwner;}

	BOOL				SetFocus( BOOL bFlag);
	BOOL				getFocus( void)								{ return m_bFocus;						}
	void				KillFocus();
	void				SetIMEMode( IME_MODE mode);
	IME_MODE			GetIMEMode(void);
	void				SetIMEEnable( BOOL bFlag);
	void				SetInput( const WCHAR* pszText);
	const WCHAR *		getIMETextBuf(void);
	BOOL				isIMEActive(void)							{ return m_pIME->IsImeActive();	}	
	const WCHAR *		getText(void)								{ return m_pTextBuf;					}
	INT32				getIMETextBufCount(void)					{ return m_nTextMaxCount;				}
	i3InputMethodEditor*getIME(void)								{ return m_pIME;					}
	EDIT_STATE			getIMEState()								{ return m_nState;						}
	UINT32				getIMEStyle(void)							{ return m_pIME->GetStyle();	}
	void				setEnterFeedCount( INT32 cnt)				{ m_pIME->setEnterFeedCount( cnt); }
	BOOL				isEnableMultiLine(void)						{ return m_bEnableMultiLine;			}
	void				EnableMultiLine( BOOL bEnable)				{ m_bEnableMultiLine = bEnable;			}

	void				DebugDump(void);

	//	EDIT_ENTER 처리를 완료한후 호출하면 IME 버퍼가 clear됩니다.
	void				InputDone(void);

	// Window control Dummy로 사용
	static LRESULT ControlDummyProc( HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);

	// Call Back으로 사용
	static void IMEProc( i3InputMethodEditor * pIME, EDIT_STATE state, void * pParam);
	
protected:
	 void		StateProcess(EDIT_STATE state);
};

#endif // __I3IME_H
