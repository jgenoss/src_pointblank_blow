#if !defined( __I3_REF_STACK_H)
#define __I3_REF_STACK_H

#include "itl/vector.h"

class i3ElementBase;

class I3_EXPORT_BASE i3RefStack 
{
public:
	i3RefStack(void);

	void			Push( i3ElementBase * pNode) {   m_list.push_back( pNode); }
	void			Pop( i3ElementBase ** pNode);
	void			Reset(void)							{ m_list.clear(); }
	i3ElementBase *	getTop(void) const	{ if (m_list.empty()) return nullptr;		return m_list.back(); 	}
	bool			IsEmpty(void) const { return m_list.empty(); } 
private:

	i3::vector<i3ElementBase*>	m_list;
};

#endif
