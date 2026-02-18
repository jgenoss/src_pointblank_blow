#ifndef _TrainingScoreBoard_h
#define _TrainingScoreBoard_h

#if 지워질_것들

enum
{
	TR_SCORE_BOARD_TEXT_POINT_GET_KEY = 0,
	TR_SCORE_BOARD_TEXT_POINT_GET,
	TR_SCORE_BOARD_TEXT_LEVEL_KEY,
	TR_SCORE_BOARD_TEXT_LEVEL,
	TR_SCORE_BOARD_TEXT_LEVEL_TEXT,

	TR_SCORE_BOARD_TEXT_COUNT,
};

class CTrainingScoreBoard: public i3GameObjBase
{
	I3_CLASS_DEFINE(CTrainingScoreBoard, i3GameObjBase);
public:
	CTrainingScoreBoard();
	virtual ~CTrainingScoreBoard();

	virtual BOOL Create(i3Node *pParentNode = NULL, BOOL bEnable = TRUE);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

	void SetScore(INT32 score);

private:
	i3Texture*		m_scoreBoardTexture;
	i3Sprite2D*		m_scoreBoardNode;
	i3TextNodeDX2*	m_scoreTextNode[TR_SCORE_BOARD_TEXT_COUNT];

	REAL32			m_updateTimer;
	INT32			m_score;
};

#endif

#endif
