#include "pch.h"
#include "UI_V11/UITabShopRecommend.h"


UITabShopRecommend::UITabShopRecommend(UIPhaseSubBase* phase, UIToolTipShop* pTTWeapon, 
									   UIToolTipShop* pTTNonWeapon) :
UITabShopBase(phase, NULL)
{

}

UITabShopRecommend::~UITabShopRecommend()
{
}
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabShopRecommend_ClickTab( LuaState * L)
	{
		return 0;
	}
}

LuaFuncReg UITabShopRecommend_Glue[] = {
	{"uiClickTab",				UITabShopRecommend_ClickTab},
	{NULL,						NULL}
};


//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
