#ifndef _STEAM_CONTEXT_H_
#define _STEAM_CONTEXT_H_

#include "../Steam/include/steam_api.h"

/* 
	LocaleCode 와 무관하게 steam_api.dll이 존재하면 이를 load하여 
	Steam에 로그인 처리가 가능하도록 만든 클래스입니다.
	
	dll이 존재할 경우
	[캐시충전] 기능은 무조건 SteamContext::ActivateWebPage() 를 실행하게 되며
	UserInfoContext::i()->GetBillingURL() 에 Get방식으로 BillingID/ExtUID 가 붙습니다.
*/

// 명시적 링크 구조로 인해 몇몇 템플릿(CCallResult/CCallback)을 사용할 수 없어서
// base를 상속받은 후, PB에서 사용할 수 있게 수정하여 정의함.
template< class T, class P >
class CCallResult_PB : private CCallbackBase
{
public:
	typedef void (T::*func_t)(P*, bool);

	CCallResult_PB()
	{
		m_hAPICall = k_uAPICallInvalid;
		m_pObj = nullptr;
		m_Func = nullptr;
		m_hDLL = nullptr;
		m_iCallback = P::k_iCallback;
	}

	void Set(SteamAPICall_t hAPICall, T *p, func_t func, HINSTANCE hDLL)
	{
		if (!hDLL)
		{
			return;
		}

		m_hDLL = hDLL;

		if (m_hAPICall)
		{
			void(*pSteamAPI_UnregisterCallResult)(class CCallbackBase*, SteamAPICall_t);
			pSteamAPI_UnregisterCallResult = (void(*)(class CCallbackBase*, SteamAPICall_t))GetProcAddress(m_hDLL, "SteamAPI_UnregisterCallResult");
			(*pSteamAPI_UnregisterCallResult)(this, m_hAPICall);
		}

		m_hAPICall = hAPICall;
		m_pObj = p;
		m_Func = func;

		if (hAPICall)
		{
			void(*pSteamAPI_UnregisterCallResult)(class CCallbackBase*, SteamAPICall_t);
			pSteamAPI_UnregisterCallResult = (void(*)(class CCallbackBase*, SteamAPICall_t))GetProcAddress(m_hDLL, "SteamAPI_RegisterCallResult");
			(*pSteamAPI_UnregisterCallResult)(this, hAPICall);
		}
	}

	bool IsActive() const
	{
		return (m_hAPICall != k_uAPICallInvalid);
	}

	void Cancel()
	{
		if (!m_hDLL)
			return;

		if (m_hAPICall != k_uAPICallInvalid)
		{
			void(*pSteamAPI_UnregisterCallResult)(class CCallbackBase*, SteamAPICall_t);
			pSteamAPI_UnregisterCallResult = (void(*)(class CCallbackBase*, SteamAPICall_t))GetProcAddress(m_hDLL, "SteamAPI_RegisterCallResult");
			(*pSteamAPI_UnregisterCallResult)(this, m_hAPICall);
			m_hAPICall = k_uAPICallInvalid;
		}

	}

	~CCallResult_PB()
	{
		Cancel();
	}

	void SetGameserverFlag() { m_nCallbackFlags |= k_ECallbackFlagsGameServer; }
private:
	virtual void Run(void *pvParam) override
	{
		m_hAPICall = k_uAPICallInvalid;
		(m_pObj->*m_Func)((P *)pvParam, false);
	}
	void Run(void *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall)
	{
		if (hSteamAPICall == m_hAPICall)
		{
			m_hAPICall = k_uAPICallInvalid;
			(m_pObj->*m_Func)((P *)pvParam, bIOFailure);
		}
	}
	int GetCallbackSizeBytes()
	{
		return sizeof(P);
	}

	SteamAPICall_t m_hAPICall;
	T *m_pObj;
	func_t m_Func;
	HINSTANCE	m_hDLL;
};

template< class T, class P>
class CCallback_PB : protected CCallbackBase
{
public:
	typedef void (T::*func_t)(P*);
	HINSTANCE m_hDLL;

	CCallback_PB(T *pObj, func_t func) : m_pObj(pObj), m_Func(func), m_hDLL()
	{
		if (pObj && func)
			Register(pObj, func);
	}

	~CCallback_PB()
	{
		if (m_nCallbackFlags & k_ECallbackFlagsRegistered)
			Unregister();
	}

	void SetDLL(HINSTANCE hDLL)
	{
		m_hDLL = hDLL;
	}
	// manual registration of the callback
	void Register(T *pObj, func_t func)
	{
		if (!pObj || !func)
			return;

		if (m_nCallbackFlags & k_ECallbackFlagsRegistered)
			Unregister();

		m_pObj = pObj;
		m_Func = func;

		if (m_hDLL)
		{
			void(*pSteamAPI_registerCall)(class CCallbackBase*, int);
			pSteamAPI_registerCall = (void(*)(class CCallbackBase*, int))GetProcAddress(m_hDLL, "SteamAPI_RegisterCallback");
			(*pSteamAPI_registerCall)(this, P::k_iCallback);
		}
	}

	void Unregister()
	{
		if (m_hDLL)
		{
			// SteamAPI_UnregisterCallback removes k_ECallbackFlagsRegistered
			void(*pSteamAPI_UnregisterCall)(class CCallbackBase*);
			pSteamAPI_UnregisterCall = (void(*)(class CCallbackBase*))GetProcAddress(m_hDLL, "SteamAPI_UnregisterCallback");
			(*pSteamAPI_UnregisterCall)(this);
		}
	}

	void SetGameserverFlag() { m_nCallbackFlags |= k_ECallbackFlagsGameServer; }
protected:
	virtual void Run(void *pvParam) override
	{
		(m_pObj->*m_Func)((P *)pvParam);
	}
	virtual void Run(void *pvParam, bool, SteamAPICall_t) override
	{
		(m_pObj->*m_Func)((P *)pvParam);
	}
	int GetCallbackSizeBytes()
	{
		return sizeof(P);
	}

	T *m_pObj;
	func_t m_Func;
};


class SteamContext : public i3::shared_ginst<SteamContext>
{
private: 
	HINSTANCE m_hSteamDll;
	ISteamUser* (*m_pSteamUser)(void);
	bool m_bEnableSteam;

public:
	SteamContext();
	virtual ~SteamContext(void);

public:
	// Client 실행시 스팀 관련하여 가장 먼저 실행되어야 합니다.
	bool SteamAPI_Init(LPTSTR lpCmdLine);

	// 스팀유저여부 체크. steam_api.dll 존재 여부도 이 함수로 체크할 수 있습니다.
	bool IsSteamUser();

	// **주의** IsSteamUser() 함수의 결과값이 true 인 경우에만 호출해야합니다. 해당 함수는 nullptr값이 리턴되어도 안됩니다.
	ISteamUser* GetSteamUser();

	// 도전과제. 완료한 도전과제의 StringKey값을 인자로 넘겨주면 Steam내 도전과제에도 완료한 것으로 출력됩니다.
	bool SetAchievement(const char* NameKey);

	// 스팀에서는 스팀 내 브라우저로 모든 웹페이지를 띄워야합니다. GET방식만 가능.
	bool ActivateWebPage(const char* strURL);

	// Steam에 암호화 된 로그인을 요청하는 과정을 거쳐야 GetEncryptedAppTicket 으로 암호화된 로그인 키 값을 받아올 수 있습니다.
	bool RequestLoginTicket();

	// Steam콜백 함수 호출. 호출 대기중인 콜백함수가 없다면 아무 일도 일어나지 않습니다.
	void SteamAPI_RunCallbacks();

public:
	bool				m_bRecvSteamCallback;

	CCallResult_PB< SteamContext, EncryptedAppTicketResponse_t > m_EncryptedTicketResult;
	void OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t *pEncryptedAppTicketResponse, bool bIOFailure);

	// 예제에서 사용되는 STEAM_CALLBACK 디파인은 아래처럼 표기할 수 있습니다.
	// 다만 DLL 문제 때문에 생성및 소멸시 Register/Unregister callback 을 직접 제어해주어야 합니다.
	CCallback_PB<SteamContext, GameOverlayActivated_t> m_CallbackGameOverlayActivated;
	void OnGameOverlayActivated(GameOverlayActivated_t *pOverlay);

};

#endif