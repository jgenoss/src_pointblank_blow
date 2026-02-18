#pragma once

#include "../../iter_swap.h"


namespace i3
{
	namespace detail
	{
		
		template<class RanIt> inline
		void Med3(RanIt First, RanIt Mid, RanIt Last)
		{	// sort median of three elements to middle
			if (*Mid < *First) i3::iter_swap(Mid, First);
			if (*Last < *Mid)  i3::iter_swap(Last, Mid);
			if (*Mid < *First) i3::iter_swap(Mid, First);
		}

		template<class RanIt, class Pred> inline
		void Med3(RanIt First, RanIt Mid, RanIt Last, Pred pred)
		{	// sort median of three elements to middle
			if ( pred(*Mid, *First)) i3::iter_swap(Mid, First);
			if ( pred(*Last, *Mid)) i3::iter_swap(Last, Mid);
			if ( pred(*Mid, *First)) i3::iter_swap(Mid, First);
		}

		template<class RanIt> inline
		void Median(RanIt First, RanIt Mid, RanIt Last)
		{	// sort median element to middle
			if (40 < Last - First)
			{	// median of nine
				size_t Step = (Last - First + 1) / 8;
				Med3(First, First + Step, First + 2 * Step);
				Med3(Mid - Step, Mid, Mid + Step);
				Med3(Last - 2 * Step, Last - Step, Last);
				Med3(First + Step, Mid, Last - Step);
			}
			else
				Med3(First, Mid, Last);
		}

		template<class RanIt, class Pred> inline
		void Median(RanIt First, RanIt Mid, RanIt Last, Pred pred)
		{	// sort median element to middle
			if (40 < Last - First)
			{	// median of nine
				size_t Step = (Last - First + 1) / 8;
				Med3(First, First + Step, First + 2 * Step, pred);
				Med3(Mid - Step, Mid, Mid + Step, pred);
				Med3(Last - 2 * Step, Last - Step, Last, pred);
				Med3(First + Step, Mid, Last - Step, pred);
			}
			else
				Med3(First, Mid, Last, pred);
		}

		template<class RanIt> inline
		i3::pair<RanIt, RanIt> Unguarded_partition(RanIt First, RanIt Last)
		{	// partition [First, Last), using operator<
			RanIt Mid = First + (Last - First) / 2;	// sort median to _Mid
			Median(First, Mid, Last - 1);
			RanIt Pfirst = Mid;
			RanIt Plast = Pfirst + 1;

			while (First < Pfirst && !( *(Pfirst - 1) < *Pfirst ) && !(*Pfirst < *(Pfirst - 1) ) )
				--Pfirst;
			while (Plast < Last   && !(*Plast < *Pfirst) && !(*Pfirst < *Plast) )
				++Plast;

			RanIt Gfirst = Plast;
			RanIt Glast = Pfirst;

			for ( ; ; )
			{	// partition
				for (; Gfirst < Last; ++Gfirst)
					if (*Pfirst < *Gfirst)
						;
					else if (*Gfirst < *Pfirst)
						break;
					else
						i3::iter_swap(Plast++, Gfirst);

					for (; First < Glast; --Glast)
						if (*(Glast - 1) < *Pfirst)
							;
						else if (*Pfirst < *(Glast - 1))
							break;
						else
							i3::iter_swap(--Pfirst, Glast - 1);

						if (Glast == First && Gfirst == Last)
							return i3::pair<RanIt, RanIt>(Pfirst, Plast);

						if (Glast == First)
						{	// no room at bottom, rotate pivot upward
							if (Plast != Gfirst)
								i3::iter_swap(Pfirst, Plast);
							++Plast;
							i3::iter_swap(Pfirst++, Gfirst++);
						}
						else if (Gfirst == Last)
						{	// no room at top, rotate pivot downward
							if (--Glast != --Pfirst)
								i3::iter_swap(Glast, Pfirst);
							i3::iter_swap(Pfirst, --Plast);
						}
						else
							i3::iter_swap(Gfirst++, --Glast);
			}
		}

		template<class RanIt, class Pred> inline
		i3::pair<RanIt, RanIt> Unguarded_partition(RanIt First, RanIt Last, Pred pred)
		{	// partition [_First, _Last), using _Pred
			RanIt Mid = First + (Last - First) / 2;
			Median(First, Mid, Last - 1, pred);
			RanIt Pfirst = Mid;
			RanIt Plast = Pfirst + 1;

			while (First < Pfirst && !pred( *(Pfirst - 1), *Pfirst)  && !pred(*Pfirst, *(Pfirst - 1) ) )
				--Pfirst;
			while (Plast < Last   && !pred( *Plast, *Pfirst) && !pred(*Pfirst, *Plast) )
				++Plast;

			RanIt Gfirst = Plast;
			RanIt Glast = Pfirst;

			for (; ; )
			{	// partition
				for (; Gfirst < Last; ++Gfirst)
					if ( pred(*Pfirst, *Gfirst) )
						;
					else if (pred(*Gfirst, *Pfirst))
						break;
					else
						i3::iter_swap(Plast++, Gfirst);
					for (; First < Glast; --Glast)
						if (pred( *(Glast - 1), *Pfirst) )
							;
						else if (pred(*Pfirst, *(Glast - 1)))
							break;
						else
							i3::iter_swap(--Pfirst, Glast - 1);

						if (Glast == First && Gfirst == Last)
							return i3::pair<RanIt, RanIt>(Pfirst, Plast);

						if (Glast == First)
						{	// no room at bottom, rotate pivot upward
							if (Plast != Gfirst)
								i3::iter_swap(Pfirst, Plast);
							++Plast;
							i3::iter_swap(Pfirst++, Gfirst++);
						}
						else if (Gfirst == Last)
						{	// no room at top, rotate pivot downward
							if (--Glast != --Pfirst)
								i3::iter_swap(Glast, Pfirst);
							i3::iter_swap(Pfirst, --Plast);
						}
						else
							i3::iter_swap(Gfirst++, --Glast);
			}
		}



	}
}