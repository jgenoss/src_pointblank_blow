#if !defined( __OPT_SAVE_TO_FILE_H)
#define __OPT_SAVE_TO_FILE_H

#include "i3SceneOptimizer.h"
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3OptSaveToFile : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptSaveToFile, i3SceneOptimizer);
	
public:
	i3OptSaveToFile(void);

	virtual void	Optimize( i3SceneGraphInfo * pSg) override;
	virtual const char *	getDescName(void) override;
};

#endif
