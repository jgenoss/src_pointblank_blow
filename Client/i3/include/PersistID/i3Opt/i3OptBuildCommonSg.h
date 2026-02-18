#if !defined( __OPT_BUILD_COMMON_SG_H)
#define __OPT_BUILD_COMMON_SG_H

#include "i3SceneOptimizer.h"

class i3OptBuildCommonSg : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildCommonSg);
protected:
	bool	m_bAddToInstanceSg;

protected:
	bool	_IsAllSameAttr( i3List * pList);
	
public:
	i3OptBuildCommonSg(void);
	virtual ~i3OptBuildCommonSg(void);

	bool			getAddToInstanceSgState(void)					{ return m_bAddToInstanceSg ;}
	void			setAddToInstanceSgState( bool bFlag)			{ m_bAddToInstanceSg = bFlag; }

	virtual void	Optimize( i3SceneGraphInfo * pSg);

	virtual const char *	getDesc(void);
};

#endif
