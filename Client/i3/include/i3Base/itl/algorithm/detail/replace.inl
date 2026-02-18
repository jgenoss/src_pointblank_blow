
namespace i3
{

	template<class FwdIt, class T> inline
	void replace(FwdIt f, FwdIt l, const T& old_val, const T& new_val)
	{
		for (; f != l; ++f)
		{
			if (*f == old_val) *f = new_val;
		}
	}

}
