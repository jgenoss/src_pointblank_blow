#pragma once

#include "UIHUDBase.h"
#include "UISecCount.h"
#include "UIBattleDef.h"

enum BATTLE_FORCIBLYREMOVE_RESULT;
enum FORCE_REMOVE_UI_TYPE;

class UIHUDForceRemove : public UIHUDBase
{
	friend class UIBattlePopupAccuseCrime;

	I3_CLASS_DEFINE( UIHUDForceRemove, UIHUDBase);

public:
	UIHUDForceRemove();
	virtual ~UIHUDForceRemove();

	virtual void OnCreate( i3GameNode * pParent) override;
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void OnLoadAllScenes() override;
	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;
	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool OnKeyInput( i3InputDeviceManager * pInputMng) override;
	virtual void _InitializeAtLoad(i3UIScene * pScene) override;

private:
	void	Vote( bool bAgree);
	void	EndVote();

private:
	void to_Lua_Switch(FORCE_REMOVE_UI_TYPE type);
	void to_lua_VoteGuide();	// 투표 권유(안내)
	void to_Lua_VoteStart();	// 투표 시작. 후보자 닉네임, 사유 표시
	void to_Lua_VoteCount();	// 투표 진행 표시(찬성,반대)

	void to_Lua_VoteResult(BATTLE_FORCIBLYREMOVE_RESULT Res); // 투표 결과
	void to_Lua_VoteCancel(); //투표 취소

private:
	i3::rc_wstring m_wstrAppNickName, m_wstrCandidateNickName;
	i3::rc_wstring m_wstrRemainTimeStr, m_wstrRemainSecondStr;
	
	FORCEREMOVE_CAUSE m_Reason;

	INT32 m_VoteTime;
	INT32 m_numAgree, m_numDisagree;			// 자기 자신의 투표현황패킷을 못받기 때문에 변수보관합니다...
	bool m_bStartVote;			//투표 진행 유무(투표 하냐? 안 하냐?)
	bool m_bVoted;				//현재 유저 투표 유무(했냐? 안 했냐?)
	bool m_bMyVoteResult;		//현재 유저 투표값(찬성/반대) - 서버와 달리 1이 동의, 0이 비동의입니다.
	bool m_bReporter;			// 신고자 Flag
	BATTLE_FORCIBLYREMOVE_RESULT m_VoteResult;
	bool m_bUISTART_finished;	// UI START의 출력 시간(3)이 끝났는지

	// UI 하단 프로그레스바 관련
	i3UIProgressBar* m_ProgressBar[3];
	float			 m_progressValue;

	// 윈도우 슬라이딩 기능
	enum { MAX_SLIDE_WND = 4 };
	i3UIFrameWnd*	m_pMainFmw[MAX_SLIDE_WND];

	UISecCount				m_SecCount;			// 시간재는 유틸..
	INT32	m_CurUIState;		// 현재 UI창 상태
	INT32	m_NextUIState;		// UI창 상태를 바꾸기 직전의 상태 to do sliding.

public:
	void			OnSecCount_Start(INT32 result_time);
	void			OnSecCount_Change(INT32 result_time);
	void			OnSecCount_DoneOrStop(INT32 result_time);

private:
	// 일반적으로 START - GUIDE - VOTE - RESULT 순으로 UI가 진행됩니다
	// 신고자의 경우 START - VOTE - RESULT
	void			ChangeUIState(INT32 _State);
	void			SetProgressBar(REAL32 rDeltaSeconds);
	void			SetPopupSlide(REAL32 rDeltaSeconds);

	void	ResetVoteWnd();	
	void	SetVoteWnd_Close(INT32 ui_type, REAL32 rDeltaSeconds, bool half);	// 슬라이드로 닫기	// half:슬라이드 반만 닫기 여부
	void	SetVoteWnd_Open(INT32 ui_type, REAL32 rDeltaSeconds);	// 슬라이드로 열기

	TUISecCallback<UIHUDForceRemove, 
		&UIHUDForceRemove::OnSecCount_Start,
		&UIHUDForceRemove::OnSecCount_Change,
		&UIHUDForceRemove::OnSecCount_DoneOrStop,
		&UIHUDForceRemove::OnSecCount_DoneOrStop
	> m_SecCountCB;

	//----------------------------------------------------------------------//
	//						Received Game Eent Functions					//
	//----------------------------------------------------------------------//
private:
	typedef void	(UIHUDForceRemove::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_KickVote_SuggestResult( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Start( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Update( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Count( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Cancel( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Result( INT32 arg, const i3::user_data&);	

};