//#pragma once
#if !defined( __UI_PHASE_GACHA_H__)
#define __UI_PHASE_GACHA_H__

#include "UIPhaseSubBase.h"
#include "UIItemInfoUtil.h"
#include "UISlide.h"
#include <queue>

#include "../PurchaseTimeCheck.h"

class UIImgSetCtrl;
class UIGachaView;

#define GACHA_OTHERITEM_MAX 5
#define GACHA_MAXANITIME 2.0

namespace GACHA_SLOTDATA
{
	struct SLOT_ANI
	{
		i3UIButtonImageSet*			pAniImage;
		REAL32						iAniTime;
		REAL32						iMaxAniTime;
		INT32						iShape;
		INT32						iMaxShape;

		bool						bPlay;
		SLOT_ANI():	pAniImage(nullptr), iAniTime(0), iMaxAniTime(0.05f),iShape(0),
					iMaxShape(20), bPlay(false)
		{

		}
		void SetButton(i3UIButtonImageSet* btn)
		{
			pAniImage = btn;
		}
		void SetStartAni(VEC3D * pos)
		{
			VEC3D* temppos = pAniImage->getPos();
			pAniImage->EnableCtrl(true);
			temppos->y = pos->y-2;
			temppos->x = pos->x+1;
			pAniImage->setPos(temppos);
			pAniImage->SetShapeIdx(0);
			bPlay = true;
			iAniTime = 0;
			iShape = 0;
		}

		void OnUpdate(REAL32 rDeltaSeconds )
		{
			if( pAniImage == nullptr)
				return;

			if( bPlay == true)
			{
				iAniTime += rDeltaSeconds;
				if( iAniTime > iMaxAniTime)
				{
					pAniImage->SetShapeIdx(iShape);
					iShape++;
					iAniTime = 0;
					if( iShape > iMaxShape)
					{
						pAniImage->EnableCtrl(false);
						bPlay = false;
						iShape = 0;
					}
				}
			}

		}
	};


}


class UIPhaseGachaShop :	public UIPhaseSubBase
{
	I3_CLASS_DEFINE( UIPhaseGachaShop, UIPhaseSubBase);

	friend class GachaRstProcMsgBoxCallBack;

public:		//Base Class 
	UIPhaseGachaShop(void);
	virtual ~UIPhaseGachaShop(void);

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart( void) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual void	Leave( void) override;
	virtual bool	OnKeyInput(i3InputDeviceManager * pInputMng) override;
	virtual bool	OnClick( i3UIControl * pControl) override;

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt,INT32 arg, const i3::user_data& UserData) override;
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code) override;
	
	void DisableUIInput(bool disable);

	enum EGACHA_RST_PROCST
	{
		EGACHA_RST_PROCST_NONE = 0,			// 가차 결과 패킷 처리중이 아님
		
		EGACHA_RST_PROCST_WAITRST_FROM_SVR,	// 서버에서 결과 패킷들 기다리는중

		EGACHA_RST_PROCST_REWARD_START,		// 결과 패킷시상

		EGACHA_RST_PROCST_UPDATE,

		EGACHA_RST_PROCST_ERR_MSGBOX,		// 메시지박스 출력후 기다리는중

		EGACHA_RST_PROCST_ERR_FATAL,		// 결과패킷중에 오류패킷 발견으로 클라이언트 즉시 종료
	};

	enum EGACHA_PURCHASEAMOUNT
	{
		EGACHA_PURCHASEAMOUNT_NONE = 0,		// 아무 것도 안 산 경우
		EGACHA_PURCHASEAMOUNT_20,
		EGACHA_PURCHASEAMOUNT_40,
		EGACHA_PURCHASEAMOUNT_60,
		EGACHA_PURCHASEAMOUNT_80,
		EGACHA_PURCHASEAMOUNT_FULL = 6,

		EGACHA_PURCHASEAMOUNT_COUNT,
	};

	enum EGACHA_CLICK
	{
		EGACHA_NON_CLICK = -1,
		EGACHA_FIRST_CLICK = 0,
		EGACHA_SECOND_CLICK,
		EGACHA_THIRD_CLICK,

		EGACHA_CLICK_COUNT,
	};

	void ResetGachaRstProcessing();
	bool IsInGachaRstProcessing() { return m_gachaRstProcStatus != EGACHA_RST_PROCST_NONE ? true : false; }
	void UpdateGachaRstProcessing(REAL32 rDeltaSeconds);

	static const wchar_t* GenerateGachaRstErrMsg(GACHA_ERROR err);
	static void	CallBackFatalErrExitMsgBox( void* pThis, INT32 nParam);

	i3::string ConvertGameStr(const char* gameStrTitle);

protected:
	INT32			GetCurrentDataIndex(INT32 ikey) 
	{
		for(INT32 i = 0; i < WIN_ITEM_COUNT_BY_TAB; i++)
		{
			if( m_pGachaGroup[m_CurrentTab]->m_arWinItems[i].m_i32ItemKey == ikey)
				return i;
		}
		return -1;
	}
		

public:		//루아 글루 함수 호출
	void			ByeOpenPopup();
	void			Click_Weapon_Detail();
	void			Click_LuckyItem_Detail();
	void			GachaStart(EGACHA_CLICK Amount);
	void			Clieck_Cash();
	void			Click_Help();

public:	//루아 UI 세팅 호출
	void 					UpdateGachaItem( void);
	void					UpdateGachaRight( void);
	void					_InitGachaItemList(INT32 i32Tab);
	void					_SetLuckyItemDetail();
	void 					_SetItemInfo();
	EGACHA_PURCHASEAMOUNT	__GetAmount(const GACHA_ITEM_INFO* pInfo);
	void					__SetItemListInfo(INT32& slot, i3::vector<const GACHA_ITEM_INFO* >* inven_list, i3::rc_wstring color);
	void					_SetOtherItem();
	void					_SetWeaponDetail(T_ItemID itemID);
	void 					_UpdateItemAni();
	void 					_SelectComboItem(INT32 Slot, INT32 ComboIdx);
	void					_SelectTab(INT32 i32Tab);
	void					_SelectSearch(const i3::wliteral_range& wKeywordRng);
	void					_ListSort(INT32 SortValue);
	void					_UpdateFlowAnnounce(REAL32 rDeltaSeconds);
	void					_SetAnnounce( );
	void					SetSlideNotice_Permanant( const i3::rc_wstring& wStr);
	void					SetSlideNotice_Temporary(const i3::rc_wstring& wStr);
	void					SetComboItem();
	void					SetButtonText();
	const GACHA_ITEM_INFO* 	GetSelectGachaInfo() 
	{
		// list empty
		if(m_ItemList_Cash[m_CurrentTab].empty() && m_ItemList_Point[m_CurrentTab].empty()) 
			return nullptr;

		// (index < 0) or (index >= list.size()) 
		if(m_CurrentIdx < 0 || m_CurrentIdx >= 
					(INT32) (m_ItemList_Cash[m_CurrentTab].size() + m_ItemList_Point[m_CurrentTab].size()))
			return nullptr;

		if(m_CurrentIdx < (INT32)m_ItemList_Cash[m_CurrentTab].size())
			return m_ItemList_Cash[m_CurrentTab].at(m_CurrentIdx);
		else
			return m_ItemList_Point[m_CurrentTab].at(m_CurrentIdx - (INT32)m_ItemList_Cash[m_CurrentTab].size());
	}
	UIGachaView*			getGachaView()		{return m_pGachaView;	}
	EGACHA_CLICK			getGachaAmount()	{ return m_iGachaAmount; }

	//UI::tmSlide		m_Slide;
	void			slide_proc();
	UI::TCallBack<UIPhaseGachaShop, &UIPhaseGachaShop::slide_proc> m_slide_call;
protected:	//맴버 변수
	GACHA_GROUP							m_CurrentTab;
	INT32								m_CurrentIdx;
	EGACHA_CLICK						m_iGachaAmount;
	//i3::vector<const GACHA_ITEM_INFO*>	m_ItemList[GACHA_GROUP_COUNT];
	i3::vector<const GACHA_ITEM_INFO*>	m_ItemList_Point[GACHA_GROUP_COUNT];
	i3::vector<const GACHA_ITEM_INFO*>	m_ItemList_Cash[GACHA_GROUP_COUNT];
	UIGachaView *						m_pGachaView;	// 3d 캐릭터 모델 뷰

	i3UIListView_Box *					m_pItemListBox;
	i3UIEditBox *						m_pItemListText;
	UIImgSetCtrl*						m_goods_img_ctrl;
	UIImgSetCtrl*						m_OtherItemimg_ctrl[RANDOM_ITEM_COUNT_BY_ITEM];
	i3UIButtonImageSet*					m_pAniImage;
	REAL32								m_AniTime;
	GACHA_SLOTDATA::SLOT_ANI			m_AniSlot;

	i3UIButtonComposed3*				m_ClickButton[3];

	i3UIScene *							m_pLeftMenuScene;
	i3UIControl*						m_pCtrlListView;
	i3UIControl*						m_pCtrlButtonList1;
	i3UIControl*						m_pCtrlButtonList2;
//-------------------Gacha 결과패킷처리----------------------------------------------------------	
	EGACHA_RST_PROCST					m_gachaRstProcStatus;		// 현재 가차결과 처리상황

	i3UIEditBox*						m_SearchEditBox;

	class GachaRstProcessor : public CMsgBoxCallBack
	{
	public:
		static void OnMsgBoxCallback(void* pThis, INT32 nParam);

	public:
		GachaRstProcessor();
		~GachaRstProcessor();

		bool Update(REAL32 rDeltaSec);

		void WaitGachaResultPopup();
		void WaitMessageBox();

		void Stop();

		i3::rc_wstring					m_msgBoxStr;

	private:
		void OnMsgBoxResult(INT32 nParam);
		void CloseMsgBox();

		UIMessageBox*					m_msgBox;
		
		INT32							m_isWaiting; // 0= no wait, 1=UPW_GACHA_RESULT, 2=GameUI::MsgBox

	public:
		UIPhaseGachaShop*				m_owner;
	};

	GachaRstProcessor					m_gachaRstProc;

//-------------------Gacha TimeOut Check---------------------------------------------------------
	PurchaseTimeCheck					m_purchaseTimecheck;
	PurchaseTimeCheck					m_UpdateStateTime;

	void								PurchaseTimeOut();
	void								UpdateStateTimeCheck();

//-------------------Gacha Announce--------------------------------------------------------------
	i3UIEditBox *						m_pAnnounce;

	REAL32								m_rFlowTime;
	bool								m_bFCMFlag;	
	bool								m_bFlowFlag;
	i3::rc_wstring						m_strPermanatNotice;
	i3::rc_wstring						m_strTemporaryNotice;

	bool								m_bItemListSend;
	bool								m_bIsLuckyItemDetail;
	const GACHA_GROUP_INFO*				m_pGachaGroup[GACHA_GROUP_COUNT];
//-----------------------------------------------------------------------------------------------
	void FocusOutItemListView();

public:
	//하동익 코드
	void								HDIObject1(bool isenable);


private:
	typedef void	(UIPhaseGachaShop::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Buy_Goods( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Gacha_Info( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_End_Gacha_State( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Start_Gacha( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Gacha_Announce( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Item_Auth( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Refresh_Cash( INT32, const i3::user_data&);
};

#endif