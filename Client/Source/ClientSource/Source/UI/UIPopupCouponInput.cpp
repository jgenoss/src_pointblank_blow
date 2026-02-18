#include "pch.h"
#include "UIPopupCouponInput.h"
#include "../CommunityContext.h"
#include "UIMainFrame.h"

#include "UIUtil.h"

#include "i3Base/string/compare/generic_is_iequal.h"


I3_CLASS_INSTANCE( UIPopupCouponInput);//, UIPopupBase);

bool CheckLength(const char *t1)
{
	if(i3::generic_string_size(t1) > GIFTCOUPON_LENGTH)
		return false;

	return true;
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupCouponInput_Confirm(LuaState * L)
	{
		UIPopupCouponInput * pThis = static_cast<UIPopupCouponInput*>(GameUI::GetPopup(UPW_COUPON_INPUT));
		if( pThis != nullptr)
			pThis->Confirm();
		return 0;
	}

	int UIPopupCouponInput_Cancel(LuaState * L)
	{
		UIPopupCouponInput * pThis = static_cast<UIPopupCouponInput*>(GameUI::GetPopup(UPW_COUPON_INPUT));
		if( pThis != nullptr)
			pThis->Cancel();
		return 0;
	}
}

LuaFuncReg UIPopupCouponInput_Glue[] = {
	{"Confirm",					UIPopupCouponInput_Confirm	},
	{"Cancel",					UIPopupCouponInput_Cancel	},
	{nullptr,						nullptr						}
};

//------------------------------------------------------------------------------//


UIPopupCouponInput::UIPopupCouponInput()
{
	m_bWaitingConfirmSubscripting = false;
	m_nOldLength = 0;
	m_pCouponBox = nullptr;
	i3mem::FillZero(m_szCurEditText, COUPON_MAX_LETTER_COUNT);
}

UIPopupCouponInput::~UIPopupCouponInput()
{
}

/*virtual*/ void UIPopupCouponInput::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene("Scene/Popup/PBRe_PopUp_InputCoupon.i3UIs",UIPopupCouponInput_Glue);
}

/*virtual*/ bool UIPopupCouponInput::OnEntranceStart(void * pArg1 , void * pArg2 )
{
	if( UIPopupBase::OnEntranceStart(pArg1,pArg2) == false)
		return false;

	m_bWaitingConfirmSubscripting = false;
	m_nOldLength = 0;

	return true;
}

/*virtual*/ void UIPopupCouponInput::OnEntranceEnd( void)
{
	UIPopupBase::OnEntranceEnd();

	if(m_pCouponBox)
	{
		m_pCouponBox->SetInputCharOnlyAlphaNumeric();  // 영문숫자입력모드
		m_pCouponBox->SetInputAlphabetAutoCase(1); // 대문자입력모드
	}

	LuaState * L = _CallLuaFunction("GetFirstEditBox");

	if(L != nullptr)
	{
		i3UIEditBox * pFirstEdit = reinterpret_cast<i3UIEditBox*>(UILua::ReturnLuaPointer(L , 0));
		g_pFramework->SetFocus(pFirstEdit);
	}
}

/*virtual*/ void UIPopupCouponInput::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_pCouponBox = (i3UIEditBox *) pScene->FindChildByName("i3UIEditInput1");
	m_pCouponBox->setAlphaNumericOnly(true);
}

/*virtual*/ bool UIPopupCouponInput::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	UIPopupBase::OnEvent( event, pObj, param2, code);

	return false;
}

void UIPopupCouponInput::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	if( evt == EVENT_COUPON_AWARD)
	{
		ReceiveGameEvent_Coupon_Award( arg, UserData);
	}
}

void UIPopupCouponInput::Confirm()
{
	LuaState* L;

	//알파벳을 제외한 문자가 버퍼에 들어오면 문제가 생길 수 있다.
	L = UILua::CallLuaFunction(GetScene(0), "GetInputText1");
	const char* _text1 = UILua::ReturnLuaString(L, 0);

	if( CheckLength(_text1) == false )
	{
		UIPopupCouponInput * pThis = static_cast<UIPopupCouponInput*>(GameUI::GetPopup(UPW_COUPON_INPUT));
		if( pThis != nullptr)
			pThis->NotifyFail(EVENT_ERROR_FAIL);
		return;
	}

	char Coupon_Num[GIFTCOUPON_LENGTH + 1] = {0};
	i3::generic_string_copy( Coupon_Num, _text1 );

	if( m_bWaitingConfirmSubscripting == true)
		return;

	GameEventSender::i()->SetEvent(EVENT_COUPON_AWARD, (void *)Coupon_Num);
	m_bWaitingConfirmSubscripting = true;
}

void UIPopupCouponInput::NotifyFail(INT32 Arg)
{
	switch( Arg ) {
		case EVENT_FAIL_COUPON_AWARD_ITEM_OF_BILLING_PROBLEM :
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_COUPON_INPUT_FAIL")); break;		// 쿠폰 인증에 문제가 있습니다.
		case EVENT_ERROR_NETWORK :			
		case EVENT_ERROR_COUPON_WRONG_NUMBER :
		case EVENT_ERROR_COUPON_ALREADY_USED :
		case EVENT_ERROR_COUPON_EXPIRED :
		default:
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_COUPON_INPUT_FAIL")); break;
	}
	GetMainFrame()->TogglePopup(UPW_COUPON_INPUT);
}

void UIPopupCouponInput::NotifySuccess()
{
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_COUPON_INPUT_SUCCESS"));
	GetMainFrame()->TogglePopup(UPW_COUPON_INPUT);
}

void UIPopupCouponInput::Cancel()
{
	GetMainFrame()->TogglePopup(UPW_COUPON_INPUT);
}

void UIPopupCouponInput::AutoFocusing(UINT32 nEditState)
{
	i3UIEditBox * pCurEdit = reinterpret_cast<i3UIEditBox*>(i3UI::getGlobalIme()->getOwner());

	if(pCurEdit == nullptr)
		return;

	if( nEditState == EDIT_STATE_CHANGE_INPUT_CHAR)
	{	
		WCHAR szText[MAX_STRING_COUNT] = {0};
		i3::string_ncopy_nullpad(szText, pCurEdit->getUIText()->getText(), MAX_STRING_COUNT);
		INT32 nLength = i3::generic_string_size(szText);
		
		i3UIEditBox * pNextEdit = nullptr;
		bool bNextFocus = true;

		// 제한 문자를 넘게 입력했을 경우 처리한다.
		// IME에 임시로 제한을 걸어주고 입력된 문자 버퍼를 클리어 후 재설정
		if(nLength > COUPON_MAX_LETTER_COUNT)
		{	
			nLength = COUPON_MAX_LETTER_COUNT;

			i3UI::getGlobalIme()->setLimit(nLength);
			i3UI::getGlobalIme()->SetInputText(m_szCurEditText, nLength);

			// KillFocus가 호출되어야, EditBox의 text가 IME에 입력된 문자열로
			// 적용되기 때문에 KillFocus후 다시 SetFocus를 해준다.
			pCurEdit->OnKillFocus(pCurEdit);
			pCurEdit->OnSetFocus();
		}

		bool bMoveToPrevEdit = i3::generic_is_iequal(m_szCurEditText, szText);
		//	i3String::NCompareW(m_szCurEditText, szText, MAX_STRING_COUNT) == 0;

		if(!bMoveToPrevEdit)
		{
			// 그냥 건너 띄는 케이스 
			// 1. [문자 입력] 문자 길이가 MAX_LETTER_COUNT 이하 0 이상,
			//		- 5글자 까지는 포커스 이동 없이 입력되어야한다.
			// 2. [백스페이스 입력] 문자길이 = 0, 이전 문자길이 1개 이상 
			//		- 이전 문자도 0인 경우에는 전 EditBox로 이동된다.
			if((nLength < COUPON_MAX_LETTER_COUNT && nLength > 0) || (nLength == 0 && m_nOldLength > 0))
			{
				i3::string_ncopy_nullpad(m_szCurEditText, szText, COUPON_MAX_LETTER_COUNT);
				m_nOldLength = nLength;					
				return;
			}
		}

		// 이전 에디트박스로 이동 (백스페이스 입력)
		// 1. 현재 에디트박스에 문자가 하나도 없는 상태.
		// 2. 캐럿이 에디트박스 맨 앞에 있는 경우.
		if((nLength == 0 && m_nOldLength == 0) || bMoveToPrevEdit)
		{
			bNextFocus = false;
		}

		// 키 입력시 Limit를 초과하면 다음 포커스로 이동한다.
		LuaState * L = _CallLuaFunction("ChangeFocusingToAuto");
		if( L != nullptr)
		{
			i3Lua::PushUserData(L , pCurEdit);
			i3Lua::PushBoolean(L , bNextFocus);
			pNextEdit = reinterpret_cast<i3UIEditBox*>(_ReturnLuaPointer(L , 2));
		}	

		i3::string_ncopy_nullpad(m_szCurEditText, szText, COUPON_MAX_LETTER_COUNT);
		m_nOldLength = nLength;		

		if(pNextEdit != nullptr && pNextEdit != pCurEdit)
		{
			g_pFramework->SetFocus(pNextEdit);
			m_nOldLength = i3::generic_string_size(pNextEdit->getUIText()->getText());
		}
	}
}

void UIPopupCouponInput::ReceiveGameEvent_Coupon_Award( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED( arg ) )
	{
		NotifySuccess();
	}
	else
	{
		NotifyFail( arg);				
	}

	m_bWaitingConfirmSubscripting = false;
}