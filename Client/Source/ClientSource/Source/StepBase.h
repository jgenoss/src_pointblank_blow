#ifndef _StepBase_h
#define _StepBase_h

#include "GuiNotifyReceiver.h"
#include "GameChat_Body.h"

class CStepBase: public CGuiNotifyReceiver
{
	I3_ABSTRACT_CLASS_DEFINE(CStepBase);
public:
	CStepBase();
	virtual ~CStepBase();

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);	
	virtual void OnInitControl(void);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);
	virtual void AuthStartSuccess(INT32 arg);
	
	BOOL IsRun(void);	

	CGameChatBody * GetChatBody(void)		{ return m_pChatBody;			}

protected:

	// Ã¤ÆÃ
	CGameChatBody *	m_pChatBody;

	WEAPON_SLOT_TYPE m_OldWeaponUsageType;
	INT32	m_OldidxStartSlot;
	INT32	m_OldidxTotalSlot;
	bool m_bVestigeDuty;

public:
	virtual void _ResetVestige();
	virtual void _SetupVestige();

	WEAPON_SLOT_TYPE _GetFirstGroup();
	UINT32 _GetFirstSelectItem();
	UINT32 _GetFirstStartItem();
};

#endif //_StepBase_h
