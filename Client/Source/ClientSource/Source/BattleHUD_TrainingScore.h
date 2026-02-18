#pragma once

#include "BattleHUD_Define.h"

//챌린지 모드일 때 생성되며, 2명 이상일 경우에 나타난다.
//개인 경쟁점수 표시
class CHud_TrainingScore : public i3GameObjBase
{
	I3_CLASS_DEFINE(CHud_TrainingScore, i3GameObjBase);

public:
	CHud_TrainingScore(void);
	~CHud_TrainingScore(void);

public:
	static CHud_TrainingScore* instance();

public:
	void	CreateHUD(i3AttrSet* pHUDRoot, i3Texture* pTexture, i3Texture* pTextureRankMark);
	void	OnUpdate( REAL32 rDeltaSeconds);

private:
	void	ShowRankMarkOnTraining(INT32 nSlot,INT32 nRank);
#if defined( ENABLE_BEST_RECORD)
	void	UpdateTrainingRecord( REAL32 rDeltaSeconds);
#endif

private:
	i3Sprite2D *	m_pSpriteTrainingBG;
	i3Sprite2D *	m_pSpriteTrainingRankMark[MAX_TEAM_USER];
	i3TextNodeDX2 * m_pTextTrainingUserNick[MAX_TEAM_USER];
	i3TextNodeDX2 * m_pTextTrainingUserScore[MAX_TEAM_USER];
	i3TextNodeDX2 * m_pTextTrainingHP[MAX_TEAM_USER];
	i3Sprite2D *	m_pSpriteTrainingHPGauge[MAX_TEAM_USER];
	i3Sprite2D *	m_pSpriteTrainingRecordBG;
	i3TextNodeDX2 * m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_COUNT];
};
