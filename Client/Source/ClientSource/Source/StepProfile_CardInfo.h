#if !defined( __STEP_PROFILE_CARDINFO_H__)
#define __STEP_PROFILE_CARDINFO_H__

#include "StepProfileDef.h"


struct MedalInfoSlot
{
	i3GuiStatic* _pMedalBg;
	i3GuiStatic* _pRibbonImage;
	i3GuiStatic* _pMedalImage;
	i3GuiEditBox* _pMedalName;
	i3GuiEditBox * _pMedalCount;
};

class CGuiNotifyReceiver;

class CStepProfile_CardInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( CStepProfile_CardInfo);
protected:
	CGuiNotifyReceiver *		m_pNotifyBase;

	i3GuiStatic*		m_pCardInfoWin;	
	i3GuiEditBox*		m_pCardInfoCaption;
	i3GuiButton*		m_pCardInfoConfirm;
	i3GuiEditBox*		m_pCaptionTitle;
	i3GuiStatic*		m_pCardInfoCardImage;
	i3GuiEditBox* 		m_pCardInfoCardName;
	i3GuiEditBox* 		m_pCardInfoPriceKey;
	i3GuiEditBox* 		m_pCardInfoPriceValue;
	i3GuiEditBox* 		m_pCardInfoPointKey;
	i3GuiEditBox* 		m_pCardInfoPointValue;
	i3GuiEditBox* 		m_pCardInfoExpKey;
	i3GuiEditBox* 		m_pCardInfoExpValue;

	// 지급표장
	i3GuiEditBox*	m_pSupplyMedalTitle;
	MedalInfoSlot	m_supplyMedal[ BADGE_TYPE_COUNT];

	
	// 최종 추가 보상 포인트/경험치
	i3GuiEditBox* m_pFinalPrizePointExpTitle;
	i3GuiEditBox * m_pFinalPrizePointKey;
	i3GuiEditBox * m_pFinalPrizePointText;
	i3GuiEditBox * m_pFinalPrizeExpKey;
	i3GuiEditBox * m_pFinalPrizeExpText;


	// 최종보상 총기
	i3GuiEditBox * m_pFinalPrizeItemKey;
	i3GuiEditBox* m_pFinalPrizeItemName;
	i3GuiStatic* m_pFinalPrizeItemImage;

public:
	CStepProfile_CardInfo();
	virtual ~CStepProfile_CardInfo();

	BOOL	LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl);
	void	Init( void);
	void	Update( REAL32 tm);
	void	GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify);

	void	OnCardInfo( CARDSET_ID cardSetId);

	BOOL	isEnable( void)					{ return m_pCardInfoWin->isEnable(); }
	void	SetEnable( BOOL bEnable)		{ m_pCardInfoWin->SetEnable( bEnable); }
};

#endif
