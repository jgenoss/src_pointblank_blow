#include "pch.h"
#include "Unit.h"
#include "Minimap.h"
#include "MinimapUtil.h"
#include "UIClanFunction.h"
#include "GameCharaContexts.h"
#include "GameCharaBoneContext.h"
#include "Camera/GameCamera_OnObject.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"
#include "Weapon/WeaponOnObject.h"
#include "../ConfigRenderBattle.h"
#include "../BattleHUD_Minimap.h"
#include "../Mode/Domination/DominationSkillObject.h"
#include "../Mode/Domination/DominationSkillSystem.h"

#include "./StageBattle/C4Context.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"
#include "BattleObjContext.h"
#include "../StageObject/ObjectWeaponBox.h"
#include "../Weapon/WeaponItemID.h"

using namespace minimap_new;

namespace
{
	using namespace minimap_new;

	static const INT32 g_destroyObjRedIdx = 0;
	static const INT32 g_destroyObjBlueIdx = 1;
	static const INT32 g_defenceObjRedIdx = 0;
	static const INT32 g_defenceObjBlueIdx = 1;

	static const REAL32 g_minimapWidth = 500.0f;
	static const REAL32 g_minimapHeight = 383.0f;

	void GetPosOnHelicopter(VEC3D& position, const i3Object* object)
	{
		I3ASSERT(object != nullptr);

		if (i3::same_of<CGameObjectRidingHelicopter*>(object)) 
			i3Vector::Copy(&position, i3Matrix::GetPos(((CGameObjectRidingHelicopter*)object)->getRootDummy()->GetCacheMatrix()));
		else if (i3::same_of<CGameObjectRespawnHelicopter*>(object))
			i3Vector::Copy(&position, i3Matrix::GetPos(((CGameObjectRespawnHelicopter*)object)->getRootDummy()->GetCacheMatrix()));
		else 
			i3Vector::Zero(&position);
	}

	REAL32 GetRotationOnHelicopter(const i3Object* object)
	{
		I3ASSERT(object != nullptr);

		WeaponOnObject* weapon = ((CGameObjectRidingHelicopter*)object)->GetWeaponOnObject();
		return (weapon->getTheta()+I3_PI2) * -1.0f;
	}

	void CalcPosWorldToMinimap(const VEC3D& inWorldPos, VEC2D& outMinimapPos)
	{
		Minimap* map = Minimap::GetInstance();

		REAL32 x = (map->GetFrameInfo().halfW) - (inWorldPos.x * map->GetTexInfo().mapImageInfo.ratio.w) + 
			map->GetTexInfo().mapImageInfo.ratio.AdjustWidth;

		REAL32 y = (map->GetFrameInfo().halfH) - (inWorldPos.z * map->GetTexInfo().mapImageInfo.ratio.h) + 
			map->GetTexInfo().mapImageInfo.ratio.AdjustHeight;

		if (x < 9.0f) x = 9.0f;
		else if (x > g_minimapWidth-9.0f) x = g_minimapWidth-9.0f;

		if (y < 9.0f) y = 9.0f;
		else if (y > g_minimapHeight) y = g_minimapHeight;

		outMinimapPos.x = x;
		outMinimapPos.y = y;
	}

	bool IsLocatedAtSameFloor(INT32 otherSlotIdx, bool isDomiDino = false)
	{
		i3MapSectorTable* sector = g_pFramework->GetCurrentStage()->getWorld()->getMapSectorTable(); 

		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		if (myPlayer == nullptr) return false;
		INT32 myFloor = sector->getFloor(myPlayer->getPortalID());

		CGameCharaBase* otherPlayer = g_pCharaManager->getCharaByNetIdx(otherSlotIdx, isDomiDino);
		if (otherPlayer == nullptr) return false;
		INT32 otherFloor = sector->getFloor(otherPlayer->getPortalID());

		if (otherFloor != myFloor) return false;
		return true;
	}

	VEC3D* GetDestroyObjPos(INT32 idx)
	{
		CGameMissionManager* missionMgr	= CGameMissionManager::i();
		i3Object* destroy[2] = {missionMgr->getRedObject(), missionMgr->getBlueObject()};
		VEC3D* position	= destroy[idx]->GetPos();

		if (i3::same_of<CGameObjectRespawnHelicopter*>(destroy[idx]))
			position = i3Matrix::GetPos(((CGameObjectRespawnHelicopter*)destroy[idx])->getRootDummy()->GetCacheMatrix());

		return position;
	}
}

#define  DEATH_UNIT_FADE_TIME 5.0f
#define  DEATH_UNIT_FADE_ALPHA 150

//----------------------------------------------
// Units
//----------------------------------------------
Unit::Unit()
{
	m_unit = new GameObject; 
	MEMDUMP_NEW( m_unit, sizeof( GameObject));
}

Unit::~Unit() 
{ 
	I3_SAFE_DELETE( m_unit);
}

UnitCharaMe::UnitCharaMe(INT32 slotIdx) 
{
	ImagesCharaMe * pImage = new ImagesCharaMe;
	MEMDUMP_NEW( pImage, sizeof( ImagesCharaMe));
	m_unit->SetComponent( pImage);

	MoveCharaMe * pMove = new MoveCharaMe(slotIdx);
	MEMDUMP_NEW( pMove, sizeof( MoveCharaMe));
	m_unit->SetComponent( pMove);

	CharaPlayState * pPlayState = new CharaPlayState(slotIdx);
	MEMDUMP_NEW( pPlayState, sizeof( CharaPlayState));
	m_unit->SetComponent( pPlayState);

	AutoActionCharaMe * pAutoAction = new AutoActionCharaMe;
	MEMDUMP_NEW( pAutoAction, sizeof( AutoActionCharaMe));
	Component* c = m_unit->SetComponent( pAutoAction);
	static_cast<AutoAction*>(c)->Register(m_unit);

	JudgeUpdatableCharaMe * pJudge = new JudgeUpdatableCharaMe(slotIdx);
	MEMDUMP_NEW( pJudge, sizeof( JudgeUpdatableCharaMe));
	m_unit->SetComponent( pJudge);
}

void UnitCharaMe::Update(REAL32 tm)
{
	if (static_cast<JudgeUpdatable*>(m_unit->GetComponent("JudgeUpdatable"))->Judge() == false) 
	{
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Hide(); return;
	}

	static_cast<Move*>(m_unit->GetComponent("Move"))->Render(tm);
	static_cast<AutoAction*>(m_unit->GetComponent("AutoAction"))->Render(tm);
}

UnitCharaFellow::UnitCharaFellow(INT32 slotIdx) 
{
	ImagesCharaFellow * pImage = new ImagesCharaFellow;
	MEMDUMP_NEW( pImage, sizeof( ImagesCharaFellow));
	m_unit->SetComponent( pImage);

	MoveCharaFellow * pMove = new MoveCharaFellow(slotIdx);
	MEMDUMP_NEW( pMove, sizeof( MoveCharaFellow));
	m_unit->SetComponent( pMove);

	CharaPlayState * pPlayState = new CharaPlayState(slotIdx);
	MEMDUMP_NEW( pPlayState, sizeof( CharaPlayState));
	m_unit->SetComponent( pPlayState);

	AutoActionCharaFellow * pAutoAction = new AutoActionCharaFellow;
	MEMDUMP_NEW( pAutoAction, sizeof( AutoActionCharaFellow));
	Component* c = m_unit->SetComponent( pAutoAction);
	static_cast<AutoAction*>(c)->Register(m_unit);

	JudgeUpdatableCharaFellow * pJudge = new JudgeUpdatableCharaFellow(slotIdx);
	MEMDUMP_NEW( pJudge, sizeof( JudgeUpdatableCharaFellow));
	m_unit->SetComponent( pJudge);
}

void UnitCharaFellow::Update(REAL32 tm)
{
	if (static_cast<JudgeUpdatable*>(m_unit->GetComponent("JudgeUpdatable"))->Judge() == false)
	{
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Hide(); return;
	}

	static_cast<Move*>(m_unit->GetComponent("Move"))->Render(tm);
	static_cast<AutoAction*>(m_unit->GetComponent("AutoAction"))->Render(tm);
}

UnitDomiDino::UnitDomiDino(INT32 slotIdx)
{
	ImagesDomiDino * pImage = new ImagesDomiDino;
	MEMDUMP_NEW( pImage, sizeof( ImagesDomiDino));
	m_unit->SetComponent( pImage);

	MoveDomiDino * pMove = new MoveDomiDino(slotIdx);
	MEMDUMP_NEW( pMove, sizeof( MoveDomiDino));
	m_unit->SetComponent( pMove);

	DomiDinoPlayState * pPlayState = new DomiDinoPlayState(slotIdx);
	MEMDUMP_NEW( pPlayState, sizeof( DomiDinoPlayState));
	m_unit->SetComponent( pPlayState);

	AutoActionDomiDino * pAutoAction = new AutoActionDomiDino;
	MEMDUMP_NEW( pAutoAction, sizeof( AutoActionDomiDino));
	Component* c = m_unit->SetComponent( pAutoAction);
	static_cast<AutoAction*>(c)->Register(m_unit);

	JudgeUpdatableDomiDino * pJudge = new JudgeUpdatableDomiDino(slotIdx);
	MEMDUMP_NEW( pJudge, sizeof( JudgeUpdatableDomiDino));
	m_unit->SetComponent( pJudge);
}

void UnitDomiDino::Update(REAL32 tm)
{
	if (static_cast<JudgeUpdatable*>(m_unit->GetComponent("JudgeUpdatable"))->Judge() == false)
	{
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Hide(); return;
	}

	static_cast<Move*>(m_unit->GetComponent("Move"))->Render(tm);
	static_cast<AutoAction*>(m_unit->GetComponent("AutoAction"))->Render(tm);
}

UnitBombA::UnitBombA() 
{
	ImageMissionObjWithSiren * pMissionObjSiren = new ImageMissionObjWithSiren;
	MEMDUMP_NEW( pMissionObjSiren, sizeof( ImageMissionObjWithSiren));

	ImageMissionObjWithSiren* image 
		= static_cast<ImageMissionObjWithSiren*>(m_unit->SetComponent( pMissionObjSiren));

	ImagesMissionObject * pMissionObj = new ImagesMissionObject;
	MEMDUMP_NEW( pMissionObj, sizeof( ImagesMissionObject));
	image->SetMissionObj( pMissionObj);

	ImagesSiren * pSiren = new ImagesSiren;
	MEMDUMP_NEW( pSiren, sizeof( ImagesSiren));
	image->SetSiren( pSiren);

	VEC2D pos;
	//CalcPosWorldToMinimap(*(C4Context::i()->getBombAreaPos(BOMB_AREA_A)), pos);
	CalcPosWorldToMinimap(C4Context::i()->FindAreaProp(BOMB_AREA_A)->pos, pos);
	image->SetPosition(&pos);

	SirenAnimatorWhirl * pSirenAnim = new SirenAnimatorWhirl(5.0f);
	MEMDUMP_NEW( pSirenAnim, sizeof( SirenAnimatorWhirl));
	m_unit->SetComponent( pSirenAnim);

	BombAreaState_A * pBombAreaState = new BombAreaState_A;
	MEMDUMP_NEW( pBombAreaState, sizeof( BombAreaState_A));
	m_unit->SetComponent( pBombAreaState);

	BombInstallArea_A * pBombInstall = new BombInstallArea_A;
	MEMDUMP_NEW( pBombInstall, sizeof( BombInstallArea_A));
	Component* processor = m_unit->SetComponent( pBombInstall);
	static_cast<MissionObject*>(processor)->Register();
}

void UnitBombA::Update(REAL32 tm)
{
	static_cast<MissionObject*>(m_unit->GetComponent("MissionObject"))->Update(tm);
}

UnitBombB::UnitBombB() 
{
	ImageMissionObjWithSiren * pMissionObjSiren = new ImageMissionObjWithSiren;
	MEMDUMP_NEW( pMissionObjSiren, sizeof( ImageMissionObjWithSiren));

	m_pImage = static_cast<ImageMissionObjWithSiren*>(m_unit->SetComponent( pMissionObjSiren));

	ImagesMissionObject * pMissionObj = new ImagesMissionObject;
	MEMDUMP_NEW( pMissionObj, sizeof( ImagesMissionObject));
	m_pImage->SetMissionObj( pMissionObj);

	ImagesSiren * pSiren = new ImagesSiren;
	MEMDUMP_NEW( pSiren, sizeof( ImagesSiren));
	m_pImage->SetSiren( pSiren);

	VEC2D pos;
	//CalcPosWorldToMinimap(C4Context::i()->FindAreaProp(BOMB_AREA_B)->pos, pos);
	CalcPosWorldToMinimap(C4Context::i()->FindAreaProp(BOMB_AREA_B)->pos, pos);
	m_pImage->SetPosition(&pos);

	SirenAnimatorWhirl * pSirenAnim = new SirenAnimatorWhirl(5.0f);
	MEMDUMP_NEW( pSirenAnim, sizeof( SirenAnimatorWhirl));
	m_unit->SetComponent( pSirenAnim);

	BombAreaState_B * pBombArea = new BombAreaState_B;
	MEMDUMP_NEW( pBombArea, sizeof( BombAreaState_B));
	m_unit->SetComponent( pBombArea);

	BombInstallArea_B * pBombInstall = new BombInstallArea_B;
	MEMDUMP_NEW( pBombInstall, sizeof( BombInstallArea_B));
	Component* processor = m_unit->SetComponent( pBombInstall);
	static_cast<MissionObject*>(processor)->Register();
}

void UnitBombB::OnStart()
{
	VEC2D pos;
	//CalcPosWorldToMinimap(C4Context::i()->FindAreaProp(BOMB_AREA_B)->pos, pos);
	CalcPosWorldToMinimap(C4Context::i()->FindAreaProp(BOMB_AREA_B)->pos, pos);
	m_pImage->SetPosition(&pos);
}

void UnitBombB::Update(REAL32 tm)
{
	static_cast<MissionObject*>(m_unit->GetComponent("MissionObject"))->Update(tm);
}

UnitDestroyRed::UnitDestroyRed() 
{
	ImageMissionObjWithSiren * pMissionObjSiren = new ImageMissionObjWithSiren;
	MEMDUMP_NEW( pMissionObjSiren, sizeof( ImageMissionObjWithSiren));
	ImageMissionObjWithSiren* image 
		= static_cast<ImageMissionObjWithSiren*>(m_unit->SetComponent( pMissionObjSiren));

	ImagesMissionObject * pMissionObj = new ImagesMissionObject;
	MEMDUMP_NEW( pMissionObj, sizeof( ImagesMissionObject));
	image->SetMissionObj( pMissionObj);

	ImagesSiren * pSiren = new ImagesSiren;
	MEMDUMP_NEW( pSiren, sizeof( ImagesSiren));
	image->SetSiren( pSiren);

	VEC2D pos;
	CalcPosWorldToMinimap(*(GetDestroyObjPos(g_destroyObjRedIdx)), pos);
	image->SetPosition(&pos);

	SirenAnimatorWhirl * pSirenAnim = new SirenAnimatorWhirl(5.0f);
	MEMDUMP_NEW( pSirenAnim, sizeof( SirenAnimatorWhirl));
	m_unit->SetComponent( pSirenAnim);

	DestroyObjState * pDestroyObj = new DestroyObjState(g_destroyObjRedIdx);
	MEMDUMP_NEW( pDestroyObj, sizeof( DestroyObjState));
	m_unit->SetComponent( pDestroyObj);

	MoveDestroyObj * pMoveObj = new MoveDestroyObj(g_destroyObjRedIdx);
	MEMDUMP_NEW( pMoveObj, sizeof( MoveDestroyObj));
	m_unit->SetComponent( pMoveObj);

	DestroyObj_Red * pDestroyRed = new DestroyObj_Red;
	MEMDUMP_NEW( pDestroyRed, sizeof( DestroyObj_Red));
	Component* processor = m_unit->SetComponent( pDestroyRed);
	static_cast<MissionObject*>(processor)->Register();
}

void UnitDestroyRed::Update(REAL32 tm)
{
	static_cast<Move*>(m_unit->GetComponent("Move"))->Render(tm);
	static_cast<MissionObject*>(m_unit->GetComponent("MissionObject"))->Update(tm);
}

UnitDestroyBlue::UnitDestroyBlue() 
{
	ImageMissionObjWithSiren * pMissionObjSiren = new ImageMissionObjWithSiren;
	MEMDUMP_NEW( pMissionObjSiren, sizeof( ImageMissionObjWithSiren));
	ImageMissionObjWithSiren* image 
		= static_cast<ImageMissionObjWithSiren*>(m_unit->SetComponent( pMissionObjSiren));

	ImagesMissionObject * pMissionObj = new ImagesMissionObject;
	MEMDUMP_NEW( pMissionObj, sizeof( ImagesMissionObject));
	image->SetMissionObj( pMissionObj);

	ImagesSiren * pSiren = new ImagesSiren;
	MEMDUMP_NEW( pSiren, sizeof( ImagesSiren));
	image->SetSiren( pSiren);

	VEC2D pos;
	CalcPosWorldToMinimap(*(GetDestroyObjPos(g_destroyObjBlueIdx)), pos);
	image->SetPosition(&pos);

	SirenAnimatorWhirl * pSirenAnim = new SirenAnimatorWhirl(5.0f);
	MEMDUMP_NEW( pSirenAnim, sizeof( SirenAnimatorWhirl));
	m_unit->SetComponent( pSirenAnim);

	DestroyObjState * pDestroyObjState = new DestroyObjState(g_destroyObjBlueIdx);
	MEMDUMP_NEW( pDestroyObjState, sizeof( DestroyObjState));
	m_unit->SetComponent( pDestroyObjState);

	MoveDestroyObj * pMoveObj = new MoveDestroyObj(g_destroyObjBlueIdx);
	MEMDUMP_NEW( pMoveObj, sizeof( MoveDestroyObj));
	m_unit->SetComponent( pMoveObj);

	DestroyObj_Blue * pDestroyBlue = new DestroyObj_Blue;
	MEMDUMP_NEW( pDestroyBlue, sizeof( DestroyObj_Blue));
	Component* processor = m_unit->SetComponent( pDestroyBlue);
	static_cast<MissionObject*>(processor)->Register();
}

void UnitDestroyBlue::Update(REAL32 tm)
{
	static_cast<Move*>(m_unit->GetComponent("Move"))->Render(tm);
	static_cast<MissionObject*>(m_unit->GetComponent("MissionObject"))->Update(tm);
}

UnitDefenceRed::UnitDefenceRed() 
{
	ImageMissionObjWithSiren * pMissionObjSiren = new ImageMissionObjWithSiren;
	MEMDUMP_NEW( pMissionObjSiren, sizeof( ImageMissionObjWithSiren));
	ImageMissionObjWithSiren* image 
		= static_cast<ImageMissionObjWithSiren*>(m_unit->SetComponent( pMissionObjSiren));

	ImagesMissionObject * pMissionObj = new ImagesMissionObject;
	MEMDUMP_NEW( pMissionObj, sizeof( ImagesMissionObject));
	image->SetMissionObj( pMissionObj);

	ImagesSiren * pSiren = new ImagesSiren;
	MEMDUMP_NEW( pSiren, sizeof( ImagesSiren));
	image->SetSiren( pSiren);

	VEC2D pos;
	CalcPosWorldToMinimap(*(CGameMissionManager::i()->getDefenceObj(g_defenceObjRedIdx)->GetPos()), pos);
	image->SetPosition(&pos);

	SirenAnimatorWhirl * pSirenAnim = new SirenAnimatorWhirl(5.0f);
	MEMDUMP_NEW( pSiren, sizeof( SirenAnimatorWhirl));
	m_unit->SetComponent( pSirenAnim);

	DefenceObjState * pDefenceObj = new DefenceObjState(g_defenceObjRedIdx);
	MEMDUMP_NEW( pDefenceObj, sizeof( DefenceObjState));
	m_unit->SetComponent( pDefenceObj);

	DefenceObj_Red * pDefenceRed = new DefenceObj_Red;
	MEMDUMP_NEW( pDefenceRed, sizeof( DefenceObj_Red));
	Component* processor = m_unit->SetComponent( pDefenceRed);
	static_cast<MissionObject*>(processor)->Register();
}

void UnitDefenceRed::Update(REAL32 tm)
{
	static_cast<MissionObject*>(m_unit->GetComponent("MissionObject"))->Update(tm);
}

UnitDefenceBlue::UnitDefenceBlue() 
{
	ImageMissionObjWithSiren * pMissionObjSiren = new ImageMissionObjWithSiren;
	MEMDUMP_NEW( pMissionObjSiren, sizeof( ImageMissionObjWithSiren));
	ImageMissionObjWithSiren* image 
		= static_cast<ImageMissionObjWithSiren*>(m_unit->SetComponent( pMissionObjSiren));

	ImagesMissionObject * pMissionObj = new ImagesMissionObject;
	MEMDUMP_NEW( pMissionObj, sizeof( ImagesMissionObject));
	image->SetMissionObj( pMissionObj);

	ImagesSiren * pSiren = new ImagesSiren;
	MEMDUMP_NEW( pSiren, sizeof( ImagesSiren));
	image->SetSiren( pSiren);

	VEC2D pos;
	CalcPosWorldToMinimap(*(CGameMissionManager::i()->getDefenceObj(g_defenceObjBlueIdx)->GetPos()), pos);
	image->SetPosition(&pos);

	SirenAnimatorWhirl * pSirenAnim = new SirenAnimatorWhirl(5.0f);
	MEMDUMP_NEW( pSirenAnim, sizeof( SirenAnimatorWhirl));
	m_unit->SetComponent( pSirenAnim);

	DefenceObjState * pDefenceObj = new DefenceObjState(g_defenceObjBlueIdx);
	MEMDUMP_NEW( pDefenceObj, sizeof( DefenceObjState));
	m_unit->SetComponent( pDefenceObj);

	DefenceObj_Blue * pDefenceBlue = new DefenceObj_Blue;
	MEMDUMP_NEW( pDefenceBlue, sizeof( DefenceObj_Blue));
	Component* processor = m_unit->SetComponent( pDefenceBlue);
	static_cast<MissionObject*>(processor)->Register();
}

void UnitDefenceBlue::Update(REAL32 tm)
{
	static_cast<MissionObject*>(m_unit->GetComponent("MissionObject"))->Update(tm);
}

UnitSentrygun::UnitSentrygun(INT32 idx)
{
	m_unit->SetComponent(new Sentrygun(idx));
	m_unit->SetComponent(new ImageSentrygun);
	m_idx = idx;
}

void UnitSentrygun::Update(REAL32 tm)
{
	static_cast<SkillObject*>(m_unit->GetComponent("SkillObject"))->Update();
}

UnitDummy::UnitDummy(INT32 idx)
{
	m_unit->SetComponent(new Dummy(idx));
	m_unit->SetComponent(new ImageDummy);
	m_idx = idx;
}

void UnitDummy::Update(REAL32 tm)
{
	static_cast<SkillObject*>(m_unit->GetComponent("SkillObject"))->Update();
}

UnitSkillShop::UnitSkillShop(INT32 idx)
{
	m_unit->SetComponent(new SkillShop(idx));
	m_unit->SetComponent(new ImageSkillShop);
	m_idx = idx;
}

void UnitSkillShop::Update(REAL32 tm)
{
	static_cast<SkillObject*>(m_unit->GetComponent("SkillObject"))->Update();
}

UnitUsurpaiton::UnitUsurpaiton(INT32 slotIdx)
{
	ImagesUsurpation* pImage = new ImagesUsurpation;
	MEMDUMP_NEW(pImage, sizeof(ImagesUsurpation));
	m_unit->SetComponent(pImage);

	MoveUsurpation *pMove = new MoveUsurpation(slotIdx);
	MEMDUMP_NEW(pMove, sizeof(MoveUsurpation));
	m_unit->SetComponent(pMove);

	JudgeUpdatableUsurpation *pJudge = new JudgeUpdatableUsurpation(slotIdx);
	MEMDUMP_NEW(pJudge, sizeof(JudgeUpdatableUsurpation));
	m_unit->SetComponent(pJudge);
}

void UnitUsurpaiton::Update(REAL32 tm)
{
	if (static_cast<JudgeUpdatable*>(m_unit->GetComponent("JudgeUpdatable"))->Judge() == false)
	{
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Hide(); return;
	}
	else
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Show();

	static_cast<Move*>(m_unit->GetComponent("Move"))->Render(tm);
}

UnitCleaverKnifeA::UnitCleaverKnifeA()
{
	ImageCleaverKnifeA* pImage = new ImageCleaverKnifeA;
	MEMDUMP_NEW(pImage, sizeof(ImageCleaverKnifeA));
	m_unit->SetComponent(pImage);

	MoveCleaverKnifeA *pMove = new MoveCleaverKnifeA(0);
	MEMDUMP_NEW(pMove, sizeof(MoveCleaverKnifeA));
	m_unit->SetComponent(pMove);
}

void UnitCleaverKnifeA::Update(REAL32 tm)
{
	// IDX 0의 중식칼을 받아옵니다. idx에는 큰의미가 없고 맵에 위치한 중식칼 두 자루를 분류하기 위함입니다.
	i3Object* _objectA = BattleObjContext::i()->getObjectCountInfo()->m_pControledObjectList[0];
	CGameObjectWeaponBox* _weaponBox = (CGameObjectWeaponBox*)_objectA;

	if(_weaponBox->m_bCleaverKnifeUsed)
	{
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Hide(); return;
	}
	else
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Show();

	static_cast<Move*>(m_unit->GetComponent("Move"))->Render(tm);
}

UnitCleaverKnifeB::UnitCleaverKnifeB()
{
	ImageCleaverKnifeB* pImage = new ImageCleaverKnifeB;
	MEMDUMP_NEW(pImage, sizeof(ImageCleaverKnifeA));
	m_unit->SetComponent(pImage);

	MoveCleaverKnifeB *pMove = new MoveCleaverKnifeB(0);
	MEMDUMP_NEW(pMove, sizeof(MoveCleaverKnifeB));
	m_unit->SetComponent(pMove);
}

void UnitCleaverKnifeB::Update(REAL32 tm)
{
	// IDX 1의 중식칼을 받아옵니다.
	i3Object* _objectB = BattleObjContext::i()->getObjectCountInfo()->m_pControledObjectList[1];
	CGameObjectWeaponBox* _weaponBox = (CGameObjectWeaponBox*)_objectB;

	if(_weaponBox->m_bCleaverKnifeUsed)
	{
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Hide(); return;
	}
	else
		static_cast<Images*>(m_unit->GetComponent("Images"))->GetImages()->Show();

	static_cast<Move*>(m_unit->GetComponent("Move"))->Render(tm);
}

//----------------------------------------------
// Images
//----------------------------------------------
Images::Images() : m_images(0) {}
Images::~Images()
{
	I3_SAFE_DELETE(m_images);
}

DrawObject* Images::GetImage(INT32 idx)
{
	DrawObject* imgs = GetImages();
	imgs->SetCurrentImage(idx);
	return imgs;
}

DrawObject* ImagesCharaMe::GetImages()
{
	if (m_images) return m_images;

	DrawObject* imgs = new DrawObject();
	MEMDUMP_NEW( imgs, sizeof( DrawObject));

	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
	imgs->AddImage(tex, 227, 44, 18, 34); // normal
	imgs->Create(Minimap::GetInstance()->GetFrame());
	imgs->SetCurrentImage(0);
	I3_MUST_RELEASE(tex);

	m_images = imgs;

	return imgs;
}


DrawObject* ImagesCharaFellow::GetImages()
{
	if (m_images) return m_images;

	DrawObject* imgs = new DrawObject();
	MEMDUMP_NEW( imgs, sizeof( DrawObject));

	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
	imgs->AddImage(tex, 172, 89, 10, 22); // normal image
	imgs->AddImage(tex, 255, 69, 12, 12); // death image
	imgs->Create(Minimap::GetInstance()->GetFrame());
	imgs->SetCurrentImage(0);
	I3_MUST_RELEASE(tex);
	m_images = imgs;

	return imgs;
}

DrawObject* ImagesDomiDino::GetImages()
{
	if (m_images) return m_images;

	DrawObject* imgs = new DrawObject();
	MEMDUMP_NEW( imgs, sizeof( DrawObject));

	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
	imgs->AddImage(tex, 156, 97, 10, 16); // normal, death
	imgs->Create(Minimap::GetInstance()->GetFrame());
	imgs->SetCurrentImage(0);
	I3_MUST_RELEASE(tex);
	m_images = imgs;

	return imgs;
}

DrawObject* ImagesMissionObject::GetImages()
{
	if (m_images) return m_images;

	DrawObject* imgs = new DrawObject();
	MEMDUMP_NEW( imgs, sizeof( DrawObject));

	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
	imgs->AddImage(tex, 153, 43, 20, 20);	// IMAGE_BOMB_AREA_A
	imgs->AddImage(tex, 175, 42, 20, 20);	// IMAGE_BOMB_AREA_B
	imgs->AddImage(tex, 125, 90, 20, 20);	// IMAGE_DESTROY_RED
	imgs->AddImage(tex, 102, 90, 20, 20);	// IMAGE_DESTROY_BLUE
	imgs->AddImage(tex, 154, 42, 20, 20);	// IMAGE_DEFENCE_RED
	imgs->AddImage(tex, 176, 42, 20, 20);	// IMAGE_DEFENCE_BLUE
	imgs->Create(Minimap::GetInstance()->GetFrame());
	imgs->SetCurrentImage(0);
	I3_MUST_RELEASE(tex);
	m_images = imgs;

	return imgs;
}

DrawObject* ImagesSiren::GetImages()
{
	if (m_images) return m_images;

	DrawObject* imgs = new DrawObject();
	MEMDUMP_NEW( imgs, sizeof( DrawObject));

	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
	imgs->AddImage(tex, 115, 52, 34, 34);	// IMAGE_SIREN_RED
	imgs->AddImage(tex, 78, 52, 34, 34);	// IMAGE_SIREN_BLUE
	imgs->Create(Minimap::GetInstance()->GetFrame());
	imgs->SetCurrentImage(0);
	I3_MUST_RELEASE(tex);
	m_images = imgs;

	return imgs;
}

ImageMissionObjWithSiren::ImageMissionObjWithSiren() : m_missionObj(0), m_siren(0) {}
ImageMissionObjWithSiren::~ImageMissionObjWithSiren()
{
	I3_SAFE_DELETE( m_missionObj);
	I3_SAFE_DELETE( m_siren);
}
void ImageMissionObjWithSiren::SetMissionObj(ImagesMissionObject* o)
{
	o->SetOwner(GetOwner());
	m_missionObj = o;
}
void ImageMissionObjWithSiren::SetSiren(ImagesSiren* s)
{
	s->SetOwner(GetOwner());
	m_siren = s;
}
void ImageMissionObjWithSiren::SetPosition(const VEC2D* pos)
{
	m_missionObj->GetImages()->SetPosition(pos->x, pos->y);
	m_siren->GetImages()->SetPosition(pos->x, pos->y);
}

DrawObject* ImageSentrygun::GetImages()
{
	if (m_images) return m_images;

	DrawObject* imgs = new DrawObject();
	MEMDUMP_NEW( imgs, sizeof( DrawObject));

	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
	imgs->AddImage(tex, 653, 390, 20, 20);	// IMAGE_SKILLOBJ_DEACTIVATED
	imgs->AddImage(tex, 653, 342, 20, 20);	// IMAGE_SKILLOBJ_ACTIVATED
	imgs->AddImage(tex, 653, 366, 20, 20);	// IMAGE_SKILLOBJ_REPAIR_REQUIRING
	imgs->Create(Minimap::GetInstance()->GetFrame());
	imgs->SetCurrentImage(0);
	I3_MUST_RELEASE(tex);
	m_images = imgs;

	return imgs;
}

DrawObject* ImageDummy::GetImages()
{
	if (m_images) return m_images;

	DrawObject* imgs = new DrawObject();
	MEMDUMP_NEW( imgs, sizeof( DrawObject));

	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
	imgs->AddImage(tex, 628, 390, 20, 20);	// IMAGE_SKILLOBJ_DEACTIVATED
	imgs->AddImage(tex, 628, 342, 20, 20);	// IMAGE_SKILLOBJ_ACTIVATED
	imgs->AddImage(tex, 628, 366, 20, 20);	// IMAGE_SKILLOBJ_REPAIR_REQUIRING
	imgs->Create(Minimap::GetInstance()->GetFrame());
	imgs->SetCurrentImage(0);
	I3_MUST_RELEASE(tex);
	m_images = imgs;

	return imgs;
}

DrawObject* ImageSkillShop::GetImages()
{
	if (m_images) return m_images;

	DrawObject* imgs = new DrawObject();
	MEMDUMP_NEW( imgs, sizeof( DrawObject));

	i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
	imgs->AddImage(tex, 603, 390, 20, 20);	// IMAGE_SKILLSHOP_DEACTIVATED
	imgs->AddImage(tex, 603, 342, 20, 20);	// IMAGE_SKILLSHOP_ACTIVATED
	imgs->Create(Minimap::GetInstance()->GetFrame());
	imgs->SetCurrentImage(0);
	I3_MUST_RELEASE(tex);
	m_images = imgs;

	return imgs;
}

DrawObject* ImagesUsurpation::GetImages()
{
	if( m_images == nullptr)
	{
		DrawObject* img = new DrawObject();
		MEMDUMP_NEW(img, sizeof(DrawObject));

		i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
			MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
		{
			img->AddImage(tex, 704, 344, 16, 25);
		}
		else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
				 MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_NORMAL) )
		{
			img->AddImage(tex, 775, 344, 32, 32);
		}
		

		img->Create(Minimap::GetInstance()->GetFrame());
		img->SetCurrentImage(0);
		I3_MUST_RELEASE(tex);

		m_images = img;
	}
	return m_images;
}

DrawObject* ImageCleaverKnifeA::GetImages()
{
	if( m_images == nullptr)
	{	
		DrawObject* img = new DrawObject();
		MEMDUMP_NEW(img, sizeof(DrawObject));

		i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
		img->AddImage(tex, 728, 344, 16, 25);
		img->Create(Minimap::GetInstance()->GetFrame());
		img->SetCurrentImage(0);
		I3_MUST_RELEASE(tex);

		m_images = img;
	}
	return m_images;
}

DrawObject* ImageCleaverKnifeB::GetImages()
{
	if( m_images == nullptr)
	{
		DrawObject* img = new DrawObject();
		MEMDUMP_NEW(img, sizeof(DrawObject));

		i3Texture* tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
		img->AddImage(tex, 728, 344, 16, 25);
		img->Create(Minimap::GetInstance()->GetFrame());
		img->SetCurrentImage(0);
		I3_MUST_RELEASE(tex);

		m_images = img;
	}
	return m_images;
}

//----------------------------------------------
// Move
//----------------------------------------------
void MoveCharaMe::Render(REAL32 tm)
{
	CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(m_slotIdx);
	if (!chara) return;

	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	// 이동
	VEC3D posWorld;
	i3Vector::Copy(&posWorld, i3Matrix::GetPos(chara->getSceneObject()->GetMatrix()));
	if ( chara->IsOnObject())
		GetPosOnHelicopter(posWorld, chara->GetOnObject());

	VEC2D posMinimap;
	CalcPosWorldToMinimap(posWorld, posMinimap);
	image->SetPosition(posMinimap.x, posMinimap.y);  

	// 회전
	REAL32 rotation = 0.0f;
	if ( chara->IsOnObject())
		rotation = GetRotationOnHelicopter(chara->GetOnObject());
	else
		rotation = chara->getBoneContext()->getTheta() * -1.0f; 
	image->SetRotate(rotation);
}

void MoveCharaFellow::Render(REAL32 tm)
{
	CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(m_slotIdx);
	if (!chara) return;

	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	// 이동
	VEC3D posWorld;
	i3Vector::Copy(&posWorld, i3Matrix::GetPos(chara->getSceneObject()->GetMatrix()));
	if ( chara->IsOnObject())
		GetPosOnHelicopter(posWorld, chara->GetOnObject());

	VEC2D posMinimap;
	CalcPosWorldToMinimap(posWorld, posMinimap);
	image->SetPosition(posMinimap.x, posMinimap.y);  

	// 회전
	REAL32 rotation = 0.0f;
	if ( chara->IsOnObject())
		rotation = GetRotationOnHelicopter(chara->GetOnObject());
	else
		rotation = chara->getBoneContext()->getTheta() * -1.0f; 
	image->SetRotate(rotation);
}

void MoveDomiDino::Render(REAL32 tm)
{
	CGameCharaBase* dino = g_pCharaManager->getCharaByNetIdx(m_slotIdx, true);
	if (!dino) return;

	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	// 이동
	VEC3D posWorld;
	i3Vector::Copy(&posWorld, i3Matrix::GetPos(dino->getSceneObject()->GetMatrix()));
	if ( dino->IsOnObject())
		GetPosOnHelicopter(posWorld, dino->GetOnObject());

	VEC2D posMinimap;
	CalcPosWorldToMinimap(posWorld, posMinimap);
	image->SetPosition(posMinimap.x, posMinimap.y);
}

void MoveDestroyObj::Render(REAL32 tm)
{
	ImageMissionObjWithSiren* imos = 
		static_cast<ImageMissionObjWithSiren*>(GetOwner()->GetComponent("ImageMissionObjWithSiren"));

	VEC2D pos;
	CalcPosWorldToMinimap(*(GetDestroyObjPos(m_objIdx)), pos);
	imos->GetMissionObj()->GetImages()->SetPosition(pos.x, pos.y);
}

void MoveUsurpation::Render(REAL32 tm)
{
	CGameMissionManager* pMgr = CGameMissionManager::i();
	m_slotIdx = pMgr->getUsurpationIdx();

	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	// 이동
	VEC3D posWorld;
	if(m_slotIdx == USURPATION_OBJECT_STATE_DEFAULT)
	{
		i3Vector::Copy(&posWorld, (VEC3D*)pMgr->getMissionObjPos());
	}
	else
	{
		CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(m_slotIdx);
		if(pChara == nullptr)	return;
		i3Vector::Copy(&posWorld, pChara->GetPos());		
	}

	VEC2D posMinimap;
	CalcPosWorldToMinimap(posWorld, posMinimap);
	image->SetPosition(posMinimap.x, posMinimap.y);  
}


void MoveCleaverKnifeA::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	// 이동
	VEC3D posWorld;
	i3::vector<i3Object*> temp  = BattleObjContext::i()->getObjectCountInfo()->m_pControledObjectList;	
	i3Vector::Copy(&posWorld,temp[0]->GetPos() );

	VEC2D posMinimap;
	CalcPosWorldToMinimap(posWorld, posMinimap);
	image->SetPosition(posMinimap.x, posMinimap.y);  
}

void MoveCleaverKnifeB::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	// 이동
	VEC3D posWorld;
	i3::vector<i3Object*> temp  = BattleObjContext::i()->getObjectCountInfo()->m_pControledObjectList;	
	i3Vector::Copy(&posWorld,temp[1]->GetPos() );

	VEC2D posMinimap;
	CalcPosWorldToMinimap(posWorld, posMinimap);
	image->SetPosition(posMinimap.x, posMinimap.y);  
}
//----------------------------------------------
// Action
//----------------------------------------------
void AttackCharaFellow::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	image->SetColor(255, 0, 0, 255);
	image->Show();
}


void UsualCharaMe::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	image->SetColor(255, 255, 255, 255);
	image->Show();
}


void UsualCharaFellow::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	image->SetCurrentImage(0);
	image->SetColor(255, 255, 255, 255);
	image->Show();
}


void DeadCharaMe::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	COLOR* color = image->GetColor();
	if (color->r != 255) color->r = 255;
	if (color->g != 0) color->g = 0;
	if (color->b != 0) color->b = 0;

	if (color->a <= 10)
	{
		image->Hide(); return;
	}

	color->a -= static_cast<UINT8>(150*tm);
	image->SetColor(color);
	image->Show();
}


void DeadCharaFellow::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	image->SetCurrentImage(1);
	
	m_fLifeTime += tm;

	if ( m_fLifeTime > DEATH_UNIT_FADE_TIME )
	{
		UINT8 alpha = image->GetAlpha();
		if (alpha > DEATH_UNIT_FADE_ALPHA)
		{
			alpha -= static_cast<UINT8>(150 * tm);
			image->SetAlpha(alpha);
		}
	}
	image->Show();
}

void UsualDomiDino::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	image->SetAlpha(255);
	image->Show();
}

void DeadDomiDino::Render(REAL32 tm)
{
	DrawObject* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	m_fLifeTime += tm;

	if (m_fLifeTime > DEATH_UNIT_FADE_TIME)
	{
		UINT8 alpha = image->GetAlpha();
		if (alpha > DEATH_UNIT_FADE_ALPHA)
		{
			alpha -= static_cast<UINT8>(150 * tm);
			image->SetAlpha(alpha);
		}
	}
	image->Show();
}

//----------------------------------------------
// PlayState
//----------------------------------------------
INT32 CharaPlayState::GetState()
{
	CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(m_slotIdx);
	if (!chara) return CHARA_COMBAT_DEATH;

	if (chara->isCharaStateMask(CHARA_STATE_DEATH) == true) return CHARA_COMBAT_DEATH;

	CGameCharaActionContext* actionCtx = chara->getActionContext();
	if (actionCtx->isUpper(CHARA_UPPER_FIRE) || 
		actionCtx->isUpper(CHARA_UPPER_SECONDARY_FIRE) ||
		actionCtx->getUpperLeft() == CHARA_UPPER_FIRE) return CHARA_COMBAT_ATTACK;

	return CHARA_COMBAT_NORMAL;
}

INT32 DomiDinoPlayState::GetState()
{
	CGameCharaBase* dino = g_pCharaManager->getCharaByNetIdx(m_slotIdx, true);
	if (!dino) return DOMI_DINO_DEATH;

	if (dino->isCharaStateMask(CHARA_STATE_DEATH) == true) return DOMI_DINO_DEATH;

	return DOMI_DINO_NORMAL;
}

//----------------------------------------------
// AutoAction
//----------------------------------------------
AutoAction::~AutoAction() 
{ 
	lookupTable::iterator it = m_actions.begin();
	for (; it != m_actions.end(); it++)
	{
		I3_SAFE_DELETE( it->second);
	}
	m_actions.clear(); 
}

void AutoAction::Render(REAL32 tm) 
{
	if (m_actions.empty()) return;

	INT32 curr = static_cast<PlayState*>(GetOwner()->GetComponent("PlayState"))->GetState();
	lookupTable::iterator it = m_actions.find(curr);
	if (it != m_actions.end())
	{
		Action* action = it->second;
		action->Render(tm);
	}
}

void AutoActionCharaMe::Register(GameObject* o)
{
	Action* usual = new UsualCharaMe; static_cast<Component*>(usual)->SetOwner(o);
	MEMDUMP_NEW( usual, sizeof( UsualCharaMe));

	Action* dead = new DeadCharaMe; static_cast<Component*>(dead)->SetOwner(o);
	MEMDUMP_NEW( dead, sizeof( DeadCharaMe));

	m_actions.insert(lookupTable::value_type(CHARA_COMBAT_NORMAL, usual));
	m_actions.insert(lookupTable::value_type(CHARA_COMBAT_DEATH, dead));
}

void AutoActionCharaFellow::Register(GameObject* o)
{
	Action* usual = new UsualCharaFellow; static_cast<Component*>(usual)->SetOwner(o);
	MEMDUMP_NEW( usual, sizeof( UsualCharaFellow));

	Action* attack = new AttackCharaFellow; static_cast<Component*>(attack)->SetOwner(o);
	MEMDUMP_NEW( attack, sizeof( AttackCharaFellow));

	Action* dead = new DeadCharaFellow; static_cast<Component*>(dead)->SetOwner(o);
	MEMDUMP_NEW( dead, sizeof( DeadCharaFellow));

	m_actions.insert(lookupTable::value_type(CHARA_COMBAT_NORMAL, usual));
	m_actions.insert(lookupTable::value_type(CHARA_COMBAT_ATTACK, attack));
	m_actions.insert(lookupTable::value_type(CHARA_COMBAT_DEATH, dead));
}

void AutoActionDomiDino::Register(GameObject* o)
{
	Action* usual = new UsualDomiDino; static_cast<Component*>(usual)->SetOwner(o);
	MEMDUMP_NEW( usual, sizeof( UsualDomiDino));

	Action* dead = new DeadDomiDino; static_cast<Component*>(dead)->SetOwner(o);
	MEMDUMP_NEW( dead, sizeof( DeadDomiDino));

	m_actions.insert(lookupTable::value_type(DOMI_DINO_NORMAL, usual));
	m_actions.insert(lookupTable::value_type(DOMI_DINO_DEATH, dead));
}

//----------------------------------------------
// CharaUpdatable
//----------------------------------------------
bool JudgeUpdatableCharaMe::Judge()
{
	if (g_pCharaManager->getCharaByNetIdx(m_slotIdx) == nullptr) return false;
	if (tu::IsInBattleField(m_slotIdx) == false) return false;
	if (UserContext::i()->IsPlayingSlot(m_slotIdx) == false) return false;
	if (MyRoomInfoContext::i()->getStage()->GetStageMode() == STAGE_MODE_CONVOY )
	{
				if(CGameMissionManager::i()->getUsurpationIdx() == m_slotIdx)
			return false;
	}
	return true;
}

bool JudgeUpdatableCharaFellow::Judge()
{
	if (g_pCharaManager->getCharaByNetIdx(m_slotIdx) == nullptr) return false;
	if (tu::IsInBattleField(m_slotIdx) == false) return false;
	if (UserContext::i()->IsPlayingSlot(m_slotIdx) == false) return false;
	if (IsLocatedAtSameFloor(m_slotIdx) == false) return false;
	if (MyRoomInfoContext::i()->getStage()->GetStageMode() == STAGE_MODE_CONVOY )
	{
		if(CGameMissionManager::i()->getUsurpationIdx() == m_slotIdx)
			return false;
	}
	return true;
}

bool JudgeUpdatableDomiDino::Judge()
{
	if (g_pCharaManager->getCharaByNetIdx(m_slotIdx, true) == nullptr) return false;
	if (g_pCharaManager->getCharaByNetIdx(m_slotIdx, true)->isCharaStateMask(CHARA_STATE_DEATH)) return false;
	if (IsLocatedAtSameFloor(m_slotIdx, true) == false) return false;
	return true;
}

bool JudgeUpdatableUsurpation::Judge()
{
	return CGameMissionManager::i()->CheckCrownMinimap();
}

//----------------------------------------------
// SirenAnimator
//----------------------------------------------
SirenAnimatorWhirl::SirenAnimatorWhirl(REAL32 scale) 
	: m_timer(0.0f), m_scale(scale), m_currScale(1.0f) {}

void SirenAnimatorWhirl::OnRun(DrawObject* img, VEC2D* pos, REAL32 tm)
{
	m_timer = MIN(1.0f, m_timer+tm*m_currScale);

	REAL32 size = MAX(0.0f, m_scale*i3Math::sin(I3_PI2*m_timer));
	REAL32 alpha = MAX(0.0f, i3Math::cos(I3_PI2*m_timer));

	if (1.0f <= m_timer)
	{
		m_timer = 0.0f;
		m_currScale += 0.01f;
	}

	img->SetPosition(pos->x, pos->y);
	img->SetScale(size);
	img->SetAlpha(static_cast<UINT8>(255*alpha));
	img->Show();
}

void SirenAnimatorWhirl::OnStop(DrawObject* img)
{
	if (m_timer != 0.0f) m_timer = 0.0f;
	if (m_currScale != 1.0f) m_currScale = 1.0f;
	img->Hide();
}


//----------------------------------------------
// MissionObject
//----------------------------------------------
namespace
{
	struct BombInstallArea_A_NothingHappened : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));
			
			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_BOMB_AREA_A);
			bombImage->SetColor(255, 255, 255, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_bomb_area_a_nothing_happened;
	struct BombInstallArea_A_Installed : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_BOMB_AREA_A);
			bombImage->SetColor(255, 0, 0, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);

			VEC2D pos; CalcPosWorldToMinimap(C4Context::i()->FindAreaProp(BOMB_AREA_A)->pos, pos);
			sirenAnimator->OnRun(sirenImage, &pos, tm);
		}
	}g_bomb_area_a_installed;
	struct BombInstallArea_A_IsntInMyFloor : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_BOMB_AREA_A);
			bombImage->Hide();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_bomb_area_a_isnt_in_my_floor;

	struct BombInstallArea_B_NothingHappened : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_BOMB_AREA_B);
			bombImage->SetColor(255, 255, 255, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_bomb_area_b_nothing_happened;
	struct BombInstallArea_B_Installed : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_BOMB_AREA_B);
			bombImage->SetColor(255, 0, 0, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);

			VEC2D pos; CalcPosWorldToMinimap(C4Context::i()->FindAreaProp(BOMB_AREA_B)->pos, pos);
			sirenAnimator->OnRun(sirenImage, &pos, tm);
		}
	}g_bomb_area_b_Installed;
	struct BombInstallArea_B_Installed_StormTubeMap : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			WeaponBase* wp = nullptr;
#ifdef USE_EVENT_SHUTTLEX
			wp = g_pWeaponManager->FindDropWeapon(nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex( WEAPON::SHUTTLEX));
#else
			wp = g_pWeaponManager->FindDropWeapon(nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
#endif
			VEC3D* worldPos;
			if (wp)
				worldPos = i3Matrix::GetPos(wp->getSceneObject()->GetCacheMatrix());
			else
				worldPos = &C4Context::i()->FindAreaProp(BOMB_AREA_B)->pos;

			VEC2D minimapPos;
			CalcPosWorldToMinimap(*worldPos, minimapPos);

			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_BOMB_AREA_B);
			bombImage->SetPosition(minimapPos.x, minimapPos.y);
			bombImage->SetColor(255, 0, 0, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnRun(sirenImage, &minimapPos, tm);
		}
	}g_bomb_area_b_Installed_in_stormtubemap;
	struct BombInstallArea_B_IsntInMyFloor : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_BOMB_AREA_B);
			bombImage->Hide();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_bomb_area_b_isnt_in_my_floor;

	struct DestroyObj_Red_NothingHappened : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DESTROY_RED);
			bombImage->SetColor(255, 255, 255, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_destroy_red_nothing_happened;
	struct DestroyObj_Red_UnderAttack : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DESTROY_RED);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			VEC2D pos; CalcPosWorldToMinimap(*(GetDestroyObjPos(g_destroyObjRedIdx)), pos);
			sirenAnimator->OnRun(sirenImage, &pos, tm);
		}
	}g_destroy_red_under_attack;

	struct DestroyObj_Blue_NothingHappened : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DESTROY_BLUE);
			bombImage->SetColor(255, 255, 255, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_BLUE);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_destroy_blue_nothing_happened;
	struct DestroyObj_Blue_UnderAttack : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DESTROY_BLUE);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_BLUE);

			VEC2D pos; CalcPosWorldToMinimap(*(GetDestroyObjPos(g_destroyObjBlueIdx)), pos);
			sirenAnimator->OnRun(sirenImage, &pos, tm);
		}
	}g_destroy_blue_under_attack;

	struct DefenceObj_Red_NothingHappened : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DEFENCE_RED);
			bombImage->SetColor(255, 255, 255, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_defence_red_nothing_happened;
	struct DefenceObj_Red_Destroyed : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DEFENCE_RED);
			bombImage->Hide();

			SirenAnimator* sirenAnimator
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_defence_red_destroyed;
	struct DefenceObj_Red_UnderAttack : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DEFENCE_RED);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);

			VEC2D pos; CalcPosWorldToMinimap(*(CGameMissionManager::i()->getDefenceObj(g_defenceObjRedIdx)->GetPos()), pos);
			sirenAnimator->OnRun(sirenImage, &pos, tm);
		}
	}g_defence_red_under_attack;

	struct DefenceObj_Blue_NothingHappened : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DEFENCE_BLUE);
			bombImage->SetColor(255, 255, 255, 255);
			bombImage->Show();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_defence_blue_nothing_happened;
	struct DefenceObj_Blue_Destroyed : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DEFENCE_BLUE);
			bombImage->Hide();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);
			sirenAnimator->OnStop(sirenImage);
		}
	}g_defence_blue_destroyed;
	struct DefenceObj_Blue_UnderAttack : MissionObjectBehavior
	{
		virtual void operator()(MissionObject* o, REAL32 tm)
		{
			ImageMissionObjWithSiren* imos 
				= static_cast<ImageMissionObjWithSiren*>(o->GetOwner()->GetComponent("ImageMissionObjWithSiren"));

			DrawObject* bombImage = imos->GetMissionObj()->GetImage(IMAGE_DEFENCE_BLUE);
			bombImage->Hide();

			SirenAnimator* sirenAnimator 
				= static_cast<SirenAnimator*>(o->GetOwner()->GetComponent("SirenAnimator"));
			DrawObject* sirenImage = imos->GetSiren()->GetImage(IMAGE_SIREN_RED);

			VEC2D pos; CalcPosWorldToMinimap(*(CGameMissionManager::i()->getDefenceObj(g_defenceObjBlueIdx)->GetPos()), pos);
			sirenAnimator->OnRun(sirenImage, &pos, tm);
		}
	}g_defence_blue_under_attack;
}

void MissionObject::Update(REAL32 tm)
{
	UINT32 curr = 
		static_cast<MissionObjectState*>(GetOwner()->GetComponent("MissionObjectState"))->GetState();

	lookupTable::iterator it = m_behaviors.find(curr);
	if (it != m_behaviors.end())
		(*it->second)(this, tm);
}

void BombInstallArea_A::Register()
{
	m_behaviors.insert(lookupTable::value_type(BOMB_A_NOTHING_HAPPENED, &g_bomb_area_a_nothing_happened));
	m_behaviors.insert(lookupTable::value_type(BOMB_A_INSTALLED, &g_bomb_area_a_installed));
	m_behaviors.insert(lookupTable::value_type(BOMB_A_AREA_IS_NOT_IN_MY_FLOOR, &g_bomb_area_a_isnt_in_my_floor));
}

void BombInstallArea_B::Register() 
{
	m_behaviors.insert(lookupTable::value_type(BOMB_B_NOTHING_HAPPENED, &g_bomb_area_b_nothing_happened));
	m_behaviors.insert(lookupTable::value_type(BOMB_B_INSTALLED, &g_bomb_area_b_Installed));
	m_behaviors.insert(lookupTable::value_type(BOMB_B_INSTALLED_IN_STORMTUBE_MAP, &g_bomb_area_b_Installed_in_stormtubemap));
	m_behaviors.insert(lookupTable::value_type(BOMB_B_AREA_IS_NOT_IN_MY_FLOOR, &g_bomb_area_b_isnt_in_my_floor));
}

void DestroyObj_Red::Register() 
{
	m_behaviors.insert(lookupTable::value_type(DESTROY_OBJ_NOTHING_HAPPENED, &g_destroy_red_nothing_happened));
	m_behaviors.insert(lookupTable::value_type(DESTROY_OBJ_UNDER_ATTACK, &g_destroy_red_under_attack));
}

void DestroyObj_Blue::Register() 
{
	m_behaviors.insert(lookupTable::value_type(DESTROY_OBJ_NOTHING_HAPPENED, &g_destroy_blue_nothing_happened));
	m_behaviors.insert(lookupTable::value_type(DESTROY_OBJ_UNDER_ATTACK, &g_destroy_blue_under_attack));
}

void DefenceObj_Red::Register() 
{
	m_behaviors.insert(lookupTable::value_type(DEFENCE_OBJ_NOTHING_HAPPENED, &g_defence_red_nothing_happened));
	m_behaviors.insert(lookupTable::value_type(DEFENCE_OBJ_DESTROYED, &g_defence_red_destroyed));
	m_behaviors.insert(lookupTable::value_type(DEFENCE_OBJ_UNDER_ATTACK, &g_defence_red_under_attack));
}

void DefenceObj_Blue::Register() 
{
	m_behaviors.insert(lookupTable::value_type(DEFENCE_OBJ_NOTHING_HAPPENED, &g_defence_blue_nothing_happened));
	m_behaviors.insert(lookupTable::value_type(DEFENCE_OBJ_DESTROYED, &g_defence_blue_destroyed));
	m_behaviors.insert(lookupTable::value_type(DEFENCE_OBJ_UNDER_ATTACK, &g_defence_blue_under_attack));
}

//----------------------------------------------
// MissionObjectState
//----------------------------------------------
INT32 BombAreaState_A::GetState()
{
	INT32 portalID = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx())->getPortalID();

	if (C4Context::i()->FindAreaProp(BOMB_AREA_A)->floor !=
		g_pFramework->GetCurrentStage()->getWorld()->getMapSectorTable()->getFloor(portalID))
		return BOMB_A_AREA_IS_NOT_IN_MY_FLOOR;

	if ((CGameMissionManager::i()->IsInstalledArea(BOMB_AREA_A)) &&
		((tu::GetMyTeamType() == TEAM_RED) || (CGameMissionManager::i()->isOpenedLocationOfBomb())))
		return BOMB_A_INSTALLED;

	return BOMB_A_NOTHING_HAPPENED;
}

INT32 BombAreaState_B::GetState()
{
	INT32 portalID = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx())->getPortalID();

	if (C4Context::i()->FindAreaProp(BOMB_AREA_B)->floor !=
		g_pFramework->GetCurrentStage()->getWorld()->getMapSectorTable()->getFloor(portalID))
		return BOMB_B_AREA_IS_NOT_IN_MY_FLOOR;

	if ((MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)		&&
		(CGameMissionManager::i()->getMissionState() == GAME_MISSION_STATE_INSTALLED)	&&
		(CGameMissionManager::i()->IsInstalledArea(BOMB_AREA_B)))
		return BOMB_B_INSTALLED_IN_STORMTUBE_MAP;

	if ((CGameMissionManager::i()->IsInstalledArea(BOMB_AREA_B)) &&
		((tu::GetMyTeamType() == TEAM_RED) || (CGameMissionManager::i()->isOpenedLocationOfBomb())))
		return BOMB_B_INSTALLED;

	return BOMB_B_NOTHING_HAPPENED;
}

DestroyObjState::DestroyObjState(INT32 objIdx) : m_objIdx(objIdx) {}
INT32 DestroyObjState::GetState()
{
	if (DestroyObjectAttackState::IsUnderAttack(m_objIdx) == true) return DESTROY_OBJ_UNDER_ATTACK;
	return DESTROY_OBJ_NOTHING_HAPPENED;
}

DefenceObjState::DefenceObjState(INT32 objIdx) : m_objIdx(objIdx) {}
INT32 DefenceObjState::GetState()
{
	if (CGameMissionManager::i()->getDefenceObj(m_objIdx)->getCurrentHP() <= 0.0f) return DEFENCE_OBJ_DESTROYED;
	if (DestroyObjectAttackState::IsUnderAttack(m_objIdx) == true) return DEFENCE_OBJ_UNDER_ATTACK;
	return DEFENCE_OBJ_NOTHING_HAPPENED;
}

//----------------------------------------------
// SkillObject
//----------------------------------------------
void Sentrygun::Update()
{
	dss::CIdentity* info = CDominationSkillSystem::i()->Find(DOMI_TYPE_SENTRYGUN, m_idx);
	if (info == nullptr) return;

	DrawObject* image = 
		static_cast<ImageSentrygun*>(GetOwner()->GetComponent("ImageSentrygun"))->GetImages();

	UINT32 currState = static_cast<DominationSkill::CServiceable*>(info)->GetState();

	switch (currState)
	{
	case DominationSkill::INSTALLABLE:
		{
			if (image->GetCurrentImage() != IMAGE_SKILLOBJ_DEACTIVATED)
				image->SetCurrentImage(IMAGE_SKILLOBJ_DEACTIVATED);
		}
		break;

	case DominationSkill::REPAIRABLE:
		{
			if (image->GetCurrentImage() != IMAGE_SKILLOBJ_REPAIR_REQUIRING)
				image->SetCurrentImage(IMAGE_SKILLOBJ_REPAIR_REQUIRING);
		}
		break;

	default:
		{
			if (image->GetCurrentImage() != IMAGE_SKILLOBJ_ACTIVATED)
				image->SetCurrentImage(IMAGE_SKILLOBJ_ACTIVATED);
		}
		break;
	}

	VEC3D pos3d = info->GetPos();
	VEC2D pos2d; CalcPosWorldToMinimap(pos3d, pos2d);
	image->SetPosition(pos2d.x, pos2d.y);
}

void Dummy::Update()
{
	dss::CIdentity* info = CDominationSkillSystem::i()->Find(DOMI_TYPE_DUMMY, m_idx);
	if (info == nullptr) return;

	DrawObject* image = 
		static_cast<ImageSentrygun*>(GetOwner()->GetComponent("ImageDummy"))->GetImages();

	UINT32 currState = static_cast<DominationSkill::CServiceable*>(info)->GetState();

	switch (currState)
	{
	case DominationSkill::INSTALLABLE:
		{
			if (image->GetCurrentImage() != IMAGE_SKILLOBJ_DEACTIVATED)
				image->SetCurrentImage(IMAGE_SKILLOBJ_DEACTIVATED);
		}
		break;

	case DominationSkill::REPAIRABLE:
		{
			if (image->GetCurrentImage() != IMAGE_SKILLOBJ_REPAIR_REQUIRING)
				image->SetCurrentImage(IMAGE_SKILLOBJ_REPAIR_REQUIRING);
		}
		break;

	default:
		{
			if (image->GetCurrentImage() != IMAGE_SKILLOBJ_ACTIVATED)
				image->SetCurrentImage(IMAGE_SKILLOBJ_ACTIVATED);
		}
		break;
	}

	VEC3D pos3d = info->GetPos();
	VEC2D pos2d; CalcPosWorldToMinimap(pos3d, pos2d);
	image->SetPosition(pos2d.x, pos2d.y);
}

void SkillShop::Update()
{
	dss::CIdentity* info = CDominationSkillSystem::i()->Find(DOMI_TYPE_SKILLSHOP, m_idx);
	if (info == nullptr) return;

	DrawObject* image = 
		static_cast<ImageSentrygun*>(GetOwner()->GetComponent("ImageSkillShop"))->GetImages();

	UINT32 currState = static_cast<DominationSkill::CServiceable*>(info)->GetState();

	if (currState == DominationSkill::SHOP_OPEN)
	{
		if (image->GetCurrentImage() != IMAGE_SKILLOBJ_ACTIVATED)
			image->SetCurrentImage(IMAGE_SKILLOBJ_ACTIVATED);
	}
	else
	{
		if (image->GetCurrentImage() != IMAGE_SKILLOBJ_DEACTIVATED)
			image->SetCurrentImage(IMAGE_SKILLOBJ_DEACTIVATED);
	}

	VEC3D pos3d = info->GetPos();
	VEC2D pos2d; CalcPosWorldToMinimap(pos3d, pos2d);
	image->SetPosition(pos2d.x, pos2d.y);
}