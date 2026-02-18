#ifndef _DIGIT_NUMBER_H_
#define _DIGIT_NUMBER_H_

class Digit;
class DigitNumberAnimator;
class DigitNumber
{
public:
	DigitNumber(i3UIFrameWnd* wnd, INT32 number, INT32 maxChipher);
	~DigitNumber();

	void	Update(REAL32 deltaSeconds);

	Digit*	GetDigit(INT32 chipher) const;
	INT32	size() const;
	void	AddAnimator(DigitNumberAnimator* animator);

private:
	void	_MakeNumberDefault(INT32 number, INT32 maxChipher);
	void	_CalcSignificantSize(INT32 number);

private:
	i3::vector<Digit*>					m_number;
	i3::vector<DigitNumberAnimator*>	m_animators;
	i3UIFrameWnd*						m_frameWindow;
	INT32								m_significantNumberSize;
};


class DigitNumberAnimator
{
public:
	DigitNumberAnimator() {}
	virtual ~DigitNumberAnimator() {}

	virtual void Update(DigitNumber* number, REAL32 deltaSeconds) = 0;
};

class DigitNumberAnimatorNone : public DigitNumberAnimator
{
public:
	DigitNumberAnimatorNone() {}
	virtual ~DigitNumberAnimatorNone() {}

	virtual void Update(DigitNumber* number, REAL32 deltaSeconds) override {}
};

class DigitNumberRollStepByStepAnimator : public DigitNumberAnimator
{
public:
	DigitNumberRollStepByStepAnimator(REAL32 rollTimePerDigit);
	virtual ~DigitNumberRollStepByStepAnimator() {}

	virtual void Update(DigitNumber* number, REAL32 deltaSeconds) override;

private:
	i3::vector<INT32>	m_digitsOrig;
	REAL32				m_rollTimePerDigit;
	REAL32				m_time;
	INT32				m_chipher;
};

#endif