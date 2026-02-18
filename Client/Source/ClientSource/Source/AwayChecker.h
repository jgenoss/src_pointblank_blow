#ifndef _AwayChecker_h
#define _AwayChecker_h

enum AwayCheckerEvent
{
	ACE_START = I3_EVT_USER + 100000,			// 혹시 있을 충돌을 피하기 위해

	ACE_AWAY = ACE_START,
	ACE_CAUTION,

	ACE_MAX
};

/*	특정 시간을 listener 하는 클래스 이다.
	AwayTime, CautionTime으로 설정된 시간이 되면, 등록된 Receiver를 통해서 
	ACE_AWAY, ACE_CAUTION 이벤트가 발생된다.
	Receiver 는 OnEvent(UINT32,i3ElementBase* ,UINT32, I3_EVT_CODE) 함수를 통해서 받는다.

	이벤트 발생 시간
	예) AwayTime : 60.f, CautionTime : 5.f
	55초에 ACE_CAUTION 이벤트 발생, 60초(1분)이며 ACE_AWAY 이벤트 발생
*/
class CAwayChecker: public i3GameNode
{
	I3_CLASS_DEFINE(CAwayChecker, i3GameNode);
public:
	CAwayChecker();
	virtual ~CAwayChecker();

	virtual void OnUpdate(REAL32 rDeltaSeconds) override;

	void SetReceiver(i3EventReceiver* pObserver);
	void SetTime(REAL32 rAwayTime,REAL32 rCautionTime,bool bOneTimeNotify = true);
	void SetNotify(bool bOneTimeNotify);
	void Restart(void);
	void Reset(void);
	REAL32 GetCurrentTime() { return m_rCurrentTime; }
	
private:
	i3EventReceiver* m_pObserver;
	REAL32 m_rAwayTime;
	REAL32 m_rCautionTime;
	REAL32 m_rCurrentTime;
	bool m_bOneTimeNotify;
	bool m_bPause;
	bool m_bAwayNotify;
	bool m_bCautionNotify;
};

#endif //_AwayChecker_h
