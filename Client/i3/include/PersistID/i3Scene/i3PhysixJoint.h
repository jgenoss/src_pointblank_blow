#if !defined( __I3_PHYSIX_JOINT_H)
#define __I3_PHYSIX_JOINT_H

#if defined ( I3_PHYSX)

#include "i3PhysixDefine.h"
#include "i3PhysixUtil.h"
#include "i3PhysixJointInfo.h"

class i3Transform2;

class I3_EXPORT_SCENE i3PhysixJoint : public i3NamedElement
{
	I3_CLASS_DEFINE( i3PhysixJoint);

protected:
	// Persistant Members

	// Volatile Members
	NxJoint *			m_pJoint;
	i3PhysixJointInfo *	m_pInfo;
	
	i3Transform2 *		m_pParentBone;
	i3Transform2 *		m_pChildBone;

public:
	i3PhysixJoint(void);
	virtual ~i3PhysixJoint(void);

	// 초기 생성용 함수
	bool			CreateJoint( NxJointDesc * pDesc);

	// Loading시 생성용 함수
	// 일반적으로, 해당 i3PhysixJoint를 소유한 i3Transform2(pChildBone)에서
	// 이 함수를 사용해, Loading시 생성한다.
	bool			Create( i3Transform2 * pParentBone, i3Transform2 * pChildBone);

	NxJoint *		getJoint(void)								{ return m_pJoint; }

	i3PhysixJointInfo *		getJointInfo(void)					{ return m_pInfo; }
	void					setJointInfo( i3PhysixJointInfo * pInfo)
	{
		I3_REF_CHANGE( m_pInfo, pInfo);
	}

	i3Transform2 *	getParentBone(void)							{ return m_pParentBone; }
	void			setParentBone( i3Transform2 * pBone)		{ m_pParentBone = pBone; }

	i3Transform2 *	getChildBone(void)						{ return m_pChildBone; }
	void			setChildBone( i3Transform2 * pBone)		{ m_pChildBone = pBone; }

	void			Dump(void);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif

#endif
