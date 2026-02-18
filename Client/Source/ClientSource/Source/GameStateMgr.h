#if !defined(__GAME_STATE_MGR_H)
#define __GAME_STATE_MGR_H

#include <unordered_map>

//순서 중요합니다. 변경하지 마세요. 
enum GAMESTAGE_STATE
{
	GAMESTAGE_STATE_NONE = 0,
	GAMESTAGE_STATE_BATTLE,				// 전투중인 상태
	GAMESTAGE_STATE_ENDROUND,			// 라운드 종료 상태
	GAMESTAGE_STATE_SHOWSCORE,			// 스코어 뷰어 상태
	GAMESTAGE_STATE_ENDGAME,			// 게임 종료 상태
	GAMESTAGE_STATE_PRESTART_ROUND,		// Round를 시작하기 전 대기 상태. (현재 보스미션에서만 사용)

	GAMESTAGE_STATE_WAIT_HOLEPUNCHING,	// 홀펀칭 완료 체크
	GAMESTAGE_STATE_DOMI_NEW_FLOW,	// 제압 라운드->라운드(스테이지 변경)

	GAMESTAGE_STATE_MAX,
};

//아 gstatemgr클래스가 i3GameNode를 상속받지 못하므로 .. 
class gstatemgr_AvatarChange : public i3GameNode
{
	I3_CLASS_DEFINE( gstatemgr_AvatarChange, i3GameNode );

public:
	gstatemgr_AvatarChange();
	virtual ~gstatemgr_AvatarChange();

	void _ProcessEvent_Respawn_AvatarChange(INT32 idx, const i3::user_data& UserData);
	static void	sAfterLoadChara( void * pAvatar, INT32 idx = -1, AI_BIND_DATA * pAIBind = nullptr);
};

class gstatemgr
{
public:	
	static gstatemgr*	i()  { return tinst<gstatemgr>(); }

	void ProcessCreateStage(CStageBattle* sb);
	void ProcessDestroyStage();
#ifdef DOMI_NEW_FLOW
	void ProcessCreateRoundJumpStage(CStageBattle* sb);
	void ProcessDestroyRoundJumpStage();
#endif
	void ProcessPreUpdate(REAL32 fDeltaTime);
	void ProcessUpdate(REAL32 fDeltaTime);
	void Process_InitObject( void);

	void setEndTime( REAL32 fTime)	{ m_fEndTime = fTime;		}
	void setRespawnEnable( bool bEnable) { m_bRespawnEnable = bEnable; }
	
	GAMESTAGE_STATE getStageState( void )		{ return m_nStageState; }
	void			setStageState( GAMESTAGE_STATE eState )		{ m_nStageState = eState; }
	bool			isPlayingBattle( void)		{ return (m_nStageState == GAMESTAGE_STATE_BATTLE); }

	void	UpdateScore(bool bVisible, REAL32 tm) { gstatemgr::_UpdateScore(bVisible, tm); }
	void	EnableHUD(bool bEnable, enum GAME_GUI_STATUS nState, bool bDeath = false) { gstatemgr::_EnableHUD(bEnable, nState, bDeath); };

	void	ForceProcessEvent( GAME_EVENT nEvent, INT32 Arg, const i3::user_data& UserData);		// Arg = 0

	bool	GetChangeChara( INT32 idx);
	void	SetChangeChara(bool val) { m_bChangeChara = val; }

	void	SetLoadSuccess(bool bFlag)	{ m_bLoadSuccess = bFlag; }
	bool	IsLoadSuccess() const	{ return m_bLoadSuccess; }

	gstatemgr_AvatarChange* GetStatemgr_AvatarChange() const { return m_pgStatemgr_AvatarChange; }
private:

	gstatemgr();
	~gstatemgr();

	void _ProcessGameContext(REAL32 fDeltaTime);
	//  리시브된 게임이벤트분기 함수들...
	typedef void	(gstatemgr::*evt_fn)(INT32 arg, const i3::user_data& UserData);
	//
	void	reg_evt( GAME_EVENT event, evt_fn);

	void	_Process_EVENT_BATTLE_ENTERANCE_USER(INT32 arg, const i3::user_data& UserData);
	void	_Process_EVENT_RENDEZVOUS_HOLEPUNCH_BATTLE(INT32 arg, const i3::user_data& UserData);
	void	_Process_EVENT_PRESTART_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_START_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_GIVEUP_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_DEATH_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_RESPAWN_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_INGAMESHOP_APPLY_ITEM(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_RESPAWN_BATTLE_FOR_AI(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_START_COUNTDOWN(INT32, const i3::user_data& UserData);
	
	void	_Process_EVENT_RESPAWN_DOMI_DINO(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_DEATH_DOMI_USER(INT32 Arg, const i3::user_data& UserData);
	void	_Process_EVENT_WAVE_END(INT32 Arg, const i3::user_data& UserData);

	void	_Process_EVENT_M_END_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_ENTER_LOBBY(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_TIMEOUT_HOST(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_TIMEOUT_CLIENT(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_ARMEDBOMB_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_DISARMBOMB_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_OBJECT_USE_BATTLE(INT32 Arg, const i3::user_data& UserData);
	void	_Process_EVENT_M_PRE_ROUNDSTART_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_ROUNDSTART_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_ROUNDEND_BATTLE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_KICK_USER(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_KICK_VOTE_MY_KICKED(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_HOSTLOST_BATTLE(INT32 Arg, const i3::user_data& UserData);
	void	_Process_EVENT_M_DRIVE_AWAY(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_GAMEGUARD_ERROR(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_ERROR(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_DISCONNECTION(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_SHUTDOWN(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_DESTROYED_NETWORK(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_M_INVALID_TIMESYNC(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_ITEM_AUTH(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_BATTLE_TOUCHDOWN(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_TUTORIAL_ROUND_END(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_BATTLE_ESCAPEBLOCK(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_BATTLE_GM_PAUSE(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_BATTLE_GM_RESUME(INT32, const i3::user_data& UserData);
	void	_Process_EVNET_GM_USER_EXIT(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_HackToolDetected(INT32, const i3::user_data& UserData);
	void	_Process_EVENT_default( GAME_EVENT nEvent, INT32 Arg, const i3::user_data& UserData);
public:
	void	_ProcessEvent_Respawn(INT32, const i3::user_data& UserData);
	void	_ProcessEvent_RespawnForAI(INT32, const i3::user_data& UserData);
private:
	void	_MyCharaRespawnInit( void);

	void	_ProcessUpdateState(REAL32 fDeltaTime, bool& bEnableShowScore);

	typedef	 void (gstatemgr::*state_fn)(REAL32, bool&);
	void	reg_state(GAMESTAGE_STATE state, state_fn f);

	void	_Process_GAMESTAGE_STATE_NONE(REAL32 fDeltaTime, bool& bEnableShowScore);
	void	_Process_GAMESTAGE_STATE_BATTLE(REAL32 fDeltaTime, bool& bEnableShowScore);
	void	_Process_GAMESTAGE_STATE_ENDROUND(REAL32 fDeltaTime, bool& bEnableShowScore);
	void	_Process_GAMESTAGE_STATE_SHOWSCORE(REAL32 fDeltaTime, bool& bEnableShowScore);
	void	_Process_GAMESTAGE_STATE_ENDGAME(REAL32 fDeltaTime, bool& bEnableShowScore);
	void	_Process_GAMESTATE_STATE_PRESTART_ROUND(REAL32 fDeltaTime, bool& bEnableShowScore);
	void	_Process_GAMESTATE_WAIT_HOLEPUNCHING(REAL32 fDeltaTime, bool& bEnableShowScore);

	void	_UpdateObject( REAL32 rDeltaTime );
	void	_Process_EndRound_ShowScore_Common(REAL32 fDeltaTime);
	void	_UpdateScore(bool bVisible, REAL32 tm);
	void	_EnableHUD(bool bEnable, enum GAME_GUI_STATUS nState, bool bDeath = false);

	void	_ChangeObjectSpecificName();

	void	_OnFirstSceneUpdate();
	void	_Function_Null() {}

	void	_UpdateRespawnUI(REAL32 fDeltaTime);
	void	_UpdateShowScore(float fDeltaTime, bool bEnableShowScore);
	void	_UpdateWeaponLRU(REAL32 fDeltaTime);

	void	_ToggleUpdateInput(void);
	void	_AutoRespawnAiCharacter( REAL32 fDeltaTime);		// 폐기예정..
	void	_ToggleChara(UINT32 Arg);

	void	_ProcessTabminimapEvent(GAME_EVENT nEvent, INT32 Arg, const i3::user_data& UserData);

	state_fn	m_state_fn[GAMESTAGE_STATE_MAX];		
	i3::unordered_map< GAME_EVENT, evt_fn >	m_evt_fn_map;
	
	void (gstatemgr::*		m_init_update_fn)();

	CStageBattle*			m_stage;

	GAMESTAGE_STATE			m_nStageState;				// 이 클래스에서는 아직 미구현상태..
	REAL32					m_fEndTime;
	REAL32					m_fRoundTime;				
	REAL32					m_fRespawnDelayTime;		// 

	INT32					m_nSvrRoundWinnerTeam;		// 서버쪽에서 라운드전의 승리팀값입니다.
	INT32					m_nRoundWinnerTeam;			// 공수교대가 실행되었으면 서버쪽에서 보낸승리팀과 반대팀으로 바뀝니다.
														// 라운드전의 승리팀 보관..(업데이트중 HUD표현에 쓰이긴 한다..분석이 더 진행되면 변수위치를 바꿀것..) --> 여기 작업....

	bool					m_bRespawnEnable;			
	bool					m_bNormalEnd;				// 폭탄해제사운드관련이지만, RoundEnd에서 범용적으로 true로 설정됨..
	bool					m_bDriveAway;				// DRIVE_AWAY시 DISCONNECTION_M 메시지를 무시하기 위한 플레그	


	REAL32					m_aiCharacterRespawnTimer[SLOT_MAX_COUNT];		// 

	class CProfileLog*	m_ProfileLog;

	friend tinst_creator<gstatemgr>;

	gstatemgr_AvatarChange* m_pgStatemgr_AvatarChange;
	
	bool m_bChangeChara;

	bool					m_bLoadSuccess;

};

tinst<gstatemgr>;



#endif