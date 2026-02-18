#include "pch.h"
#include "PurchaseTimeCheck.h"

PurchaseTimeCheck::PurchaseTimeCheck() : base_time_(0), working_(false), 
										start_time_(0)
{

}

PurchaseTimeCheck::~PurchaseTimeCheck()
{

}

void PurchaseTimeCheck::setBaseTime( INT32 time )
{
	base_time_ = time;
	working_ = false;
}

void PurchaseTimeCheck::Start()
{
	if(start_time_ != 0)
		return;

	Reset();
	working_ = true;
	start_time_ = timeGetTime();
}

bool PurchaseTimeCheck::Update()
{
	if(working_)
	{
		INT32 time_interval_ = static_cast<INT32>((timeGetTime() - start_time_) / 1000);

		if(time_interval_ >= base_time_)
		{
			time_interval_ = base_time_;
			
			Reset();

			return true;
		}
	}

	return false;
}

void PurchaseTimeCheck::Stop()
{
	working_ = false;
}

void PurchaseTimeCheck::Reset()
{
	working_ = false;
	start_time_ = 0;
}