#pragma once

#include "DominationSkillUtil.h"
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
		UINT32	m_ItemID;
		INT16	m_TYPE_SLOT;			// SLOT과 ObjIdx보관하고 서버에 전달..
		INT16	m_ObjIdx;	
		i3::string m_Nick;

		void Instantiate( i3Chara* pChara);

	public:
		CIdentity(void);
		virtual ~CIdentity(void);

		virtual void OnCreate( void* pRes, UINT32 ItemID, INT16 Type, INT16 Idx) { m_ItemID = ItemID; m_TYPE_SLOT = Type; m_ObjIdx = Idx;}
		virtual void OnDestroy() {}
		virtual void OnUpdate( REAL32 fElapsedTime) { ShowNick();}
		virtual void OnInstall( VEC3D* pPos, REAL32 theta = 0.0f, CGameCharaBase* chara = NULL) {}
		virtual void OnUnInstall() {}

		virtual VEC3D GetBoundSize() { return VEC3D();}
		virtual VEC3D GetPos() const { return VEC3D();}
		virtual WeaponBase* GetWeapon() { return NULL;}

		virtual void ShowNick() {}

		bool IsObjectSame(INT16 Type_Slot, INT16 ObjIdx) const { return m_TYPE_SLOT == Type_Slot && m_ObjIdx == ObjIdx;}
		bool IsObjectSame( CIdentity*& o) const { return o == this;}
		bool IsObjectSame( const CIdentity*& o) const { return o == this;}
		bool IsItemIDSame(UINT32 ItemID ) const  { return m_ItemID == ItemID;}

		UINT32	GetItemID() const { return m_ItemID; }
		INT16	GetID() const { return m_ObjIdx; }
		INT16	GetType() const { return m_TYPE_SLOT; }

		virtual const char* GetName() const { return ""; }
	};


	//----------------------------------------------
	// CServiceable : 서비스 가능 상태 보관 및 서버에 요청
	//----------------------------------------------
	enum SKILLOBJ_POSSIBLE_STATE
	{
		STATE_NONE,
		STATE_INSTALLABLE,
		STATE_UPGRADABLE,
		STATE_REPAIRABLE,
		STATE_HP_RECOVER,
		STATE_AMMO_RECOVER,
		STATE_SHOP_SALABLE
	};
	struct Actor;
	class CServiceable : public CIdentity
	{
	protected:
		UINT32 m_state;
		std::tr1::unordered_map<UINT32, Actor*> m_map;
		bit_flags::U8Flags m_flagPermitUserIdx;

	public:
		CServiceable();
		virtual ~CServiceable();

		void	OnButtonClicked(CGameCharaBase* customer);
		void	SetState(UINT32 state) { m_state = state; }
		UINT32	GetState() const { return m_state; }
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
		i3Chara*			m_pChara;
		i3AmbientAttr*		m_pAmbientAttr;
		dss::Info*			m_Info;

	public:
		CRawObject(void);
		~CRawObject(void);
				
		void OnCreate( void* pRes, UINT32 ItemID, INT16 Type, INT16 Idx);
		void OnInstall( VEC3D* pPos, REAL32 theta = 0.0f, CGameCharaBase* chara = NULL);

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
		i3::string			m_owner_nick;
	public:
		CCompletedObject(void);
		~CCompletedObject(void);

		void OnCreate( void* pRes, UINT32 ItemID, INT16 Type, INT16 Idx);
		void OnDestroy();
		void OnInstall( VEC3D* pPos, REAL32 theta = 0.0f, CGameCharaBase* chara = NULL);

		VEC3D GetBoundSize();
		VEC3D GetPos() const;
		WeaponBase* GetWeapon() { return m_pWeapon;}
		void ShowNick();

		virtual const char* GetName() const { return "COMPLETED_OBJECT"; }
		virtual void InitializeAtInstall( CGameCharaBase* chara = NULL);
		
	};


	//----------------------------------------------
	// 실제 오브젝트들...
	//----------------------------------------------
	class CDummy : public CCompletedObject
	{
	public:
		void OnUnInstall();
		void InitializeAtInstall( CGameCharaBase* chara = NULL);
		virtual const char* GetName() const { return "DUMMY"; }
	};
		  
	class CSentryGun : public CCompletedObject
	{
	public:
		void OnUnInstall();
		void InitializeAtInstall( CGameCharaBase* chara = NULL);
		virtual const char* GetName() const { return "SENTRYGUN"; }
	};

	class CSupply : public CCompletedObject
	{
	public:
		void OnUnInstall();
		void InitializeAtInstall( CGameCharaBase* chara = NULL) {}
		virtual const char* GetName() const { return "SUPPLY"; }
	};
};

namespace dss = DominationSkill;