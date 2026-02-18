#if !defined( __I3_GFX_VERTEX_BLENDING_H)
#define __I3_GFX_VERTEX_BLENDING_H

enum I3G_VERTEX_BLEND_DRAW
{
	NON_INDEXED_HW		=	0,		// Hardware가 지원하는 Non-Indexed Vertex Blending
	INDEXED_HW,						// Hardware가 지원하는 Indexed Vertex Blending
	NON_INDEXED_SW,					// Software가 처리하는 Non-Indexed Vertex Blending
	INDEXED_SW,						// Software로 처리하는 Indexed Vertex Blending
};

typedef struct _tagI3VertexBlendDrawInfo
{
	INT32						m_StartIndex;		// Primitive Group의 시작 Index
	INT32						m_PrimCount;		//
	I3G_VERTEX_BLEND_DRAW		m_Draw;
	UINT8						m_MatrixIndices[4];
} I3G_VERTEX_BLEND_DRAW_INFO;

I3_EXPORT_GFX
void _MakeRuntimeVertexArray( I3G_PRIMITIVE prim, i3VertexArray * pVA, i3IndexArray * pIA, i3List * pPrimLengthList);

#endif
