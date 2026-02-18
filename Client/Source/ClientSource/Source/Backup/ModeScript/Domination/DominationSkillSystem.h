#pragma once

#include "DominationSkillUtil.h"
#include "DominationSkillObject.h"
#include "../../CommonSource/CommonDediCli.h"


class SkillNotice;
class UserSkillApply;

namespace UI_Util { class CIndicateHUD; class CIndicateObject;}

template <> struct is_tinst_manual<class CDominationSkillSystem> : true_type {};
class CDominationSkillSystem
{
	friend class dss::CRawObject;
	friend class dss::CSupply;
	friend class dss::CSentryGun;
	friend class dss::CCompletedObject;

private:
	//New Skill System
	i3::vector<dss::CIdentity*>			m_SkillList;
	i3::map<UINT32, dss::Info*>			m_InfoList;
	dss::CServiceable*					m_CrossHited;
	SkillNotice*						m_skillNotice;
	UserSkillApply*						m_userSkill;

	//skill shop hud
	UI_Util::CIndicateObject* m_skill_shop_hud_obj;
	UI_Util::CIndicateHUD* m_skill_shop_hud;
public:
	CDominationSkillSystem();
	~CDominationSkillSystem();

	void Initialize();

	void OnButtonClicked(CGameCharaBase* customer);

	bool Install( UINT32 ItemID, INT16 ObjType, INT16 ObjIdx, VEC3D* pPos, CGameCharaBase* pChara = NULL, REAL32 theta = 0.0f);
	void UnInstall( dss::CIdentity* p);
	void Upgrade(const INT8 type, const INT8 idx, const INT8 customerIdx);
	void Fix(const INT8 type, const INT8 idx, const INT8 customerIdx);

	void CreateSkillShopHud( INT16 Idx);
	void DeleteSkillShopHud();
	void SetEnableSkillShopHud( bool inter_mission, bool open_skill_popup);

	UserSkillApply* GetUserSkillApply() const { return m_userSkill; }
	SkillNotice* GetSkillNotice() const { return m_skillNotice; }

	dss::CIdentity*	Find( UINT32 ItemID, INT16 ObjType, INT16 ObjIdx);
	dss::CIdentity*	Find( INT16 ObjType, INT16 ObjIdx);
	bool CheckCrossHit( CGameCharaBase* pChara);
	void MoveShapePosition( WeaponBase* pWeapon, VEC3D* pos);
	void Update( REAL32 fElapsedTime);
	void ChangeUseItemID( UINT32 ItemID, UINT32 UseItemID);
	const dss::CServiceable* GetCrossHited() const { return m_CrossHited;}
};