#if !defined( __I3_OPT_BUILD_LOD_BONE_MAP_H)
#define __I3_OPT_BUILD_LOD_BONE_MAP_H

#include "i3SceneOptimizer.h"

//
// Scene-Graph에 존재하는 i3Body Node를 검색하여, 각 LOD에 대해 적절한 Bone Map Data
// 를 생성한다.
// Bone Map Data는 LOD 간에 Animation Data를 공유시키는데 있어, Performance에 크게
// 영향을 미칠 수 있다. Bone Map Data가 없는 경우에는 문자열 검색으로 해당 Bone을
// 찾기 때문이다.
//

class I3_EXPORT_OPT i3OptBuildLODBoneMap : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildLODBoneMap);
protected:

public:
	i3OptBuildLODBoneMap(void);
	virtual ~i3OptBuildLODBoneMap(void);

	virtual BOOL OnNode( i3Node * pNode);
};

#endif
