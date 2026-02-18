#if !defined( I3OPT_NODE_FLAG_H)
#define I3OPT_NODE_FLAG_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptNodeFlag : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptNodeFlag);

protected:
	UINT32		m_RemoveFlag;
	UINT32		m_AddFlag;

public:
	i3OptNodeFlag(void);
	virtual ~i3OptNodeFlag(void);

	UINT32		getAddFlag(void)					{	return m_AddFlag; }
	void		setAddFlag( UINT32 mask)			{ m_AddFlag = mask; }

	UINT32		getRemoveFlag(void)					{ return m_RemoveFlag; }
	void		setRemoveFlag( UINT32 mask)			{ m_RemoveFlag = mask; }

public:
	virtual BOOL	OnNode( i3Node * pNode);
};

#endif