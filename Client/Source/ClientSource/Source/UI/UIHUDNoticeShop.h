#if !defined( __UI_HUD_NOTICE_SHOP_H__)
#define __UI_HUD_NOTICE_SHOP_H__


#include "UIHUDBase.h"
#include "UISecCount.h"
#include "i3Base/itl/deque.h"

class UIHUDNoticeShop : public UIHUDBase
{
	I3_CLASS_DEFINE( UIHUDNoticeShop, UIHUDBase);
	
	friend class igshop_ctrl;

	bool m_observer_mode;
	bool m_visible;

public:
	UIHUDNoticeShop();
	virtual ~UIHUDNoticeShop();

	static UIHUDNoticeShop* instance();

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnExitEnd() override;

	void SetVisible(bool visible);
	void			SetObserverMode(bool mode);
	void			ModifyPos(void);

private:
	void			RestorePos();
};


class igshop_ctrl
{
public:
	igshop_ctrl();

	void		OnUpdate(REAL32 delta);
	void		SetGameShopValidTime(INT32 sec);
	void		SetDeath();				// 디지면 킴..
	void		SetDeathDelay();		// 라운드제의 경우 다 뒈진 경우 중간창 뜰때 막아야하는 문제로 1초간 딜레이하고 상황판단.....
	void		SetRespawn();			// 리스폰시 킴..
	
	void		SetRoundEnd();			// 라운드 종료시 다 끈다...(T키의 쿨타임과는 다르게 처리..)
	void		SetGameEnd();			// 게임 종료시 끔..

	void		SetIntermissionStart();
	void		SetIntermissionEnd();

	struct handler 
	{  
		virtual void SetDeath(igshop_ctrl* owner) = 0;
		virtual void SetDeathDelay(igshop_ctrl* owner) = 0;
		virtual void SetRespawn(igshop_ctrl* owner) = 0;
		virtual void SetRoundEnd(igshop_ctrl* owner) = 0;
		virtual void SetGameEnd(igshop_ctrl* owner) = 0;
		virtual void OnRespawnTimeDone(igshop_ctrl* owner) = 0;
		virtual void OnUpdate(igshop_ctrl* owner, REAL32 delta) = 0;		
		
		virtual void SetIntermissionStart(igshop_ctrl* owner) = 0;
		virtual void SetIntermissionEnd(igshop_ctrl* owner) = 0;
		
		virtual ~handler() {} 
	};

	struct get_handler
	{
		virtual handler*		idle() = 0;
		virtual handler*		death() = 0;
		virtual handler*		death_delay() = 0;
		virtual handler*		respawn() = 0;
	};

	void		SetHandler_Idle() { m_handler = m_get_handler->idle();  }
	void		SetHandler_Death() { m_handler = m_get_handler->death();  }
	void		SetHandler_Death_Delay() { m_handler = m_get_handler->death_delay(); }
	void		SetHandler_Respawn()	{ m_handler = m_get_handler->respawn(); }

	void		StartShopTimer();
	void		StopShopTimer();

	INT32		GetTCRespawnIdx() const { return m_tcRespawnIdx; }
	void		SetTCRespawnIdx(INT32 idx) { m_tcRespawnIdx = idx; }
	void		EnableNoticeShop(bool bEnable);
	bool		IsEnableNoticeShop() const {  return m_bEnableNoticeShop;  }
	
	void		AddDeathDelayTime(REAL32 rDelta) { m_fCurrDeathDelayTime_Round += rDelta; }
	REAL32		GetCurrDeathDelayTime() const { return m_fCurrDeathDelayTime_Round; }
	void		ResetDeathDelayTime() { m_fCurrDeathDelayTime_Round = 0.f; }

private:
	
	void		OnSec_Start(INT32 result);
	void		OnSec_Change(INT32 result);
	void		OnSec_Done(INT32 result);
	void		OnSec_Stop(INT32 result);

	typedef TUISecCallback<igshop_ctrl, 
		&igshop_ctrl::OnSec_Start, &igshop_ctrl::OnSec_Change,	&igshop_ctrl::OnSec_Done, &igshop_ctrl::OnSec_Stop
		> SecCallback;
	
	void		Process_NoticeShopQueue();

	friend class SecCallback;

	get_handler*	m_get_handler;		// abstract factory의 변형형태..
	handler*		m_handler;
	UISecCount		m_sc;
	SecCallback		m_sc_cb;
	i3::deque<bool>	m_queue_popup;

	INT32			m_tcRespawnIdx;
	REAL32			m_fCurrDeathDelayTime_Round;
	bool			m_bEnableNoticeShop;
};



#endif

