#include "pch.h"
#include "DesigAttr.h"
#include "DesigApplyInfo.h"

#include "i3Base/factory/factory.h"
#include "i3Base/string/get_class_name.h"

DesigAttr::DesigAttr(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
m_wstrID(wstrID), m_wstrText(wstrText), m_val(val) {}



class DesigAttr_CharaMoveSpeed : public DesigAttr
{
public:
	DesigAttr_CharaMoveSpeed(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigChara.charaMoveSpeed += GetValue();
	  }
};

class DesigAttr_WeaponFireSpeed : public DesigAttr
{
public:
	DesigAttr_WeaponFireSpeed(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponFireSpeed += GetValue();
	  }
};

class DesigAttr_WeaponMoveSpeed : public DesigAttr
{
public:
	DesigAttr_WeaponMoveSpeed(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponMoveSpeed += GetValue();
	  }
};

class DesigAttr_CharaJump : public DesigAttr
{
public:
	DesigAttr_CharaJump(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigChara.charaJump += GetValue();
	  }
};

class DesigAttr_CharaAddSpeed : public DesigAttr
{
public:
	DesigAttr_CharaAddSpeed(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigChara.charaAddSpeed += GetValue();
	  }
};

class DesigAttr_FallDown : public DesigAttr
{
public:
	DesigAttr_FallDown(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigChara.charaFallDamageReduce += GetValue();
	  }
};

class DesigAttr_ReloadTime : public DesigAttr
{
public:
	DesigAttr_ReloadTime(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponReloadTime += GetValue();
	  }
};

class DesigAttr_AttackRange : public DesigAttr
{
public:
	DesigAttr_AttackRange(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponAttackRange += GetValue();
	  }
};

class DesigAttr_HorzReact : public DesigAttr
{
public:
	DesigAttr_HorzReact(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponHorzReaction += GetValue();
	  }
};

class DesigAttr_VertReact : public DesigAttr
{
public:
	DesigAttr_VertReact(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponVertReaction += GetValue();
	  }
};

class DesigAttr_Declination : public DesigAttr
{
public:
	DesigAttr_Declination(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponDeclination += GetValue();
	  }
};

class DesigAttr_ThrowSpeed : public DesigAttr
{
public:
	DesigAttr_ThrowSpeed(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponThrowSpeed += GetValue();
	  }
};

class DesigAttr_BombTime : public DesigAttr
{
public:
	DesigAttr_BombTime(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponBombTime += GetValue();
	  }
};

class DesigAttr_BombRange : public DesigAttr
{
public:
	DesigAttr_BombRange(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponBombRange += GetValue();
	  }
};

class DesigAttr_WeaponReact : public DesigAttr
{
public:
	DesigAttr_WeaponReact(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponReaction += GetValue();
	  }
};

class DesigAttr_DPDRate : public DesigAttr
{
public:
	DesigAttr_DPDRate(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponDPDRate += GetValue();
	  }
};

class DesigAttr_FirstShot : public DesigAttr
{
public:
	DesigAttr_FirstShot(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}	

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  inout_info->desigWeapon.weaponFirstShotRate += GetValue();
	  }
};

class DesigAttr_Purchase : public DesigAttr
{
public:
	DesigAttr_Purchase(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}	

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override {}
	  virtual bool	IsPurchasable() const override { return true; }
};

class DesigAttr_WeaponDamage : public DesigAttr
{
public:
	DesigAttr_WeaponDamage(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}	

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  // None Impl.		쓰이지 않고 있지만..파생클래스는 일단 방치함..
	  }
};

class DesigAttr_ScoopeRange : public DesigAttr
{
public:
	DesigAttr_ScoopeRange(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}	

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  // None Impl.		쓰이지 않고 있지만..파생클래스는 일단 방치함..
	  }
};

class DesigAttr_CrossHairRestore : public DesigAttr
{
public:
	DesigAttr_CrossHairRestore(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}	

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override
	  {
		  // None Impl.		쓰이지 않고 있지만..파생클래스는 일단 방치함..
	  }
};


namespace
{


	template<class Desig_Derived>
	void	RegisterCreator(DesigAttrFactory& fac, i3::identity<Desig_Derived> t)
	{
		const char* class_name = i3::get_class_name<Desig_Derived>();
		const char* prefix = "DesigAttr_";
		static const size_t prefix_len = 10;    // strlen(prefix);

		if (i3::generic_is_nequal(class_name, prefix, prefix_len) )		// Desig_ 로 시작하는 클래스만 처리하도록 조건처리..
		{
			const char* class_title = class_name + prefix_len;
			i3::rc_wstring wstr_class_title;
			i3::utf8_to_utf16(class_title, wstr_class_title);
			fac.register_creator(wstr_class_title, t );
		}
	}
}

void	RegisterDesigAttrs(DesigAttrFactory& fac)
{
	RegisterCreator(fac, i3::identity< DesigAttr_CharaMoveSpeed  >());
	RegisterCreator(fac, i3::identity< DesigAttr_WeaponFireSpeed  >());
	RegisterCreator(fac, i3::identity< DesigAttr_WeaponMoveSpeed  >());
	RegisterCreator(fac, i3::identity< DesigAttr_CharaJump  >());
	RegisterCreator(fac, i3::identity< DesigAttr_CharaAddSpeed >());

	RegisterCreator(fac, i3::identity< DesigAttr_FallDown >());
	RegisterCreator(fac, i3::identity< DesigAttr_ReloadTime >());
	RegisterCreator(fac, i3::identity< DesigAttr_AttackRange  >());
	RegisterCreator(fac, i3::identity< DesigAttr_HorzReact  >());
	RegisterCreator(fac, i3::identity< DesigAttr_VertReact >());

	RegisterCreator(fac, i3::identity< DesigAttr_Declination >());
	RegisterCreator(fac, i3::identity< DesigAttr_ThrowSpeed >());
	RegisterCreator(fac, i3::identity< DesigAttr_BombTime >());
	RegisterCreator(fac, i3::identity< DesigAttr_BombRange >());
	RegisterCreator(fac, i3::identity< DesigAttr_WeaponReact >());

	RegisterCreator(fac, i3::identity< DesigAttr_DPDRate >());
	RegisterCreator(fac, i3::identity< DesigAttr_FirstShot >());
	RegisterCreator(fac, i3::identity< DesigAttr_WeaponDamage >());		// 쓰이지 않는 것처럼 보이지만 등록..
	RegisterCreator(fac, i3::identity< DesigAttr_ScoopeRange >());		// 쓰이지 않는 것처럼 보이지만 등록..
	RegisterCreator(fac, i3::identity< DesigAttr_CrossHairRestore>());	// 쓰이지 않는 것처럼 보이지만 등록..

	RegisterCreator(fac, i3::identity< DesigAttr_Purchase>());			// 상점아이템 구매 권한..
}


