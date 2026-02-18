#if !defined( __I3_OPT_REMOVE_ATTRIBUTE_H)
#define __I3_OPT_REMOVE_ATTRIBUTE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveAttribute : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptRemoveAttribute, i3SceneOptimizer);
protected:
	i3ClassMeta *			m_pMeta;

public:
	i3OptRemoveAttribute(void);

	i3ClassMeta *		getClassMeta(void)					{ return m_pMeta; }
	void				setClassMeta( i3ClassMeta * pMeta)	{ m_pMeta = pMeta; }

	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif
