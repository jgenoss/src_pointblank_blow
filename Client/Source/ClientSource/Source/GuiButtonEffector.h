#ifndef		__GUI_BUTTON_EFFECTOR_H__
#define		__GUI_BUTTON_EFFECTOR_H__

#if legacy_gui_function

class CGuiButtonEffector
{
public:
	CGuiButtonEffector(REAL32 EffectTime = 0.25f);

	void	Set(i3GuiButton * pButton);
	void	SetEffectTime(REAL32 Sec)	{m_EffectTime = Sec;}
	void	Reset();

	void	Update(bool Update, REAL32 DeltaSeconds);

private:
	void	ButtonEffect(bool Reverse);

private:
	REAL32	m_ActiveBtnTime;
	REAL32	m_EffectTime;
	bool	m_ReverseEffect;

	i3GuiButton * m_pTargetButton;
};

#endif

#endif	// __GUI_BUTTON_EFFECTOR_H__