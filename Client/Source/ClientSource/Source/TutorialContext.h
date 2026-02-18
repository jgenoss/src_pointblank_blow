#pragma once

class TutorialContext : public i3::shared_ginst<TutorialContext>
{
public:
	TutorialContext();
	~TutorialContext();

	void SetRoundPlayTime(UINT32 uiTime)	{ m_uiRoundPlayTime = uiTime; }
	UINT32 GetRoundPlayTime() const			{ return m_uiRoundPlayTime; }

	void SetShowedMessage(bool bFlag)		{ m_bShowedMessage = bFlag; }

	void SetWasTutorial(bool bFlag)			{ m_WasTutorial = bFlag; }
	bool WasTutorial() const				{ return m_WasTutorial; }

	bool IsShowTutorialMessage() const		{ return m_WasTutorial && IsFirstFinishTutorial() && m_bShowedMessage == false; }
	bool IsShowMissionButtonEffect() const	{ return IsFirstFinishTutorial() && m_bShowMissionButtonEffect; }

	bool IsFirstFinishTutorial() const;
	bool IsFinishTutorial() const;
	void SetOldTutorialIng();
	void DeActiveMissionButtonEffect();


private:
	UINT32	m_uiRoundPlayTime;
	bool m_WasTutorial;
	UINT8 m_ui8OldTutorialIng;
	bool m_bShowMissionButtonEffect;
	bool m_bShowedMessage;
};