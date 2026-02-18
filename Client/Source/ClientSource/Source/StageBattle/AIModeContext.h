#if !defined( __AI_MODE_CONTEXT_H)
#define __AI_MODE_CONTEXT_H

struct AIModeContext : public i3::shared_ginst<AIModeContext>
{
	INT32 FriendCount;			// 우군 수
	
	INT32 EnemyCount;			// 적군 수
	INT32 NewEnemyCount;		// 

	INT32 StartLevel;
	INT32 CurrentLevel;			// AIMode 난이도

	INT32 CurrentAILevel[SLOT_MAX_COUNT];		// 첼린지에 난입한 경우 Ai 케릭터 별 레벨
	AI_ENTRY AIEntry[SLOT_MAX_COUNT];			// 훈련전 난이도 정보

	INT32 KillCount;
	UINT16 UserScore[SLOT_MAX_COUNT]; 

	AIModeContext();

	/** \brief 다음 레벨에 사용하는 무기들을 BGLoad에 담는다. */
	void		PrepareAIWeapon( void);
	
};

#endif