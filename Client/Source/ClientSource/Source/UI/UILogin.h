#if !defined( __UI_LOGIN_H__)
#define __UI_LOGIN_H__

#include "UIPopupFocus.h"
#include "../UI/UIDefine.h"

class UIPhaseLogin;
class UIPhaseBase;

class UILogin : public i3Stage
{
	I3_CLASS_DEFINE( UILogin, i3Stage);
private:
	i3::vector<UIPhaseBase *>			m_LoginPhase;
	LOGIN_UIPHASE						m_currentPhase;
	LOGIN_UIPHASE						m_lastPhase;
	i3::vector<LOGIN_UIPHASE>			m_PhaseQueue;
	PhaseState							m_phaseState;

private:
	void			__UpdatePhase(REAL32 rDeltaSeconds);

public:
	UILogin();
	virtual ~UILogin();

	virtual void	OnCreate( void) override;
	virtual void	OnLoadEnd( void) override;
	virtual bool	OnFinish( void) override;
	virtual bool	OnEvent( UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	void OnEscape( void);

public:
	UIPhaseBase *	GetUIPhaseLogin(void) { return m_LoginPhase.at(LOGIN_UIPHASE_LOGIN); }
	UIPhaseBase *	GetCurrentPhase(void)
	{
		if (m_currentPhase > LOGIN_UIPHASE_START && m_currentPhase < LOGIN_UIPHASE_MAX)
			return GetPhase(m_currentPhase);
		return nullptr;
	}
	UIPhaseBase *	GetPhase(LOGIN_UIPHASE phase)
	{
		I3_BOUNDCHK(phase, LOGIN_UIPHASE_MAX);
		UIPhaseBase * pPhasebase = m_LoginPhase.at(phase);

		return pPhasebase;
	}

	void		MovePhase(LOGIN_UIPHASE Phase);
	void		StartPhase(LOGIN_UIPHASE MovePhase);
	void		EndPhase(void);

};

#endif
