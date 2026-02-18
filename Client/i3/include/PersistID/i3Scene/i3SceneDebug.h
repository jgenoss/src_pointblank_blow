#if !defined( __I3_SCENE_DEBUG_H)
#define __I3_SCENE_DEBUG_H

#if defined( I3_DEBUG)
#include "i3Node.h"

I3_EXPORT_SCENE void i3SceneGraphDump( i3Node * pNode, INT32 Level, bool bDumpAttr = true);

#define I3_SG_DUMP( pNode)			i3SceneGraphDump( pNode, 0, true)

#endif
#endif
