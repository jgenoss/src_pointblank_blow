#ifndef _MessageBoxManager_h
#define _MessageBoxManager_h

#define MAX_MESSAGE_BOX_TEXT	MAX_STRING_LENGTH
#define ERROR_POPUP_WIDTH		272
#define ERROR_POPUP_HEIGHT		175

// MBT MessageBox Flag
#define MBF_OK						0x00000001
#define MBF_OKCANCEL				0x00000002
#define MBF_WAIT					0x00000004
#define MBF_USER_OKCANCEL			0x00000008
#define MBF_WEB_OK					0x00000010
#define MBF_TEXTBOX					0x00000020
#define MBF_SYSTEM					0x00010000
#define MBF_GAME					0x00020000
#define MBF_ERROR					0x80000000

#define MBF_GAME_OK					(MBF_GAME | MBF_OK)
#define MBF_GAME_OKCANCEL			(MBF_GAME | MBF_OKCANCEL)
#define MBF_GAME_USER_OKCANCEL		(MBF_GAME | MBF_USER_OKCANCEL)
#define MBF_GAME_WAIT				(MBF_GAME | MBF_WAIT)
#define MBF_GAME_ERROR				(MBF_GAME | MBF_OK | MBF_ERROR)

#define MBF_SYSTEM_OK				(MBF_SYSTEM | MBF_OK)
#define MBF_SYSTEM_OKCANCEL			(MBF_SYSTEM | MBF_OKCANCEL)
#define MBF_SYSTEM_USER_OKCANCEL	(MBF_SYSTEM | MBF_USER_OKCANCEL)
#define MBF_SYSTEM_ERROR			(MBF_SYSTEM | MBF_OK | MBF_ERROR)
#define MBF_WEB						(MBF_SYSTEM | MBF_WEB_OK)
#define MBF_SYSTEM_BLOCKNOTICE		(MBF_SYSTEM | MBF_OK | MBF_TEXTBOX)
// MBT end

// MBR MessageBox Return
enum MBR_TYPE
{
	MBR_OK	= 0,
	MBR_CANCEL,
	MBR_USER,
	MAX_MBR
};
// MBR end

// MBIS MessageBox Input State
#define MBIS_NULL			0x00000000
#define MBIS_ENTER_DOWN		0x00020000
#define MBIS_ESC_DOWN		0x00200000
#define	MBIS_LBUTTON_PRESS	0x00000001
#define	MBIS_LBUTTON_DOWN	0x00000002
#define MBIS_LBUTTON_UP		0x00000004
// MBIS end

// MessageBox StackOffset
#define MB_STACK_OFFSET_X	8
#define MB_STACK_OFFSET_Y	30
// MessageBox StackOffset end

class UIMessageBox;
class CMessageBox;

typedef		void(*MSG_BOX_CALLBACK)(void*,INT32);
class	CMsgBoxCallBack
{
public:
	CMsgBoxCallBack(LPCTSTR _File = "", int _Line = -1, MSG_BOX_CALLBACK CallBack = nullptr, bool IfOkExit = false, bool bForcedExit = false);
	virtual	void	OnCallback(void * pThis, INT32 nParam) const;

protected:
	LPCTSTR		m_File;
	int			m_Line;
	MSG_BOX_CALLBACK	m_CallBack;
	bool		m_IfOkExit;			// 종료 (게임 서버에 접속 종료를 알리고 클라이언트 종료)
	bool		m_bForcedExit;		// 강제 종료 (네트워크 상관 없이 클라이언트 종료)
};


struct	MsgBoxOpenParam
{
	i3::rc_wstring	Msg;
	i3::rc_wstring	Caption;
	INT32	Style = 0;
	void *	pThis = nullptr;
	INT32	FontSize = 0;
	INT32	AlignX = 0, AlignY = 0;
	i3::rc_wstring	WebURL;
	SIZE32	WebScreenSize = { 0, 0 };
	CMsgBoxCallBack		CB = nullptr;
};



#define		MAKE_CALLBACK(fn)			CMsgBoxCallBack(__FILE__, __LINE__, fn, false, false)
#define		EXIT_CALLBACK(fn,forced)	CMsgBoxCallBack(__FILE__, __LINE__, fn, true, forced)


class CMessageBoxManager: public i3GameObjBase
{
	I3_CLASS_DEFINE(CMessageBoxManager, i3GameObjBase);

public:
	CMessageBoxManager();
	virtual ~CMessageBoxManager();

	UIMessageBox* OpenMessageBox(const MsgBoxOpenParam & Param);
	UIMessageBox* OpenMessageBox(const i3::rc_wstring& strMessage, const i3::rc_wstring& strCaption,INT32 flagStyle,
		const CMsgBoxCallBack & fnEventCB,void*pThis,INT32 nFontSize, INT32 nAlignX, INT32 nAlignY);

	void CloseMessageBox(UIMessageBox* pMessageBox);

	void ChangeMessage(UIMessageBox* pMessageBox, const char* strMessageBox,const char* strCaption);
	void ChangeButton(UIMessageBox* pMessageBox, const char * pUser, const char * pOK, const char * pCancel);

	void PreventMessageBoxWhenPhaseChange();

	void CloseAllMessageBox(void);
	void CloseGameMessageBox(void);
	virtual bool Create(i3Node* pParentNode = 0,bool bEnable = 1) override;
	virtual bool OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;

	REAL32 GetInputPositionX(void) const;
	REAL32 GetInputPositionY(void) const;
	INT32 GetInputState(void) const;
	bool IsEnabled(void) const; //OpenMessageBox 함수 호출 뒤 IsEnabled 반환값은 false 이다. 몇 Frame이 지나야지 true를 반환함.. @.@
	void RefreshTop(void) const;

	REAL32 GetScreenRate(void)			{ return m_fScreenRate;		}

	void OnResize(void);
	
protected:	
	virtual void OnUpdate(REAL32 rDeltaSeconds) override;

	UIMessageBox * _FindMessageBox( const i3::rc_wstring& strMessage);

private:	
	void _RemoveAll(void);
	void _Release(void);
	void _DisableAll(void);
	void _EnableTop(void);
	bool _ProcessInput(i3InputDeviceManager* pMrg);
	void _ArrayGameMessageBox(void);

private:
	i3GameObjBase* m_pGameMessageRoot = nullptr;
	i3GameObjBase* m_pSystemMessageRoot = nullptr;

	UIMessageBox *	m_pOldMessageBox = nullptr;

	REAL32 m_fPosX = 0.0f;
	REAL32 m_fPosY = 0.0f;
	REAL32 m_fScreenRate = 1.0f;
	INT32 m_mbInputState = MBIS_NULL;
	INT32 m_mkPress = MBIS_NULL;
	INT32 m_mkStroke = MBIS_NULL;
	INT32 m_nCountGameMessgeTop = 0;
	
public:
	UIMessageBox *	GetTopMsgBox( void);
	UIMessageBox *	FindMsgBox( LuaState * L);
	bool IsTopMsgBox(UIMessageBox* msg);

private:
	bool	m_bPhaseTransition = false;

public:
	bool m_is_open_message_box = false; //IsEnabled 함수의 부족한 기능을 대신함
};

#endif //_MessageBoxManager_h

