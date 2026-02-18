#if !defined( __WEAPON_SILENCER_H__)
#define __WEAPON_SILENCER_H__

#include "WeaponBase.h"

class WeaponSilencer : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponSilencer, WeaponBase );
protected:
	INT32				m_idxFireEffect_Silencer;	// 소음기 장착시 발사 이펙트 인덱스

	// 소음기 모델링에 대한 알파처리용
	i3::vector<i3Node*>	m_SilencerMaterialList;

protected:
	virtual void	_OnInitVairable( void) override;
	
public:
	WeaponSilencer();
	virtual ~WeaponSilencer();

	virtual void	PostCreate( void) override;
	virtual void	OnSetVisibleExtension( bool bVal) override;
	virtual void	_OnEmitFireEffect( void) override;
	virtual void	_OnPlayFireSound( void) override;

	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = 1) override;
	virtual void	OnProcessExtension( bool bVal) override;


	///////////////////////////////////////////////////////////////////////////////////////////
	// Animation Callback
	

};

#endif
