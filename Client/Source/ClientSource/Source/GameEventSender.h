#pragma once

class CGameEventHandler;

class GameEventSender : public i3::shared_ginst<GameEventSender>
{
public:
	GameEventSender();
	~GameEventSender();
	
	void	OnCreate();

	//게임에서 필요한 이벤트를 등록합니다. 내부적으로는 서버로 메시지를 보내요.(Game -> GameContext)
	bool	SetEvent(INT32 Event, const void * pArg = nullptr, const void * pArg1 = nullptr, const void * pArg2 = nullptr, LPCSTR szFile = nullptr, int nLine = 0);
	// SetEvent와 동일한 기능을 하나 발생시킨 이벤트의 응답이 오기전까지 동일한 이벤트가 발생할 경우 무기하도록 하는 기능을 가집니다.
	bool	SetEventOnce(INT32 Event, const void * pArg = nullptr, const void * pArg1 = nullptr, const void * pArg2 = nullptr);
	
private:
	CGameEventHandler *	m_pEventHandler;

	INT32 m_BeforeEvent;			// 변수 위치를 이쪽으로 변경..
};
