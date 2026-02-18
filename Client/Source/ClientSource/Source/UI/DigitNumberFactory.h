#ifndef _DIGITNUMBER_FACTORY_H_
#define _DIGITNUMBER_FACTORY_H_


class DigitNumber;

class DigitNumberFactory
{
public:
	static DigitNumber* MakeDomiEndResultRanking(i3UIFrameWnd* wnd, INT32 number);
	static DigitNumber* MakeDomiEndResultTotalScore(i3UIFrameWnd* wnd, INT32 number);
};

#endif