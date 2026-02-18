/**
 * \file	UI\UIPopupUserAction.h
 *
 * Declares the user interface popup user action class.
 */


#if !defined( __UI_POPUP_USERACTION_H__)
#define __UI_POPUP_USERACTION_H__

#include "../UI/UIDefine.h"
#include "../UI/UIUserActionDef.h"
#include "../UI/UIPopupBase.h"

struct USERACTION_INFO
{
	USER_CONTEXT_MENU	_Command;
	INT32				_CommunityIndex;
	LOBBY_USER_INFO_LIST _tUserInfo;
	USERLIST_TAB		_UserTab;

	INT32				_SlotIdx;
};

class UIPopupUserAction_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUserAction_V11, UIPopupBase);

private:
	INT32				m_commandCount;
	USER_CONTEXT_MENU	m_commandMap[ UCM_MAX];
	
	i3::rc_wstring		m_commandText[ UCM_MAX];

	i3EventReceiver*	m_pObserver;
	USERACTION_INFO		m_RequestInfo;

	i3UIListView_Box *	m_pListBox;

	i3UITemplate *		m_pIconTemplate;

	VEC2D				m_vOriginSize;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	INT32			_GetCommandIcon( INT32 idx);

	/** \brief Command 실행 */
	void			_ExecuteCommand( INT32 cmd);
	void			_CmdViewUserInfo( void);
	void			_CmdAddFriend( void);
	void			_CmdInviteClan( void);
	void			_CmdSendMail( void);
	void			_CmdReportUser( void);
	void			_CmdWhispToUser( void);
	void			_CmdSendPresent( void);
	void			_CmdAddBlock( void);
	void			_CmdRemoveBlock( void);
	void			_CmdViewOtherItem( TTCHAR *);
	void			_CmdKickUser( INT32 idxSlot);
	void			_CmdGMOption( void);

public:
	UIPopupUserAction_V11();
	virtual ~UIPopupUserAction_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnReset( void) override;
	/*
	param[in] pArg1 i3EventReceiver*
	param[in] pArg2 INT32*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual void	OnEntranceEnd( void) override;

	/** \brief Command List를 초기화한다 */
	void			ResetCommand( void);
	
	/** \brief Command List에 Cmd를 추가한다. */
	void			AddCommand( USER_CONTEXT_MENU cmd);

	/** \brief Command 실행
		\desc Observer에게 Command를 보냅니다. */
	void			OnCommand( INT32 cmd);

	//void			DoBlockUser( void);
	void			DoClanInvite( void);

private:
	REAL32			_ResizeListView();
	void			_ReposFrameWnd(REAL32 MaxWidth);
	void			_ReSizeAndPosIcon();
	void			_SetIconSizeAndPos(i3UIListView_Cell * pCell, REAL32 sepWidth);

	//------------------------------------------------------------------------------//
	//								Callback rutine									//
	//------------------------------------------------------------------------------//
	//static void		CbBlockUser( void* pThis, INT32 nParam);
	static void		CbClanInvite(void* pThis, INT32 nParam);
	static void		CbDeleteFriend(void* pThis,INT32 nParam);
	static void		CbMoveShop(void* pThis,INT32 nParam);
	static void		CbAddFriend(void* pThis, INT32 nParam);
};

#endif
