
namespace i3
{

	template<class BidIt, class OutIt> inline
	OutIt reverse_copy(BidIt First, BidIt Last, OutIt Dest)
	{
		for (; First != Last; ++Dest)
			*Dest = *--Last;
		return Dest;
	}

}
