#pragma once

#include "DominationSkillUtil.h"
#include "DominationSkillObject.h"
#include "../../../Common/CommonSource/CommonDediCli.h"


class SkillNotice;
class UserSkillApply;

namespace UI_Util { class CIndicateHUD; class CIndicateObject;}

template <> struct is_tinst_manual<class CDominationSkillSystem> : std::tr1::true_type {};
class CDominationSkillSystem
{
	friend class dss::CRawObject;
	friend class dss::CSupply;
	friend class dss::CSentryGun;
	friend class dss::CCompletedObject;

private:
	//New Skill System
	i3::vector<dss::CIdentity*>			m_SkillList;
	i3::vector_map<UINT32, dss::Info*>			m_InfoList;
	dss::CServiceable*					m_CrossHited;

	//skill shop hud
	UI_Util::CIndicateObject* m_skill_shop_hud_obj;
	UI_Util::CIndicateHUD* m_skill_shop_hud;

public:
	static CDominationSkillSystem* i() { return tinst<CDominationSkillSystem>(); }

	CDominationSkillSystem();
	~CDominationSkillSystem();

	void Initialize();

	void OnButtonClicked(CGameCharaBase* customer);

	bool Install( T_ItemID ItemID, INT32 ObjType, INT32 ObjIdx, const VEC3D* pPos, CGameCharaBase* pChara = nullptr, REAL32 theta = 0.0f);
	void UnInstall( dss::CIdentity* p);
	void Upgrade(const INT32 type, const INT32 idx, const INT32 customerIdx, REAL32 theta);
	void Fix(const INT32 type, const INT32 idx, const INT32 customerIdx);

	void CreateSkillShopHud( INT32 Idx);
	void DeleteSkillShopHud();
	void SetEnableSkillShopHud( bool inter_mission, bool open_skill_popup);

	dss::CIdentity*	Find( T_ItemID ItemID, INT32 ObjType, INT32 ObjIdx);
	dss::CIdentity*	Find( INT32 ObjType, INT32 ObjIdx);
	bool CheckCrossHit( CGameCharaBase* pChara);
	void MoveShapePosition( WeaponBase* pWeapon, VEC3D* pos);
	void Update( REAL32 fElapsedTime);
	void ChangeUseItemID( T_ItemID ItemID, UINT32 UseItemID);
	const dss::CServiceable* GetCrossHited() const { return m_CrossHited;}
};