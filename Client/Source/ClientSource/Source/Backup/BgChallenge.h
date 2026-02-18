#ifndef _BgChallenge_h
#define _BgChallenge_h

#if 지워질_것들

#include "GuiNotifyReceiver.h"

struct ChallengeControlGroup
{
	i3GuiStatic* _pMark;
	i3GuiEditBox* _pText;
};


class CBgChallenge: public CGuiNotifyReceiver
{
	I3_CLASS_DEFINE(CBgChallenge, CGuiNotifyReceiver);
public:
	CBgChallenge();
	virtual ~CBgChallenge();

	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl( const i3::vector_set<STRCINFO*>* plist, i3GameNode * pControl);
	virtual void OnInitControl(void);

	void OnOpen(void);
	void OnClose(void);

	void ToggleMinimize(void);

private:
	i3GuiStatic* m_pChallengeWinMin;
	i3GuiEditBox* m_pChallengeCaptionMin;

	i3GuiStatic* m_pChallengeWin;
	i3GuiEditBox* m_pChallengeCaption;
	ChallengeControlGroup m_ChallengeGroup[MAX_CHALLENGE_COUNT];

	BOOL m_isMinimize;
};

#endif

#endif
