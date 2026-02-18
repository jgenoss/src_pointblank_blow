#include "pch.h"

#include "UI_V11/UIToolTipShop.h"
#include "UI_V11/UIPhaseShop_V11.h"
#include "UI_V11/UITabShopBase.h"

#include "UI/UIUtil.h"
#include "UI/ItemImageMgr.h"

I3_CLASS_INSTANCE( UIToolTipShop);
I3_CLASS_INSTANCE( UIToolTipShop_Weapon );
I3_CLASS_INSTANCE( UIToolTipShop_NonWeapon);

UIToolTipShop::UIToolTipShop() : m_pImageSet(NULL), m_pEnteredPack(0), m_interval_tooltip(4.f)
{
	ZeroMemory(&m_pos_by_slot, (sizeof(VEC3D)*TAPSHOP_ITEM_SLOT));
}

UIToolTipShop::~UIToolTipShop()
{
	
}

void	UIToolTipShop::OnCreate(i3GameNode* pParent)
{
	UIToolTipBase::OnCreate(pParent);	
}

void	UIToolTipShop::EnableCurrentSlotToolTip(INT32 slotidx, SHOP_GOODS_PACK* pack)
{
	if ( m_pEnteredPack == pack )
		return;

	VEC3D& targetPos = m_pos_by_slot[slotidx];
	i3UIFrameWnd* pFrameWnd = static_cast<i3UIFrameWnd*>(GetScene(0)->getFirstChild());
	pFrameWnd->setPos(&targetPos);
	
	GetScene(0)->EnableScene(true);
	GetScene(0)->ShowTop();

	this->SetMoreInfo(pack);

	UIToolTipShop::SetMainInfo(pack);
	
	m_pEnteredPack = pack;

}

/*
void	UIToolTipShop::_SetPriceInfo(SHOP_GOODS_PACK* pack)
{
	//
	// 공통된 부분이므로 공통된 함수로 처리하는게 편할것이다..
	// (호출함수는 공통이지만, 카테고리별로 정보를 취득하는 루틴이 공통이 아님..)
	// 이 부분을 편하게 호출하기 위해, 일단 탭클래스에서의 가상함수를 사용...	
	//

	
	// 가격정보는 팝업 구매창에도 있으므로, 이부분을 공통함수로 빼는게 좋을지도 모르겠다.
	char astrRadioText[4][MAX_STRING_COUNT];
	char astrStaticText[4][MAX_STRING_COUNT];
	const INT32 count = pack->GetGoodsCount();
	int i;
	for ( i = 0 ; i < count ; ++i )
	{
		SHOP_GOODS* good = pack->GetGoods(i);
		usf::GetShopPeriodCountString(good, astrRadioText[i]);
		usf::GetShopPriceString(good, astrStaticText[i]);
	}
	for ( ; i < 4; ++i)
	{
		astrRadioText[i][0] = 0;	astrStaticText[i][0] = 0;
	}

	// 이제 루아로 밀면 된다..
	{
		LuaState* L = _CallLuaFunction(0, "SetPriceInfo");
		i3Lua::PushInteger(L, count);	
		for (INT32 i = 0 ; i < 4 ; ++i )
		{
			i3Lua::PushA2UTF8String(L, astrRadioText[i]);
			i3Lua::PushA2UTF8String(L, astrStaticText[i]);
		}
		_EndLuaFunction(L, 9);
	}
	//
	//
}

*/
void	UIToolTipShop::SetMainInfo(SHOP_GOODS_PACK* pack)
{
	GetItemImageMgr()->SelectImage(m_pImageSet, pack->GetGoods(0));
}

void	UIToolTipShop::DisableToolTip(INT32 slotidx, SHOP_GOODS_PACK* pack)	//화면에서 제거..
{
	if ( m_pEnteredPack != pack )		// 엔터만 2번들어온 경우, 먼저것을 무시..
		return;
	
	GetScene(0)->EnableScene(false);

	m_pEnteredPack = 0;
}

void	UIToolTipShop::DisableToolTip_Forced()
{
	GetScene(0)->EnableScene(false);
	m_pEnteredPack = 0;
}


// 이 시점에서 이미 리소스는 수중에 있음...잔여처리로 무엇이 필요한지 생각할것..
UIPhaseShop_V11*	UIToolTipShop::GetPhaseShop() const 
{ 
	return static_cast<UIPhaseShop_V11*>( 
		const_cast<UIToolTipShop*>(this)->getParent() ); 
}


//enum { TAPSHOP_ITEM_ROW = 3 };										
//enum { TAPSHOP_ITEM_COLUMN = 4 };
//enum { TAPSHOP_ITEM_SLOT = TAPSHOP_ITEM_ROW * TAPSHOP_ITEM_COLUMN };	


void	UIToolTipShop::_Makeup_Pos_Array()
{
	int nRow, nColumn;
	UIPhaseSubBase* phase = GetPhaseShop();
	VEC2D size_slot;
	VEC3D first_pos;

	nRow = TAPSHOP_ITEM_ROW;
	nColumn = TAPSHOP_ITEM_COLUMN;
	size_slot = static_cast<UIPhaseShop_V11*>(phase)->GetSlotSize();
	first_pos = static_cast<UIPhaseShop_V11*>(phase)->GetFirstSlotPos();

	i3UIFrameWnd* pFrameWnd = static_cast<i3UIFrameWnd*>(GetScene(0)->getFirstChild());
	const VEC2D& size_frame = *pFrameWnd->getSize();


	float slotPosX;
	float slotPosY = first_pos.y + 
		( REAL32(nRow) * size_slot.y - size_frame.y ) * .5f;

	for (INT32 i = 0; i < nRow ; ++i )		
	{
		slotPosX = first_pos.x;

		for (INT32 j = 0 ; j < nColumn; 
			++j, slotPosX += size_slot.x )
		{
			VEC3D& targetPos = m_pos_by_slot [i * nColumn +j];

			targetPos.x = ( j < (nColumn/2)) 
				? (slotPosX + size_slot.x + m_interval_tooltip) 
				: (slotPosX - size_frame.x - m_interval_tooltip) ;

			targetPos.y = slotPosY;
			targetPos.z = 0.f;
		}
	}
}



void	UIToolTipShop::OnLoadAllScenes()
{
//
	m_pImageSet = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), this->GetImageSetName() );

	_Makeup_Pos_Array();				// 포지션 구성부터 먼저..
	GetScene(0)->EnableScene(false);	// 우선 닫을 것..
//

}

void	UIToolTipShop::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_pImageSet);
}



/////////////////////////////////////////////////////

UIToolTipShop_Weapon::UIToolTipShop_Weapon()
{
	
}

void UIToolTipShop_Weapon::OnCreate(i3GameNode* pParent)
{
	base::OnCreate(pParent);
	AddScene("Scene/Main/PBRe_Tooltip_WeaponitemSlot.i3UIs");
}



const char* 	UIToolTipShop_Weapon::GetImageSetName() const
{
	return "i3UIButtonImageSet1";
}


void UIToolTipShop_Weapon::SetMoreInfo(SHOP_GOODS_PACK* pack)
{

	i3::rc_wstring itemName;
	i3::rc_wstring itemDesc;
	i3::rc_wstring itemCategory;
	i3::rc_wstring itemStatus;
	i3::rc_wstring itemRequired;

	GetPhaseShop()->GetCurrentTab()->Update_ToolTip_ByShopCategory(pack, itemName, itemDesc, itemCategory, itemStatus, itemRequired);
	//
	{
		LuaState* L = _CallLuaFunction(0, "SetMainInfo");
		i3Lua::PushStringUTF16To8(L, itemName);
		i3Lua::PushStringUTF16To8(L, itemCategory);
		i3Lua::PushStringUTF16To8(L, itemStatus);
		i3Lua::PushStringUTF16To8(L, itemRequired);

		_EndLuaFunction(L, 4);

	}

	// 프로그레시브 바..
	i3::stack_wstring strRange;
	i3::stack_wstring strBulletCount;
	i3::stack_wstring temp;

	INT32 damage, rapid, accuracy, recoil_v, recoil_h, speed;

	GameUI::GetNormalizedWeaponInfo(pack->GetGoods(0)->_ItemID, damage, rapid, accuracy, recoil_v, recoil_h, speed, 
		strRange, strBulletCount);
	
	{	// 루아 함수 작성..
		LuaState* L = _CallLuaFunction(0, "SetWeaponInfo");
		i3Lua::PushNumber(L, float(damage) / 100.f);
		i3Lua::PushNumber(L, float(rapid) / 100.f);
		i3Lua::PushNumber(L, float(accuracy) / 100.f);
		i3Lua::PushNumber(L, float(recoil_v) / 100.f);
		i3Lua::PushNumber(L, float(recoil_h) / 100.f);
		i3Lua::PushNumber(L, float(speed) / 100.f);
		
		GameUI::GetGaugeString(damage, temp);
		i3Lua::PushStringUTF16To8(L, temp);
		GameUI::GetGaugeString(rapid, temp);
		i3Lua::PushStringUTF16To8(L, temp);
		GameUI::GetGaugeString(accuracy, temp);
		i3Lua::PushStringUTF16To8(L, temp);
		GameUI::GetGaugeString(recoil_v, temp);
		i3Lua::PushStringUTF16To8(L, temp);
		GameUI::GetGaugeString(recoil_h, temp);
		i3Lua::PushStringUTF16To8(L, temp);
		GameUI::GetGaugeString(speed, temp);
		i3Lua::PushStringUTF16To8(L, temp);
			
		i3Lua::PushStringUTF16To8(L, strRange);
		i3Lua::PushStringUTF16To8(L, strBulletCount);
		
		_EndLuaFunction(L, 14);
	}
	
}

void UIToolTipShop_NonWeapon::SetMoreInfo(SHOP_GOODS_PACK* pack)
{
	
	i3::rc_wstring itemName;
	i3::rc_wstring itemDesc;
	i3::rc_wstring itemCategory;
	i3::rc_wstring itemStatus;
	i3::rc_wstring itemRequired;

	GetPhaseShop()->GetCurrentTab()->Update_ToolTip_ByShopCategory(pack, itemName, itemDesc, itemCategory, itemStatus, itemRequired);
	//
	{
		LuaState* L = _CallLuaFunction(0, "SetMainInfo");
		i3Lua::PushStringUTF16To8(L, itemName);
		i3Lua::PushStringUTF16To8(L, itemDesc);
		i3Lua::PushStringUTF16To8(L, itemCategory);
		i3Lua::PushStringUTF16To8(L, itemStatus);
		i3Lua::PushStringUTF16To8(L, itemRequired);

		_EndLuaFunction(L, 5);

	
	}

	//UI의 크기를 늘립니다. 
	INT32 pLength = itemDesc.length();
	INT32 pAddsize = 0;
	if( pLength > 140)
	{
		pAddsize = 40;
	}
	
	m_pInfoCtrl[0]->setSize(m_pOriginalsize[0].x, m_pOriginalsize[0].y + pAddsize);
	m_pInfoCtrl[1]->setSize(m_pOriginalsize[1].x, m_pOriginalsize[1].y + pAddsize);
	m_pInfoCtrl2->setSize(m_pOriginalsize[2].x, m_pOriginalsize[2].y + pAddsize);
	
}


UIToolTipShop_NonWeapon::UIToolTipShop_NonWeapon()
{
	m_pInfoCtrl[0]			= NULL;
	m_pInfoCtrl[1]			= NULL;
	m_pInfoCtrl2			= NULL;
	memset( &m_pOriginalsize, 0, sizeof(VEC2D)*3);
}
UIToolTipShop_NonWeapon::~UIToolTipShop_NonWeapon()
{
}
void UIToolTipShop_NonWeapon::OnCreate(i3GameNode* pParent)
{
	base::OnCreate(pParent);
	AddScene("Scene/Main/PBRe_Tooltip_itemSlot.i3UIs");

}

const char* 	UIToolTipShop_NonWeapon::GetImageSetName() const
{
	return "i3UIButtonImageSet1";
}


/*virtual*/ void UIToolTipShop_NonWeapon::_InitializeAtLoad( i3UIScene * pScene)
{
	UIToolTipBase::_InitializeAtLoad( pScene);

	m_pInfoCtrl[0] = (i3UIFrameWnd *)pScene->FindChildByName("main");
	m_pInfoCtrl[1] = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd3_LCR");
	m_pInfoCtrl2 = (i3UIStaticText *)pScene->FindChildByName("i3UIStaticText8_LCR");

	m_pOriginalsize[0] = *m_pInfoCtrl[0]->getSize();
	m_pOriginalsize[1] = *m_pInfoCtrl[1]->getSize();
	m_pOriginalsize[2] = *m_pInfoCtrl2->getSize();

}

