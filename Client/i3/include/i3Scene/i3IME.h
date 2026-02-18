#if !defined( __I3IME_H)
#define __I3IME_H

struct CREATE_IME_ELEMENT
{
	HWND _hWnd = nullptr;
	HINSTANCE _hInst = nullptr;
	INT32 _nTextLimitCount = 0;
	UINT32 _StyleIME = 0;
};

class I3_EXPORT_SCENE i3IME : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3IME, i3ElementBase);

public:
	i3EventReceiver	*	m_pOwner = nullptr;
	HWND				m_hParentHwnd = nullptr;
	HWND				m_hWnd = nullptr;

	i3::wstring			m_wstrTextBuf;			

	i3InputIME*			m_pInputIME = nullptr;
	bool				m_bFocus = false;
	bool				m_bEnableMultiLine = false;

	bool				m_bAlphaNumericOnly = false;

	EDIT_STATE			m_nState = EDIT_STATE_NONE;

public:
	i3IME(void* pVoid = nullptr) {}
	virtual ~i3IME(void);

	void				CreateIME( CREATE_IME_ELEMENT* pStruct);
	
	void				setOwner( i3EventReceiver * pRecv)			{ m_pOwner = pRecv;}
	i3EventReceiver	*	getOwner( void)								{ return m_pOwner;}

	void				SetIMEFont( HFONT hFont, LOGFONTA * lpFont);

	bool				SetFocus( bool bFlag);
	bool				getFocus( void)								{ return m_bFocus;						}
	void				KillFocus();
	void				SetIMEMode( IME_MODE mode);
	IME_MODE			GetIMEMode(void);
	void				SetIMEEnable( bool bFlag);
	void				SetInput( const i3::wliteral_range& wTextRng);

	bool				isIMEActive(void)							{ return m_pInputIME->IsImeActive();	}	
	const i3::wstring&	getText() const								{ return m_wstrTextBuf; }

	i3InputIME *		getIME(void)								{ return m_pInputIME;					}
	EDIT_STATE			getIMEState()								{ return m_nState;						}
	UINT32				getIMEStyle(void)							{ return m_pInputIME->GetImeStyle();	}
	void				setEnterFeedCount( INT32 cnt)				{ m_pInputIME->setEnterFeedCount( cnt); }
	bool				isEnableMultiLine(void)						{ return m_bEnableMultiLine;			}

	void				setAlphaNumericOnly(bool bANOnly = true);

	void				SetWindowPosition(INT32 x, INT32 y);

	void				DebugDump(void);

	//	EDIT_ENTER 처리를 완료한후 호출하면 IME 버퍼가 clear됩니다.
	void				InputDone(void);

	// Window control Dummy로 사용
	static LRESULT ControlDummyProc( HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);

	// Call Back으로 사용
	static void IMEProc( i3InputIME * pIME, EDIT_STATE state, void * pParam);
	
protected:
	 void		StateProcess(EDIT_STATE state);
};

#endif // __I3IME_H
