#if !defined( __UI_MESSAGEBOX_H__)
#define __UI_MESSAGEBOX_H__

#include "UIPopupBase.h"
#include "MessageBoxManager.h"

enum UIButtonType
{
	UIBT_USER = 0,
	UIBT_OK,
	UIBT_CANCEL,

	UIBT_MAX
};

enum UITextType
{
	UITT_START = 0,

	UITT_CAPTION = UITT_START,
	UITT_MESSAGE,
	UITT_OK,
	UITT_CANCEL,
	UITT_USER,

	UITT_MAX
};

enum UIMessageBoxStyle
{
	UIMBS_START = 0,

	UIMBS_OK = UIMBS_START,
	UIMBS_OKCANCEL,
	UIMBS_USER_OKCANCEL,
	UIMBS_WAIT,
	UIMBS_WEB,				// 웹 빌링에서 쓰이는 MessageBox로 예외적으로 처리합니다.
	UIMBS_BLOCK_NOTICE,

	UIMBS_MAX
};

enum UIBTN_STYLE
{
	UIBTN_NONE = -1,
	UIBTN_OK,			// 확인
	UIBTN_NOTUSE,		// 사용안함
	UIBTN_CANCEL,		// 취소
	UIBTN_ITEMUSE,		// 아이템 사용
};

typedef void (*cbExitMessage)();

class UIMessageBox : public UIPopupBase
{
	I3_CLASS_DEFINE( UIMessageBox, UIPopupBase);
private:
	CMessageBoxManager *	m_pManager;

	UIMessageBoxStyle		m_MBStyle;
	
	bool			m_bReserved;	// 예약된 MessageBox 잠시 hide.. 이 값이 true가 되면 박스가 삭제되지 않음..
	bool			m_bClosing;
	bool			m_bInitilized;

	SIZE32			m_WebScreenSize;
	bool			m_bCreatedWebBilling;
	bool			m_bShowWebBilling;
	HWND			m_hExplorerhWnd;
	UINT32			m_CheckCRC;

	i3::rc_wstring	m_wstrMsg;
	i3::rc_wstring	m_wstrCaption;
	INT32			m_AlignX, m_AlignY;
	CMsgBoxCallBack m_pCallBackFunc;
	void *			m_pThis;
	UINT32			m_nParam;

	bool			m_bIdentified;			// 메세지박스를 삭제하는 플래그로 지연삭제용..(완전히 닫힌 경우에만 설정됨)

	INT32			m_nTextWidth;
	INT32			m_nTextHeight;


	INT32			m_nX;
	INT32			m_nY;

	i3UIStaticText *	m_pMsg;
	i3UIButtonComposed3 *	m_pButton[ UIBT_MAX];
	UIBTN_STYLE		m_ButtonStyle[ UIBT_MAX];

	const static UINT32 DefaultWidth = 200;
	const static UINT32 DefaultHeight = 100;
	const static UINT32 MaxString = MAX_STRING_COUNT * 4;

	UIBTN_STYLE		m_BTN_Style;
	i3UIFrameWnd *	m_pBGFrame;
	cbExitMessage	cbExitFunc;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	void			_CreateWeb( const i3::rc_wstring& URL, SIZE32 Size);

	void			_SetButtonState( void);
	void			_SetText( const i3::rc_wstring& wstrMessage, const i3::rc_wstring& wstrCaption, INT32 nAlignX = 1, INT32 nAlignY = 1);
	void			_SetBlockText( const i3::rc_wstring& wstrMessage, const i3::rc_wstring& wstrCaption);
	void			_RecalcShape( void);

	void			_ConfirmParam( UIBTN_STYLE state);

public:
	/** \brief 확인 완료 여부
		\desc OK를 한경우 true가된다.	*/
	bool			IsIdentified( void)				{ return m_bIdentified; }
	bool			IsHided() const					{ return m_bReserved; }

public:
	UIMessageBox();
	virtual ~UIMessageBox();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd(void) override;

	virtual bool	OnExitStart( void) override;
	virtual void	OnExitEnd( void) override;

	virtual bool	OnOKButton( void) override;
	virtual bool	OnCloseButton( void) override;

	void			ClickUserButton( void);

	bool			Create( i3GameNode * pParent, CMessageBoxManager* pManager, const MsgBoxOpenParam & Param);

	/** \brief MsgBox를 잠시 보이지 않도록 한다. */
	void			Hide( bool bForced = false);

	/** \brief MsgBox를 완전히 닫는다. */
	void			Close(void);
	void			CloseDirect(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	bool UIMessageBox::IsClosing() const
	///
	/// \brief	Query if this object is closing.
	///
	/// \author	
	/// \date	2011-12-19
	///
	/// \return	true if closing, false if not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool			IsClosing() const { return m_bClosing; }

	void			Notify( void);
	void			SetPosition( INT32 x, INT32 y);
	INT32			GetPositionX( void)			{ return m_nX; }
	INT32			GetPositionY( void)			{ return m_nY; }

	UINT32			GetCheckCRC(void) const		{ return m_CheckCRC; }

	INT32			GetWidth( void)
	{
		if( m_pPopupFrame)
			return (INT32) m_pPopupFrame->getWidth();
		return 0;
	}

	INT32			GetHeight( void)
	{
		if( m_pPopupFrame)
			return (INT32) m_pPopupFrame->getHeight();
		return 0;
	}

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape() override;

	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;

	void			ChangeButton( const char * pUser, const char * pOK, const char * pCancel);
	void			ChangeMessage( const i3::rc_wstring& wstrMessage, const i3::rc_wstring* pwstrCaption);
	void			ChangeMessage();

	void SetCBDocumentComplete( cbExitMessage cb) { cbExitFunc = cb;}
	void OnExitComplete() { if( cbExitFunc) cbExitFunc(); cbExitFunc = nullptr;}

	i3UIButtonComposed3* GetButtonControl(UIButtonType idx){	return m_pButton[ idx ];	}
};

#endif
