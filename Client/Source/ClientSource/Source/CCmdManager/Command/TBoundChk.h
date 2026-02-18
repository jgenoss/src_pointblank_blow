#pragma once

namespace
{
	template<class T>
	bool TBoundChk(const T& var, const T min, const T max)
	{
		if ((min <= var) && (var <= max)) return true;

		I3ASSERT((min <= var) && (var <= max));

		return false;
	}

	template<class T>
	bool TBoundChk(const size_t len, const T* vars, const T min, const T max)
	{
		for (size_t i=0; i<len; i++)
		{
			if (!TBoundChk(*vars, min, max)) return false;
			vars++;
		}

		return true;
	}

	template<class T>
	bool TBoundChk(const i3::vector<T>& vars, const T min, const T max)
	{
		for (size_t i=0; i < vars.size(); i++)
		{
			if (!TBoundChk(vars[i])) return false;
		}

		return true;
	}
}