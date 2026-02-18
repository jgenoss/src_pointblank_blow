#include "pch.h"
#include "DigitNumber.h"
#include "Digit.h"

static const INT32 g_maxChipher = 256;

DigitNumber::DigitNumber(i3UIFrameWnd* wnd, INT32 number, INT32 maxChipher) 
	: m_frameWindow(wnd), m_significantNumberSize(-1)
{
	_MakeNumberDefault(number, maxChipher);
}

DigitNumber::~DigitNumber()
{
	i3::vector<Digit*>::iterator digitIter;
	for (digitIter = m_number.begin(); digitIter != m_number.end(); digitIter++)
	{
		I3_SAFE_DELETE( *digitIter);
	}
	m_number.clear();

	i3::vector<DigitNumberAnimator*>::iterator animatorIter;
	for (animatorIter = m_animators.begin(); animatorIter != m_animators.end(); animatorIter++)
	{
		I3_SAFE_DELETE( *animatorIter);
	}
	m_animators.clear();
}

void DigitNumber::Update(REAL32 deltaSeconds)
{
	i3::vector<DigitNumberAnimator*>::iterator it;
	for (it = m_animators.begin(); it != m_animators.end(); it++)
		(*it)->Update(this, deltaSeconds);
}

void DigitNumber::_MakeNumberDefault(INT32 number, INT32 maxChipher)
{
	_CalcSignificantSize(number);

	char strDigit[g_maxChipher+1] = {0,};
	// 유효한 숫자의 첫 번째 자리를 구한다.
	// ex) 0001234의 경우 유효한 첫 번째 자리는 인덱스 3이다.
	INT32 significantStartPos = maxChipher-m_significantNumberSize;
	::itoa(number, &strDigit[significantStartPos], 10);

	m_number.reserve(maxChipher);
	for (INT32 i=0; i<maxChipher; i++)
	{
		char d[2] = {0,};
		d[0] = strDigit[i];
		INT32 decimal = ::atoi(d);
		Digit* digit = new Digit(m_frameWindow);
		MEMDUMP_NEW( digit, sizeof(Digit));

		digit->SetDigit(decimal);

		DigitAnimatorNone * pNode = new DigitAnimatorNone;
		MEMDUMP_NEW( pNode, sizeof( DigitAnimatorNone));
		digit->AddAnimator( pNode);
		digit->SetPosition(0.0f, 0.0f);
		digit->SetColor(255, 255, 255, 255);
		m_number.push_back(digit);
	}

	DigitNumberAnimatorNone * pNumberNode = new DigitNumberAnimatorNone;
	MEMDUMP_NEW( pNumberNode, sizeof(DigitNumberAnimatorNone));
	m_animators.push_back( pNumberNode);
}

void DigitNumber::_CalcSignificantSize(INT32 number)
{
	if (number > 0) 
		_CalcSignificantSize(static_cast<INT32>(number/10));
		
	m_significantNumberSize++;
}

Digit* DigitNumber::GetDigit(INT32 chipher) const
{
	return m_number[chipher];
}

INT32 DigitNumber::size() const
{
	return m_number.size();
}

void DigitNumber::AddAnimator(DigitNumberAnimator* animator)
{
	m_animators.push_back(animator);
}




DigitNumberRollStepByStepAnimator::DigitNumberRollStepByStepAnimator(REAL32 rollTimePerDigit) 
	: m_time(0.0f), m_chipher(0)
{
	m_rollTimePerDigit = rollTimePerDigit;
	m_digitsOrig.reserve(7);
}

void DigitNumberRollStepByStepAnimator::Update(DigitNumber* number, REAL32 deltaSeconds)
{
	if (m_digitsOrig.empty())
	{
		for (INT32 i=0; i<number->size(); i++)
		{
			INT32 digitOrig = number->GetDigit(i)->GetDigit();
			m_digitsOrig.push_back(digitOrig);
			number->GetDigit(i)->SetDigit(0);
		}
		m_chipher = 0;
	}

	if (m_chipher < number->size())
	{
		m_time += deltaSeconds;

		Digit* digit = number->GetDigit(m_chipher);
		digit->Update(deltaSeconds);

		if (m_time > m_rollTimePerDigit)
		{
			if (m_digitsOrig[m_chipher] == digit->GetDigit())
			{
				++m_chipher;
				m_time = 0.0f;
			}
		}
	}
}
