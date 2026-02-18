#if !defined( __STEP_PROFILE_CARDSHOP_WIN_H__)
#define __STEP_PROFILE_CARDSHOP_WIN_H__

class CGuiNotifyReceiver;

class CStepProfile_CardShopWin : public i3ElementBase
{
	I3_CLASS_DEFINE( CStepProfile_CardShopWin);
protected:
	CGuiNotifyReceiver *		m_pNotifyBase;

	i3GuiStatic* m_pCardShopWin;
	i3GuiEditBox* m_pCardShopCaption;
	i3GuiButton* m_pCardShopConfirm;
	i3GuiButton* m_pCardShopCancel;
	i3GuiEditBox* m_pCardShopName;
	i3GuiEditBox* m_pCardShopPrice;
	i3GuiEditBox* m_pCardShopCurrentKey;
	i3GuiEditBox* m_pCardShopCurrentValue;
	i3GuiEditBox* m_pCardShopRemainderKey;
	i3GuiEditBox* m_pCardShopRemainderValue;
	i3GuiStatic* m_pCardShopOneDummy;
	i3GuiStatic* m_pCardShopSetDummy;
	i3GuiEditBox* m_pCardShopNotice;

public:
	CStepProfile_CardShopWin();
	virtual ~CStepProfile_CardShopWin();

	BOOL	LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl);
	void	Init( void);
	void	Update( REAL32 tm);
	void	GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify);

	BOOL	isEnable( void)				{ return m_pCardShopWin->isEnable(); }
	void	SetEnable( BOOL bEnable)	{ m_pCardShopWin->SetEnable( bEnable); }

//	void	OnCardBuyOne( UINT8 cardSetId, UINT8 cardIndex);
	void	OnCardBuyAll( UINT8 cardSetId);

	BOOL	IsBuyingOneCard( void)		{ return m_pCardShopOneDummy->isEnable(); }
};

#endif
