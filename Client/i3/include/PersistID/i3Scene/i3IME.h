#if !defined( __I3IME_H)
#define __I3IME_H

typedef struct Ime_Element
{
	HWND _hWnd;
	HINSTANCE _hInst;
	INT32 _nTextLimitCount;
	UINT32 _Flag;
	UINT32 _StyleIME;
		
	Ime_Element(void) : _hWnd(NULL), _hInst(NULL), _nTextLimitCount(0), _Flag(0), _StyleIME(0) {}

}CREATE_IME_ELEMENT;

class I3_EXPORT_SCENE i3IME : public i3ElementBase
{
	I3_CLASS_DEFINE( i3IME);

public:
	HWND				m_hParentHwnd;
	HWND				m_hWnd;
	char*				m_pTextBuf;
	i3InputIME*			m_pInputIME;
	INT32				m_nTextMaxCount;
	BOOL				m_bFocus;
	BOOL				m_bEnableMultiLine;

	EDIT_STATE			m_nState;

public:
	i3IME(void* pVoid = NULL);
	virtual ~i3IME(void);

	void				CreateIME( CREATE_IME_ELEMENT* pStruct);
	
	BOOL				SetFocus( BOOL bFlag);
	BOOL				getFocus( void)								{ return m_bFocus;						}
	void				KillFocus();
	void				SetIMEMode( IME_MODE mode);
	IME_MODE			GetIMEMode(void);
	void				SetIMEEnable( BOOL bFlag);
	void				SetInput( char* pszText);
	char *				getIMETextBuf(void);
	BOOL				isIMEActive(void)							{ return m_pInputIME->IsImeActive();	}	
	void				setImeProcFlag( UINT32 flag)				{ m_pInputIME->SetProcFlag( flag);		}	
	char *				getText(void)								{ return m_pTextBuf;					}
	INT32				getIMETextBufCount(void)					{ return m_nTextMaxCount;				}
	i3InputIME *		getIME(void)								{ return m_pInputIME;					}
	EDIT_STATE			getIMEState()								{ return m_nState;						}
	UINT32				getIMEStyle(void)							{ return m_pInputIME->GetImeStyle();	}
	void				setEnterFeedCount( INT32 cnt)				{ m_pInputIME->setEnterFeedCount( cnt); }

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
