#pragma once


namespace i3
{

	template<class BidIt, class Pred1> inline 
	BidIt rfind_not_if(BidIt f1, BidIt l1, Pred1 pr) 
	{	
		if ( f1 == l1 )
			return l1;				// 못찾으면 어쨋거나 l1으로 끝낸다..

		BidIt it = l1;

		while ( --it  )
		{
			if ( !pr(*it) ) return it;
			if ( f1 == it )	break;
		}
		return l1;
	}	

}
