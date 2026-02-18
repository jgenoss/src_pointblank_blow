#if !defined( __I3_SHAPESET_CONTAINER_H)
#define __I3_SHAPESET_CONTAINER_H

#if defined ( I3_PHYSX)

#include "i3Node.h"
#include "i3PhysixShapeSet.h"

class I3_EXPORT_SCENE i3ShapeSetContainer : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3ShapeSetContainer, i3Node);
public:
	enum USAGE
	{
		USAGE_NA = 0,
		USAGE_SIMULATION,
		USAGE_RAYCAST,
		USAGE_TRIGGER
	};


protected:
	i3PhysixShapeSet *		m_pShapeSet = nullptr;
	USAGE					m_Usage = USAGE_NA;

public:
	i3ShapeSetContainer(void);
	virtual ~i3ShapeSetContainer(void);

	i3PhysixShapeSet *		getShapeSet(void)							{ return m_pShapeSet; }
	void					setShapeSet( i3PhysixShapeSet * pShapeSet)	{ I3_REF_CHANGE( m_pShapeSet, pShapeSet); }

	USAGE					getUsage(void)								{ return m_Usage; }
	void					setUsage( USAGE usage)						{ m_Usage = usage; if(m_pShapeSet!=nullptr)m_pShapeSet->setUsage((INT32)usage); }

	void					ModifyDynamicState( UINT32 addMask, UINT32 removeMask);

	virtual void			OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
	virtual void			OnInstancing( I3_ONINSTANCING_INFO * pInfo) override;
};

#endif

#endif
