#pragma once

#include "DominationSkillUtil.h"
#include "SkillObjectModel.h"
#include "../../ui/hud/UIHudDomi_SkillHp.h"

#include <unordered_map>
#include "../BitFlags.h"

namespace DominationSkill
{
	//----------------------------------------------
	// CIdentity : 스킬 오브젝트 고유 속성
	//----------------------------------------------
	class CIdentity
	{
	protected:
		T_ItemID	m_ItemID;
		INT32		m_TYPE_SLOT;			// SLOT과 ObjIdx보관하고 서버에 전달..
		INT32		m_ObjIdx;	
		i3::wstring	m_Nick;

		void Instantiate( i3Chara* pChara);

	public:
		CIdentity(void);
		virtual ~CIdentity(void);

		virtual void OnCreate( void* pRes, T_ItemID ItemID, INT32 Type, INT32 Idx) { m_ItemID = ItemID; m_TYPE_SLOT = Type; m_ObjIdx = Idx;}
		virtual void OnDestroy() {}
		virtual void OnUpdate( REAL32 fElapsedTime) { ShowHud();}
		virtual void OnInstall( const VEC3D* pPos, REAL32 theta = 0.0f, CGameCharaBase* chara = nullptr) {}
		virtual void OnUnInstall() {}

		virtual VEC3D GetBoundSize() { return VEC3D();}
		virtual VEC3D GetPos() const { return VEC3D();}
		virtual WeaponBase* GetWeapon() { return nullptr;}

		virtual void ShowHud() {}

		bool IsObjectSame(INT32 Type_Slot, INT32 ObjIdx) const { return m_TYPE_SLOT == Type_Slot && m_ObjIdx == ObjIdx;}
		bool IsObjectSame( CIdentity*& o) const { return o == this;}
		bool IsObjectSame( const CIdentity*& o) const { return o == this;}
		bool IsItemIDSame( T_ItemID ItemID ) const  { return m_ItemID == ItemID;}

		UINT32	GetItemID() const { return m_ItemID; }
		INT32	GetID() const { return m_ObjIdx; }
		INT32	GetType() const { return m_TYPE_SLOT; }

		virtual const char* GetName() const { return ""; }
	};


	//----------------------------------------------
	// CServiceable : 서비스 가능 상태 보관 및 서버에 요청
	//----------------------------------------------
	enum SKILLOBJ_SERVICEABLE_STATE
	{
		NONE,
		INSTALLABLE,
		UPGRADABLE,
		REPAIRABLE,
		HP_RECOVERABLE,
		AMMO_RECOVERABLE,
		SHOP_OPEN,
		SERVICEABLE_STATE_MAX,
	};
	struct Actor;
	class CServiceable : public CIdentity
	{
	protected:
		UINT32 m_currState;
		Actor* m_actors[SERVICEABLE_STATE_MAX];
		bit_flags::U8Flags m_flagPermitUserIdx;

	public:
		CServiceable();
		virtual ~CServiceable();

		void	OnButtonClicked(CGameCharaBase* customer);
		void	SetState(UINT32 state);
		UINT32	GetState() const { return m_currState; }
		void	EnableCharaIdx(INT32 idx) { m_flagPermitUserIdx.SetBit(idx); }
		void	DisableCharaIdx(INT32 idx) { m_flagPermitUserIdx.ClearBit(idx); }
		void	ClearCharaIdx() { m_flagPermitUserIdx.Clear(); }
	};
	
	//----------------------------------------------
	// CRawObject : 설치 전 투명 스킬 오브젝트
	//----------------------------------------------
	class CRawObject : public CServiceable
	{
	protected:
		CSkillObjectModel*	m_pChara;
		i3AmbientAttr*		m_pAmbientAttr;
		dss::Info*			m_Info;

	public:
		CRawObject(void);
		~CRawObject(void);
				
		void OnCreate( void* pRes, T_ItemID ItemID, INT32 Type, INT32 Idx);
		void OnInstall( const VEC3D* pPos, REAL32 theta = 0.0f, CGameCharaBase* chara = nullptr);

		VEC3D GetBoundSize();
		VEC3D GetPos() const;
		REAL32 GetMaxTime() const{ return 0.3f;}

		virtual const char* GetName() const { return "RAW_OBJECT"; }
	};

	//----------------------------------------------
	// CCompletedObject : 설치된 스킬 오브젝트
	//----------------------------------------------
	class CCompletedObject : public CServiceable
	{
	protected:
		WeaponBase*			m_pWeapon;
		i3::rc_wstring		m_owner_nick;

		REAL32 hud_x, hud_y;
		INT32 hud_len;
		bool _calc_hud();

		dui::skill_hp* hud_hp;

#if defined (I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		INT32 m_lv;
		INT32 m_hp;
#endif

	public:
		CCompletedObject(void);
		~CCompletedObject(void);

		void OnCreate( void* pRes, T_ItemID ItemID, INT32 Type, INT32 Idx);
		void OnDestroy();
		void OnInstall( const VEC3D* pPos, REAL32 theta = 0.0f, CGameCharaBase* chara = nullptr);

		VEC3D GetBoundSize();
		VEC3D GetPos() const;
		WeaponBase* GetWeapon() { return m_pWeapon;}
	
		virtual const char* GetName() const { return "COMPLETED_OBJECT"; }
		virtual void InitializeAtInstall( CGameCharaBase* chara = nullptr);
	};


	//----------------------------------------------
	// 실제 오브젝트들...
	//----------------------------------------------
	class CDummy : public CCompletedObject
	{
	public:
		void OnUnInstall();
		void InitializeAtInstall( CGameCharaBase* chara = nullptr);
		void ShowHud();
		virtual const char* GetName() const { return "DUMMY"; }
	};
		  
	class CSentryGun : public CCompletedObject
	{
	public:
		void OnUnInstall();
		void InitializeAtInstall( CGameCharaBase* chara = nullptr);
		void ShowHud();
		virtual const char* GetName() const { return "SENTRYGUN"; }
	};

	class CSupply : public CCompletedObject
	{
	public:
		void OnUnInstall();
		void InitializeAtInstall( CGameCharaBase* chara = nullptr) {}
		virtual const char* GetName() const { return "SUPPLY"; }
	};
};

namespace dss = DominationSkill;