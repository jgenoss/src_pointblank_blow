#pragma once

// Timeout Check Å¬·¡½º
class PurchaseTimeCheck
{
public:
	enum TIME_OUT
	{ 
		GACHA_STATE_UPDATE = 2,
		GACHA_ACK_TIME_OUT = 10, 
		BUY_ACK_TIME_OUT = 20, 
	};

public:
	PurchaseTimeCheck();
	~PurchaseTimeCheck();

	void setBaseTime(INT32 time);

	void Start();
	bool Update();
	void Stop();
	void Reset();

private:
	INT32 base_time_;
	DWORD start_time_;

	bool working_;
};