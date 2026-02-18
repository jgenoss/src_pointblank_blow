#ifndef __UNIT_H__
#define __UNIT_H__

#include "DrawObject.h"
#include "../ComponentBase.h"


namespace minimap_new
{
	//----------------------------------------------
	// Units
	//----------------------------------------------
	class Unit
	{
	public:
		Unit();
		virtual ~Unit();

		virtual const char* GetName() const = 0;
		virtual void OnStart() {}
		virtual void Update(REAL32 tm) {}
		virtual void OnEnd() {}

		GameObject* GetGameObject() { return m_unit; }

	protected:
		GameObject* m_unit;
	};

	class UnitCharaMe : public Unit
	{
	public:
		explicit UnitCharaMe(INT32 slotIdx);
		virtual ~UnitCharaMe() {}

		virtual const char* GetName() const override { return "UnitCharaMe"; }
		virtual void Update(REAL32 tm) override;
	};

	class UnitCharaFellow : public Unit
	{
	public:
		explicit UnitCharaFellow(INT32 slotIdx);
		virtual ~UnitCharaFellow() {}

		virtual const char* GetName() const override { return "UnitCharaFellow"; }
		virtual void Update(REAL32 tm) override;
	};

	class UnitBombA : public Unit
	{
	public:
		UnitBombA();
		virtual ~UnitBombA() {}

		virtual const char* GetName() const override { return "UnitBombA"; }
		virtual void Update(REAL32 tm) override;
	};

	class ImageMissionObjWithSiren;
	class UnitBombB : public Unit
	{
	private:
		ImageMissionObjWithSiren* m_pImage;
	public:
		UnitBombB();
		virtual ~UnitBombB() {}

		virtual const char* GetName() const override { return "UnitBombB"; }
		virtual void OnStart() override;
		virtual void Update(REAL32 tm) override;
	};
	class UnitDestroyRed : public Unit
	{
	public:
		UnitDestroyRed();
		virtual ~UnitDestroyRed() {}

		virtual const char* GetName() const override { return "UnitDestroyRed"; }
		virtual void Update(REAL32 tm) override;
	};
	class UnitDestroyBlue : public Unit 
	{
	public:
		UnitDestroyBlue();
		virtual ~UnitDestroyBlue() {}

		virtual const char* GetName() const override { return "UnitDestroyBlue"; }
		virtual void Update(REAL32 tm) override;
	};
	class UnitDefenceRed : public Unit 
	{
	public:
		UnitDefenceRed();
		virtual ~UnitDefenceRed() {}

		virtual const char* GetName() const override { return "UnitDefenceRed"; }
		virtual void Update(REAL32 tm) override;
	};
	class UnitDefenceBlue : public Unit 
	{
	public:
		UnitDefenceBlue();
		virtual ~UnitDefenceBlue() {}

		virtual const char* GetName() const override { return "UnitDefenceBlue"; }
		virtual void Update(REAL32 tm) override;
	};

	class UnitDomiDino : public Unit
	{
	public:
		explicit UnitDomiDino(INT32 slotIdx);
		virtual ~UnitDomiDino() {}

		virtual const char* GetName() const override { return "UnitDomiDino"; }
		virtual void Update(REAL32 tm) override;
	};

	class UnitSentrygun : public Unit
	{
	public:
		explicit UnitSentrygun(INT32 idx);
		virtual ~UnitSentrygun() {}

		virtual const char* GetName() const override { return "UnitSentrygun"; }
		virtual void Update(REAL32 tm) override;
		INT32 GetObjIdx() const { return m_idx; }

	private:
		INT32 m_idx;
	};

	class UnitDummy : public Unit
	{
	public:
		explicit UnitDummy(INT32 idx);
		virtual ~UnitDummy() {}

		virtual const char* GetName() const override { return "UnitDummy"; }
		virtual void Update(REAL32 tm) override;
		INT32 GetObjIdx() const { return m_idx; }

	private:
		INT32 m_idx;
	};

	class UnitSkillShop : public Unit
	{
	public:
		explicit UnitSkillShop(INT32 idx);
		virtual ~UnitSkillShop() {}

		virtual const char* GetName() const override { return "UnitSkillShop"; }
		virtual void Update(REAL32 tm) override;
		INT32 GetObjIdx() const { return m_idx; }

	private:
		INT32 m_idx;
	};

	// Å»Ãë¸ðµå À¯´Ö Ãß°¡
	class UnitUsurpaiton : public Unit
	{
	public:
		explicit UnitUsurpaiton(INT32 slotIdx);
		virtual ~UnitUsurpaiton() {}

		virtual const char* GetName() const override { return "UnitUsurpaiton"; }
		virtual void Update(REAL32 tm) override;
	};
	class UnitCleaverKnifeA : public Unit
	{
	public:
		explicit UnitCleaverKnifeA();
		virtual ~UnitCleaverKnifeA() {}

		virtual const char* GetName() const override { return "UnitCleaverKnifeA"; }
		virtual void Update(REAL32 tm) override;
	};
	class UnitCleaverKnifeB : public Unit
	{
	public:
		explicit UnitCleaverKnifeB();
		virtual ~UnitCleaverKnifeB() {}

		virtual const char* GetName() const override { return "UnitCleaverKnifeB"; }
		virtual void Update(REAL32 tm) override;
	};

	//----------------------------------------------
	// Images
	//----------------------------------------------
	class Images : public Component
	{
	public:
		DECLARE_FAMILY_ID("Images");

		Images();
		virtual ~Images();

		virtual DrawObject* GetImages() = 0;
		DrawObject* GetImage(INT32 idx);

	protected:
		DrawObject* m_images;
	};

	class ImagesCharaMe : public Images
	{
	public:
		DECLARE_COMPONENT_ID("ImagesCharaMe");

		ImagesCharaMe() {}
		virtual ~ImagesCharaMe() {}
		virtual DrawObject* GetImages() override;
	};

	class ImagesCharaFellow : public Images
	{
	public:
		DECLARE_COMPONENT_ID("ImagesCharaFellow");

		ImagesCharaFellow() {}
		virtual ~ImagesCharaFellow() {}
		virtual DrawObject* GetImages() override;
	};

	class ImagesDomiDino : public Images
	{
	public:
		DECLARE_COMPONENT_ID("ImagesDomiDino");

		ImagesDomiDino() {}
		virtual ~ImagesDomiDino() {}
		virtual DrawObject* GetImages() override;
	};

	enum IMAGES_MISSION_OBJ
	{
		IMAGE_BOMB_AREA_A,
		IMAGE_BOMB_AREA_B,
		IMAGE_DESTROY_RED,
		IMAGE_DESTROY_BLUE,
		IMAGE_DEFENCE_RED,
		IMAGE_DEFENCE_BLUE,
	};
	class ImagesMissionObject : public Images
	{
	public:
		DECLARE_COMPONENT_ID("ImagesMissionObject");

		ImagesMissionObject() {}
		virtual ~ImagesMissionObject() {}
		virtual DrawObject* GetImages() override;
	};

	enum IMAGES_SIREN
	{
		IMAGE_SIREN_RED,
		IMAGE_SIREN_BLUE,
	};
	class ImagesSiren : public Images
	{
	public:
		DECLARE_COMPONENT_ID("ImagesSiren");

		ImagesSiren() {}
		virtual ~ImagesSiren() {}
		virtual DrawObject* GetImages() override;
	};

	class ImageMissionObjWithSiren : public Component
	{
	public:
		DECLARE_FAMILY_ID("ImageMissionObjWithSiren");
		DECLARE_COMPONENT_ID("ImageMissionObjWithSiren");

		ImageMissionObjWithSiren();
		virtual ~ImageMissionObjWithSiren();

		void SetMissionObj(ImagesMissionObject* o);
		ImagesMissionObject* GetMissionObj() const { return m_missionObj;}

		void SetSiren(ImagesSiren* s);
		ImagesSiren* GetSiren() const { return m_siren; }

		void SetPosition(const VEC2D* pos);

	private:
		ImagesMissionObject* m_missionObj;
		ImagesSiren* m_siren;
	};

	enum IMAGES_SKILLOBJ
	{
		IMAGE_SKILLOBJ_DEACTIVATED,
		IMAGE_SKILLOBJ_ACTIVATED,
		IMAGE_SKILLOBJ_REPAIR_REQUIRING,
	};
	class ImageSentrygun : public Images
	{
	public:
		DECLARE_FAMILY_ID("ImageSentrygun");
		DECLARE_COMPONENT_ID("ImageSentrygun");

		ImageSentrygun(){}
		virtual ~ImageSentrygun(){}

		virtual DrawObject* GetImages() override;
	};

	class ImageDummy : public Images
	{
	public:
		DECLARE_FAMILY_ID("ImageDummy");
		DECLARE_COMPONENT_ID("ImageDummy");

		ImageDummy(){}
		virtual ~ImageDummy(){}

		virtual DrawObject* GetImages() override;
	};

	enum IMAGE_SKILLSHOP
	{
		IMAGE_SKILLSHOP_DEACTIVATED,
		IMAGE_SKILLSHOP_ACTIVATED,
	};
	class ImageSkillShop : public Images
	{
	public:
		DECLARE_FAMILY_ID("ImageSkillShop");
		DECLARE_COMPONENT_ID("ImageSkillShop");

		ImageSkillShop(){}
		virtual ~ImageSkillShop(){}

		virtual DrawObject* GetImages() override;
	};

	class ImagesUsurpation : public Images
	{
	public:
		DECLARE_COMPONENT_ID("ImagesUsurpation");

		ImagesUsurpation() {}
		virtual ~ImagesUsurpation() {}
		virtual DrawObject* GetImages() override;
	};
	class ImageCleaverKnifeA : public Images
	{
	public: 
		DECLARE_COMPONENT_ID("UnitCleaverKnifeA");
		
		ImageCleaverKnifeA() {}
		virtual ~ImageCleaverKnifeA() {}
		virtual DrawObject* GetImages() override;
	};
	class ImageCleaverKnifeB : public Images
	{
	public: 
		DECLARE_COMPONENT_ID("ImageCleaverKnifeB");

		ImageCleaverKnifeB() {}
		virtual ~ImageCleaverKnifeB() {}
		virtual DrawObject* GetImages() override;
	};

	//----------------------------------------------
	// Move
	//----------------------------------------------
	class Move : public Component
	{
	public:
		DECLARE_FAMILY_ID("Move");

		virtual ~Move() {}
		virtual void Render(REAL32 tm) = 0;
	};

	class MoveCharaMe : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveCharaMe");

		MoveCharaMe(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~MoveCharaMe() {}
		virtual void Render(REAL32 tm) override;

	private:
		INT32 m_slotIdx;
	};

	class MoveCharaFellow : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveCharaFellow");

		MoveCharaFellow(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~MoveCharaFellow() {}
		virtual void Render(REAL32 tm) override;

	private:
		INT32 m_slotIdx;
	};

	class MoveDomiDino : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveDomiDino");

		MoveDomiDino(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~MoveDomiDino() {}
		virtual void Render(REAL32 tm) override;

	private:
		INT32 m_slotIdx;
	};

	class MoveDestroyObj : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveDestroyObj");

		MoveDestroyObj(INT32 objIdx) : m_objIdx(objIdx){}
		virtual ~MoveDestroyObj() {}
		virtual void Render(REAL32 tm) override;

	private:
		INT32 m_objIdx;
	};

	class MoveUsurpation : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveUsurpation");

		MoveUsurpation(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~MoveUsurpation() {}
		virtual void Render(REAL32 tm) override;

	private:
		INT32 m_slotIdx;
	};

	class MoveCleaverKnifeA : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveCleaverKnifeA");

		MoveCleaverKnifeA(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~MoveCleaverKnifeA() {}
		virtual void Render(REAL32 tm) override;

	private:
		INT32 m_slotIdx;
	};

	class MoveCleaverKnifeB : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveCleaverKnifeB");

		MoveCleaverKnifeB(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~MoveCleaverKnifeB() {}
		virtual void Render(REAL32 tm) override;

	private:
		INT32 m_slotIdx;
	};

	//----------------------------------------------
	// Action
	//----------------------------------------------
	class Action : public Component
	{
	public:
		DECLARE_FAMILY_ID("Action");

		virtual ~Action() {}
		virtual void Render(REAL32 tm) = 0;
	};

	class AttackCharaFellow : public Action
	{
	public:
		DECLARE_COMPONENT_ID("AttackCharaFellow");

		AttackCharaFellow() {}
		virtual ~AttackCharaFellow() {}
		virtual void Render(REAL32 tm) override;
	};

	class UsualCharaMe : public Action
	{
	public:
		DECLARE_COMPONENT_ID("UsualCharaMe");

		UsualCharaMe() {}
		virtual ~UsualCharaMe() {}
		virtual void Render(REAL32 tm) override;
	};

	class UsualCharaFellow : public Action
	{
	public:
		DECLARE_COMPONENT_ID("UsualFellow");

		UsualCharaFellow() {}
		virtual ~UsualCharaFellow() {}
		virtual void Render(REAL32 tm) override;
	};

	class DeadCharaMe : public Action
	{
	public:
		DECLARE_COMPONENT_ID("DeadCharaMe");

		DeadCharaMe() {}
		virtual ~DeadCharaMe() {}
		virtual void Render(REAL32 tm) override;
	};

	class DeadCharaFellow : public Action
	{
	private:
		REAL32 m_fLifeTime;
	public:
		DECLARE_COMPONENT_ID("DeadCharaFellow");

		DeadCharaFellow() { m_fLifeTime = 0.f; }
		virtual ~DeadCharaFellow() {}
		virtual void Render(REAL32 tm) override;
	};

	class UsualDomiDino : public Action
	{
	public:
		DECLARE_COMPONENT_ID("UsualDomiDino");

		UsualDomiDino() {}
		virtual ~UsualDomiDino() {}
		virtual void Render(REAL32 tm) override;
	};

	class DeadDomiDino : public Action
	{
	private:
		REAL32 m_fLifeTime;
	public:
		DECLARE_COMPONENT_ID("DeadDomiDino");

		DeadDomiDino() { m_fLifeTime = 0.f; }
		virtual ~DeadDomiDino() {}
		virtual void Render(REAL32 tm) override;
	};

	//----------------------------------------------
	// PlayState
	//----------------------------------------------
	class PlayState : public Component
	{
	public:
		DECLARE_FAMILY_ID("PlayState");

		virtual ~PlayState() {}
		virtual INT32 GetState() = 0;
	};


	enum CHARA_COMBAT_STATE
	{
		CHARA_COMBAT_NORMAL,
		CHARA_COMBAT_DEATH,
		CHARA_COMBAT_ATTACK,
		CHARA_COMBAT_MAX,
	};
	class CharaPlayState : public PlayState
	{
	public:
		DECLARE_COMPONENT_ID("CharaPlayState");

		CharaPlayState(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~CharaPlayState() {}
		virtual INT32 GetState() override;

	private:
		INT32 m_slotIdx;
	};

	enum DOMI_DINO_STATE
	{
		DOMI_DINO_NORMAL,
		DOMI_DINO_DEATH,
		DOMI_DINO_STATE_MAX,
	};
	class DomiDinoPlayState : public PlayState
	{
	public:
		DECLARE_COMPONENT_ID("DomiDinoPlayState");

		DomiDinoPlayState(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~DomiDinoPlayState() {}
		virtual INT32 GetState() override;

	private:
		INT32 m_slotIdx;
	};

	//----------------------------------------------
	// AutoAction
	//----------------------------------------------
	class AutoAction : public Component
	{
	public:
		DECLARE_FAMILY_ID("AutoAction");

		AutoAction() {}
		virtual ~AutoAction();

		virtual void Render(REAL32 tm);
		virtual void Register(GameObject* o) = 0;

	protected:
		typedef i3::unordered_map<INT32, Action*> lookupTable;
		lookupTable m_actions;
	};

	class AutoActionCharaMe : public AutoAction
	{
	public:
		DECLARE_COMPONENT_ID("AutoActionCharaMe");

		AutoActionCharaMe() {}
		virtual ~AutoActionCharaMe() {}

		virtual void Register(GameObject* o) override;
	};

	class AutoActionCharaFellow : public AutoAction
	{
	public:
		DECLARE_COMPONENT_ID("AutoActionCharaFellow");

		AutoActionCharaFellow() {}
		virtual ~AutoActionCharaFellow() {}

		virtual void Register(GameObject* o) override;
	};

	class AutoActionDomiDino : public AutoAction
	{
	public:
		DECLARE_COMPONENT_ID("AutoActionDomiDino");

		AutoActionDomiDino() {}
		virtual ~AutoActionDomiDino() {}

		virtual void Register(GameObject* o) override;
	};

	//----------------------------------------------
	// CharaUpdatable
	//----------------------------------------------
	class JudgeUpdatable : public Component
	{
	public:
		DECLARE_FAMILY_ID("JudgeUpdatable");

		JudgeUpdatable() {}
		virtual ~JudgeUpdatable() {}

		virtual bool Judge() = 0;
	};

	class JudgeUpdatableCharaMe : public JudgeUpdatable
	{
	public:
		DECLARE_COMPONENT_ID("JudgeUpdatableChara");

		JudgeUpdatableCharaMe(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~JudgeUpdatableCharaMe() {}

		virtual bool Judge() override;

	private:
		INT32 m_slotIdx;
	};

	class JudgeUpdatableCharaFellow : public JudgeUpdatable
	{
	public:
		DECLARE_COMPONENT_ID("JudgeUpdatableCharaFellow");

		JudgeUpdatableCharaFellow(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~JudgeUpdatableCharaFellow() {}

		virtual bool Judge() override;

	private:
		INT32 m_slotIdx;
	};

	class JudgeUpdatableDomiDino : public JudgeUpdatable
	{
	public:
		DECLARE_COMPONENT_ID("JudgeUpdatableDomiDino");

		JudgeUpdatableDomiDino(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~JudgeUpdatableDomiDino() {}

		virtual bool Judge() override;

	private:
		INT32 m_slotIdx;
	};

	class JudgeUpdatableUsurpation : public JudgeUpdatable
	{
	public:
		DECLARE_COMPONENT_ID("JudgeUpdatableUsurpation");

		JudgeUpdatableUsurpation(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~JudgeUpdatableUsurpation() {}

		virtual bool Judge() override;

	private:
		INT32 m_slotIdx;
	};

	//----------------------------------------------
	// SirenAnimator
	//----------------------------------------------
	class SirenAnimator : public Component
	{
	public:
		DECLARE_FAMILY_ID("SirenAnimator");

		SirenAnimator() {}
		virtual ~SirenAnimator() {}

		virtual void OnRun(DrawObject* img, VEC2D* pos, REAL32 tm) {}
		virtual void OnStop(DrawObject* img) {}
	};

	class SirenAnimatorWhirl : public SirenAnimator
	{
	public:
		DECLARE_COMPONENT_ID("SirenAnimatorWhirl");

		SirenAnimatorWhirl(REAL32 scale);
		virtual ~SirenAnimatorWhirl() {}

		virtual void OnRun(DrawObject* img, VEC2D* pos, REAL32 tm) override;
		virtual void OnStop(DrawObject* img) override;

	private:
		REAL32	m_timer;
		REAL32	m_scale;
		REAL32	m_currScale;
	};


	//----------------------------------------------
	// MissionObject
	//----------------------------------------------
	class MissionObject;
	struct MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm) = 0;
	};

	class MissionObject : public Component
	{
	public:
		DECLARE_FAMILY_ID("MissionObject");

	public:
		typedef i3::unordered_map<UINT32, MissionObjectBehavior*> lookupTable;

		MissionObject() {}
		virtual ~MissionObject() {}

		virtual void Update(REAL32 tm);
		virtual void Register() = 0;

	protected:
		lookupTable m_behaviors;
	};

	class BombInstallArea_A : public MissionObject
	{
	public:
		DECLARE_COMPONENT_ID("BombInstallArea_A");

		BombInstallArea_A() {}
		virtual ~BombInstallArea_A() {}

		virtual void Register() override;
	};

	class BombInstallArea_B : public MissionObject
	{
	public:
		DECLARE_COMPONENT_ID("BombInstallArea_B");

		BombInstallArea_B() {}
		virtual ~BombInstallArea_B() {}

		virtual void Register() override;
	};

	class DestroyObj_Red : public MissionObject
	{
	public:
		DECLARE_COMPONENT_ID("DestroyObj_Red");

		DestroyObj_Red() {}
		virtual ~DestroyObj_Red() {}

		virtual void Register() override;
	};

	class DestroyObj_Blue : public MissionObject
	{
	public:
		DECLARE_COMPONENT_ID("DestroyObj_Blue");

		DestroyObj_Blue() {}
		virtual ~DestroyObj_Blue() {}

		virtual void Register() override;

	};

	class DefenceObj_Red : public MissionObject
	{
	public:
		DECLARE_COMPONENT_ID("DefenceObj_Red");

		DefenceObj_Red() {}
		virtual ~DefenceObj_Red() {}

		virtual void Register() override;
	};

	class DefenceObj_Blue : public MissionObject
	{
	public:
		DECLARE_COMPONENT_ID("DefenceObj_Blue");

		DefenceObj_Blue() {}
		virtual ~DefenceObj_Blue() {}

		virtual void Register() override;
	};

	//----------------------------------------------
	// MissionObjectState
	//----------------------------------------------
	class MissionObjectState : public Component
	{
	public:
		DECLARE_FAMILY_ID("MissionObjectState");

		virtual ~MissionObjectState() {}
		virtual INT32 GetState() = 0;
	};

	enum BOMB_AREA_A_STATE
	{
		BOMB_A_NOTHING_HAPPENED,
		BOMB_A_INSTALLED,
		BOMB_A_AREA_IS_NOT_IN_MY_FLOOR,
	};
	class BombAreaState_A : public MissionObjectState
	{
	public:
		DECLARE_COMPONENT_ID("BombAreaState_A");

		virtual ~BombAreaState_A() {}
		virtual INT32 GetState() override;
	};

	enum BOMB_AREA_B_STATE
	{
		BOMB_B_NOTHING_HAPPENED,
		BOMB_B_INSTALLED_IN_STORMTUBE_MAP,
		BOMB_B_INSTALLED,
		BOMB_B_AREA_IS_NOT_IN_MY_FLOOR,
	};
	class BombAreaState_B : public MissionObjectState
	{
	public:
		DECLARE_COMPONENT_ID("BombAreaState_B");

		virtual ~BombAreaState_B() {}
		virtual INT32 GetState() override;
	};

	enum DESTROY_OBJ_STATE
	{
		DESTROY_OBJ_UNDER_ATTACK,
		DESTROY_OBJ_NOTHING_HAPPENED,
	};
	class DestroyObjState : public MissionObjectState
	{
	public:
		DECLARE_COMPONENT_ID("DestroyObjState");

		DestroyObjState(INT32 objIdx);
		virtual ~DestroyObjState() {}
		virtual INT32 GetState() override;

	private:
		INT32 m_objIdx;
	};

	enum DEFENCE_OBJ_STATE
	{
		DEFENCE_OBJ_UNDER_ATTACK,
		DEFENCE_OBJ_DESTROYED,
		DEFENCE_OBJ_NOTHING_HAPPENED,
	};
	class DefenceObjState : public MissionObjectState
	{
	public:
		DECLARE_COMPONENT_ID("DefenceObjState");

		DefenceObjState(INT32 objIdx);
		virtual ~DefenceObjState() {}
		virtual INT32 GetState() override;

	private:
		INT32 m_objIdx;
	};

	//----------------------------------------------
	// SkillObject
	//----------------------------------------------
	class SkillObject : public Component
	{
	public:
		DECLARE_FAMILY_ID("SkillObject");

	public:
		explicit SkillObject(INT32 idx) : m_idx(idx){}
		virtual ~SkillObject() {}

		virtual void Update() = 0;

	protected:
		INT32 m_idx;
	};

	class Sentrygun : public SkillObject
	{
	public:
		DECLARE_COMPONENT_ID("Sentrygun");

	public:
		explicit Sentrygun(INT32 idx) : SkillObject(idx) {}
		~Sentrygun() {}

		virtual void Update() override;
	};

	class Dummy : public SkillObject
	{
	public:
		DECLARE_COMPONENT_ID("Dummy");

	public:
		explicit Dummy(INT32 idx) : SkillObject(idx) {}
		~Dummy() {}

		virtual void Update() override;
	};

	class SkillShop : public SkillObject
	{
	public:
		DECLARE_COMPONENT_ID("SkillShop");

	public:
		explicit SkillShop(INT32 idx) : SkillObject(idx) {}
		~SkillShop() {}

		virtual void Update() override;
	};
}

#endif