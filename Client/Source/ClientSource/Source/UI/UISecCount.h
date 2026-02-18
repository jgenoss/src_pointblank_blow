#if !defined( __UI_SEC_COUNT_H__)
#define __UI_SEC_COUNT_H__

/// 고해상도카운팅은 필요없고, 하지만 timeGetTime까지는 쓰는게 좋을것임..(11.06.10.수빈)
/// 이 클래스는 gui루틴이 아님..
/// float은 쓰지 말것...DWORD타이머 수준에서 처리함..
/// Pause나 Stop은 쓰지 말것...(실제로 사용할 일이 없으며 루틴이 길어짐)

class UISecCountCallback
{
public:
	virtual void OnStart(INT32 result_time) = 0;
	virtual void OnSecChange(INT32 result_time) = 0;
	virtual void OnDone(INT32 result_time) = 0;
	virtual void OnStop(INT32 result_time) = 0;
};

class UISecCountCallback_Null : public UISecCountCallback
{
	virtual void OnStart(INT32 result_time) override {}
	virtual void OnSecChange(INT32 result_time) override {}
	virtual void OnDone(INT32 result_time) override {}
	virtual void OnStop(INT32 result_time) override {}
};

template<class C, 
			void (C::*MF_SEC_START)(INT32),
			void (C::*MF_SEC_CHANGE)(INT32),
			void (C::*MF_ON_DONE)(INT32),
			void (C::*MF_ON_STOP)(INT32)
		>
class TUISecCallback : public UISecCountCallback
{
public:
	TUISecCallback() : m_pObj(nullptr) {}
	TUISecCallback( C* obj) : m_pObj(obj) {}
	void SetObj(C* obj) { m_pObj = obj; }
private:
	virtual void OnStart(INT32 result_time) override {  (m_pObj->*MF_SEC_START)(result_time);   }
	virtual void OnSecChange(INT32 result_time) override { (m_pObj->*MF_SEC_CHANGE)(result_time);   }
	virtual void OnDone(INT32 result_time) override {  (m_pObj->*MF_ON_DONE)(result_time); }
	virtual void OnStop(INT32 result_time) override {  (m_pObj->*MF_ON_STOP)(result_time); }
	C*	m_pObj;
};


class UISecCount
{
public:
	UISecCount(INT32 sec = 10);
	
	void SetCallback(UISecCountCallback* pCallback) { m_pCallback = pCallback; }
	void ResetCallback() { m_pCallback = &ms_pDefaultCB; }
	void SetBaseSec(INT32 sec);
	void SetReverse(bool reverse_count);
	void Start();
	void Stop();
	void  Reset();
	INT32 Update();		// 매 프레임 감시해야하는 함수...rDeltaSeconds를 쓰지 않음..
	INT32 GetBaseSec() const { return m_base_time; }
	INT32 GetResultSec() const { return m_result_time; }
private:
	
	UISecCountCallback*				m_pCallback;
	DWORD	m_init_timer_value;
	INT32	m_result_time;	
	INT32	m_base_time;
	bool	m_reverse;
	static UISecCountCallback_Null  ms_pDefaultCB;
};

#endif