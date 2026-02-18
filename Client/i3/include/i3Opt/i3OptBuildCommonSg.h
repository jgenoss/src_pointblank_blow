#if !defined( __OPT_BUILD_COMMON_SG_H)
#define __OPT_BUILD_COMMON_SG_H

#include "i3SceneOptimizer.h"
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3OptBuildCommonSg : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildCommonSg, i3SceneOptimizer);
protected:
	bool	m_bAddToInstanceSg = false;

protected:
	bool	_IsAllSameAttr(const i3::vector<i3RenderAttr*>& List) const;
	
public:
	i3OptBuildCommonSg(void);
	virtual ~i3OptBuildCommonSg(void);

	bool			getAddToInstanceSgState(void)					{ return m_bAddToInstanceSg ;}
	void			setAddToInstanceSgState( bool bFlag)			{ m_bAddToInstanceSg = bFlag; }

	virtual void	Optimize( i3SceneGraphInfo * pSg) override;

	virtual const char *	getDesc(void) override;
};

#endif
