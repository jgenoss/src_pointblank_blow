#if !defined( __GAME_MODE_H)
#define __GAME_MODE_H

/*
enum STAGE_MODE
{
	STAGE_MODE_NA				= 0,
	STAGE_MODE_DEATHMATCH,
	STAGE_MODE_BOMB,
	STAGE_MODE_DESTROY,	
	STAGE_MODE_ANNIHILATION,
	STAGE_MODE_DEFENCE,
	STAGE_MODE_SUDDEN_DEATH,
	STAGE_MODE_ESCAPE,
	STAGE_MODE_HEAD_HUNTER,
	STAGE_MODE_HEAD_KILLER,
	STAGE_MODE_TUTORIAL,
	STAGE_MODE_DOMINATION,
	STAGE_MODE_CROSSCOUNT, --->추가된것으로 보인다..
	STAGE_MODE_MAX
};
*/

enum MADNESS_AMBIENT
{
	MADNESS_AMBIENT_DROPWATER,
	MADNESS_AMBIENT_DROPWATER2,
	MADNESS_AMBIENT_SCREEM2,
	MADNESS_AMBIENT_SCREEM,

	MADNESS_AMBIENT_COUNT,
};

void				create_gmode(STAGE_MODE t);
void				destroy_gmode();

namespace Domination_Util { class CNaviMesh;}
namespace du = Domination_Util;

template<> struct is_tinst_manual<class gmode> : std::tr1::true_type {};

class CStageBattle;

class gmode
{
public:
	static gmode*		i() { return tinst<gmode>(); }
	void	SetStage( CStageBattle* pStage )		{ m_pStage	= pStage;	}

	virtual class CTutorialMode*	GetTutorialMode() const { return nullptr; }

	virtual void ProcessOnFinish() {}
	virtual void ProcessCreateStage( CStageBattle* pStage)	{ SetStage( pStage ); }
	virtual void ProcessLoadProperties(i3RegKey* stage_key) {}
	virtual void Process_Event_RoundStart_MissionNoticeString() {}
	virtual void Process_Event_RoundEnd_MissionEnd_ObjectExplosion(INT32 i32Winner){};

	virtual void ProcessEventPreStartRound(void)	{}
	virtual void ProcessEventRoundStart(void)		{}
	virtual void ProcessEventRoundEnd(void)			{}

	virtual void ProcessEventRespawn( INT32)		{}
	virtual void ProcessEventGiveUpBattle( INT32)	{}

	virtual void OnUpdateEndGame( REAL32 rDeltaSeconds)	{}

	virtual void Process_Event_RoundEnd_MissionEndBombFire(bool& bNormalEnd) {}
	virtual void Process_Event_RoundEnd_IfNoBombFire_ResetBomb(MISSION_END_TYPE met) {}

	virtual void ProcessEventDefault( INT32 nEvent, INT32 Arg, const i3::user_data& UserData)	{}

	virtual void ProcessUpdate(REAL32 fDeltaTime) {}

	virtual void ResetObjectStatus(void);
	virtual void InitialIzeStage(void)				{ ResetObjectStatus();	}

	virtual bool OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)	{ return false;}

	virtual du::CNaviMesh* GetNaviMesh() const { return nullptr;}

	virtual ~gmode() {}
protected:
	gmode();

	
protected:
	CStageBattle*		m_pStage;
};

struct BombInfo
{ 
	BombInfo();
	bool IsValid() const;

	VEC3D Pos; 
	INT32 FloorID;
};
class gmode_include_bomb : public gmode
{
protected:
	gmode_include_bomb();
	// 폭파 미션 관련
	//VEC3D *			getBombPosition( BOMB_AREA idx)	{ I3_BOUNDCHK( idx, MAX_BOMB_AREA_COUNT); return &m_BombInfos[ idx].Pos; }
	//INT32			getBombFloorId( BOMB_AREA idx)	{ I3_BOUNDCHK( idx, MAX_BOMB_AREA_COUNT); return m_BombInfos[ idx].FloorID; }
	const BombInfo& GetBombInfo(BOMB_AREA area) const;

private:
	virtual void	ProcessLoadProperties(i3RegKey* stage_key) override;
	
	// 폭파미션 폭탄위치, 층 ID
	//BombInfo	m_BombInfos[MAX_BOMB_AREA_COUNT];	// 0->A지점 , 1->B지점
};


class gmode_na : public gmode
{
public:
	gmode_na();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
};

class gmode_deathmatch : public gmode
{
public:
	gmode_deathmatch();
private:
	int		m_nMadnessState;
	int		m_nMadness_Ambient_Seed;
	int		m_nMadnessAmbientPlayCount;
	bool	m_bMadnessAmientTable[MADNESS_AMBIENT_COUNT];
	float	m_fMadnessAmbientTime;

	void	ProcessMadnessAmbient(REAL32 rDeltaSeconds);
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
	virtual void ProcessUpdate(REAL32 fDeltaTime) override;
	virtual void ProcessEventRoundStart(void) override;
};

class gmode_bomb : public gmode_include_bomb
{
public:
	gmode_bomb();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
	virtual void Process_Event_RoundStart_MissionNoticeString() override;
	virtual void Process_Event_RoundEnd_MissionEndBombFire(bool& bNormalEnd) override;
	virtual void Process_Event_RoundEnd_IfNoBombFire_ResetBomb(MISSION_END_TYPE met) override;
};

class gmode_destroy : public gmode
{
public:
	gmode_destroy();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
	virtual void Process_Event_RoundStart_MissionNoticeString() override;
	virtual void Process_Event_RoundEnd_MissionEnd_ObjectExplosion(INT32 i32Winner) override;
};

class gmode_annihilation : public gmode
{
public:
	gmode_annihilation();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
	virtual void Process_Event_RoundStart_MissionNoticeString() override;
};

class gmode_defense : public gmode
{
public:
	gmode_defense();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
	virtual void Process_Event_RoundStart_MissionNoticeString() override;
	virtual void ProcessUpdate(REAL32 fDeltaTime) override;
	virtual void Process_Event_RoundEnd_MissionEnd_ObjectExplosion(INT32 i32Winner) override;

	REAL32 m_rRemainTimeForBGM;
};

class gmode_sudden_death : public gmode
{
public:
	gmode_sudden_death();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
	virtual void Process_Event_RoundStart_MissionNoticeString() override;
};

class gmode_escape : public gmode
{
public:
	gmode_escape();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
};

class gmode_head_hunter : public gmode
{
public:
	gmode_head_hunter();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
};

class gmode_head_killer : public gmode
{
public:
	gmode_head_killer();
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
	virtual void Process_Event_RoundStart_MissionNoticeString() override;
};


class gmode_tutorial : public gmode_include_bomb
{
public:
	gmode_tutorial();

private:
	virtual CTutorialMode*	GetTutorialMode() const override { return m_pTutorialMode; }
	virtual void ProcessCreateStage(CStageBattle* s) override;
	virtual void ProcessOnFinish() override;
	CTutorialMode* m_pTutorialMode;			// 튜토리얼 모드

};

class gmode_cc : public gmode 
{
public:
	
private:
	virtual void ProcessCreateStage(CStageBattle* s) override;
};

#endif