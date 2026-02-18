#ifndef __ROOMMASTERKICK_MANAGER_H__
#define __ROOMMASTERKICK_MANAGER_H__

#include "AwayChecker.h"

class UIMessageBox;

class CRoomMasterKickManager : public i3GameNode
{
	I3_CLASS_DEFINE(CRoomMasterKickManager, i3GameNode);

public:
	CRoomMasterKickManager();
	virtual ~CRoomMasterKickManager();

public:

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;

public:
	void RemoveAwayChecker();
	void AddAwayChekcer();
	const CAwayChecker* GetAwayChecker();

	void SetGameStart(bool start) { gamestart = start; }
	void CloseMessageBox();

	void ResetAwayChecker();
	void ReStartAwayChecker();

	void MoveSubPhase();
	
private:

	bool _CheckRoomMasterKick();
	bool _IsHost();

	void _ProcessAway();

	void __CountDownSound();
	void __CheckAllReady();
	void __ProcessMessageBox();
	
	void _EnterLobby();
	

private:
	
	INT32 other_user_count;
	INT32 roommaster_countdown_time;
	
	CAwayChecker*		awaychecker;
	i3SoundPlayInfo*	count_down_sound;
	UIMessageBox*		messagebox;

	bool roommaster_countdown;
	bool all_ready;
	bool gamestart;
	bool roomKick;
};

#endif