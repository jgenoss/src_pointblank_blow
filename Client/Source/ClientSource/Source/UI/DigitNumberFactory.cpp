#include "pch.h"
#include "DigitNumberFactory.h"
#include "Digit.h"
#include "DigitNumber.h"


DigitNumber* DigitNumberFactory::MakeDomiEndResultRanking(i3UIFrameWnd* wnd, INT32 number)
{
	DigitNumber* score = new DigitNumber(wnd, number, 4);
	MEMDUMP_NEW( score, sizeof(DigitNumber));

	for (INT32 i=0; i<score->size(); i++)
	{
		Digit* digit = score->GetDigit(i);

		DigitRollingAnimatior * pRool = new DigitRollingAnimatior(0.005f);
		MEMDUMP_NEW( pRool, sizeof(DigitRollingAnimatior));
		digit->AddAnimator(pRool);
		digit->SetPosition(372.5f+(digit->GetImageSizeX()*i), 100.0f);
	}

	DigitNumberRollStepByStepAnimator * pStep = new DigitNumberRollStepByStepAnimator(0.5f);
	MEMDUMP_NEW( pStep, sizeof(DigitNumberRollStepByStepAnimator));
	score->AddAnimator(pStep);

	return score;
}

DigitNumber* DigitNumberFactory::MakeDomiEndResultTotalScore(i3UIFrameWnd* wnd, INT32 number)
{
	DigitNumber* score = new DigitNumber(wnd, number, 7);
	MEMDUMP_NEW( score, sizeof(DigitNumber));

	for (INT32 i=0; i<score->size(); i++)
	{
		Digit* digit = score->GetDigit(i);

		DigitRollingAnimatior * pRool = new DigitRollingAnimatior(0.005f);
		MEMDUMP_NEW( pRool, sizeof( DigitRollingAnimatior));
		digit->AddAnimator( pRool);
		digit->SetPosition(85.0f+(digit->GetImageSizeX()*i), 100.0f);
		digit->SetColor(0, 255, 0, 255);
	}

	DigitNumberRollStepByStepAnimator * pStep = new DigitNumberRollStepByStepAnimator(0.5f);
	MEMDUMP_NEW( pStep, sizeof(DigitNumberRollStepByStepAnimator));
	score->AddAnimator( pStep);
	return score;
}