#ifndef _SINGLE_DIGIT_IMAGEBOX_H_
#define _SINGLE_DIGIT_IMAGEBOX_H_

#include "DrawObject.h"


class DigitAnimator;
class Digit : public DrawObject
{
public:
	explicit Digit(i3UIFrameWnd* window);
	virtual ~Digit();

	void	Update(REAL32 tm);
	void	AddAnimator(DigitAnimator* animator);

	void	SetDigit(UINT32 digit);
	INT32	GetDigit() const;

	INT32	GetImageSizeX() const;
	INT32	GetImageSizeY() const;

private:
	i3::vector<DigitAnimator*> m_animators;
	UINT32	m_digit;
	INT32	m_imageSizeX;
	INT32	m_imageSizeY;
};



class DigitAnimator
{
public:
	DigitAnimator() {}
	virtual ~DigitAnimator() {}
	virtual void Update(DrawObject* digit, REAL32 deltaSeconds) = 0;
};

class DigitAnimatorNone : public DigitAnimator
{
public:
	DigitAnimatorNone() {}
	virtual ~DigitAnimatorNone() {}

	virtual void Update(DrawObject* digit, REAL32 deltaSeconds) override {}
};

class DigitRollingAnimatior : public DigitAnimator
{
public:
	explicit DigitRollingAnimatior(REAL32 rollingSpd);
	virtual ~DigitRollingAnimatior() {}

	virtual void Update(DrawObject* digit, REAL32 deltaSeconds) override;

private:
	REAL32 m_rollingSpd;
	REAL32 m_accumulatedTime;
};

#endif