#if !defined( __STEP_PROFILE_PRIVATE_INFO_BG_H__)
#define __STEP_PROFILE_PRIVATE_INFO_BG_H__

#if defined( ENABLE_VIEW_USER)
	#define STEPPROFILE_USERLIST_VIEW_COUNT		5

	struct __tagStepProfile_UserList_View
	{
		i3GuiEditBox *	_pUserName;
		i3GuiButton *	_pButton;
	};
#endif

class CGuiNotifyReceiver;

class CStepProfile_PrivateInfoBG : public i3ElementBase
{
	I3_CLASS_DEFINE( CStepProfile_PrivateInfoBG);
protected:
	CGuiNotifyReceiver *		m_pNotifyBase;

	i3GuiStatic *	m_pWindowBG;

	i3GuiEditBox *	m_pCaption_Text;	/*개인정보*/
	i3GuiEditBox *	m_pUserNick;
	i3GuiStatic *	m_pUserRank;
	i3GuiStatic*	m_pUserClanMark;	

	i3GuiEditBox*	m_pStatic_Designation_Text;	/*호칭*/

	i3GuiStatic *	m_pDesignation_Image[		MAX_EQUIP_USER_TITLE];
	i3GuiStatic *	m_pDesignation_Image_Lock[	MAX_EQUIP_USER_TITLE];
	i3GuiEditBox *	m_pDesignation_Text[		MAX_EQUIP_USER_TITLE];

	i3GuiPopupList* m_pPopup_TargetList;
	i3GuiEditBox *	m_pCombo_Target;
	i3GuiButton *	m_pButton_Target;

	i3GuiList*		m_pPopup_WhoList;
	i3GuiStatic*	m_pStatic_WhoBG;
	i3GuiEditBox *	m_pCombo_Who;
	i3GuiButton *	m_pButton_WhoList;

#if defined( ENABLE_VIEW_USER)
	__tagStepProfile_UserList_View m_User[ STEPPROFILE_USERLIST_VIEW_COUNT];
#endif

	i3BinList		m_UserList;
	REAL32			m_rWheelDelta;

	bool			m_bIsMyInfoView;

public:
	bool	isMyInfoView( void)				{ return m_bIsMyInfoView; }

	BOOL	isEnable( void)					{ return m_pWindowBG->isEnable(); }
	void	SetEnable( BOOL bEnable)		{ m_pWindowBG->SetEnable( bEnable);	}

public:
	CStepProfile_PrivateInfoBG();
	virtual ~CStepProfile_PrivateInfoBG();

	BOOL	LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl);
	void	Init( void);
	void	Update( REAL32 tm);
	void	GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify);

	void	OnUserSelect( INT32 iUserIdx);
	void	OnGameEvent( INT32 event, INT64 arg);
	void	WheelUpdate( void);

private:
	enum _USER_LIST_TYPE {
		_USER_LIST_TYPE_NONE = 0,
		_USER_LIST_TYPE_CLAN,
		_USER_LIST_TYPE_FRIEND,
	};

	_USER_LIST_TYPE		m_nCurrentTargetType;

	void	__AddUserList( _USER_LIST_TYPE nUserListType);
	void	__OnSelectionTargetList( _USER_LIST_TYPE nUserListType);
	
	void	__UpdateUserInfo( bool bMy);
	void	__UpdateComboWhoBox( INT32 iUserIdx);
	void	__UpdateWhoListBox( void);

	void	__UpdateDesignation( bool bMy);
};

#endif
