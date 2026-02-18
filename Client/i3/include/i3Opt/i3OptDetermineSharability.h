#if !defined( __I3_OPT_DETERMINE_SHARABILITY_H)
#define __I3_OPT_DETERMINE_SHARABILITY_H

#include "i3SceneOptimizer.h"

/// i3OptDetermineSharability class는 직접 어떠한 Optimization을 수행하는
/// Class가 아니라, 주어진 Scene-Graph가 단순한 Pointer Referencing과 같은
/// 방법으로 공유될 수 있는 것인지, 그렇지 않으면 반드시 Clone을 생성해야
/// 하는 것인지를 판단하는 기능을 제공한다.
class I3_EXPORT_OPT i3OptDetermineSharability : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptDetermineSharability, i3SceneOptimizer);
protected:
	bool	m_bSharable = false;

public:
	i3OptDetermineSharability(void);

	bool			GetSharable(void)			{ return m_bSharable; }

	virtual void	Trace( i3Node * pNode) override;
};

#endif


