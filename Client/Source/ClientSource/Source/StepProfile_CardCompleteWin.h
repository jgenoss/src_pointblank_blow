#if !defined( __STEP_PROFILE_CARDCOMPLETE_WIN_H__)
#define __STEP_PROFILE_CARDCOMPLETE_WIN_H__

class CGuiNotifyReceiver;

class CStepProfile_CardCompleteWin : public i3ElementBase
{
	I3_CLASS_DEFINE( CStepProfile_CardCompleteWin);
protected:
	CGuiNotifyReceiver *		m_pNotifyBase;

	i3GuiStatic* m_pCardComplateWin;
	i3GuiEditBox* m_pCardComplateCaption;
	i3GuiButton* m_pCardComplateConfirm;
	i3GuiEditBox* m_pCardComplateName;
	i3GuiEditBox* m_pCardComplateDescription;
	i3GuiStatic* m_pCardComplateBg;
	i3GuiStatic* m_pCardComplateDummy;

public:
	BOOL	isEnable( void)						{ return m_pCardComplateWin->isEnable(); }
	void	SetEnable( BOOL bEnable)			{ m_pCardComplateWin->SetEnable( bEnable); }

public:
	CStepProfile_CardCompleteWin();
	virtual ~CStepProfile_CardCompleteWin();

	BOOL	LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl);
	void	Init( void);
	void	Update( REAL32 tm);
	void	GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify);
};

#endif
