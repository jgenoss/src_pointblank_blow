#pragma once

#include "../UI/UIPopupBase.h"

class UIPopupMissionCardShop_V10 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMissionCardShop_V10, UIPopupBase);

	UIPopupMissionCardShop_V10();
	virtual ~UIPopupMissionCardShop_V10();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 INT32*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

private:
	INT32	m_CardSetType;
	void	_SetCardImage(ENUM_QUEST_CARDSET_TYPE);
	void	_SetCardTitle(const i3::wliteral_range& wstrText);
	void	_SetCardPrice(const i3::wliteral_range& wstrText);
	void	_SetCardMymoney(const i3::wliteral_range& wstrText);
	void	_SetCardMybalance(const i3::wliteral_range& wstrText);
	void	_SetCardConfirm(bool);


public:
	virtual bool	OnKey_Enter( void)  override {   BuyCardOK(); return true;   }
	virtual bool	OnKey_Escape() override { BuyCardCancel(); return true; }
	void	BuyCardOK();
	void	BuyCardCancel();
	void	OnCardBuyAll( INT32 cardSetType);
};
