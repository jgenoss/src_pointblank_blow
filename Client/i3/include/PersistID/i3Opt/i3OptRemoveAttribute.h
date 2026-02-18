#if !defined( __I3_OPT_REMOVE_ATTRIBUTE_H)
#define __I3_OPT_REMOVE_ATTRIBUTE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveAttribute : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptRemoveAttribute);
protected:
	i3ClassMeta *			m_pMeta;

public:
	i3OptRemoveAttribute(void);
	virtual ~i3OptRemoveAttribute(void);

	i3ClassMeta *		getClassMeta(void)					{ return m_pMeta; }
	void				setClassMeta( i3ClassMeta * pMeta)	{ m_pMeta = pMeta; }

	virtual BOOL OnNode( i3Node * pNode);
};

#endif
