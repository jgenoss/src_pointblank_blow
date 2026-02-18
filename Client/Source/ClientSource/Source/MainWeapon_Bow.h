#pragma once

#include "MainWeapon_Throw.h"

class MainWeapon_Bow : public MainWeapon_Throw
{
public:
	MainWeapon_Bow(WeaponBase* p);

	virtual void	OnEndFire(i3AIContext * pCtx, REAL32 tm) override;

	void			Transmission_Bow(bool bToWorld);
	void			PlayAnim_UI_IdleA_Bow();
	void			PlayAnim_UI_IdleB_Bow();
	void			PlayAnim_ChangeRun(bool Flag_3PV);
	void			PlayAnim_ChangeEnd();
	void			PlayAnim_ExtActivate3PV_Run();
	void			PlayAnim_ExtActivate3PV_End();
	void			PlayAnim_ExtDeActivate3PV_Run();
	void			PlayAnim_ExtDeActivate3PV_End();
	
	VEC3D *			getRightVec( void)										{ return &m_vRightDir; }
	
private:
	virtual void	OnCreate( bool bCreateInWeapon) override;
	virtual void	Reset() override;
	virtual void	OnBindResource() override;

	VEC3D			m_vRightDir;

	i3Transform *	m_pAttachObjectTransform;
	i3Object *		m_pAttachObject;
	REAL32			m_rBoundObjectLen;

	bool OnChangingFlag;
	bool OnActivateFlag;

	bool OnDeActivateFlag;

	INT8	m_aii_UI_Idle_A;
	INT8	m_aii_UI_Idle_B;
	INT8	m_aii_Change_1PV;
	INT8	m_aii_Change_3PV;
	INT8	m_aii_ExtensionActivate_3PV;
	INT8	m_aii_ExtensionDeactivate_3PV;


	bool			m_bExplosion;
	i3::vector<i3Node*> m_ArrowMaterialList;
};

