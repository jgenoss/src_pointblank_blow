#include "pch.h"
#include "Digit.h"
#include "../ConfigRenderBattle.h"


static const INT32 g_maxDigitCount = 10;


Digit::Digit(i3UIFrameWnd* window) 
	: m_digit(0), m_imageSizeX(21), m_imageSizeY(30)
{
	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD2);

	for (INT32 i=0; i<g_maxDigitCount; i++)
	{
		INT32 x = 115 + (m_imageSizeX * i);
		AddImage(tex, x, 0, m_imageSizeX, m_imageSizeY);
	}

	I3_MUST_RELEASE(tex);

	Create(window);
}

Digit::~Digit()
{
	i3::vector<DigitAnimator*>::iterator it;
	for (it = m_animators.begin(); it != m_animators.end(); it++)
	{
		I3_SAFE_DELETE( *it);
	}
	m_animators.clear();

	m_digit = 0;
}

void Digit::AddAnimator(DigitAnimator* animator)
{
	m_animators.push_back(animator);
}

void Digit::Update(REAL32 tm)
{
	i3::vector<DigitAnimator*>::iterator it;
	for (it = m_animators.begin(); it != m_animators.end(); it++)
		(*it)->Update(this, tm);
}

void Digit::SetDigit(UINT32 digit)
{
	SetCurrentImage(digit);
	m_digit = digit;
}

INT32 Digit::GetDigit() const 
{
	return m_digit;
}

INT32 Digit::GetImageSizeX() const
{
	return m_imageSizeX;
}

INT32 Digit::GetImageSizeY() const
{
	return m_imageSizeY;
}





DigitRollingAnimatior::DigitRollingAnimatior(REAL32 rollingSpd) 
	: m_rollingSpd(rollingSpd), m_accumulatedTime(0.0f)
{
}

void DigitRollingAnimatior::Update(DrawObject* digit, REAL32 deltaSeconds)
{
	m_accumulatedTime += deltaSeconds;

	if (m_accumulatedTime > m_rollingSpd)
	{
		Digit* dgt = static_cast<Digit*>(digit);
		INT32 curDigit = dgt->GetDigit();
		dgt->Show();
		INT32 cnt = ++curDigit % g_maxDigitCount;
		dgt->SetDigit(cnt);
		m_accumulatedTime = 0.0f;
	}
}
