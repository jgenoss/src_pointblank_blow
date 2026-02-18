#if !defined( __CSTAGE_BATTLE_H__)
#define __CSTAGE_BATTLE_H__

#include "RevengeManager.h"
#include "./StageBattle/HitPartContext.h"
#include "./StageBattle/GrenadeContext.h"

#include "./StageBattle/C4Context.h"
#include "./StageBattle/IntrudeContext.h"

#include "StageBattleMovement_struct.h"

#define RESPAWN_COOL_TIME 3.0f

class UI_HUD_Tutorial;
class UIBattleFrame;
class CTutorialMode;
class CRevengeManager;
class UIBattleFrame;

class CStageBattle : public i3Stage
{
	I3_CLASS_DEFINE( CStageBattle, i3Stage);

	enum BATTLE_LOAD_STATE
	{
		BLS_STEP1,
		BLS_STEP2,
		BLS_STEP3,
		BLS_STEP4,
		BLS_STEP5,
		BLS_STEP6,
		BLS_STEP7,
		BLS_COMPLETE
	};
private:
	CSBM StageMovement;

protected:
	bool				m_bLoadCompleteGameData;
	BATTLE_LOAD_STATE	m_BattleLoadState;
	
	UIBattleFrame *		m_pUIFrame;

public:
	void				LoadProperties(void);

protected:
	void			_LoadReflectMap(void);
	void			_LoadPostProcess(void);
	void			_CalcBoundForShadow(void);
	
	////////////////////
	// Edge Trail (칼날 궤적)
	void			_InitEdgeTrail(void);
	////////////////////

	// Respawn battle init
	void			_InitObject( void);


public:
	CStageBattle(void);
	virtual ~CStageBattle(void);

	virtual bool	OnQueryLoad(INT32	nStageIndex) override;
	virtual void	OnLoading(void) override;
	virtual void	OnLoadingGameData(void) override;
	virtual void	OnLoadEnd(void) override;

	virtual i3Node	* OnQueryAttachNode( i3GameRes * pRes) override;

	virtual void	OnCreate(void) override;
	virtual bool	OnFinish(void) override;
	virtual void	OnUpdate(REAL32 fDeltaTime) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override {}
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual bool	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;

	virtual bool	IsLoadCompleteGameData(void) override { return m_bLoadCompleteGameData; }
	
	void			Destroy( void );

	void			SetObjectPhysixEnable( bool bValue );

	bool			GetChatEnable(void);
	bool			IsEnableChatScroll(void);
	bool			IsEnableGui(void);

	const REAL32	GetStageAddMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT)		{ I3ASSERT(eMST < MST_COUNT); I3ASSERT(eMDT < MDT_COUNT); return StageMovement.GetStageAddMoveSpeed(eMST, eMDT); }
	const REAL32    GetStageAddAccelerationTime()											{ return StageMovement.AccelerationTime; }
	const REAL32    GetStageAddAcceleration_Down_Speed_late()								{ return StageMovement.Acceleration_Down_Speed_late; }
	const REAL32    GetStageAddAcceleration_Up_Speed_late()									{ return StageMovement.Acceleration_Up_Speed_late; }
	
	const REAL32    GetStageGravity()														{ return StageMovement.m_rGravity; }
	const REAL32    GetStage_Chara_Down_Gravity_Speed_late()								{ return StageMovement.m_rChara_Down_Gravity_Speed_late; }
	const REAL32    GetStage_Chara_Up_Gravity_Speed_late()									{ return StageMovement.m_rChara_Up_Gravity_Speed_late; }
	const REAL32    GetStage_Chara_Fall_Gravity_late()										{ return StageMovement.m_rChara_Fall_Gravity_Speed_late; }
	const REAL32	GetGrageGravityRestore()												{ return StageMovement.m_rTempGravity; }

private:

	void			_AdjustObjectMaterial(void);
	void			_ValidateShaders(void);
	void			_SubValidateShader( i3ShaderCode * pCode);

	void			_OnLoadGameData(void);

	// 공통으로 생성하는 데이터
	bool			_OnLoadCommon_Step1(void);
	bool			_OnLoadCommon_Step2(void);
	bool			_OnLoadCommon_Step3(void);
	bool			_OnLoadCommon_Step4(void);
	bool			_OnLoadCommon_Step6(void);
	bool			_OnLoadCommon_Step7(void);

	// 일반 모드에서의 데이터 생성
	void			_OnLoadDefault_Step1(void);
	void			_OnLoadDefault_Step2(void);
	void			_OnLoadDefault_Step3(void);
	void			_OnLoadDefault_Step4(void);
	void			_OnLoadDefault_Step5(void);
	void			_OnLoadDefault_Step6(void);
	void			_OnLoadDefault_Step7(void);
	void			_DestroyInDefault();

#ifdef DOMI_NEW_FLOW
	void			_OnLoadStageJump_Step1(void);
	void			_OnLoadStageJump_Step2(void);
	void			_OnLoadStageJump_Step3(void);
	void			_OnLoadStageJump_Step4(void);
	void			_OnLoadStageJump_Step5(void);
	void			_OnLoadStageJump_Step6(void);
	void			_OnLoadStageJump_Step7(void);
	void			_DestroyInRoundStageJump();
#endif

	bool			_IsDominationFlow(void);
	
public:
	i3Object* GetI3Object(i3ClassMeta* pMeta);
	i3Object* GetI3Object(INT32 iIdx);

	//LSR_AVATAR_SYSTEMz
	static void			sAfterLoadChara( void * pAvatar, INT32 idx = -1);

	static void			cbExitBattle(void *pThis, int nParam);

};

#endif
