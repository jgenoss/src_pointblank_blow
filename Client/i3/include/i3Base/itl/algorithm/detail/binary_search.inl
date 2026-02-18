
namespace i3
{

	template<class forward_iterator, class T> inline
	bool	binary_search(forward_iterator f, forward_iterator l, const T& val)
	{
		f = i3::lower_bound(f, l, val);
		return f != l && !(val < *f);
	}

	template<class forward_iterator, class T, class Pred> inline
	bool	binary_search(forward_iterator f, forward_iterator l, const T& val, Pred pr)
	{
		f = i3::lower_bound(f, l, val, pr);
		return f != l && !pr(val, *f);
	}

}
