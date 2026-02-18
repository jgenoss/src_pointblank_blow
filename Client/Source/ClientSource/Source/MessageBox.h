#ifndef __MESSAGE_BOX_H__
#define __MESSAGE_BOX_H__

enum WindowShape
{
	WS_START = 0,

	WS_LEFT_TOP = WS_START,
	WS_CENTER_TOP,
	WS_RIGHT_TOP,
	WS_LEFT_MIDDLE,
	WS_CENTER_MIDDLE,
	WS_RIGHT_MIDDLE,
	WS_LEFT_BOTTOM,
	WS_CENTER_BOTTOM,
	WS_RIGHT_BOTTOM,	

	WS_MAX
};

enum ButtonShape
{
	BS_START = 0,

	BS_NORMAL = BS_START,
	BS_ON_MOUSE,
	BS_CLICKED,
	BS_DISABLE,

	BS_MAX
};

enum ButtonType
{
	BT_START = 0,

	BT_OK = BT_START,
	BT_CANCEL,
	BT_USER,

	BT_MAX
};

enum TextType
{
	TT_START = 0,

	TT_CAPTION = TT_START,
	TT_MESSAGE,
	TT_OK,
	TT_CANCEL,
	TT_USER,

	TT_MAX
};

enum MessageBoxStyle
{
	MBS_START = 0,

	MBS_OK = MBS_START,
	MBS_OKCANCEL,
	MBS_USER_OKCANCEL,
	MBS_WAIT,
	MBS_WEB,				// 웹 빌링에서 쓰이는 MessageBox로 예외적으로 처리합니다.

	MBS_MAX
};

class CMessageBoxManager;
struct MsgBoxOpenParam;

class CMessageBox: public i3GameObjBase
{
	I3_CLASS_DEFINE(CMessageBox, i3GameObjBase);

public:
	CMessageBox();
	virtual ~CMessageBox();

	virtual bool Create(i3Node* pParentNode,CMessageBoxManager* pManager, const MsgBoxOpenParam & Param);
	virtual void OnUpdate(REAL32 rDeltaSeconds);

	void ShowEx(const char* strMessage,const char* strCaption, const CMsgBoxCallBack & fnEventCB,void*pThis, INT32 nAlignX, INT32 nAlignY);
	void Hide(void);
	void Resize(void);
	bool IsComplate(void) const;
	void Notify(void) const;
	void SetInputEnable(bool bEnable);

	void ChangeMessage(const char* strMessage,const char* strCaption);
	void ChangeButton(const i3::wliteral_range& wUser, const i3::wliteral_range& wOK, const i3::wliteral_range& wCancel);

	void SetPosition(INT32 x,INT32 y);	
	UINT32 GetWidth(void)				{ return m_nWidth;	}
	UINT32 GetHeight(void)				{ return m_nHeight;	}
	UINT32 GetCheckCRC(void)	const		{ return m_CheckCRC;						}

	// Web Billing관련
	void CreateWeb(const wchar_t* URL, SIZE32 Size);
			
private:
	void _RecalcShape(void);
	void _Complate(INT32 nParam);
	bool _IsOnMouse(ButtonType type);
	void _SetText(const char* strMessage,const char* strCaption = nullptr, INT32 nAlignX = 1, INT32 nAlignY = 1);

private:
    static i3Texture* m_pTexture;
	bool m_bInited;
	i3Sprite2D* m_pWindow;
	i3Sprite2D* m_pSeparate;
	i3Sprite2D* m_pButton[BT_MAX];
	i3TextNodeDX2* m_pText[TT_MAX];
	INT32 m_nX;
	INT32 m_nY;
	UINT32 m_nWidth;
	UINT32 m_nHeight;
	UINT32 m_style;	
	INT32 m_nTextWidth;
	INT32 m_nTextHeight;
	CMessageBoxManager* m_pManager;
	UINT32 m_nCapturedButton;
	bool m_bComplate;
	UINT32 m_nParam;
	void* m_pThis;
	CMsgBoxCallBack 	m_pCallBackFunc;
	bool m_bInputEnable;
	bool m_bFirst;
	UINT32 m_CheckCRC;

	bool bCreatedWebBilling;
	bool bShowWebBilling;
	HWND m_hExplorerhWnd;

	SIZE32	m_WebScreenSize;

	i3SoundPlayInfo	*	m_pBtnClickSound[BT_MAX + 1];


	const static UINT32 DefaultWidth = 200;
	const static UINT32 DefaultHeight = 35;
	const static UINT32 MaxString = MAX_STRING_COUNT * 4;
};

#endif // __MESSAGE_BOX_H__
