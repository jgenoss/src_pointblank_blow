#include "i3OptPCH.h"
#include "i3Gfx.h"
#include "i3OptBuildPSPCommandList.h"

//I3_CLASS_INSTANCE( i3OptBuildPSPCommandList, i3SceneOptimizer);
I3_CLASS_INSTANCE( i3OptBuildPSPCommandList);


/* display list mode */
#define SCEGU_IMMEDIATE                    0
#define SCEGU_CALL                         1
#define SCEGU_LIST                         2

/* CALL mode */
#define SCEGU_NORMAL_CALL				   0
#define SCEGU_SIGNAL_CALL				   1

/* Matrix Type */
#define SCEGU_MATRIX_PROJECTION            0
#define SCEGU_MATRIX_VIEW                  1
#define SCEGU_MATRIX_WORLD                 2
#define SCEGU_MATRIX_TEXTURE               3

/* Sync Type */
#define SCEGU_SYNC_FINISH                  0
#define SCEGU_SYNC_SIGNAL                  1
#define SCEGU_SYNC_END                     2
#define SCEGU_SYNC_LIST                    3
#define SCEGU_SYNC_SENDLIST                4

/* Sync Mode */
#define SCEGU_SYNC_WAIT                    0
#define SCEGU_SYNC_NOWAIT                  1

/* Function Name */
#define SCEGU_ALPHA_TEST                   0
#define SCEGU_DEPTH_TEST                   1
#define SCEGU_SCISSOR_TEST                 2
#define SCEGU_STENCIL_TEST                 3
#define SCEGU_BLEND                        4
#define SCEGU_CULL_FACE                    5
#define SCEGU_DITHER                       6
#define SCEGU_FOG                          7
#define SCEGU_CLIP_PLANE                   8
#define SCEGU_TEXTURE                      9
#define SCEGU_LIGHTING                    10
#define SCEGU_LIGHT0                      11
#define SCEGU_LIGHT1                      12
#define SCEGU_LIGHT2                      13
#define SCEGU_LIGHT3                      14
#define SCEGU_AALINE                      15
#define SCEGU_CULL_PATCH                  16
#define SCEGU_COLOR_TEST                  17
#define SCEGU_LOGICAL_OPERATION           18
#define SCEGU_NORMAL_REVERSE_PRIM         19
#define SCEGU_NORMAL_REVERSE_PATCH        20
#define SCEGU_COLOR_DOUBLE                21

/* Test Function */
#define SCEGU_NEVER                        0
#define SCEGU_ALWAYS                       1
#define SCEGU_EQUAL                        2
#define SCEGU_NOTEQUAL                     3
#define SCEGU_LESS                         4
#define SCEGU_LEQUAL                       5
#define SCEGU_GREATER                      6
#define SCEGU_GEQUAL                       7

/* Stencil Operation */
#define SCEGU_KEEP                         0
#define SCEGU_ZERO                         1
#define SCEGU_REPLACE                      2
#define SCEGU_INVERT                       3
#define SCEGU_INCR                         4
#define SCEGU_DECR                         5

/* Blend Operation */
#define SCEGU_ADD                          0 /* ( Cs @ A ) + ( Cd @ B ) */
#define SCEGU_SUBTRACT                     1 /* ( Cs @ A ) - ( Cd @ B ) */
#define SCEGU_REVERSE_SUBTRACT             2 /* ( Cd @ B ) - ( Cs @ A ) */
#define SCEGU_MIN                          3 /* min( Cs, Cd )         */
#define SCEGU_MAX                          4 /* max( Cs, Cd )         */
#define SCEGU_ABS                          5 /* abs( Cs - Cd )        */

/* Blend Function */
#define SCEGU_SRC_COLOR                    0
#define SCEGU_ONE_MINUS_SRC_COLOR          1
#define SCEGU_DST_COLOR                    0
#define SCEGU_ONE_MINUS_DST_COLOR          1
#define SCEGU_SRC_ALPHA                    2
#define SCEGU_ONE_MINUS_SRC_ALPHA          3
#define SCEGU_DST_ALPHA                    4
#define SCEGU_ONE_MINUS_DST_ALPHA          5
#define SCEGU_DOUBLE_SRC_ALPHA             6
#define SCEGU_ONE_MINUS_DOUBLE_SRC_ALPHA   7
#define SCEGU_DOUBLE_DST_ALPHA             8
#define SCEGU_ONE_MINUS_DOUBLE_DST_ALPHA   9
#define SCEGU_FIX_VALUE                   10

/* Logical Operation */
#define SCEGU_LOP_CLEAR                    0
#define SCEGU_LOP_AND                      1
#define SCEGU_LOP_AND_REVERSE              2
#define SCEGU_LOP_COPY                     3
#define SCEGU_LOP_AND_INVERTED             4
#define SCEGU_LOP_NOOP                     5
#define SCEGU_LOP_XOR                      6
#define SCEGU_LOP_OR                       7
#define SCEGU_LOP_NOR                      8
#define SCEGU_LOP_EQUIV                    9
#define SCEGU_LOP_INVERT                  10
#define SCEGU_LOP_OR_REVERSE              11
#define SCEGU_LOP_COPY_INVERTED           12
#define SCEGU_LOP_OR_INVERTED             13
#define SCEGU_LOP_NAND                    14
#define SCEGU_LOP_SET                     15

/* Face Mode */
#define SCEGU_CW                           0
#define SCEGU_CCW                          1

/* Shading Type */
#define SCEGU_FLAT                         0
#define SCEGU_SMOOTH                       1

/* Light Type */
#define SCEGU_AMBIENT                      1
#define SCEGU_DIFFUSE                      2
#define SCEGU_SPECULAR                     4
#define SCEGU_AMBIENT_AND_DIFFUSE          3
#define SCEGU_DIFFUSE_AND_SPECULAR         6
#define SCEGU_POWERED_DIFFUSE              8

#define SCEGU_LIGHT_DIRECTION              0
#define SCEGU_LIGHT_POINT                  1
#define SCEGU_LIGHT_SPOT                   2

/* Texture Multi Clut */
#define SCEGU_SINGLE_CLUT                  0
#define SCEGU_MULTI_CLUT                   1

/* Texture Pixel Format */
#define SCEGU_PF5650                       0
#define SCEGU_PF5551                       1
#define SCEGU_PF4444                       2
#define SCEGU_PF8888                       3
#define SCEGU_PFIDX4                       4
#define SCEGU_PFIDX8                       5
#define SCEGU_PFIDX16                      6
#define SCEGU_PFIDX32                      7
#define	SCEGU_PFDXT1                       8
#define	SCEGU_PFDXT3                       9
#define	SCEGU_PFDXT5                      10
#define	SCEGU_PFDXT1EXT                  264
#define	SCEGU_PFDXT3EXT                  265
#define	SCEGU_PFDXT5EXT                  266

/* Texture Color Component */
#define SCEGU_RGB                          0
#define SCEGU_RGBA                         1

/* Texture Function */
#define SCEGU_TEX_MODULATE                 0
#define SCEGU_TEX_DECAL                    1
#define SCEGU_TEX_BLEND                    2
#define SCEGU_TEX_REPLACE                  3
#define SCEGU_TEX_ADD                      4

/* Texture Filter Mode */
#define SCEGU_NEAREST                      0
#define SCEGU_LINEAR                       1
#define SCEGU_NEAREST_MIPMAP_NEAREST       4
#define SCEGU_LINEAR_MIPMAP_NEAREST        5
#define SCEGU_NEAREST_MIPMAP_LINEAR        6
#define SCEGU_LINEAR_MIPMAP_LINEAR         7

/* Texture Wrap Mode */
#define SCEGU_REPEAT                       0
#define SCEGU_CLAMP                        1

/* Texture Map Mode */
#define SCEGU_UV_MAP                       0
#define SCEGU_PROJECTION_MAP               1
#define SCEGU_SHADE_MAP                    2

/* Texture Projection Map Mode */
#define SCEGU_PROJ_XYZ                     0
#define SCEGU_PROJ_UV                      1
#define SCEGU_PROJ_NORMALIZE_NORMAL        2
#define SCEGU_PROJ_NORMAL                  3

/* Texture Level Mode */
#define SCEGU_LOD_AUTO                     0
#define SCEGU_LOD_CONSTANT                 1
#define SCEGU_LOD_SLOPE                    2

/* Texture Buffer Mode */
#define SCEGU_TEXBUF_NORMAL                0
#define SCEGU_TEXBUF_FAST                  1

/* Clear Mask */
#define SCEGU_CLEAR_COLOR             0x0001
#define SCEGU_CLEAR_STENCIL           0x0002
#define SCEGU_CLEAR_DEPTH             0x0004
#define SCEGU_CLEAR_ALL               0x0007
#define SCEGU_CLEAR_FAST              0x0010
#define SCEGU_CLEAR_FASTALL           (SCEGU_CLEAR_FAST|SCEGU_CLEAR_ALL)

/* Primitive Type */
#define SCEGU_PRIM_POINTS                  0
#define SCEGU_PRIM_LINES                   1
#define SCEGU_PRIM_LINE_STRIP              2
#define SCEGU_PRIM_TRIANGLES               3
#define SCEGU_PRIM_TRIANGLE_STRIP          4
#define SCEGU_PRIM_TRIANGLE_FAN            5
#define SCEGU_PRIM_RECTANGLES              6

/* Spline Parameter */
#define SCEGU_CLOSE_CLOSE                  0
#define SCEGU_OPEN_CLOSE                   1
#define SCEGU_CLOSE_OPEN                   2
#define SCEGU_OPEN_OPEN                    3

/* Sprite Flip Type */
#define SCEGU_NOFLIP                       0
#define SCEGU_FLIP_U                       1
#define SCEGU_FLIP_V                       2
#define SCEGU_FLIP_UV                      3
#define SCEGU_NOROTATE                     0
#define SCEGU_ROTATE_90                    1
#define SCEGU_ROTATE_180                   2
#define SCEGU_ROTATE_270                   3

/* Vertex Type */
#define SCEGU_TEXTURE_NONE       ( 0 <<  0 )
#define SCEGU_TEXTURE_UBYTE      ( 1 <<  0 )
#define SCEGU_TEXTURE_USHORT     ( 2 <<  0 )
#define SCEGU_TEXTURE_FLOAT      ( 3 <<  0 )

#define SCEGU_COLOR_NONE         ( 0 <<  2 )
#define SCEGU_COLOR_PF5650       ( 4 <<  2 )
#define SCEGU_COLOR_PF5551       ( 5 <<  2 )
#define SCEGU_COLOR_PF4444       ( 6 <<  2 )
#define SCEGU_COLOR_PF8888       ( 7 <<  2 )

#define SCEGU_NORMAL_NONE        ( 0 <<  5 )
#define SCEGU_NORMAL_BYTE        ( 1 <<  5 )
#define SCEGU_NORMAL_SHORT       ( 2 <<  5 )
#define SCEGU_NORMAL_FLOAT       ( 3 <<  5 )

#define SCEGU_VERTEX_NONE        ( 0 <<  7 )
#define SCEGU_VERTEX_BYTE        ( 1 <<  7 )
#define SCEGU_VERTEX_SHORT       ( 2 <<  7 )
#define SCEGU_VERTEX_FLOAT       ( 3 <<  7 )

#define SCEGU_WEIGHT_NONE        ( 0 <<  9 )
#define SCEGU_WEIGHT_UBYTE       ( 1 <<  9 )
#define SCEGU_WEIGHT_USHORT      ( 2 <<  9 )
#define SCEGU_WEIGHT_FLOAT       ( 3 <<  9 )

#define SCEGU_INDEX_NONE         ( 0 << 11 )
#define SCEGU_INDEX_UBYTE        ( 1 << 11 )
#define SCEGU_INDEX_USHORT       ( 2 << 11 )

#define SCEGU_WEIGHT_1           ( 0 << 14 )
#define SCEGU_WEIGHT_2           ( 1 << 14 )
#define SCEGU_WEIGHT_3           ( 2 << 14 )
#define SCEGU_WEIGHT_4           ( 3 << 14 )
#define SCEGU_WEIGHT_5           ( 4 << 14 )
#define SCEGU_WEIGHT_6           ( 5 << 14 )
#define SCEGU_WEIGHT_7           ( 6 << 14 )
#define SCEGU_WEIGHT_8           ( 7 << 14 )

#define SCEGU_VERTEX_1           ( 0 << 18 )
#define SCEGU_VERTEX_2           ( 1 << 18 )
#define SCEGU_VERTEX_3           ( 2 << 18 )
#define SCEGU_VERTEX_4           ( 3 << 18 )
#define SCEGU_VERTEX_5           ( 4 << 18 )
#define SCEGU_VERTEX_6           ( 5 << 18 )
#define SCEGU_VERTEX_7           ( 6 << 18 )
#define SCEGU_VERTEX_8           ( 7 << 18 )

#define SCEGU_THROUGH            ( 1 << 23 )

/* Lignt Mode */
#define SCEGU_SINGLE_COLOR                 0
#define SCEGU_SEPARATE_SPECULAR_COLOR      1
#define SCEGU_EYE_FIX                      0
#define SCEGU_EYE_OBJECT                   1

/* Degree to Radian */
#define SCEGU_PI                           3.1415926535897932384626433832f
#define	SCEGU_RAD(a)                       ((a)*SCEGU_PI/180.0f)

/* Signal Mode */
#define SCEGU_SIGNAL_WAIT		1
#define SCEGU_SIGNAL_NOWAIT		2
#define SCEGU_SIGNAL_PAUSE		3

/* Interrupt Id */
#define SCEGU_INT_SIGNAL		1
#define SCEGU_INT_END			2
#define SCEGU_INT_FINISH		4

/* Queue type */
#define SCEGU_QUEUE_TAIL		0
#define SCEGU_QUEUE_HEAD		1

/* Break mode */
#define SCEGU_BREAK_PAUSE		0
#define SCEGU_BREAK_CANCEL		1

i3OptBuildPSPCommandList::i3OptBuildPSPCommandList(void)
{
	m_Class = CLASS_SCENEGRAPH;
}

i3OptBuildPSPCommandList::~i3OptBuildPSPCommandList(void)
{
	I3_SAFE_RELEASE( m_pAttrStack);
	I3_SAFE_RELEASE( m_pModeViewMatrixAttrPool);

	RemoveAllBBox();
}

void i3OptBuildPSPCommandList::PutWorldMatrix( i3PSPCommandBuffer * pBuffer, MATRIX * pMatrix)
{
	if( i3Matrix::IsSame( &m_LastWorldMatrix, pMatrix))
		return;

	i3Matrix::Copy( &m_LastWorldMatrix, pMatrix);

	pBuffer->Add( i3PSPGfx::WORLDN( 0));

	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get11( pMatrix)));
	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get12( pMatrix)));
	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get13( pMatrix)));

	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get21( pMatrix)));
	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get22( pMatrix)));
	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get23( pMatrix)));

	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get31( pMatrix)));
	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get32( pMatrix)));
	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get33( pMatrix)));

	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get41( pMatrix)));
	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get42( pMatrix)));
	pBuffer->Add( i3PSPGfx::WORLDD( i3Matrix::Get43( pMatrix)));
}

void i3OptBuildPSPCommandList::AddBBox( i3Node * pNode, UINT32	Addr, UINT32 count)
{
	OPTPSPCMD_BBOXINFO * pNew;

	//INT32 hMem;
	//hMem = i3MemAlloc( sizeof(OPTPSPCMD_BBOXINFO));
	pNew = (OPTPSPCMD_BBOXINFO *) i3MemAlloc( sizeof(OPTPSPCMD_BBOXINFO));
	I3ASSERT( pNew != nullptr);

	pNew->m_pNode = pNode;
	pNew->m_Addr = Addr;
	pNew->m_Count = count;

	m_BBoxList.push_back( pNew);
}

OPTPSPCMD_BBOXINFO * i3OptBuildPSPCommandList::FindBBox( i3Node * pNode)
{
	OPTPSPCMD_BBOXINFO * pBox;

	for(size_t i = 0; i < m_BBoxList.size(); i++)
	{
		pBox = m_BBoxList[i];

		if( pBox->m_pNode == pNode)
			return pBox;
	}

	return nullptr;
}

void i3OptBuildPSPCommandList::RemoveAllBBox(void)
{
	
	OPTPSPCMD_BBOXINFO * pBox;

	for(size_t i = 0; i < m_BBoxList.size(); i++)
	{
		pBox = m_BBoxList[i];

		i3MemFree( pBox);
	}

	m_BBoxList.clear();
}

UINT32 i3OptBuildPSPCommandList::GetPSPVertexFormat( i3VertexFormat * pFormat)
{
	UINT32 Format = 0;
	static UINT32 s_BlendWeightTable[] =
	{
		0,
		SCEGU_WEIGHT_1,
		SCEGU_WEIGHT_2,
		SCEGU_WEIGHT_3,
		SCEGU_WEIGHT_4,
		SCEGU_WEIGHT_5,
		SCEGU_WEIGHT_6,
		SCEGU_WEIGHT_7,
		SCEGU_WEIGHT_8,
	};

	if( pFormat->GetHasPosition())
	{
		switch( pFormat->GetPositionType())
		{
			case I3VF_TYPE_REAL32 :			Format |= SCEGU_VERTEX_FLOAT;	break;
			case I3VF_TYPE_FIXED16_15 :		Format |= SCEGU_VERTEX_SHORT;	break;
			case I3VF_TYPE_FIXED8_7 :		Format |= SCEGU_VERTEX_BYTE;	break;
			default :
				i3Log( "i3PSPUT_GetNativeVertexFormat()", "Invalid Position Format : %d\n", pFormat->GetPositionType());
				break;
		}
	}

	if( pFormat->GetHasNormal())
	{
		switch( pFormat->GetNormalType())
		{
			case I3VF_TYPE_REAL32 :			Format |= SCEGU_NORMAL_FLOAT;	break;
			case I3VF_TYPE_FIXED16_15 :		Format |= SCEGU_NORMAL_SHORT;	break;
			case I3VF_TYPE_FIXED8_7 :		Format |= SCEGU_NORMAL_BYTE;	break;
			default :
				i3Log( "i3PSPUR_GetNativeVertexFormat()", "Invalid Normal Format : %d\n", pFormat->GetNormalType());
				break;
		}
	}

	if( pFormat->GetHasColor())
	{
		switch( pFormat->GetColorType())
		{
			case I3VF_TYPE_COLOR32_RGBA_8888 :	Format |= SCEGU_COLOR_PF8888;	break;
			case I3VF_TYPE_COLOR16_RGBA_1555 :	Format |= SCEGU_COLOR_PF5551;	break;
			case I3VF_TYPE_COLOR16_RGBX_565 :	Format |= SCEGU_COLOR_PF5650;	break;
			case I3VF_TYPE_COLOR16_RGBA_4444 :	Format |= SCEGU_COLOR_PF4444;	break;
			default :
				i3Log( "i3PSPUT_GetNativeVertexFormat()", "Invalid Color Format : %d\n", pFormat->GetColorType());
				break;
		}
	}

	if( pFormat->GetTextureCoordSetCount() > 0) 
	{
		switch( pFormat->GetTextureCoordType())
		{
			case I3VF_TYPE_REAL32 :				Format |= SCEGU_TEXTURE_FLOAT;		break;
			case I3VF_TYPE_FIXED16_15_U :
			case I3VF_TYPE_FIXED16_15 :			Format |= SCEGU_TEXTURE_USHORT;		break;
			case I3VF_TYPE_FIXED8_7_U :
			case I3VF_TYPE_FIXED8_7 :			Format |= SCEGU_TEXTURE_UBYTE;		break;
			default :
				i3Log( "i3PSPUT_GetNativeVertexFormat()", "Invalid Texture Coord. format : %d\n", pFormat->GetTextureCoordType());
				break;
		}
	}

	if( pFormat->GetBlendWeightCount() > 0)
	{
		I3ASSERT( pFormat->GetBlendWeightCount() <= 8);

		switch( pFormat->GetBlendWeightType())
		{
			case I3VF_TYPE_REAL32 :				Format |= SCEGU_WEIGHT_FLOAT;		break;
			case I3VF_TYPE_FIXED16_15 :			Format |= SCEGU_WEIGHT_USHORT;		break;
			case I3VF_TYPE_FIXED8_7 :			Format |= SCEGU_WEIGHT_UBYTE;		break;
			default :
				i3Log( "i3PSPUT_GetNativeVertexFormat()", "Invalid Blend weight format : %d\n", pFormat->GetBlendWeightType());
				break;
		}

		Format |= s_BlendWeightTable[ (int)pFormat->GetBlendWeightCount()];
	}

	if( pFormat->IsTransformed())
	{
		Format |= SCEGU_THROUGH;
	}

	return Format;
}

void i3OptBuildPSPCommandList::UpdateTextureStageState( i3PSPCommandBuffer * pBuffer, bool bIsVertexColor )
{
	INT32 TexFunc[2][2][2] =
	{
		// Light Off
		{
			// Texture Off
			{
				// Vertex Color Off
				// TEC
				i3PSPGfx::TFUNC_BLEND,

				// Vertex Color On
                // Diffuse * TEC
				// No Available Method. Use Diffuse
				i3PSPGfx::TFUNC_MODULATE
			},
			// Texture On
			{
				// Vertex Color Off
				// Texture * TEC
				//i3PSPGfx::TFUNC_BLEND, // <= 이게 맞음. 그러나, 일부 월드가 이상하게 나오고 기대했던 효과와 차이가 많이 남. -_-;
				i3PSPGfx::TFUNC_MODULATE,

				// Vertex Color On
				// TEC * Diffuse
				i3PSPGfx::TFUNC_MODULATE
			},
		},
		// Light On
		{
			// Texture Off
			{
				// Vertex Color Off
				// Diffuse
				i3PSPGfx::TFUNC_MODULATE,

				// Vertex Color On
				// Diffuse
				i3PSPGfx::TFUNC_MODULATE
			},
			// Texture On
			{
				// Vertex Color Off
				// Texture * Diffuse
				i3PSPGfx::TFUNC_MODULATE,

				// Vertex Color On
				//Texture * Diffuse
				i3PSPGfx::TFUNC_MODULATE
			},
		}
	};

	m_TexFunc = TexFunc[ m_bLightingEnable ][ m_bTextureEnable ][ bIsVertexColor ];

	//if( m_TexFunc == m_OldTexFunc )
	//	return;	

	pBuffer->Add( i3PSPGfx::TFUNC( i3PSPGfx::TFUNC_TXF( m_TexFunc ), i3PSPGfx::TFUNC_RGBA, FALSE ) );

	m_OldTexFunc = m_TexFunc;
}

void i3OptBuildPSPCommandList::DrawPrim( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr)
{
	int vert_count, idx;
	i3PSPGfx::PRIM_TYPE	type;

	switch( pGeoAttr->GetPrimitiveType())
	{
		default :						
			type = i3PSPGfx::PRIM_POINTLIST;
			vert_count = pGeoAttr->GetPrimitiveCount();
			break;

		case I3G_PRIM_LINELIST :
			type = i3PSPGfx::PRIM_LINELIST;
			vert_count = pGeoAttr->GetPrimitiveCount() << 1;
			break;

		case I3G_PRIM_LINESTRIP :
			type = i3PSPGfx::PRIM_LINESTRIP;
			vert_count = pGeoAttr->GetPrimitiveCount() + 1;	
			break;

		case I3G_PRIM_TRILIST :			
			type = i3PSPGfx::PRIM_TRILIST;
			vert_count = pGeoAttr->GetPrimitiveCount() * 3;
			break;

		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :	
			type = i3PSPGfx::PRIM_TRISTRIP;

			if( pGeoAttr->IsPrimitiveLength())
				vert_count = pGeoAttr->GetVertexArray()->GetCount();
			else
                vert_count = pGeoAttr->GetPrimitiveCount() + 2;	
			break;
	}

	// Vertex Array에 대한 Pointer는 PSP Runtime Loading시에
	// 재설정된다.
	idx = m_pGPUCmdList->AddResource( pGeoAttr->GetVertexArray());

	UpdateTextureStageState( pBuffer, pGeoAttr->GetVertexArray()->GetFormat()->GetHasColor());

	pBuffer->Add( i3PSPGfx::BASE( (void *)(idx << 16)));
	pBuffer->Add( i3PSPGfx::VADR( (void *)(idx << 16)));
	pBuffer->Add( i3PSPGfx::VTYPE( GetPSPVertexFormat( pGeoAttr->GetVertexArray()->GetFormat())));
	pBuffer->Add( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) type, (UINT16) vert_count));

	m_DrawCount ++;
}

void i3OptBuildPSPCommandList::DrawStripPrim( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr)
{
	UINT32 vert_count, i;
	UINT32 PrimCount;
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	UINT32 offset, val;
	INT32 idx;
	i3PSPGfx::PRIM_TYPE	type;

	idx = m_pGPUCmdList->AddResource( pGeoAttr->GetVertexArray());
	offset = (UINT32) (pGeoAttr->GetStartIndex() * pVA->GetStride());

	for( i = 0; i < pGeoAttr->GetPrimitiveCount(); i++)
	{
		PrimCount = pGeoAttr->GetPrimitiveLength( i);

		switch( pGeoAttr->GetPrimitiveType())
		{
			default :
				type = i3PSPGfx::PRIM_POINTLIST;
				vert_count = PrimCount;	
				break;

			case I3G_PRIM_LINELIST :
				type = i3PSPGfx::PRIM_LINELIST;
				vert_count = (PrimCount) << 1;
				break;

			case I3G_PRIM_LINESTRIP :
				type = i3PSPGfx::PRIM_LINESTRIP;
				vert_count = (PrimCount) + 1;
				break;

			case I3G_PRIM_TRILIST :
				type = i3PSPGfx::PRIM_TRILIST;
				vert_count = (PrimCount) * 3;
				break;

			case I3G_PRIM_TRISTRIP :
			case I3G_PRIM_TRIFAN :			
				type = i3PSPGfx::PRIM_TRISTRIP;
				vert_count = (PrimCount) + 2;			
				break;

		}

		UpdateTextureStageState( pBuffer, pVA->GetFormat()->GetHasColor());

		val = (idx << 16) | ((offset >> 2) & 0xFFFF);		// VADR의 offset은 4의 배수로 지정한다.

		pBuffer->Add( i3PSPGfx::BASE( (void *) val));
		pBuffer->Add( i3PSPGfx::VADR( (void *) val));
		pBuffer->Add( i3PSPGfx::VTYPE( GetPSPVertexFormat( pVA->GetFormat())));
		pBuffer->Add( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) type, (UINT16) vert_count));

		offset += (vert_count * pVA->GetStride());
	}

	m_DrawStripCount ++;
}

void i3OptBuildPSPCommandList::DrawIndexedPrim( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr)
{
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();;
	i3IndexArray * pIA = pGeoAttr->GetIndexArray();
	UINT32 vert_count;
	UINT32 PrimCount = pGeoAttr->GetPrimitiveCount();
	INT32 vidx, iidx;
	i3PSPGfx::PRIM_TYPE type;

	switch( pGeoAttr->GetPrimitiveType())
	{
		default :
			type = i3PSPGfx::PRIM_POINTLIST;
			vert_count = PrimCount;
			break;

		case I3G_PRIM_LINELIST :
			type = i3PSPGfx::PRIM_LINELIST;
			vert_count = PrimCount << 1;
			break;

		case I3G_PRIM_LINESTRIP :
			type = i3PSPGfx::PRIM_LINESTRIP;
			vert_count = PrimCount + 1;	
			break;

		case I3G_PRIM_TRILIST :
			type = i3PSPGfx::PRIM_TRILIST;
			vert_count = PrimCount * 3;
			break;

		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :			
			type = i3PSPGfx::PRIM_TRISTRIP;
			vert_count = PrimCount + 2;					
			break;
	}

	UpdateTextureStageState( pBuffer, pVA->GetFormat()->GetHasColor());

	vidx = m_pGPUCmdList->AddResource( pVA);
	iidx = m_pGPUCmdList->AddResource( pIA);

	pBuffer->Add( i3PSPGfx::BASE( (void *) (vidx << 16)));
	pBuffer->Add( i3PSPGfx::VADR( (void *) (vidx << 16)));

	pBuffer->Add( i3PSPGfx::BASE( (void *) (iidx << 16)));
	pBuffer->Add( i3PSPGfx::IADR( (void *) (iidx << 16)));

	pBuffer->Add( i3PSPGfx::VTYPE( GetPSPVertexFormat( pVA->GetFormat())  | SCEGU_INDEX_USHORT));

	pBuffer->Add( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) type, (UINT16) vert_count));

	m_DrawIndexedCount++;
}

void i3OptBuildPSPCommandList::DrawStripIndexedPrim( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr)
{
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	UINT32 vert_count, i;
	INT32 vidx, iidx;
	UINT32 offset, val;
	i3PSPGfx::PRIM_TYPE type;

	vidx = m_pGPUCmdList->AddResource( pVA);
	iidx = m_pGPUCmdList->AddResource( pGeoAttr->GetIndexArray());
	offset = 0;

	for( i = 0; i < pGeoAttr->GetPrimitiveCount(); i++)
	{
		switch( pGeoAttr->GetPrimitiveType())
		{
			default :
				type = i3PSPGfx::PRIM_POINTLIST;
				vert_count = pGeoAttr->GetPrimitiveLength( i);	
				break;

			case I3G_PRIM_LINELIST :
				type = i3PSPGfx::PRIM_LINELIST;
				vert_count = pGeoAttr->GetPrimitiveLength( i) << 1;
				break;

			case I3G_PRIM_LINESTRIP :
				type = i3PSPGfx::PRIM_LINESTRIP;
				vert_count = pGeoAttr->GetPrimitiveLength( i) + 1;
				break;

			case I3G_PRIM_TRILIST :
				type = i3PSPGfx::PRIM_TRILIST;
				vert_count = pGeoAttr->GetPrimitiveLength( i) * 3;
				break;

			case I3G_PRIM_TRISTRIP :
			case I3G_PRIM_TRIFAN :
				type = i3PSPGfx::PRIM_TRISTRIP;
				vert_count = pGeoAttr->GetPrimitiveLength( i) + 2;
				break;
		}

		UpdateTextureStageState( pBuffer, pVA->GetFormat()->GetHasColor());

		pBuffer->Add( i3PSPGfx::BASE( (void *) (vidx << 16)));
		pBuffer->Add( i3PSPGfx::VADR( (void *) (vidx << 16)));

		val = (iidx << 16) | (offset & 0xFFFF);		// Index Array인 경우 Offset은 1 byte 단위다.

		pBuffer->Add( i3PSPGfx::BASE( (void *) val));
		pBuffer->Add( i3PSPGfx::IADR( (void *) val));

		pBuffer->Add( i3PSPGfx::VTYPE( GetPSPVertexFormat( pVA->GetFormat())) | SCEGU_INDEX_USHORT);

		pBuffer->Add( i3PSPGfx::PRIM( (i3PSPGfx::PRIM_TYPE) type, (UINT16) vert_count));

		offset += (vert_count << 1);
	}

	m_DrawIndexedStripCount++;
}

void i3OptBuildPSPCommandList::DrawPointSprite(i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
UINT32	i3OptBuildPSPCommandList::Oni3AlphaFuncAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3AlphaFuncAttr * pAttr)
{
	i3PSPGfx::ATEST_FUNC func;

	switch( pAttr->GetFunction())
	{
		case I3G_COMP_NEVER :		func = i3PSPGfx::ATEST_NEVER;	break;
		case I3G_COMP_LESS	:		func = i3PSPGfx::ATEST_LESS;	break;
		case I3G_COMP_EQUAL	:		func = i3PSPGfx::ATEST_EQUAL;	break;
		case I3G_COMP_LEQUAL	:	func = i3PSPGfx::ATEST_LEQUAL;	break;
		case I3G_COMP_GREATER	:	func = i3PSPGfx::ATEST_GREATER;	break;
		case I3G_COMP_NOTEQUAL	:	func = i3PSPGfx::ATEST_NOTEQUAL;	break;
		case I3G_COMP_GEQUAL	:	func = i3PSPGfx::ATEST_GEQUAL;	break;
		default					:	func = i3PSPGfx::ATEST_ALWAYS;	break;
	}

	pBuffer->Add( i3PSPGfx::ATEST( (i3PSPGfx::ATEST_FUNC) func, pAttr->GetRefValue(), 0xFF));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3AlphaTestEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3AlphaTestEnableAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::ATE( pAttr->Get()));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3BillboardAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BillboardAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3BlendEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BlendEnableAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::ABE( pAttr->Get()));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3BlendModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BlendModeAttr * pAttr)
{
	I3G_BLEND src = pAttr->GetSource();
	I3G_BLEND dst = pAttr->GetDestination();
	I3G_BLEND_OP op = pAttr->GetBlendOperation();
	i3PSPGfx::BLEND_A a;
	i3PSPGfx::BLEND_B b = (i3PSPGfx::BLEND_B) dst;
	i3PSPGfx::BLEND_EQU equ = (i3PSPGfx::BLEND_EQU) op;
	UINT32	Rc = 1;

	switch( src)
	{
		case I3G_BLEND_SRCCOLOR		:	a = (i3PSPGfx::BLEND_A) 0;	break;
		case I3G_BLEND_INVSRCCOLOR	:	a = (i3PSPGfx::BLEND_A) 1;	break;
		case I3G_BLEND_SRCALPHA		:	a = (i3PSPGfx::BLEND_A) 2;	break;
		case I3G_BLEND_INVSRCALPHA	:	a = (i3PSPGfx::BLEND_A) 3;	break;
		case I3G_BLEND_DESTALPHA	:	a = (i3PSPGfx::BLEND_A) 4;	break;
		case I3G_BLEND_INVDESTALPHA	:	a = (i3PSPGfx::BLEND_A) 5;	break;
		case I3G_BLEND_DESTCOLOR	:	a = (i3PSPGfx::BLEND_A) 0;	break;
		case I3G_BLEND_INVDESTCOLOR	:	a = (i3PSPGfx::BLEND_A) 1;	break;
		case I3G_BLEND_ZERO			:	a = (i3PSPGfx::BLEND_A) 20;	break;
		case I3G_BLEND_ONE			:	a = (i3PSPGfx::BLEND_A) 21;	break;
		default						:	a = (i3PSPGfx::BLEND_A) 2;	break;
	}

	switch( dst)
	{
		case I3G_BLEND_SRCCOLOR		:	b = (i3PSPGfx::BLEND_B) 0;	break;
		case I3G_BLEND_INVSRCCOLOR	:	b = (i3PSPGfx::BLEND_B) 1;	break;
		case I3G_BLEND_SRCALPHA		:	b = (i3PSPGfx::BLEND_B) 2;	break;
		case I3G_BLEND_INVSRCALPHA	:	b = (i3PSPGfx::BLEND_B) 3;	break;
		case I3G_BLEND_DESTALPHA	:	b = (i3PSPGfx::BLEND_B) 4;	break;
		case I3G_BLEND_INVDESTALPHA	:	b = (i3PSPGfx::BLEND_B) 5;	break;
		case I3G_BLEND_DESTCOLOR	:	b = (i3PSPGfx::BLEND_B) 0;	break;
		case I3G_BLEND_INVDESTCOLOR	:	b = (i3PSPGfx::BLEND_B) 1;	break;
		case I3G_BLEND_ZERO			:	b = (i3PSPGfx::BLEND_B) 20;	break;
		case I3G_BLEND_ONE			:	b = (i3PSPGfx::BLEND_B) 21;	break;
		default						:	b = (i3PSPGfx::BLEND_B) 2;	break;
	}

	switch( op)
	{
		case I3G_BLEND_OP_NONE			:	equ = (i3PSPGfx::BLEND_EQU) -1;	break;
		case I3G_BLEND_OP_ADD			:	equ = (i3PSPGfx::BLEND_EQU) 0;	break;
		case I3G_BLEND_OP_SUBTRACT		:	equ = (i3PSPGfx::BLEND_EQU) 1;	break;
		case I3G_BLEND_OP_REVSUBTRACT	:	equ = (i3PSPGfx::BLEND_EQU) 2;	break;
		case I3G_BLEND_OP_MIN			:	equ = (i3PSPGfx::BLEND_EQU) 3;	break;
		default							:	equ = (i3PSPGfx::BLEND_EQU) 4;	break;
	}

	// 두가지 경우에 대해서만 제대로 동작하도록 임시로 설정.
	// src = I3G_BLEND_ONE, dst = I3G_BLEND_ZERO, op = I3G_BLEND_OP_ADD
	// src = I3G_BLEND_SRCALPHA, dst = I3G_BLEND_ONE_MINUS_SRCALPHA, op = I3G_BLEND_OP_ADD

	if( src == I3G_BLEND_ONE && dst == I3G_BLEND_ZERO && op == I3G_BLEND_OP_ADD )
	{
		pBuffer->Add( i3PSPGfx::FIXA( 255, 255, 255 ) );
		pBuffer->Add( i3PSPGfx::FIXB( 0, 0, 0 ) );
		a = i3PSPGfx::BLEND_A_FIXA;
		b = i3PSPGfx::BLEND_B_FIXB;

		Rc += 2;
	}
	else if( src == I3G_BLEND_SRCALPHA && dst == I3G_BLEND_INVSRCALPHA && op == I3G_BLEND_OP_ADD )
	{
	}
	else
	{
		a = (i3PSPGfx::BLEND_A) 2;
		b = (i3PSPGfx::BLEND_B) 3;
		equ = (i3PSPGfx::BLEND_EQU) 0;
	}

	pBuffer->Add( i3PSPGfx::BLEND( a, b, equ ) );

	return Rc;
}

UINT32	i3OptBuildPSPCommandList::Oni3ClearRenderTargetAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ClearRenderTargetAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3ClippingEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ClippingEnableAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::CLE( pAttr->Get()));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3ColorAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ColorAttr * pAttr)
{
	COLOR col;

	i3Color::Set( &col, pAttr->Get());

	// CMD_MAC와 CMD_MAA를 이용한다.
	pBuffer->Add( i3PSPGfx::MAC( i3Color::GetR( &col), i3Color::GetG( &col), i3Color::GetB( &col)));
	pBuffer->Add( i3PSPGfx::MAA( i3Color::GetA( &col)));

	return 2;
}

/*
UINT32	i3OptBuildPSPCommandList::Oni3EdgeAntiAliasEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3EdgeAntiAliasEnableAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::AAE( pAttr->Get()));
	
	return 1;
}
*/

UINT32	i3OptBuildPSPCommandList::Oni3FaceCullModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FaceCullModeAttr * pAttr)
{
	if( pAttr->Get() == I3G_FACE_CULL_NONE)
	{
		pBuffer->Add( i3PSPGfx::BCE( FALSE));
		return 1;
	}

	pBuffer->Add( i3PSPGfx::BCE( TRUE));

	switch( pAttr->Get())
	{
		case I3G_FACE_CULL_BACK :	pBuffer->Add( i3PSPGfx::CULL( i3PSPGfx::CULL_CCW));	break;
		default :					pBuffer->Add( i3PSPGfx::CULL( i3PSPGfx::CULL_CW));	break;
	}

	return 2;
}

UINT32	i3OptBuildPSPCommandList::Oni3FogColorAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogColorAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::FC( i3Color::GetR( pAttr->Get()), i3Color::GetG( pAttr->Get()), i3Color::GetB( pAttr->Get())));
	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3FogDensityAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogDensityAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3FogEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogEnableAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::FGE( pAttr->Get()));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3FogModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogModeAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3FogNearFarAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogNearFarAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3GeometryAttr( i3OptBuildPSPCommandList *pOpt, i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pAttr)
{
	i3VertexArray * pVA = pAttr->GetVertexArray();
	i3IndexArray * pIA = pAttr->GetIndexArray();
	UINT32 oldCount = pBuffer->GetCount();

	if( pVA->GetBlendWeightCount() > 0)
	{
		//pContext->SetVertexBlendWeightCount( m_pVA->GetBlendWeightCount());
	}

	if( pAttr->IsPrimitiveLength() == false)
	{
		// Strip | Fan이 아니다.
		if( pIA == nullptr)
			pOpt->DrawPrim( pBuffer, pAttr);
		else
			pOpt->DrawIndexedPrim( pBuffer, pAttr);
	}
	else
	{
		// Strip 또는 Fan 이다.
		if( pIA == nullptr)
		{
			if( pAttr->GetPrimitiveCount() > 1)
				pOpt->DrawStripPrim( pBuffer, pAttr);
			else
				pOpt->DrawPrim( pBuffer, pAttr);
		}
		else
		{
			if( pAttr->GetPrimitiveCount() > 1)
				pOpt->DrawStripIndexedPrim( pBuffer, pAttr);
			else
				pOpt->DrawIndexedPrim( pBuffer, pAttr);
		}
	}

	if( pVA->GetBlendWeightCount() > 0)
	{
		//pContext->SetVertexBlendWeightCount( 0);
	}

	return pBuffer->GetCount() - oldCount;
}

UINT32	i3OptBuildPSPCommandList::Oni3LightAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3LightAttr * pAttr)
{
	i3Light * pLight = pAttr->GetLightObject();
	UINT32 oldCount = pBuffer->GetCount();
	UINT32 idx = 0;

	{
		COLOR col;

		// Enable
		/*switch( idx)
		{
			case 1 :	pBuffer->Add( i3PSPGfx::LE1( TRUE));	break;
			case 2 :	pBuffer->Add( i3PSPGfx::LE2( TRUE));	break;
			case 3 :	pBuffer->Add( i3PSPGfx::LE3( TRUE));	break;
			default :	pBuffer->Add( i3PSPGfx::LE0( TRUE));	break;
		}*/

		pBuffer->Add( i3PSPGfx::LE0( TRUE));

		// Light Colors
		{
			// Ambient
			/*
			i3Color::Set( &col, pLight->GetAmbient());

			pBuffer->Add( i3PSPGfx::LAC( (UINT8) idx, 
						i3Color::GetR( &col),
						i3Color::GetG( &col),
						i3Color::GetB( &col)))
						*/

			// Diffuse
			i3Color::Set( &col, pLight->GetDiffuse());

			pBuffer->Add( i3PSPGfx::LDC( (UINT8) idx, 
						i3Color::GetR( &col),
						i3Color::GetG( &col),
						i3Color::GetB( &col)));

			// Specular
			i3Color::Set( &col, pLight->GetSpecular());

			pBuffer->Add( i3PSPGfx::LSC( (UINT8) idx, 
						i3Color::GetR( &col),
						i3Color::GetG( &col),
						i3Color::GetB( &col)));
		}

		// Spot & Point Light
		switch( pLight->GetType())
		{
			case I3G_LIGHT_SPOT :
			case I3G_LIGHT_POINT :
				{
					// Direction
					pBuffer->Add( i3PSPGfx::LDX( (UINT8) idx, i3Vector::GetX( pLight->GetWorldSpaceDirection())));
					pBuffer->Add( i3PSPGfx::LDY( (UINT8) idx, i3Vector::GetY( pLight->GetWorldSpaceDirection())));
					pBuffer->Add( i3PSPGfx::LDZ( (UINT8) idx, i3Vector::GetZ( pLight->GetWorldSpaceDirection())));

					// Attenuation
					/*
					pBuffer->Add( i3PSPGfx::LKA( (UINT8) idx, i3Vector::GetX( pAtt)));
					pBuffer->Add( i3PSPGfx::LKB( (UINT8) idx, i3Vector::GetY( pAtt)));
					pBuffer->Add( i3PSPGfx::LKC( (UINT8) idx, i3Vector::GetZ( pAtt)));
					*/

					// Cut-off angle
					//pBuffer->Add( i3PSPGfx::LKO( (UINT8) idx, i3Math::cos( pLight->GetInnerAngle())));

					// Fall-off rate
					//pBuffer->Add( i3PSPGfx::LKS( (UINT8) idx, pLight->GetFalloffRate()));
				}
				break;

			default :
				break;
		}

		switch( pLight->GetLightType())
		{
			case I3G_LIGHT_DIRECTIONAL :
				pBuffer->Add( i3PSPGfx::LTYPE( (UINT8) idx, i3PSPGfx::LTYPE_DIFFUSE, i3PSPGfx::LIGHT_DIRECTIONAL));
				break;

			case I3G_LIGHT_SPOT :
				pBuffer->Add( i3PSPGfx::LTYPE( (UINT8) idx, i3PSPGfx::LTYPE_DIFFUSE, i3PSPGfx::LIGHT_SPOT));
				break;

			case I3G_LIGHT_POINT :
				pBuffer->Add( i3PSPGfx::LTYPE( (UINT8) idx, i3PSPGfx::LTYPE_DIFFUSE, i3PSPGfx::LIGHT_POINT));
				break;
		}
		
	}

	return pBuffer->GetCount() - oldCount;
}

UINT32	i3OptBuildPSPCommandList::Oni3LightingEnableAttr( i3OptBuildPSPCommandList * pOpt, i3PSPCommandBuffer * pBuffer, i3LightingEnableAttr * pAttr)
{
	pOpt->m_bLightingEnable = pAttr->Get();
	pBuffer->Add( i3PSPGfx::LTE( pAttr->Get()));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3MaterialAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3MaterialAttr * pAttr)
{
	COLOR D, S, E;

	//i3Color::Set( &A, pAttr->GetAmbient());
	i3Color::Set( &D, pAttr->GetDiffuse());
	i3Color::Set( &S, pAttr->GetSpecular());
	i3Color::Set( &E, pAttr->GetEmissive());

	// Ambient
	//Buffer->Add( i3PSPGfx::MAC( i3Color::GetR( &A),	i3Color::GetG( &A), i3Color::GetB( &A)));

	// Diffuse
	pBuffer->Add( i3PSPGfx::MDC( i3Color::GetR( &D), i3Color::GetG( &D), i3Color::GetB( &D)));

	// Specular
	pBuffer->Add( i3PSPGfx::MSC( i3Color::GetR( &S), i3Color::GetG( &S), i3Color::GetB( &S)));

	// Emissive
	pBuffer->Add( i3PSPGfx::MEC( i3Color::GetR( &E), i3Color::GetG( &E), i3Color::GetB( &E)));

	// Shininess
	pBuffer->Add( i3PSPGfx::MK( pAttr->GetShininess()));

	return 5;
}

UINT32	i3OptBuildPSPCommandList::Oni3ModelViewMatrixAttr( i3OptBuildPSPCommandList *pInstance, i3PSPCommandBuffer * pBuffer, i3ModelViewMatrixAttr * pAttr)
{
	MATRIX * pMatrix = pAttr->GetMatrix();

	pInstance->PutWorldMatrix( pBuffer, pMatrix);

	return 13;
}

UINT32	i3OptBuildPSPCommandList::Oni3PointSpriteAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3PointSpriteAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3ProjectMatrixAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ProjectMatrixAttr * pAttr)
{
	MATRIX * pMatrix = pAttr->GetMatrix();

	pBuffer->Add( i3PSPGfx::PROJN( 0));

	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get11( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get12( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get13( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get14( pMatrix)));

	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get21( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get22( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get23( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get24( pMatrix)));

	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get31( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get32( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get33( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get34( pMatrix)));

	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get41( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get42( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get43( pMatrix)));
	pBuffer->Add( i3PSPGfx::PROJD( i3Matrix::Get44( pMatrix)));

	return 17;
}

UINT32	i3OptBuildPSPCommandList::Oni3QuadAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3QuadAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3RenderAttrListAttr( i3OptBuildPSPCommandList *pOpt, i3PSPCommandBuffer * pBuffer, i3RenderAttrListAttr * pAttr)
{
	return pOpt->MakePSPCommandList( pBuffer, pAttr->GetAttrList());
}

UINT32	i3OptBuildPSPCommandList::Oni3RenderModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3RenderModeAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3RenderTargetAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3RenderTargetAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3ShadeModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ShadeModeAttr * pAttr)
{
	switch( pAttr->Get())
	{
		case I3G_SHADE_FLAT :		pBuffer->Add( i3PSPGfx::SHADE( i3PSPGfx::SHADE_FLAT));	break;
		default :					pBuffer->Add( i3PSPGfx::SHADE( i3PSPGfx::SHADE_GOURAUD));	break;
	}

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3SpriteAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3SpriteAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::Oni3StencilFuncAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3StencilFuncAttr * pAttr)
{
	i3PSPGfx::STEST_FUNC func;

	switch( pAttr->GetFunction())
	{
		case I3G_COMP_NEVER		:	func = i3PSPGfx::STEST_NEVER;	break;
		case I3G_COMP_LESS		:	func = i3PSPGfx::STEST_LESS;	break;
		case I3G_COMP_EQUAL		:	func = i3PSPGfx::STEST_EQUAL;	break;
		case I3G_COMP_LEQUAL	:	func = i3PSPGfx::STEST_LEQUAL;	break;
		case I3G_COMP_GREATER	:	func = i3PSPGfx::STEST_GREATER;	break;
		case I3G_COMP_NOTEQUAL	:	func = i3PSPGfx::STEST_NOTEQUAL;	break;
		case I3G_COMP_GEQUAL	:	func = i3PSPGfx::STEST_GEQUAL;	break;
		default					:	func = i3PSPGfx::STEST_ALWAYS;	break;
	}

	pBuffer->Add( i3PSPGfx::STEST( func, pAttr->GetRefValue(), pAttr->GetWriteMask()));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3StencilOperationAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3StencilOperationAttr * pAttr)
{
	i3PSPGfx::SOP_MODE sfail, zfail, spass;

	switch ( pAttr->GetSFail())
	{
		case I3G_STENCIL_OP_KEEP		:	sfail = i3PSPGfx::SOP_KEEP;		break;
		case I3G_STENCIL_OP_ZERO		:	sfail = i3PSPGfx::SOP_ZERO;		break;
		case I3G_STENCIL_OP_REPLACE		:	sfail = i3PSPGfx::SOP_REPLACE;	break;
		case I3G_STENCIL_OP_INC_CLAMP	:	sfail = i3PSPGfx::SOP_INCR;		break;
		case I3G_STENCIL_OP_DEC_CLAMP	:	sfail = i3PSPGfx::SOP_DECR;		break;
		case I3G_STENCIL_OP_INVERT		:	sfail = i3PSPGfx::SOP_INVERT;	break;
		case I3G_STENCIL_OP_INC			:	sfail = i3PSPGfx::SOP_INCR;		break;
		default							:	sfail = i3PSPGfx::SOP_DECR;		break;
	}

	switch ( pAttr->GetSPass())
	{
		case I3G_STENCIL_OP_KEEP		:	spass = i3PSPGfx::SOP_KEEP;		break;
		case I3G_STENCIL_OP_ZERO		:	spass = i3PSPGfx::SOP_ZERO;		break;
		case I3G_STENCIL_OP_REPLACE		:	spass = i3PSPGfx::SOP_REPLACE;	break;
		case I3G_STENCIL_OP_INC_CLAMP	:	spass = i3PSPGfx::SOP_INCR;		break;
		case I3G_STENCIL_OP_DEC_CLAMP	:	spass = i3PSPGfx::SOP_DECR;		break;
		case I3G_STENCIL_OP_INVERT		:	spass = i3PSPGfx::SOP_INVERT;	break;
		case I3G_STENCIL_OP_INC			:	spass = i3PSPGfx::SOP_INCR;		break;
		default							:	spass = i3PSPGfx::SOP_DECR;		break;
	}

	switch ( pAttr->GetZFail())
	{
		case I3G_STENCIL_OP_KEEP		:	zfail = i3PSPGfx::SOP_KEEP;		break;
		case I3G_STENCIL_OP_ZERO		:	zfail = i3PSPGfx::SOP_ZERO;		break;
		case I3G_STENCIL_OP_REPLACE		:	zfail = i3PSPGfx::SOP_REPLACE;	break;
		case I3G_STENCIL_OP_INC_CLAMP	:	zfail = i3PSPGfx::SOP_INCR;		break;
		case I3G_STENCIL_OP_DEC_CLAMP	:	zfail = i3PSPGfx::SOP_DECR;		break;
		case I3G_STENCIL_OP_INVERT		:	zfail = i3PSPGfx::SOP_INVERT;	break;
		case I3G_STENCIL_OP_INC			:	zfail = i3PSPGfx::SOP_INCR;		break;
		default							:	zfail = i3PSPGfx::SOP_DECR;		break;
	}

	pBuffer->Add( i3PSPGfx::SOP( sfail, zfail, spass));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3StencilTestEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3StencilTestEnableAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::STE( pAttr->Get()));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextAttr * pAttr)
{
	return 0;
}

UINT32	i3OptBuildPSPCommandList::GetNativeImageFormat( I3G_IMAGE_FORMAT format)
{
	switch( format & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB))
	{
	// 4 bit formats
	case I3G_IMAGE_FORMAT_I4		:	return SCEGU_PFIDX4;

	// 8bit formats
	case I3G_IMAGE_FORMAT_I8		:	return SCEGU_PFIDX8;
	case i3G_IMAGE_FORMAT_L8		:	return SCEGU_PFIDX8;
	case i3G_IMAGE_FORMAT_A8		:	return SCEGU_PFIDX8;

	// 16 bit formats
	case I3G_IMAGE_FORMAT_RGB_565	:	return SCEGU_PF5650;
	case I3G_IMAGE_FORMAT_XRGB_1555	:	return SCEGU_PF5551;
	case I3G_IMAGE_FORMAT_ARGB_1555	:	return SCEGU_PF5551;
	case I3G_IMAGE_FORMAT_XRGB_4444	:	return SCEGU_PF4444;
	case I3G_IMAGE_FORMAT_ARGB_4444	:	return SCEGU_PF4444;
	case I3G_IMAGE_FORMAT_ABGR_4444	:	return SCEGU_PF4444;

	// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888	:	return SCEGU_PF8888;
	case I3G_IMAGE_FORMAT_BGR_888	:	return SCEGU_PF8888; // D3DFMT_B8G8R8;

	// 32 bit formats
	case I3G_IMAGE_FORMAT_RGBX_8888	:	return SCEGU_PF8888;
	case I3G_IMAGE_FORMAT_BGRX_8888	:	return SCEGU_PF8888; // D3DFMT_X8B8G8R8;
	case I3G_IMAGE_FORMAT_RGBA_8888	:	return SCEGU_PF8888;
	case I3G_IMAGE_FORMAT_BGRA_8888	:	return SCEGU_PF8888;

	case I3G_IMAGE_FORMAT_DXT1 :		return 8;
	case I3G_IMAGE_FORMAT_DXT3 :		return 9;
	case I3G_IMAGE_FORMAT_DXT5 :		return 10;

	default :
	#if defined( I3_DEBUG)
		i3Log( "i3GfxUtilDX", "[I3ERROR] Unsupported raster format : %c\n", format);
	#endif
		break;
	}

	return 0;
}

inline UINT32 getLog2( UINT32 val)
{
	int rv = 0;

	for(; val > 1; val >>= 1)
	{
		rv ++;
	}

	return rv;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureBindAttr( i3OptBuildPSPCommandList * pOpt, i3PSPCommandBuffer * pBuffer, i3TextureBindAttr * pAttr)
{
	i3Texture * pTexture = pAttr->GetTexture();
	UINT32 oldCount = pBuffer->GetCount();

	if( pTexture != nullptr)
	{
		UINT32 i, tbw, lnSz, idx;

		idx = pOpt->m_pGPUCmdList->AddResource( pTexture);

		pBuffer->Add( i3PSPGfx::TFLUSH());

		if( pTexture->IsIndexed())
		{	// 인덱스 컬러일 경우 CLUT 로드
			i3Clut * pClut = pTexture->GetClut(0);

			pBuffer->Add( i3PSPGfx::CBP( (UINT32) pClut));
			pBuffer->Add( i3PSPGfx::CBW( (UINT32) pClut));

			pBuffer->Add( i3PSPGfx::CLUT( (i3PSPGfx::CLUT_FORMAT)SCEGU_PF8888, 0, 0xFF, 0 ) );
			pBuffer->Add( i3PSPGfx::CLOAD( pClut->GetColorCount() >> 3));
		}

		UINT32 level = pTexture->GetLevelCount() - 1;

		i3PSPGfx::TMODE_HSM hsm = ( pTexture->GetFormat() & I3G_IMAGE_FORMAT_MASK_PSP ) ? (i3PSPGfx::TMODE_HIGHSPEED) : (i3PSPGfx::TMODE_NORMAL);
		i3PSPGfx::TMODE_MC mc = ( pTexture->GetLevelCount() > 1) ? (i3PSPGfx::TMODE_MULTI_CLUT) : (i3PSPGfx::TMODE_SINGLE_CLUT);

		// Texture Mode
		pBuffer->Add( i3PSPGfx::TMODE( hsm, mc, level ) );

		// Texture Pixel Format
		pBuffer->Add( i3PSPGfx::TPF( (i3PSPGfx::TEX_FORMAT) pOpt->GetNativeImageFormat( pTexture->GetFormat())));

		UINT32 w = getLog2( pTexture->GetWidth());
		UINT32 h = getLog2( pTexture->GetHeight());
		UINT32 bitscount = i3Gfx::GetImageFormatBitCount( pTexture->GetFormat());

		for( i = 0; i < pTexture->GetLevelCount(); i++)
		{
			lnSz = i3Gfx::CalcRasterBufferSize( (2 << w), 1, 4, pTexture->GetFormat());
			tbw = ((lnSz << 3) + bitscount - 1) / bitscount;

			pBuffer->Add( i3PSPGfx::TBP( (UINT8) i, (UINT32) idx));
			pBuffer->Add( i3PSPGfx::TBW( (UINT8) i, (UINT32) idx, tbw));

			pBuffer->Add( i3PSPGfx::TSIZE( (UINT8) i, (UINT8) w, (UINT8) h));
			w--;
			h--;
		}
	}

	return pBuffer->GetCount() - oldCount;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureCoordSourceAttr( i3OptBuildPSPCommandList * pOpt, i3PSPCommandBuffer * pBuffer, i3TextureCoordSourceAttr * pAttr)
{
	i3PSPGfx::TMAP_TMI tmi;

	switch( pAttr->GetMode())
	{
		case I3G_TEXTURE_COORD_SOURCE_VERTEX					: tmi = i3PSPGfx::TMAP_TMI_UV;	break;	// SCEGU_PROJ_UV,
		case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION		: tmi = i3PSPGfx::TMAP_TMI_XYZ;	break;	// SCEGU_PROJ_XYZ,
		case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL		: tmi = i3PSPGfx::TMAP_TMI_NORMAL;	break;	// SCEGU_PROJ_NORMALIZE_NORMAL,
		default													: tmi = i3PSPGfx::TMAP_TMI_DENORMAL;	break;	// SCEGU_PROJ_NORMAL,
	}

	if( pOpt->m_bProjectiveTexture)
	{
		pBuffer->Add( i3PSPGfx::TMAP(	i3PSPGfx::TMAP_TMN_PROJECTION, tmi));
	}
	else
	{
		pBuffer->Add( i3PSPGfx::TMAP(	i3PSPGfx::TMAP_TMN_UV, tmi));
	}

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureEnableAttr( i3OptBuildPSPCommandList * pOpt, i3PSPCommandBuffer * pBuffer, i3TextureEnableAttr * pAttr)
{
	pOpt->m_bTextureEnable = pAttr->Get();
	pBuffer->Add( i3PSPGfx::TME( pAttr->Get()));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureFilterAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureFilterAttr * pAttr)
{
	i3PSPGfx::TFILTER_MODE minfilter, maxfilter;

	switch( pAttr->GetMinFilter())
	{
		case I3G_TEXTURE_FILTER_NONE				:	minfilter = (i3PSPGfx::TFILTER_MODE) -1;	break;
		case I3G_TEXTURE_FILTER_POINT				:	minfilter = i3PSPGfx::TFILTER_NEAREST;	break;
		case I3G_TEXTURE_FILTER_LINEAR				:	minfilter = i3PSPGfx::TFILTER_LINEAR;	break;
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT	:	minfilter = i3PSPGfx::TFILTER_NEAREST_MIPMAP_NEAREST;	break;
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR	:	minfilter = i3PSPGfx::TFILTER_NEAREST_MIPMAP_LINEAR;	break;
		case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT	:	minfilter = i3PSPGfx::TFILTER_LINEAR_MIPMAP_NEAREST;	break;
		default										:	minfilter = i3PSPGfx::TFILTER_LINEAR_MIPMAP_LINEAR;		break;
	}

	switch( pAttr->GetMagFilter())
	{
		case I3G_TEXTURE_FILTER_NONE				:	maxfilter = (i3PSPGfx::TFILTER_MODE) -1;	break;
		case I3G_TEXTURE_FILTER_POINT				:	maxfilter = i3PSPGfx::TFILTER_NEAREST;	break;
		case I3G_TEXTURE_FILTER_LINEAR				:	maxfilter = i3PSPGfx::TFILTER_LINEAR;	break;
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT	:	maxfilter = i3PSPGfx::TFILTER_NEAREST_MIPMAP_NEAREST;	break;
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR	:	maxfilter = i3PSPGfx::TFILTER_NEAREST_MIPMAP_LINEAR;	break;
		case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT	:	maxfilter = i3PSPGfx::TFILTER_LINEAR_MIPMAP_NEAREST;	break;
		default										:	maxfilter = i3PSPGfx::TFILTER_LINEAR_MIPMAP_LINEAR;		break;
	}

	//pBuffer->Add( i3PSPGfx::TFILTER( minfilter, maxfilter));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureFunctionAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureFunctionAttr * pAttr)
{
	i3PSPGfx::TFUNC_TXF func;

	switch( pAttr->GetFunction())
	{
		case I3G_TEXTURE_FUNC_REPLACE				:	func = i3PSPGfx::TFUNC_REPLACE;	break;
		case I3G_TEXTURE_FUNC_MODULATE				:	func = i3PSPGfx::TFUNC_MODULATE;	break;
		case I3G_TEXTURE_FUNC_ADD					:	func = i3PSPGfx::TFUNC_ADD;			break;
		default										:	func = i3PSPGfx::TFUNC_BLEND;		break;
	}

	pBuffer->Add( i3PSPGfx::TFUNC( (i3PSPGfx::TFUNC_TXF) func, i3PSPGfx::TFUNC_RGBA, FALSE));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureMatrixAttr( i3OptBuildPSPCommandList * pOpt, i3PSPCommandBuffer * pBuffer, i3TextureMatrixAttr * pAttr)
{
	MATRIX * pMatrix = pAttr->GetMatrix();

	pBuffer->Add( i3PSPGfx::TGENN( 0));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get11( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get12( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get13( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get21( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get22( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get23( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get31( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get32( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get33( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get41( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get42( pMatrix )));
	pBuffer->Add( i3PSPGfx::TGEND( i3Matrix::Get43( pMatrix )));

	pOpt->m_bProjectiveTexture = pAttr->IsProjective();

	return 13;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureCoordScaleAttr( i3OptBuildPSPCommandList * pOpt, i3PSPCommandBuffer * pBuffer, i3TextureCoordScaleAttr * pAttr)
{
	VEC3D * pVec = pAttr->GetScaleFactor();

	pBuffer->Add( i3PSPGfx::SU( i3Vector::GetX( pVec)));
	pBuffer->Add( i3PSPGfx::SV( i3Vector::GetY( pVec)));

	return 2;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureCoordOffsetAttr( i3OptBuildPSPCommandList * pOpt, i3PSPCommandBuffer * pBuffer, i3TextureCoordOffsetAttr * pAttr)
{
	VEC3D * pVec = pAttr->GetOffset();

	pBuffer->Add( i3PSPGfx::TU( i3Vector::GetX( pVec)));
	pBuffer->Add( i3PSPGfx::TV( i3Vector::GetY( pVec)));

	return 2;
}

UINT32	i3OptBuildPSPCommandList::Oni3TextureWrapAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureWrapAttr * pAttr)
{
	i3PSPGfx::TWRAP_MODE horz, vert;

	switch( pAttr->GetHorzWrap())
	{
		case I3G_TEXTURE_WRAP_WRAP		:	horz = i3PSPGfx::TWRAP_REPEAT;	break;
		default							:	horz = i3PSPGfx::TWRAP_CLAMP;	break;
	}

	switch( pAttr->GetVertWrap())
	{
		case I3G_TEXTURE_WRAP_WRAP		:	vert = i3PSPGfx::TWRAP_REPEAT;	break;
		default							:	vert = i3PSPGfx::TWRAP_CLAMP;	break;
	}

	pBuffer->Add( i3PSPGfx::TWRAP( horz, vert));

	return 1;
}

/*
UINT32	i3OptBuildPSPCommandList::Oni3VertexBlendEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3VertexBlendEnableAttr * pAttr)
{
	return 0;
}
*/

UINT32	i3OptBuildPSPCommandList::Oni3BoneMatrixListAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BoneMatrixListAttr * pAttr)
{
	return 0;
}

/*
UINT32	i3OptBuildPSPCommandList::Oni3VertexColorEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3VertexColorEnableAttr * pAttr)
{
	return 0;
}
*/

UINT32	i3OptBuildPSPCommandList::Oni3ViewMatrixAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ViewMatrixAttr * pAttr)
{
	MATRIX * pMatrix = pAttr->GetMatrix();

	pBuffer->Add( i3PSPGfx::VIEWN( 0));

	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get11( pMatrix)));
	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get12( pMatrix)));
	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get13( pMatrix)));

	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get21( pMatrix)));
	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get22( pMatrix)));
	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get23( pMatrix)));

	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get31( pMatrix)));
	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get32( pMatrix)));
	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get33( pMatrix)));

	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get41( pMatrix)));
	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get42( pMatrix)));
	pBuffer->Add( i3PSPGfx::VIEWD( i3Matrix::Get43( pMatrix)));

	return 13;
}

UINT32	i3OptBuildPSPCommandList::Oni3ZFuncAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ZFuncAttr * pAttr)
{
	i3PSPGfx::ZTEST_MODE func;

	switch( pAttr->Get())
	{
		case I3G_COMP_LESS		:	func = i3PSPGfx::ZTEST_GREATER;	break;
		case I3G_COMP_LEQUAL	:	func = i3PSPGfx::ZTEST_GEQUAL;		break;
		case I3G_COMP_GREATER	:	func = i3PSPGfx::ZTEST_LESS;		break;
		case I3G_COMP_GEQUAL	:	func = i3PSPGfx::ZTEST_LEQUAL;		break;
		
		case I3G_COMP_NEVER		:	func = i3PSPGfx::ZTEST_NEVER;		break;
		case I3G_COMP_ALWAYS	:	func = i3PSPGfx::ZTEST_ALWAYS;		break;
		case I3G_COMP_EQUAL		:	func = i3PSPGfx::ZTEST_EQUAL;		break;
		default					:	func = i3PSPGfx::ZTEST_NOTEQUAL;	break;
	}

	pBuffer->Add( i3PSPGfx::ZTEST( func));

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3ZTestEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ZTestEnableAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::ZTE( pAttr->Get()) );

	return 1;
}

UINT32	i3OptBuildPSPCommandList::Oni3ZWriteEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ZWriteEnableAttr * pAttr)
{
	pBuffer->Add( i3PSPGfx::ZMSK( ! pAttr->Get()) );
	return 0;
}

static PSPATTRPROC s_AttrProcs[] =
{
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3AlphaFuncAttr,				// I3_ATTRID_ALPHA_FUNC	=	0,		//  0
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3AlphaTestEnableAttr,		// I3_ATTRID_ALPHA_TEST_ENABLE,		//  1
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3BlendEnableAttr,			// I3_ATTRID_BLEND_ENABLE,			//  2
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3BlendModeAttr,				// I3_ATTRID_BLEND_MODE,			//  3
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ClippingEnableAttr,			// I3_ATTRID_CLIPPING_ENABLE,		//  4
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ColorAttr,					// I3_ATTRID_COLOR,					//  5
	//(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3EdgeAntiAliasEnableAttr,	// I3_ATTRID_EDGEANTIALIAS_ENABLE,	//  6
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3FaceCullModeAttr,			// I3_ATTRID_FACECULL_MODE,			//  7
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3FogColorAttr,				// I3_ATTRID_FOG_COLOR,				//  8
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3FogDensityAttr,				// I3_ATTRID_FOG_DENSITY,			//  9
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3FogEnableAttr,				// I3_ATTRID_FOG_ENABLE,			// 10
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3FogModeAttr,				// I3_ATTRID_FOG_MODE,				// 11
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3FogNearFarAttr,				// I3_ATTRID_FOG_NEARFAR,			// 12
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3LightingEnableAttr,			// I3_ATTRID_LIGHTING_ENABLE,		// 13
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3MaterialAttr,				// I3_ATTRID_MATERIAL,				// 14
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3RenderModeAttr,				// I3_ATTRID_RENDER_MODE,			// 15
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ShadeModeAttr,				// I3_ATTRID_SHADE_MODE,			// 16
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3StencilFuncAttr,			// I3_ATTRID_STENCIL_FUNC,			// 17
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3StencilOperationAttr,		// I3_ATTRID_STENCIL_OPERATION,		// 18
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3StencilTestEnableAttr,		// I3_ATTRID_STENCILTEST_ENABLE,	// 19
	//(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3VertexBlendEnableAttr,		// I3_ATTRID_VERTEX_BLEND_ENABLE,	// 20
	//(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3VertexColorEnableAttr,		// I3_ATTRID_VERTEX_COLOR_ENABLE,	// 21
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ZFuncAttr,					// I3_ATTRID_Z_FUNC,				// 22
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ZTestEnableAttr,			// I3_ATTRID_Z_TEST_ENABLE,			// 23
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ZWriteEnableAttr,			// I3_ATTRID_Z_WRITE_ENABLE,		// 24
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ProjectMatrixAttr,			// I3_ATTRID_PROJECT_MATRIX,		// 25
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ViewMatrixAttr,				// I3_ATTRID_VIEW_MATRIX,			// 26
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ModelViewMatrixAttr,		// I3_ATTRID_MODEL_VIEW_MATRIX,		// 27
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureEnableAttr,			// I3_ATTRID_TEXTURE_ENABLE,		// 28
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureBindAttr,			// I3_ATTRID_TEXTURE_BIND,			// 29
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureFilterAttr,			// I3_ATTRID_TEXTURE_FILTER,		// 30
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureWrapAttr,			// I3_ATTRID_TEXTURE_WRAP,			// 31
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureFunctionAttr,		// I3_ATTRID_TEXTURE_FUNCTION,		// 32
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureCoordSourceAttr,		// I3_ATTRID_TEXTURE_COORD_SOURCE,	// 33
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureMatrixAttr,			// I3_ATTRID_TEXTURE_MATRIX,		// 34
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureCoordScaleAttr,		// I3_ATTRID_TEXTURE_COORD_SCALE,	// 35
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextureCoordOffsetAttr,		// 
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3BoneMatrixListAttr,			// I3_ATTRID_BONEMATRIXLIST,	// 36
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3LightAttr,					// I3_ATTRID_LIGHT,					// 37
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3RenderAttrListAttr,			// I3_ATTRID_RENDER_ATTR_LIST,		// 38
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3GeometryAttr,				// I3_ATTRID_GEOMETRY,				// 39
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3PointSpriteAttr,			// I3_ATTRID_POINTSPRITE,			// 40
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3QuadAttr,					// I3_ATTRID_QUAD,					// 41
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3SpriteAttr,					// I3_ATTRID_SPRITE,				// 42
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3BillboardAttr,				// I3_ATTRID_BILLBOARD,				// 43
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3RenderTargetAttr,			// I3_ATTRID_RENDERTARGET,			// 44
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3ClearRenderTargetAttr,		// I3_ATTRID_CLEARRENDERTARGET,		// 45
	(PSPATTRPROC) i3OptBuildPSPCommandList::Oni3TextAttr,					// I3_ATTRID_TEXT,					// 46
};

UINT32 i3OptBuildPSPCommandList::MakePSPCommandList( i3PSPCommandBuffer * pBuffer, i3RenderAttr * pRenderAttr)
{
	I3TRACE( "  %s\n", pRenderAttr->meta()->class_name() );

	return s_AttrProcs[ pRenderAttr->GetID()]( this, pBuffer, pRenderAttr);
}

UINT32 i3OptBuildPSPCommandList::MakePSPCommandList( i3PSPCommandBuffer * pBuffer, const i3::vector<i3RenderAttr*>& List)
{
	UINT32 Rc, Result = 0;

	for(size_t i = 0; i < List.size(); i++)
	{
		Rc = MakePSPCommandList( pBuffer, List[i]);

		Result += Rc;
	}
	
	return Result;
}

void i3OptBuildPSPCommandList::OnAttrSet( i3AttrSet * pAttrSet)
{
	MakePSPCommandList( &m_MainBuffer, pAttrSet->GetAttrs());

	m_pAttrStack->PushAttrs( pAttrSet->GetAttrs());
}


void i3OptBuildPSPCommandList::OnPostAttrSet( i3AttrSet * pAttrSet)
{
	INT32 i;
	i3RenderAttr * pAttr, * pDefAttr;
	i3AttrStack * pStack;

	m_pAttrStack->PopAttrs( pAttrSet->GetAttrs());

	for( i = 0; i < pAttrSet->GetAttrCount(); i++)
	{
		pAttr = pAttrSet->GetAttr( i);

		// 변경되었던 Render Attr.들을 복구한다.
		pStack = m_pAttrStack->GetAttrStackByID( pAttr->GetID());
		if( pStack != nullptr)
		{
			pDefAttr = pStack->GetTop();

			if( pDefAttr != nullptr)
			{
				MakePSPCommandList( &m_MainBuffer, pDefAttr);
			}
		}
	}
}

void i3OptBuildPSPCommandList::OnTransform( i3Transform * pTrans)
{
	// 자신의 Model View Matrix를 tracer에 등록해 둔다.
	i3ModelViewMatrixAttr * pAttr = (i3ModelViewMatrixAttr *) m_pModeViewMatrixAttrPool->Alloc();

	pAttr->SetMatrix( m_pStack->GetTop());

	m_pAttrStack->PushAttr( pAttr);
}

void i3OptBuildPSPCommandList::OnPostTransform( i3Transform * pTrans)
{
	m_pAttrStack->PopAttr( I3_ATTRID_MODEL_VIEW_MATRIX);
}

//void i3OptBuildPSPCommandList::GatherDifferAttrs( List * pList)		// 쓰이지 않음..
//{
//}

void i3OptBuildPSPCommandList::OnGeometry( i3Geometry * pGeo)
{
//	i3::vector List;
	INT32 i;

	// i3ModelViewMatrixAttr
	if( m_pStack->IsModified())
	{
		i3ModelViewMatrixAttr * pAttr = i3ModelViewMatrixAttr::new_object();

		pAttr->SetMatrix( m_pStack->GetTop());

		MakePSPCommandList( &m_MainBuffer, pAttr);

		I3_MUST_RELEASE(pAttr);

		m_pStack->SetModified(false);
	}

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		// GeometryAttr들을 적용해야 한다.
		MakePSPCommandList( &m_MainBuffer, pGeo->GetGeometryAttr(i));
	}

}

void i3OptBuildPSPCommandList::OnPostGeometry( i3Geometry * pGeo)
{
}

bool i3OptBuildPSPCommandList::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( m_bBuildCommand == true)
	{
		if( i3::kind_of<i3AttrSet* >(pNode))
		{
			OnAttrSet( (i3AttrSet *) pNode);
		}
		else if( i3::kind_of< i3Transform*>(pNode))
		{
			OnTransform( (i3Transform *) pNode);
		}
		else if( i3::kind_of<i3Geometry* >(pNode))
		{
			OnGeometry( (i3Geometry *) pNode);
		}
	}

	return true;
}

bool i3OptBuildPSPCommandList::OnPostNode( i3Node * pNode, i3Node * pParent)
{
	if( m_bBuildCommand == true)
	{
		if( i3::kind_of<i3AttrSet* >(pNode))
		{
			OnPostAttrSet( (i3AttrSet *) pNode);
		}
		else if( i3::kind_of<i3Transform* >(pNode))
		{
			OnPostTransform( (i3Transform *) pNode);
		}
		else if( i3::kind_of<i3Geometry* >(pNode))
		{
			OnPostGeometry( (i3Geometry *) pNode);
		}
	}

	return true;
}

void i3OptBuildPSPCommandList::Rec_Trace( i3Node * pNode, i3Node * pParent)
{
	INT32 i, Count;
	i3Node * pChild;
	bool bPushTrans = false;
	UINT32	BJumpIdx = 0xFFFFFFFF;
	bool bAddCmd = false;

	if( m_bFirstTrace == true)
	{
		if( m_bRotateAxis)
		{
			MATRIX tmx;

			i3Matrix::SetRotateX( &tmx, I3_DEG2RAD( -90.0f));
			i3Matrix::PostRotateY( &tmx, I3_DEG2RAD( 180.0f));

			m_pStack->PushPre(nullptr, &tmx);
		}

		m_bFirstTrace = false;
	}

	if( (m_bRecord == false) && pNode->IsFlag( I3_NODEFLAG_STATIC))
	{
		m_bRecord = true;
		bAddCmd = true;

		m_pGPUCmdList = i3GPUCommandList::new_object_ref();

		m_pGPUCmdList->SetGPUType( I3G_GPU_PSP);

		m_MainBuffer.Reset();
		m_BBoxBuffer.Reset();
		RemoveAllBBox();

		// 우선 그냥 Traverse 하면서 BoundBox Vertex Array를 생성한다.
		m_bBuildCommand = false;
		Rec_Trace( pNode, pParent);

		m_bBuildCommand = true;

		m_MainBuffer.Add( i3PSPGfx::BASE( (void *)(8 + (m_BBoxBuffer.GetCount() * 4))));
		m_MainBuffer.Add( i3PSPGfx::JUMP( (void *)(8 + (m_BBoxBuffer.GetCount() * 4))));	// BBox Vertex들을 건너뛴 위치로 Jump 시킨다.

		// BoundBox용 Vertex Array들을 저장한다.
		{
			UINT32 i, j, startidx;
			OPTPSPCMD_BBOXINFO * pInfo;
			REAL32 val;

			for( i = 0; i < m_BBoxList.size(); i++)
			{
				pInfo = m_BBoxList[i];

				startidx = pInfo->m_Addr;

				// BBox Vertex의 시작 Address를 재설정한다.
				pInfo->m_Addr = m_MainBuffer.GetCount() * 4;

				for( j = 0; j < 24; j++)
				{
					val = m_BBoxBuffer.Get( j + startidx);

					m_MainBuffer.Add( *((UINT32 *) &val));				// BBox Vertex의 복사
				}
			}
		}

		m_MainBuffer.Add( i3PSPGfx::TMAP( i3PSPGfx::TMAP_TMN_UV, i3PSPGfx::TMAP_TMI_UV));
		m_MainBuffer.Add( i3PSPGfx::TU( 0.0f));
		m_MainBuffer.Add( i3PSPGfx::TV( 0.0f));
	}

	if( m_bRecord)
	{
		if( m_bBuildCommand)
		{
			if( pNode->GetBound() != nullptr)
			{
				OPTPSPCMD_BBOXINFO * pInfo;

				pInfo = FindBBox( pNode);
				I3ASSERT( pNode != nullptr);

				// BBOX의 Bound Box는 Local-Space로 설정되며, World Matrix의해
				// Transform 된 후, 최종적으로 Clipping 처리된다.
				// 그러므로 World Matrix를 필요한 경우, 설정해 주어야 한다.
				PutWorldMatrix( &m_MainBuffer, m_pStack->GetTop());

				// BoundBox Jump를 위한 설정.
				m_MainBuffer.Add( i3PSPGfx::VTYPE( 0x03 << 7));		// Position만 REAL32 Type
				m_MainBuffer.Add( i3PSPGfx::BASE( (void *)(pInfo->m_Addr)));
				m_MainBuffer.Add( i3PSPGfx::VADR( (void *)(pInfo->m_Addr)));
				m_MainBuffer.Add( i3PSPGfx::BBOX( 8));

				BJumpIdx = m_MainBuffer.GetCount();
				m_MainBuffer.Add( i3PSPGfx::BASE( (void *) 0));
				m_MainBuffer.Add( i3PSPGfx::BJUMP( (void *) 0));		// 현재는 Placeholder로만 설정. (후에 재설정한다.)

				I3TRACE( "BBOX\n");
			}
		}
		else
		{
			// Command를 Build하지 않고, Node의 Bound Box만을 확인하는 처리..
			if( pNode->GetBound() != nullptr)
			{
				i3BoundBox * pBBox = pNode->GetBound();
				REAL32 x1, y1, z1, x2, y2, z2;

				AddBBox( pNode, m_BBoxBuffer.GetCount(), 8);

				x1 = i3Vector::GetX( pBBox->GetMin());
				y1 = i3Vector::GetY( pBBox->GetMin());
				z1 = i3Vector::GetZ( pBBox->GetMin());

				x2 = i3Vector::GetX( pBBox->GetMax());
				y2 = i3Vector::GetY( pBBox->GetMax());
				z2 = i3Vector::GetZ( pBBox->GetMax());

				// Upper plane
				m_BBoxBuffer.Add( x1);	m_BBoxBuffer.Add( y1);	m_BBoxBuffer.Add( z1);
				m_BBoxBuffer.Add( x1);	m_BBoxBuffer.Add( y1);	m_BBoxBuffer.Add( z2);
				m_BBoxBuffer.Add( x2);	m_BBoxBuffer.Add( y1);	m_BBoxBuffer.Add( z1);
				m_BBoxBuffer.Add( x2);	m_BBoxBuffer.Add( y1);	m_BBoxBuffer.Add( z2);

				// Lower plane
				m_BBoxBuffer.Add( x1);	m_BBoxBuffer.Add( y2);	m_BBoxBuffer.Add( z1);
				m_BBoxBuffer.Add( x1);	m_BBoxBuffer.Add( y2);	m_BBoxBuffer.Add( z2);
				m_BBoxBuffer.Add( x2);	m_BBoxBuffer.Add( y2);	m_BBoxBuffer.Add( z1);
				m_BBoxBuffer.Add( x2);	m_BBoxBuffer.Add( y2);	m_BBoxBuffer.Add( z2);
			}
		}
	}

	if( m_bBuildCommand )
	{
		if( (m_bMatrixStackEnable) && (i3::kind_of<i3Transform*>(pNode)) )
		{
			i3Transform * pTrans = (i3Transform *) pNode;

			// Matrix Stack에 반영..
			m_pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());

			bPushTrans = true;
		}

		if( OnNode( pNode, pParent) == false)
			return;
	}

	// Children Recursive Call
	Count = pNode->GetChildCount();
	for( i = 0; i < Count; i++)
	{
		pChild = pNode->GetChild( i);

		Rec_Trace( pChild, pNode);
	}

	if( bPushTrans)
	{
		m_pStack->Pop();
	}

	if( m_bBuildCommand)
	{
		if( m_bRecord)
		{
			if( pNode->GetBound() != nullptr)
			{
				// BJUMP를 재설정해야 한다.
				if( m_MainBuffer.GetCount() > (BJumpIdx + 2))
				{
					m_MainBuffer.Set( BJumpIdx, i3PSPGfx::BASE( (void *)(m_MainBuffer.GetCount() * 4)));
					m_MainBuffer.Set( BJumpIdx + 1, i3PSPGfx::BJUMP( (void *)(m_MainBuffer.GetCount() * 4)));
				}
				else
				{
					m_MainBuffer.Set( BJumpIdx,		i3PSPGfx::NOP());
					m_MainBuffer.Set( BJumpIdx + 1, i3PSPGfx::NOP());
				}
			}
		}

		if( OnPostNode( pNode, pParent) == false)
			return;

		if( bAddCmd)
		{
			//  하나의 Static Node에 대한 Build가 완료되었기 때문에...
			i3Node * pParentNode = pNode->GetParent();
			i3Geometry * pGeo;
			i3GPUCommandListAttr * pCmdAttr;

			m_MainBuffer.Add( i3PSPGfx::RET());

			if( pParentNode != nullptr)
			{
				m_pGPUCmdList->CopyBuffer( (INT8 *) m_MainBuffer.GetBuffer(), m_MainBuffer.GetCount() * 4);

				pCmdAttr = i3GPUCommandListAttr::new_object_ref();
				pCmdAttr->Set( m_pGPUCmdList);

				#if defined( I3_DEBUG)
				m_pGPUCmdList->Dump();
				#endif

				pGeo = i3Geometry::new_object_ref();
				pGeo->AppendGeometryAttr( pCmdAttr);

				pParentNode->AddChild( pGeo);

				m_MainBuffer.Reset();
				m_BBoxBuffer.Reset();
				RemoveAllBBox();

				if( pNode->IsFlag( I3_NODEFLAG_DELETEORIGINALNODE))
				{
					// I3_NODEFLAG_DELETEORIGINALNODE flag이 포함되어 있다면...
					// 원본 노드는 Scene Graph에서 제거한다.
					pParentNode->RemoveChild( pNode);
				}
			}
		}
	}
}

/// 
/// 기본적인 Command List의 생성 구조는 아래와 같다.
///
/// CMD_ORIGIN
/// CMD_JUMP
/// Bound Box Vertex Array 1
/// Bound Box Vertex Array 2
/// ...
/// Bound Box Vertex Array n
/// CMD_VTYPE
/// CMD_BASE
/// CMD_VADR
/// CMD_BBOX
/// CMD_BJUMP
///		CMD_BBOX
///		CMD_BJUMP
///		...
///		CMD_BBOX
///		CMD_BJUMP
///			CMD_BBOX
///			CMD_BJUMP
///			...
///		CMD_BBOX
///		CMD_BJUMP
///		...
/// CMD_RET
///
/// VertexArray, IndexArray, Texture 등과 같은 Resource에 대한 포인터는
/// 현재의 Pointer를 직접 설정한다.
///
/// PSP에서는 Loading한 다음, 다시 한번 Command List를 Parsing하면서
/// 각 Resource에 대한 Pointer를 재설정(Rebinding)한다.
///
/// CMD_ORIGIN에서 설정할 수 있는 Pointer는 하위 8bits가 항상 0으로
/// 간주되기 때문에, PSP에서 Command List는 반드시 256 Align된 Buffer로
/// 할당되어야 한다.
///
void i3OptBuildPSPCommandList::Trace( i3Node * pRoot)
{
	SetMatrixStackEnable(true);

	m_bFirstTrace = true;

	m_pAttrStack = i3AttrStackManager::new_object();

	m_pModeViewMatrixAttrPool = i3ObjectPool::new_object();
	m_pModeViewMatrixAttrPool->Create( i3ModelViewMatrixAttr::static_meta());

	m_DrawCount = 0;
	m_DrawStripCount = 0;
	m_DrawIndexedCount = 0;
	m_DrawIndexedStripCount = 0;

	m_bTextureEnable = false;
	m_bLightingEnable = false;
	m_TexFunc = -1;
	m_OldTexFunc = -1;

	// 실제 PSP Command List를 생성한다.
	m_bBuildCommand = false;
	i3SceneOptimizer::Trace( pRoot);

	// 종료
	RemoveAllBBox();

	I3TRACE( "Draw Count : %d\n", m_DrawCount);
	I3TRACE( "Draw Strip Count : %d\n", m_DrawStripCount);
	I3TRACE( "Draw Indexed Count : %d\n", m_DrawIndexedCount);
	I3TRACE( "Draw Indexed Strip Count : %d\n", m_DrawIndexedStripCount);
}
