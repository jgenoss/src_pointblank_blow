#if !defined( __I3TRANSFORM_LINK_SHAPESET_H__)
#define __I3TRANSFORM_LINK_SHAPESET_H__

#include "i3Transform.h"
#include "i3PhysixShapeSet.h"

#if defined ( I3_PHYSX)
/** \brief Transform에 붙는 ShapeSet class (for TDK)
	\details ShapeSet의 World Matrix를 Transform에 업데이트 해준다.	*/
class I3_EXPORT_SCENE i3TransformLinkShapeSet : public i3Transform
{
	I3SG_NODE_CLASS_DEFINE( i3TransformLinkShapeSet);
protected:
	i3PhysixShapeSet *	m_pShapeSet;

public:
	/** \brief PhysX ShapeSet을 반환한다.
		\return i3PhysixShapeSet 객체 포인터 */
	i3PhysixShapeSet *	getShapeSet( void)								{ return m_pShapeSet; }

	/** \brief PhysX ShapeSet을 설정한다.
		\note pShapeSet의 RefCount를 증가시킨다.
		\param[in] pShapeSet i3PhysixShapeSet 객체 포인터
		\return N/A */
	void				setShapeSet( i3PhysixShapeSet * pShapeSet)
	{ 
		I3_REF_CHANGE( m_pShapeSet, pShapeSet); 
	}

public:
	i3TransformLinkShapeSet();
	virtual ~i3TransformLinkShapeSet();

	/** \brief Update
		\param[in] pTracer i3SceneTracer 객체 포인터
		\note ShapeSet의 World Matrix를 Transform의 Cache Matrix에 업데이트 해준다.
		\return N/A */
	virtual void	OnUpdate( i3SceneTracer * pTracer);
};

#endif
#endif
