#if !defined( __I3_PHYSICS_IK_H )
#define __I3_PHYSICS_IK_H

class I3_EXPORT_PHYSICS i3PhysicsIK : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PhysicsIK );
protected:
	i3List		m_IKBoneList;

	BOOL		m_bDamping;			// Damping
	BOOL		m_bRestriction;		// restitution

	REAL32		m_Thresh;			// 삭제값(이하값은 계산하지 않는다.)

public:
	BOOL		getDamp( void )				{ return m_bDamping; }
	void		setEnableDamp( void )		{ m_bDamping = TRUE; }
	void		setDisableDamp( void )		{ m_bDamping = FALSE; }

	BOOL		getRestriction( void )		{ return m_bRestriction; }
	void		setEnableRestriction( void ){ m_bRestriction = TRUE; }
	void		setDisableRestriction(void)	{ m_bRestriction = FALSE; }

	REAL32		getThresh( void )			{ return m_Thresh; }
	void		setThresh( REAL32 thresh )	{ m_Thresh = thresh; }

	INT32		getBoneCount( void )		{ return m_IKBoneList.GetCount(); }
	i3PhysicsAdvanceBone *	getAdvanceBone( INT32 idx )		{ return (i3PhysicsAdvanceBone *) m_IKBoneList.Items[idx]; }
	i3Transform * getBone( INT32 idx )		{ return getAdvanceBone(idx)->getBone(); }
	i3Transform * getParentBone( INT32 idx ){ return getAdvanceBone(idx)->getParentBone(); }

public:
	i3PhysicsIK();
	virtual ~i3PhysicsIK();

	void Create();

	void AddBone( i3PhysicsAdvanceBone * pObj, i3Transform * pBone, i3Transform * pParent );
	void RemoveAllBone();

	INT32 _FindAdvanceBoneIndexByBoneAddress( i3Transform * pBoneAddress );
	i3PhysicsAdvanceBone * _FindAdvanceBoneByBoneAddress( i3Transform * pBoneAddress );

	// 2006. 1. 18 by dfly79
	// pEffectPoint : 영향을 받는 지점(힘을 받아 이동한 지점이 된다)
	// pEffectBone : 영향을 받는 EndBone
	void CalcCCDLink( VEC3D * pEffectPoint, i3Transform * pEffectBone );

	void StepSimulation( REAL32 dTime );
};

#endif // __I3_PHYSICS_IK_H
