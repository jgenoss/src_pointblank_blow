#ifndef _UI_BLINK_H_
#define _UI_BLINK_H_

// 컨트롤의 깜빡거림 효과 구현.

class IntervalTimeEvent
{
public:
	explicit IntervalTimeEvent(REAL32 interval);
	virtual ~IntervalTimeEvent();

	void Start();
	bool OnUpdate(REAL32 tm);
	void Stop();
	bool IsOn() const;

	virtual void operator()() = 0;

private:
	REAL32 m_current;
	REAL32 m_interval;
	bool   m_isOn;
};

class UIBlink : public IntervalTimeEvent
{
public:
	UIBlink(i3UIControl* ctrl, REAL32 interval);
	virtual ~UIBlink();

	virtual void operator()();

private:
	i3UIControl* m_ctrl;
	bool m_isBright;
};

#endif