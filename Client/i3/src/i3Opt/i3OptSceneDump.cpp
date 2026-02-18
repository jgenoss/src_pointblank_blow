#include "i3OptPCH.h"
#include "i3OptSceneDump.h"
#include <stdio.h>

I3_CLASS_INSTANCE( i3OptSceneDump, i3SceneOptimizer);

typedef struct _tagAnimInfo
{
	UINT32			m_KeyCount;
	REAL32			m_Duration;
	REAL32			m_Interval;
	VEC3D *			m_pTranslate;
	QUATERNION *	m_pRotation;
	VEC3D *			m_pScale;
} ANIM_INFO;

typedef struct _tagMeshInfo
{
	UINT32			m_PrimCount;
	VEC3D *			m_pPosition;
	VEC3D *			m_pNormal;
	VEC2D *			m_pTextureCoord;
} MESH_INFO;

typedef struct _tagNodeInfo
{
	// Parent Node
	struct _tagNodeInfo *	m_pParent;

	// First Child Node
	struct _tagNodeInfo *	m_pFirstChild;

	// Next
	struct _tagNodeInfo *	m_pNext;

	BOOL					m_bTransform;
	REAL32					m_Matrix[4][4];

	ANIM_INFO *				m_pAnim;
} NODE_INFO;

i3OptSceneDump::i3OptSceneDump(void)
{
	m_szPath[0] = 0;
}

i3OptSceneDump::~i3OptSceneDump(void)
{
}

void i3OptSceneDump::DumpHeader( i3Node * pRoot)
{
	i3List List;
	i3FileStream str;
	BOOL Rv;
	char szFull[MAX_PATH];
	char conv[1024];

	sprintf( szFull, "%s.h", m_szName);

	Rv = str.Create( szFull, STREAM_WRITE);
	if( Rv == FALSE)
	{
		i3Log( "i3OptSceneDump::DumpHeader()", "Could not create %s file.", szFull);
		return;
	}

	i3String::SplitFileName( m_szPath, szFull, FALSE);

	str.Printf( conv, "#if !defined( __%s_H)\n", szFull);
	str.Printf( conv, "#define __%s_H\n", szFull);
	str.Printf( conv, "\n");

	str.Printf( conv, "extern NODE_INFO * g_Scene_%s_%s");

	str.Printf( conv, "#endif\n");
	str.Printf( conv, "\n");

	str.Close();
}

BOOL i3OptSceneDump::OnNode( i3Node * pNode)
{
	if( pNode->IsTypeOf( i3Transform::GetClassMeta()))
	{
		i3TransformDump( (i3Transform *) pNode);
	}
	else if( pNode->IsTypeOf( i3Geometry::GetClassMeta()))
	{
		i3GeometryDump( (i3Geometry *) pNode);
	}
	else
	{
		i3NodeDump( pNode);
	}
	
	return TRUE;
}

void i3OptSceneDump::Trace( i3Node * pRoot)
{
	i3FileStream stream;
	BOOL Rv;

	i3String::Copy( m_szName, m_szPath);
	i3String::RemoveExt( m_szName);

	Rv = stream.Create( m_szPath, STREAM_WRITE);
	if( Rv == FALSE)
	{
		i3Log( "i3OptSceneDump::Trace()", "Could not create %s file.", m_szPath);
		return;
	}

	m_pStream = &stream;

	i3SceneOptimizer::Trace( pRoot);
}
