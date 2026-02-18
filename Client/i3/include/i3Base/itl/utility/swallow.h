#pragma once

namespace i3
{
	
	struct swallow_copy		{	template<class T> swallow_copy(const T&) {}		};
	struct swallow_assign	{	template<class T> const swallow_assign& operator=(const T&) const { return *this; } };	
	
}