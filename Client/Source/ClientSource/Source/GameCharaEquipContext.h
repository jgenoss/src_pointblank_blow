/**
 * \file	GameCharaEquipContext.h
 *
 * Declares the game chara equip context class.
 */


#if !defined( __GAME_CHARA_EQUIP_CONTEXT_H__)
#define __GAME_CHARA_EQUIP_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaEquipDef.h"
#include "GameCharaDef.h"

class CGameCharaEquipControl;
class CGameCharaEquipControl1PV;
class CGameCharaEquipControl3PV;

class CGameCharaBase;
class CGameCharaEquip;
class CEquipInfo;

class CGameCharaEquipContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaEquipContext, CGameCharaContextBase);

protected:
	CGameCharaEquipControl *		m_pEquipCurrentControl = nullptr;
	CGameCharaEquipControl1PV *		m_pEquipControl1PV = nullptr;
	CGameCharaEquipControl3PV *		m_pEquipControl3PV = nullptr;
	
	//	장비품
	CGameCharaEquip	*	m_pCharaEquip[ EQUIP::ePART_COUNT];		// [initialize at constructor]
	CEquipInfo *		m_pEquipInfo[ EQUIP::ePART_COUNT];		// [initialize at constructor]

	// 일부 장비는 BoneRef에 붙인다.
	i3BoneRef *			m_pDummy[ EQUIP::ePART_COUNT];			// [initialize at constructor]

	// 히트로 드랍된 경우 체크 (헬멧 프로텍션에서 판별하기 위한 용도)
	bool				m_bHelmetProtect = false;

	// 목표추적 핼멧 유무
	bool				m_bEnableTargetChaseHelmet = false;
	REAL32				m_rTargetChaseC5 = 0.0f;


public:
	/** \brief 장비를 반환합니다. */
	CGameCharaEquip *	getEquip( EQUIP::ePART part)	{ return m_pCharaEquip[ part]; }

	/** \brief 장비를 설정합니다. */
	void				setEquip( EQUIP::ePART part, CGameCharaEquip * pEquip)	{ m_pCharaEquip[ part] = pEquip; }

	/** \brief 장비의 정보를 반환합니다.
		\note 장착된 장비와 정보는 다를 수 있습니다.
			아바타 시스템에서 임의로 모델링을 바꿔줄 수도 있습니다. 하지만 정보는 서버의 정보를 사용할 수도 있습니다.*/
	const CEquipInfo *	getEquipInfo( EQUIP::ePART part)		{ return m_pEquipInfo[ part]; }

	/** \brief 장비의 정보를 설정합니다. */
	void				setEquipInfo( EQUIP::ePART part, CEquipInfo *pInfo)	
	{ 
		I3ASSERT_RETURN(part != EQUIP::ePART_UNKNOWN);
		m_pEquipInfo[ part] = pInfo; 
	}

	i3BoneRef *			getDummy(EQUIP::ePART part) const { I3ASSERT_RETURN(part >= 0, nullptr);  return m_pDummy[part]; }
	void				setDummy( EQUIP::ePART part, i3BoneRef * pNode)	{ I3_REF_CHANGE( m_pDummy[ part], pNode); }
	

	/** \brief 현재 캐릭터에 붙어 있는지 여부 */
	bool				isAttachOnChara( EQUIP::ePART part);

	/** \brief 머리 보호 여부를 반환합니다. */
	bool				getHelmetProtect( void)						{ return m_bHelmetProtect; }

	/** \brief 머리 보호 여부를 설정합니다. */
	void				setHelmetProtect( bool bVal)				{ m_bHelmetProtect = bVal; }

	/** \brief 목표 추적 기능 사용 여부를 반환합니다. */
	REAL32				getTargetChaseC5( void)						{ return m_rTargetChaseC5; }
	void				setTargetChaseC5( REAL32 range)				{ m_rTargetChaseC5 = range; }

	/** \brief 장비에 의한 능력치를 반환합니다. */
	REAL32				GetAbility( EQUIP::eABILITY ability);

	/** \brief 머리 보호율을 반환합니다. */
	REAL32				GetHeadDefenseRate();

	/** \brief 머리 보호 시간을 반환합니다. */
	REAL32				GetHeadProtectionTime();


public:
	CGameCharaEquipContext();
	virtual ~CGameCharaEquipContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV( void) override;
	virtual void	OnChange3PV( void) override;

	virtual void	OnChangeLocal( void) override;
	virtual void	OnChangeNetwork( void) override;
	virtual void	OnChangeNPC( void) override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief 장비를 월드에 버린다.
		\param[in] type 장비 타입
		\param[in] pVecDir 버려지는 방향 */
	void			DropToWorld( EQUIP::ePART type, VEC3D * pVecDir);

	/** \brief 장비의 색상을 설정 */
	void			SetMaterialColor( EQUIP::ePART part, COLORREAL * pDiff, COLORREAL * pSpec, bool bOnChara);

	/** \brief 장비의 알파값을 설정한다.
		\param[in] type 장비 타입
		\param[in] fAlpha alpha value
		\param[in] bOnChara 캐릭터에 부착 여부 */
	void			SetBlendAlpha( EQUIP::ePART type, REAL32 fAlpha, bool bOnChara);

	/** \brief 장비를 제거하고 메모리에서 삭제합니다.
		\param[in] type 장비 타입*/
	void			ReleaseEquip( EQUIP::ePART type);

	/** \brief 헬멧을 생성합니다.
		\param[in] charaResType Resource type
		\param[in] EquipID Resource id*/
	void			CreateEquip( CHARA_RES_ID charaResType, T_ItemID ItemID, INT32 res_subtype);

	/** \brief 장비를 버립니다.
		\param[in] type 장비 클래스 타입
		\param[in] pSceneNode 새로 붙여질 scene  node
		\param[in] pGameNode 새로 붙여질 game node
		\param[in] pVecPos 장비 위치*/
	bool			DropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos);

	/** \brief 장비를 캐릭터에 붙입니다.
		\param[in] type 장비 클래스 타입
		\param[in] bReset 장비 초기화 여부 */
	bool			Attach( EQUIP::ePART type, bool bReset = true);

	/** \brief 장비를 띄어 냅니다.
		\note 월드나 캐릭터에서 띄어내게 됩니다.
		\param[in] type 장비 클래스 타입 */
	bool			Detach( EQUIP::ePART type);

	void			CreateEquipmentDummy( void);

private:
	void			__SetDummyBoneRef(const char* szBoneName, i3BoneRef* destBoneRef);
};

#endif
