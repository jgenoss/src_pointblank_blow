#include "pch.h"
#include "ShopDef.h"

#include "GlobalVariables.h"
#include "ClientDef.h"
#include "Shop.h"
#include "UI/UIUtil.h"

#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/generic_string_cat.h"

#if legacy_gui_function
	#include "GuiNotifyReceiver.h"
#endif

#include "GameLoginContext.h"
#include "UserInfoContext.h"
#include "StageBattle/NetworkContext.h"

#include "SteamContext.h"


#if defined( LOCALE_ITALY )

// AES암호화 라이브러리 함수 원형
int GetBaseKey(int Base);
int	GetPVKey(WORD idx, char* PV_KEY);
int GetSiteCodeEncIdx(char* szSiteCode);

int PLAES2Encrypt(char* szPlain,  char* szChiper, char* szPKEY);
int PLAES2Decrypt(char* szChiper, char* szPlain,  char* szPKEY);
// AES암호화 라이브러리 함수 원형

#endif

void	CopyPriceTypeText(GOODS_PRICE_TYPE PriceType, i3::rc_wstring& outText)
{
	switch(PriceType)
	{
	case GOODS_PRICE_POINT:
		outText = GAME_RCSTRING("STBL_IDX_SHOP_MONEY");
		break;
	case GOODS_PRICE_CASH:
		outText = GAME_RCSTRING("STR_SHOP_CASH");
		break;
	default:
		I3ASSERT_0;
		outText.clear();
		break;
	}
}

bool operator==(const GOODS_PRICE_INFO & lhs, const GOODS_PRICE_INFO & rhs)
{
	if( lhs._Cash != rhs._Cash )			return false;
	if( lhs._Point != rhs._Point )			return false;
	if( lhs._Type != rhs._Type )			return false;
	if( lhs._SaleMark != rhs._SaleMark )	return false;

	return true;
}

bool operator!=(const GOODS_PRICE_INFO & lhs, const GOODS_PRICE_INFO & rhs)
{
	return !operator==(lhs, rhs);
}

bool operator==(const SHOP_GOODS & lhs, const SHOP_GOODS & rhs)
{
	if( lhs._GoodsID != rhs._GoodsID )						return false;
	if( lhs._AuthType != rhs._AuthType )					return false;
	if( lhs._AuthArg != rhs._AuthArg )						return false;
	if( lhs._ItemID != rhs._ItemID )						return false;
	if( lhs._SaleDesignation != rhs._SaleDesignation )		return false;

	for(INT32 idx = 0 ; idx < PRICE_TYPE_MAX; ++idx)
		if( lhs._PriceInfo[idx] != rhs._PriceInfo[idx] )	return false;

	return true;
}

bool operator!=(const SHOP_GOODS & lhs, const SHOP_GOODS & rhs)
{
	return !operator==(lhs, rhs);
}

bool operator==(const SHOP_GOODS_PACK & lhs, const SHOP_GOODS_PACK & rhs)
{
	INT32 size1 = lhs.GetGoodsCount();
	INT32 size2 = rhs.GetGoodsCount();

	if( size1 != size2 ) return false;

	for(INT32 i=0;i<size1;++i)
	{
		if( *lhs.GetGoods(i) != *rhs.GetGoods(i) ) 
			return false;
	}

	return true;
}

bool operator!=(const SHOP_GOODS_PACK & lhs, const SHOP_GOODS_PACK & rhs)
{
	return !operator==(lhs, rhs);
}


SHOP_GOODS_PACK::SHOP_GOODS_PACK()
{

}

SHOP_GOODS_PACK::~SHOP_GOODS_PACK()
{
	ClearAll();
}

INT32		SHOP_GOODS_PACK::GetGoodsCount(void) const
{
	return (INT32)_pGoodsList.size();
}

bool		SHOP_GOODS_PACK::AddGoods(SHOP_GOODS * pAddGoods)
{
	_pGoodsList.push_back(pAddGoods);
	return true;
}

SHOP_GOODS *	SHOP_GOODS_PACK::GetGoods(INT32 index) const
{
	if( index < 0 || (INT32)_pGoodsList.size() <= index)
		return nullptr;

	return _pGoodsList[index];
}

void	SHOP_GOODS_PACK::Sort(bool (*pProc)( const SHOP_GOODS*, const SHOP_GOODS*))
{
	i3::sort(_pGoodsList.begin(), _pGoodsList.end(), pProc);
}

void	SHOP_GOODS_PACK::ClearAll()
{
	i3::cu::for_each_SafeFree_clear(_pGoodsList);
}

INT32	SHOP_GOODS_PACK::GetDisplayShopGoodsIndex(bool bigger)
{
	if (!bigger)	return 0; //기존꺼 그대로 유지.나중에 필요한 사람이 구현하면 될듯.	
	else
	{
		INT32 idx = 0;

		INT32 point_idx = -1;
		INT32 cash_idx  = -1;
		UINT32 point	= 0;
		UINT32 cash		= 0;

		INT32 count = GetGoodsCount();
		for (INT32 i = 0; i < count; ++i)
		{
			if (GetGoods(i)->_SaleType > GOODS_SALE_BUY)	continue;
			
			if (GetGoods(i)->GetPricePoint(PRICE_TYPE_SHOP_REAL) != 0 && GetGoods(i)->GetPricePoint(PRICE_TYPE_SHOP_REAL) >= point)
			{
				point		= GetGoods(i)->GetPricePoint(PRICE_TYPE_SHOP_REAL);
				point_idx	= i;
			}

			if (GetGoods(i)->GetPriceCash(PRICE_TYPE_SHOP_REAL) != 0 && GetGoods(i)->GetPriceCash(PRICE_TYPE_SHOP_REAL) >= cash)
			{
				cash		= GetGoods(i)->GetPriceCash(PRICE_TYPE_SHOP_REAL);
				point_idx	= i;
			}
		}

		// 무조건적으로 포인트 값이 표시되는게 맞는데, 캐쉬 값만 있으면 캐쉬에서 가장 큰 가격을 출력
		if (point_idx != -1)	 idx = point_idx;
		else if (cash_idx != -1) idx = cash_idx;
		
		return idx;
	}
}


GOODS_PRICE_TYPE	SHOP_GOODS_PACK::GetPriceTypeAll(INT32 price_idx)
{
	GOODS_PRICE_TYPE PriceTypeAll = GOODS_PRICE_UNKNOWN;

	INT32 count = GetGoodsCount();
	for ( INT32 i = 0 ; i < count ; ++i )
	{
		PriceTypeAll |= GetGoods(i)->GetPriceType(price_idx);
	}

	return PriceTypeAll;
}


EQUIP::GOODS EQUIP::EquipGoods( const SHOP_GOODS * pGoods, bool bUpdateViewModel /*= true*/)
{
	I3ASSERT( pGoods != nullptr);
	
	ITEM_TYPE itemType = (ITEM_TYPE) GET_ITEM_TYPE( pGoods->_ItemID);

	//	아이템을 인벤에 장비
	switch( itemType)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		{
			if( g_pFramework->EquipWeaponToMyInven( pGoods->_ItemID, bUpdateViewModel ))		//	무기 장착
			{
				return EQUIP::GOODS_WEAPON;	//	장비된 무기가 변경됬다.
			}

		}
		break;
	case ITEM_TYPE_CHARA:
		CCharaInfoContext::i()->SetTeamSlotByCharaID(pGoods->_ItemID);
	case ITEM_TYPE_HEADGEAR:
	case ITEM_TYPE_FACEGEAR :
	case ITEM_TYPE_UPPER :
	case ITEM_TYPE_LOWER :
	case ITEM_TYPE_GLOVE :
	case ITEM_TYPE_BELT :
	case ITEM_TYPE_HOLSTER :
	case ITEM_TYPE_SKIN :
	case ITEM_TYPE_BERET :
		{
			if( g_pFramework->EquipCharacterToMyInven( pGoods->_ItemID))	//	캐릭터 장착
			{
				return EQUIP::GOODS_CHARA;	//	장비된 캐릭터가 변경됬다.
			}
		}
		break;
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:	
	case ITEM_TYPE_TEMP:
		break;
	}

	return EQUIP::GOODS_NONE;
}

// 기간제 아이템의 판매 기간을 계산합니다.
INT32	GetMaintenanceItemDay( T_ItemID ItemID, UINT8 AuthType, UINT32 AuthArg)
{
	// 지속성 아이템이 아니라면 리턴합니다.
	if ( GET_ITEM_TYPE(ItemID) != ITEM_TYPE_MAINTENANCE )
		return -1;

	INT32	Day = -1;
	switch(AuthType)
	{
	case ITEM_TYPE_COUNT	:	Day = GET_ITEM_TIME(ItemID);		break;
	case ITEM_TYPE_TIME		:	Day = ConvertSecToUseDay(AuthArg);		break;
	}

	return Day;
}
// 기간제 아이템의 판매 시간을 계산합니다.
INT32	GetMaintenanceItemHour( T_ItemID ItemID, UINT8 AuthType, UINT32 AuthArg)
{
	// 지속성 아이템이 아니라면 리턴합니다.
	if ( GET_ITEM_TYPE(ItemID) != ITEM_TYPE_WRAP_PERIOD )
		return -1;

	INT32	Hour = -1;
	switch(AuthType)
	{
	case ITEM_TYPE_COUNT	:	Hour = GET_ITEM_TIME(ItemID); Hour = Hour * 24; break;
	case ITEM_TYPE_TIME		:	Hour = ConvertSecToUseHour(AuthArg);		break;
	}

	return Hour;
}


UINT8 GetItemSaleMarkIndex( INT32 ItemType )
{
	switch (ItemType)
	{
	case ITEM_ATTR_TYPE_PC_1	:
	case ITEM_ATTR_TYPE_PC_2	:	return GOODS_MARK_PCROOM;		// PC방용
	case ITEM_ATTR_TYPE_SPECIAL	:	return GOODS_MARK_EVENT;		// 이벤트용
	}

	return 0;
}

bool CheckYourRank( UINT8 bCheckRank)
{
	UINT32 wRank = UserInfoContext::i()->GetRealRank();

	//	계급 이상만 확인
	if( wRank >= bCheckRank)
		return true;

	return false;
}

bool IsCashBuyingFromGoods( SHOP_GOODS * pGoods, UINT8 price_idx)
{
	GOODS_PRICE_INFO info;
	i3mem::FillZero(&info, sizeof(GOODS_PRICE_INFO));

	CShop * shop = CShop::i();
	shop->GetGoodsPrice(pGoods->_GoodsID, info, price_idx);

	if (0 < info._Cash)
		return true;

	return false;
}

bool ExcuteSNDAWeb(void)
{
	SIZE32	Size = {0,0};
	Size.w	= 445;
	Size.h	= 445;

	//char postData[1024] = "";
	//char szChargeURL[1024] = "";
	//char szChargeURL2[1024] = "";
	i3::rc_wstring URL = L"http://www.naver.com";
	GameUI::WebBox( GAME_RCSTRING("STR_WEB_POPUP_CAPTION"), Size, URL);
	return true;
	//return ::ExecuteInternetExplorer( &URL[0], &postData[0], true);
}

bool ExcuteCashWeb(void)
{
	//
	// 캐쉬 충전이 활성화 되어 있는지 확인한다.
	//
	if ( ! UserInfoContext::i()->IsChargeCashEnable() )
		return false;

	if ( g_pConfigEx->GetShop().ChargeCash_Preparing )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_CHARGE_CASH_PREPARING"));
		return false;
	}
	//
	// 게임 내에서 팝업을 띄운다.
	//
	if ( g_pConfigEx->GetShop().ChargeCash_InGame )
	{
		SIZE32	Size = {0,0};
		Size.w	= g_pConfigEx->GetShop().ChargeCash_InGameSizeWidth;
		Size.h	= g_pConfigEx->GetShop().ChargeCash_InGameSizeHeight;

#ifdef LOCALE_NORTHAMERICA
		// 북미/스팀 : 아래 코드 사용하지 않습니다.

		//UINT128 code;
		//char strMD5Key[33] = {};
		//i3MD5::Gen( (char*)(GameLoginContext::i()->getUserPW()), i3::generic_string_size( GameLoginContext::i()->getUserPW()), (char *) &code);
		//i3MD5::GetMD5String(strMD5Key, code, sizeof(strMD5Key));

		//INT32 iLevel = UserInfoContext::i()->GetMyRank();
		//i3::rc_wstring wstrTemp;
		//
		//i3::sprintf(wstrTemp, UserInfoContext::i()->GetBillingURL(), GameLoginContext::i()->getUserID(), strMD5Key, iLevel);	// 자동 형변환될것 같음 (2014.07.21.수빈)

		//GameUI::WebBox( GAME_RCSTRING("STR_WEB_POPUP_CAPTION"), Size, wstrTemp);

#elif defined( LOCALE_ITALY )

		char	PV_KEY[16]				= {0,};		//Base키 16자리 이용(변경불가)

		char	szRaw[4096+1]			= {0,};		//암호화할 문자열
		char    szSiteCode[]			= "ACITL";	//암호화시 이용되는 SITECODE값(변경불가)
		char	szEncrypted[8192+16+1]	= {0,};

		_i64toa_s( NetworkContext::i()->UID, szRaw, 4096+1, 10 );

		int nSiteCode = GetSiteCodeEncIdx(szSiteCode);

		GetPVKey((WORD)nSiteCode, PV_KEY);	//사전 정의된 SITECODE값으로 사설키 설정(변경불가)
		PLAES2Encrypt(szRaw, szEncrypted, PV_KEY);				//암호화

		// 자동 형변환될것 같음 (2014.07.21.수빈)
		i3::rc_wstring wstr_temp = L"%s?g_MemberNo=%s&sitecode=%d";
		i3::rc_wstring wstr_url = i3::format_string(wstr_temp, UserInfoContext::i()->GetBillingURL(), szEncrypted, nSiteCode );
#else
		GameUI::WebBox( GAME_RCSTRING("STR_WEB_POPUP_CAPTION"), Size, UserInfoContext::i()->GetBillingURL() );
#endif
		
		return true;
	}


	//
	// COM을 이용해서 웹브라우저로 충전페이지를 연결시켜준다.
	//
	char postData[1024] = "";
	i3::rc_wstring wstrChargeURL, wstrChargeURL2;
	i3::wstring wstr_url;

	wstrChargeURL = UserInfoContext::i()->GetBillingURL();
	wstrChargeURL2 = UserInfoContext::i()->GetBillingURL();					// TODO : URL 과 URL2가 같아도 되는건지....해당 작업자쪽에서 체크 요망..(2014.07.21.수빈)

	if (SteamContext::i()->IsSteamUser())
	{
		char strURL[1024] = { 0, };
		char strTemp[1024] = { 0, };

		i3::utf16_to_mb(wstrChargeURL, strURL, 1024);
		i3::snprintf(strTemp, 1024, "?AccountUID=%s&SteamID=%s", GameLoginContext::i()->getBillingID(), GameLoginContext::i()->GetExtUID());
		i3::generic_string_cat(strURL, strTemp);

		// Steam 캐시충전 팝업은 GET방식으로 Steam 자체 브라우저를 띄웁니다
		if (SteamContext::i()->ActivateWebPage(strURL))
			return true;
	}

#if defined( LOCALE_KOREA)
	{
		i3::wstring wstrSessionID;	i3::mb_to_utf16(GameLoginContext::i()->GetSessionID(), wstrSessionID);
		wstr_url = wstrChargeURL;
		wstr_url += wstrSessionID;
	}
#elif defined( LOCALE_TURKEY )	// revision 54585
	{
		i3::snprintf( postData, 1024, "accountUid=%s&tempKey=%s", GameLoginContext::i()->GetLoginUID(), GameLoginContext::i()->GetLoginKey());
		wstr_url = wstrChargeURL;
	}
#elif defined( LOCALE_ITALY )
	{
		char	PV_KEY[16]				= {0,};		//Base키 16자리 이용(변경불가)

		char	szRaw[4096+1]			= {0,};		//암호화할 문자열
		char    szSiteCode[]			= "ACITL";	//암호화시 이용되는 SITECODE값(변경불가)
		char	szEncrypted[8192+16+1]	= {0,};

		_i64toa_s( NetworkContext::i()->UID, szRaw, 4096+1, 10 );

		int nSiteCode = GetSiteCodeEncIdx(szSiteCode);

		GetPVKey((WORD)nSiteCode, PV_KEY);	//사전 정의된 SITECODE값으로 사설키 설정(변경불가)
		PLAES2Encrypt(szRaw, szEncrypted, PV_KEY);				//암호화

		//** Post 방식 **//
		//wstr_url = wstrChargeURL;
		//i3::snprintf( postData, 1024, "g_MemberNo=%s&sitecode=%d", szEncrypted, nSiteCode);

		//** Get 방식**//
		i3::rc_wstring wstr_temp = L"%s?g_MemberNo=%s&sitecode=%d";
		wstr_url = i3::format_string(wstr_temp, wstrChargeURL, szEncrypted, nSiteCode);	// 자동 형변환될것 같음 (2014.07.21.수빈)
	}
#elif defined( LOCALE_PHILIPPINES)	// 필리핀 임시 가레나 로그인 페이지로 이동
	wstr_url = wstrChargeURL;
#elif defined( LOCALE_SINGMAL)
	wstr_url = wstrChargeURL;
#elif defined(LOCALE_JAPAN)
	/************************************************************************/
	/*             일본 충전용 웹 사이트 접속을 위한 데이터					*/
	/*																		*/
	/* 1. GameID : Game Code (PB는 CM001014 고정)							*/
	/* 2. USN : userIndex (ArGameStartInfo::userIndex)						*/
	/* 3. ServerID : -1 (PB에서 사용 안함)									*/
	/* 4. CID : -1 (PB에서 사용 안함)										*/
	/************************************************************************/
	i3::string str_game_id = "CM001014";	// Arario 등록된 게임 코드 (고정)

	////** Post 방식 **//
	//wstr_url = wstrChargeURL;
	//i3::snprintf( postData, 1024, "GameID=%s&USN=%d&ServerID=%d&CID=%d", str_game_id.c_str(), GameLoginContext::i()->GetArarioUSN(), -1, -1);

	////** Get 방식**//
	i3::rc_wstring wstr_temp = L"%s?GameID=%s&USN=%d&ServerID=%d&CID=%d";
	wstr_url = i3::format_string(wstr_temp, wstrChargeURL, str_game_id.c_str(), GameLoginContext::i()->GetArarioUSN(), -1, -1);	// 자동 형변환될것 같음 (2014.07.21.수빈)
#else
	{
		i3::string strChargeURL2;		i3::utf16_to_mb(wstrChargeURL2, strChargeURL2);
		i3::snprintf( postData, 1024 , strChargeURL2, GameLoginContext::i()->GetSessionID());
		wstr_url = wstrChargeURL;
	}

#endif	

#if  defined( LOCALE_RUSSIA )
	{
		//	유료 코인 충전 웹페이지를 띄운다.
		g_pFramework->OpenWebBrowserCurrSet(wstr_url.c_str());
		return true;
	}
#else
	//	유료 코인 충전 웹페이지를 띄운다.
	return ::ExecuteInternetExplorer( wstr_url.c_str(), &postData[0], true);
#endif
}


//	톨 충전용 인터넷익스플로러 띄우기 (NC에서 받은 소스)
bool ExecuteInternetExplorer(LPCWSTR url, LPCSTR postData /*= nullptr*/, bool isPopup /*= false*/)
{
	if (wcslen(url) == 0)
		return false;

	bool result = false;
	IWebBrowser2* pNewWebBrowser = nullptr;
	HRESULT hr;

	if (SUCCEEDED((hr = CoCreateInstance(CLSID_InternetExplorer, nullptr, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pNewWebBrowser))))
	{
		CComVariant vtUrl(url);
		CComVariant vtEmpty;
		CComVariant vtHeader;
		CComVariant vtPostData;
		BSTR bstrHeaders = SysAllocString(L"Content-Type: application/x-www-form-urlencoded\r\n");

		// KB Q167658
		if (postData)
		{
			if (bstrHeaders)
			{
				vtHeader.vt = VT_BSTR;
				vtHeader.bstrVal = bstrHeaders;
			}
			LPSAFEARRAY psa;

			UINT32 cElems = strlen(postData);

			LPSTR pPostData;

			psa = SafeArrayCreateVector(VT_UI1, 0, cElems);

			if (psa)
			{
				HRESULT hr2 = SafeArrayAccessData(psa, (LPVOID*)&pPostData);

				if (SUCCEEDED(hr2))
				{
					memcpy(pPostData, postData, cElems);
//					hr = 
					SafeArrayUnaccessData(psa);
				}

				vtPostData.vt = VT_ARRAY | VT_UI1;

				vtPostData.parray = psa;
			}
		}

		if (SUCCEEDED(pNewWebBrowser->Navigate2(&vtUrl, &vtEmpty, &vtEmpty, &vtPostData, &vtHeader)))
		{
			long hwnd;

			if (SUCCEEDED(pNewWebBrowser->get_HWND(&hwnd)))
			{
				::ShowWindow((HWND)hwnd, SW_SHOW);
				::SetWindowPos((HWND)hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

				if (isPopup)
				{
					pNewWebBrowser->put_Silent(VARIANT_FALSE);
					pNewWebBrowser->put_AddressBar(VARIANT_FALSE);
					pNewWebBrowser->put_ToolBar(0);
					pNewWebBrowser->put_StatusBar(VARIANT_FALSE);
					pNewWebBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
					pNewWebBrowser->put_MenuBar(VARIANT_FALSE);
				}

				result = true;
			}
		}
		else
		{
			result = false;
		}

		::SysFreeString(bstrHeaders); 
		pNewWebBrowser->Release();
	}
	else
	{
		result = false;
	}

	return result;
}


void	FormatHaveItemTermText(i3::stack_wstring& out, UINT32 Term)
{
	if (48 <= Term)
	{
		Term = Term / 24;
		//	보유중 : %d일
		i3::sprintf(out, L"%s : {col:255,255,255,255}%d%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_HAVE_TERM"), Term, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
	}
	else if( 24 <= Term  )
	{
		Term = Term / 24;
		i3::sprintf(out, L"%s : {col:210,0,0,255}%d%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_HAVE_TERM"), Term, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
	}
	else
	{
		//	보유중 : 1일 미만
		i3::sprintf(out, L"%s : {col:210,0,0,255}%d%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_HAVE_TERM"), Term, GAME_RCSTRING("STR_UI_HOUR"));
	}
}


bool	GetEnduranceZero(UINT8 InvenItemAttr, UINT32 InvenItemArg)
{
	// 보유중인 아이템인데 보유개수나 기간이 0일 경우 입니다.
	// itemArg는 개수제이거나 기간제일때 의미가 다르지만
	// 더이상 사용할수 없다는 의미로 itemArg == 0으로 비교합니다.. (영권)
	if ( (ITEM_ATTR_TYPE_BUY == InvenItemAttr || ITEM_ATTR_TYPE_AUTH == InvenItemAttr)
		&& InvenItemArg == 0 )
		return true;
	return false;
}

bool	CanUse_ViewOtherUserItemInfo()
{
	T_ItemID ItemID = CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO;

	// 판매되지 않거나 가지고 있지 않으면 사용할 수 없음.
	if ( ! CShop::i()->GetItemData(ItemID) ) return false;
	if ( ! CInvenList::i()->IsUsingCashItem(ItemID) ) return false;

	return true;
}
