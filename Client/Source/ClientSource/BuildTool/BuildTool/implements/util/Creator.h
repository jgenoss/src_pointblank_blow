#pragma once

template <class B, class D>
struct Creator
{
	template <typename ...Args>
	static B* Call(const Args& ...args)
	{
		B* p = new D(args...);
		return p;
	}
};