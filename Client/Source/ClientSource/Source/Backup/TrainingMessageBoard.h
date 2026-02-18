#ifndef _TrainingMessageBoard_h
#define _TrainingMessageBoard_h

#if 지워질_것들

#define TRAINING_MESSAGE_START_DELAY			0.3f
#define TRAINING_MESSAGE_END_DELAY				5.0f
#define TRAINING_MESSAGE_SPEED_PER_SECONDS		30.0f	// 초당 출력 문자 수
#define MAX_TUTORIAL_MESSAGE					29

enum TRAINING_MESSAGE
{
	TM_NONE = -1,

	TM_20_KILL,
	TM_40_KILL,
	TM_60_KILL,
	TM_80_KILL,
	TM_100_KILL,
	TM_120_KILL,
	TM_140_KILL,
	TM_160_KILL,
	TM_180_KILL,
	TM_200_KILL,
	TM_5_DEATH,
	TM_20_DEATH,
	TM_5_CHAIN_KILL,
	TM_20_CHAIN_KILL,
	TM_PIERCING_KILL,
	TM_MASS_KILL,
	TM_CHAIN_SLUGGER,
	TM_CHAIN_STOPPER,
	TM_BATTLE_START,

	MAX_TRAINING_MESSAGE
};

enum TRAINING_MESSAGE_BOARD_STATE
{
	TMBS_IDLE,
	TMBS_START,
	TMBS_PRINT,
	TMBS_WAIT
};

enum MESSAGE_BOARD_TYPE
{
	MBT_TRAINING,
	MBT_TUTORIAL,
	MBT_NOT_SET
};

class CMessageBoard: public i3GameObjBase
{
	I3_CLASS_DEFINE(CMessageBoard, i3GameObjBase);
public:
	CMessageBoard();
	virtual ~CMessageBoard();

	virtual BOOL Create(MESSAGE_BOARD_TYPE eType, i3Node *pParentNode = NULL, BOOL bEnable = TRUE);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

	void SetTeam(TEAM_TYPE teamType);
	void SetMessage(INT32 message);	
	void SetEnableBoardText(BOOL bFlag);

private:
	void _ChangeState(TRAINING_MESSAGE_BOARD_STATE state);
	const char* _GetMessageText(void);

	static INT32 _LLength(const char* string);
	static INT32 _LCopy(char* dest, const char* src, INT32 letterCount);

private:
	TRAINING_MESSAGE_BOARD_STATE m_state;
	REAL32				m_stateTimer;
	INT32				m_currentMessage;	
	TEAM_TYPE			m_team;

	i3Texture*			m_messageBoardTexture;
	i3Sprite2D*			m_messageBoardNode;
	
	MESSAGE_BOARD_TYPE	m_eMessageBoardType;

public:
	i3TextNodeDX2*		m_messageTextNode;
	i3TextNodeDX2*		m_TextNodeForTutorial;
	void SetMessageBoardType(MESSAGE_BOARD_TYPE eType) { m_eMessageBoardType = eType; }

	// 출력 문구 관련
private:
	const char*			m_szTrainingTableText[MAX_TRAINING_MESSAGE];
	const char*			m_szTutorialTableText[MAX_TUTORIAL_MESSAGE];

public:
	void InitMessageText();
};

#endif

#endif
