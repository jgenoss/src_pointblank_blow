#if !defined( __STEP_PROFILE_COMPENSATE_WIN_H__)
#define __STEP_PROFILE_COMPENSATE_WIN_H__

#define PROFILE_COMPENSATE_CARD_COUNT		4

struct __tagProfileCompensateCard
{
	i3GuiButton *	_CardButton;
	i3GuiStatic *	_CardImage;
	i3GuiEditBox *	_CardName;
};

class CGuiNotifyReceiver;

class CStepProfile_CompensateWin : public i3ElementBase
{
	I3_CLASS_DEFINE( CStepProfile_CompensateWin);
protected:
	CGuiNotifyReceiver *		m_pNotifyBase;

	i3GuiStatic *				m_pPopupWinBG;
	i3GuiEditBox *				m_pCaption;
	i3GuiButton *				m_pButton_Confirm;
	i3GuiEditBox *				m_pNoticeCaption;

	i3GuiEditBox *				m_pDescription_CardName_Key;
	i3GuiEditBox *				m_pDescription_CardName_Value;
	i3GuiEditBox *				m_pDescription_CardInfo_Key;
	i3GuiEditBox *				m_pDescription_CardInfo_Value;
	i3GuiEditBox *				m_pDescription_CardNotice;
	i3GuiEditBox *				m_pDescription_CardNotice1;

	__tagProfileCompensateCard	m_CardSetList[ PROFILE_COMPENSATE_CARD_COUNT];

public:
	CStepProfile_CompensateWin();
	virtual ~CStepProfile_CompensateWin();

	BOOL	LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl);
	void	Init( void);
	void	Update( REAL32 tm);
	void	GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify);

	void	ConfirmOK( void);
	void	SelectCard( INT32 iSlot);

	BOOL	isEnable( void)					{ return m_pPopupWinBG->isEnable(); }
	void	SetEnable( BOOL bEnable)		{ m_pPopupWinBG->SetEnable( bEnable); }

private:
	enum __CardSlot
	{
		__CardSlot_0 = 0,
		__CardSlot_1,
		__CardSlot_2,
		__CardSlot_3,
	};
	void	__UpdateCardInfo( __CardSlot iSlot);

	void	__UpdateCardList( void);
};

#endif
