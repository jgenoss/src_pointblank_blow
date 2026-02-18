#include "i3SceneDef.h"
#include "i3Node.h"
#include "i3AttrSet.h"
#include "i3Geometry.h"
#include "i3BoneRef.h"
#include "i3TimeSequence.h"
#include "i3BinaryPartitionNode.h"

#if defined( I3_DEBUG)

static char s_temp[512];

static void MakeDepthString( INT32 Level, char mark)
{
	INT32 len, i;

	s_temp[0] = 0;

	for( i = 0; i < Level; i++)
	{
		strncat( s_temp, "\t", sizeof(s_temp)-1);
	}

	len = strlen( s_temp);

	s_temp[ len] = mark;
	s_temp[ len + 1] = ' ';
	s_temp[ len + 2] = 0;
}

I3_EXPORT_SCENE
void i3SceneGraphDump( i3Node * pNode, INT32 Level, bool bDumpAttr)
{
	INT32 i;
	bool bDump = true;

	//I3TRACE( "ENTER > %s, pNode = %p, Level = %d\n", __FUNCTION__, pNode, Level );

	I3ASSERT( pNode != nullptr );
	i3ClassMeta * pMeta = pNode->meta();
	I3ASSERT( pMeta != nullptr );

	MakeDepthString( Level, '+');	

	if( pNode->hasName())
	{
		strncat( s_temp, pNode->GetName(), sizeof(s_temp)-1);
	}

	if( pNode->IsFlag( I3_NODEFLAG_DISABLE))
	{
		I3TRACE( "%s (%s) [%p, %d] (X)\n", s_temp, pMeta->class_name(), pNode, pNode->GetRefCount());
	}
	else
	{
		I3TRACE( "%s (%s) [%p, %d]\n", s_temp, pMeta->class_name(), pNode, pNode->GetRefCount());
	}

	if( i3::kind_of<i3AttrSet*>(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;
		
		if( bDumpAttr)
		{
			MakeDepthString( Level + 1, '-');

			for( i = 0; i < pAttrSet->GetAttrCount(); i++)
			{
				i3RenderAttr * pAttr = pAttrSet->GetAttr(i);

				I3TRACE( "%s (%s)\n", s_temp, pAttr->meta()->class_name());
			}
		}
	}
	else if( i3::kind_of<i3Geometry*>(pNode))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;

		if( bDumpAttr)
		{
			MakeDepthString( Level + 1, '-');

			for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
			{
				i3RenderAttr * pAttr = pGeo->GetGeometryAttr( i);

				I3TRACE( "%s (%s)\n", s_temp, pAttr->meta()->class_name());
			}
		}
	}
	else if( i3::same_of<i3TimeSequence*>(pNode))
	{
		i3TimeSequence * pTimeSeq = (i3TimeSequence *) pNode;

		for( i = 0; i < pTimeSeq->GetSequenceCount(); i++)
		{
			i3TimeSequenceInfo * pInfo = pTimeSeq->GetSequence( i);

			i3SceneGraphDump( pInfo->GetNode(), Level + 1, bDumpAttr);
		}

		bDump = false;
	}
	else if( i3::same_of<i3BinaryPartitionNode*>(pNode))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pNode;

		if( pBSP->getFront() != nullptr)
			i3SceneGraphDump( pBSP->getFront(), Level + 1, bDumpAttr);

		if( pBSP->getBack() != nullptr)
			i3SceneGraphDump( pBSP->getBack(), Level + 1, bDumpAttr);
	}


	//I3TRACE( "TRACE : Dump Children( %d )\n", pNode->GetChildCount() );
	if( bDump)
	{
		for( i = 0; i < pNode->GetChildCount(); i++)
		{
			i3SceneGraphDump( pNode->GetChild( i), Level + 1, bDumpAttr);
		}
	}

	//I3TRACE( "EXIT > %s, pNode = %p, Level = %d\n", __FUNCTION__, pNode, Level );
}
#endif
