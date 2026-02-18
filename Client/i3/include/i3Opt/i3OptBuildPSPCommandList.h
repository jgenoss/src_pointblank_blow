#if !defined( __I3_OPT_BUILD_PSP_COMMAND_LIST_H)
#define __I3_OPT_BUILD_PSP_COMMAND_LIST_H

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3SceneOptimizer.h"

typedef i3TBandBuffer<UINT32>		i3PSPCommandBuffer;
typedef i3TBandBuffer<REAL32>		i3PSPBoundBoxBuffer;

struct OPTPSPCMD_BBOXINFO
{
	i3Node *	m_pNode = nullptr;
	UINT32		m_Addr = 0;
	UINT32		m_Count = 0;
} ;

class i3OptBuildPSPCommandList;

typedef UINT32	(*PSPATTRPROC)( i3OptBuildPSPCommandList *, i3PSPCommandBuffer *, i3RenderAttr *);

class I3_EXPORT_OPT i3OptBuildPSPCommandList : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildPSPCommandList, i3SceneOptimizer);

protected:

	i3AttrStackManager *	m_pAttrStack = nullptr;

	i3PSPCommandBuffer		m_MainBuffer;
	i3PSPBoundBoxBuffer		m_BBoxBuffer;

	i3::vector<OPTPSPCMD_BBOXINFO*>	m_BBoxList;

	bool					m_bRecord = false;
	bool					m_bBuildCommand = false;

	bool					m_bRotateAxis = false;

	i3GPUCommandList *		m_pGPUCmdList = nullptr;
	bool					m_bProjectiveTexture = false;

	i3ObjectPool *			m_pModeViewMatrixAttrPool = nullptr;

	INT32					m_DrawCount = 0;
	INT32					m_DrawStripCount = 0;
	INT32					m_DrawIndexedCount = 0;
	INT32					m_DrawIndexedStripCount = 0;

	MATRIX					m_LastWorldMatrix;
	bool					m_bFirstTrace = false;

	bool					m_bTextureEnable = false;
	bool					m_bLightingEnable = false;
	INT32					m_TexFunc = 0;
	INT32					m_OldTexFunc = 0;

protected:
	/// 현재까지의 Last Render Attrobute들 중에서 Default 상태가 아닌 것들에 대한
	/// List를 구한다.
	/// 결과로 pList에 i3RenderAttr class 들이 담겨진다.
	// void			GatherDifferAttrs( List * pList);		// 쓰이지 않음..

	UINT32			MakePSPCommandList( i3PSPCommandBuffer * pBuffer, i3RenderAttr * pAttr);
	UINT32			MakePSPCommandList( i3PSPCommandBuffer * pBuffer, const i3::vector<i3RenderAttr*>& List);

	void			AddBBox( i3Node * pNode, UINT32	Addr, UINT32 count);
	OPTPSPCMD_BBOXINFO * FindBBox( i3Node * pNode);
	void			RemoveAllBBox(void);
	
	virtual void	Rec_Trace( i3Node * pNode, i3Node * pParent);

	UINT32			GetPSPVertexFormat( i3VertexFormat * pFormat);
	UINT32			GetNativeImageFormat( I3G_IMAGE_FORMAT format);

	void			DrawPrim( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr);
	void			DrawStripPrim( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr);
	void			DrawIndexedPrim( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr);
	void			DrawStripIndexedPrim( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr);
	void			DrawPointSprite( i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pGeoAttr);

	void			OnAttrSet( i3AttrSet * pAttrSet);
	void			OnPostAttrSet( i3AttrSet * pAttrSet);
	void			OnTransform( i3Transform * pTrans);
	void			OnPostTransform( i3Transform * pTrans);
	void			OnGeometry( i3Geometry * pGeo);
	void			OnPostGeometry( i3Geometry * pGeo);

	void			PutWorldMatrix( i3PSPCommandBuffer * pBuffer, MATRIX * pMatrix);

	void			UpdateTextureStageState( i3PSPCommandBuffer * pBuffer, bool bIsVertexColor );

public:
	i3OptBuildPSPCommandList(void);
	virtual ~i3OptBuildPSPCommandList(void);

	void			SetRotateAxis( bool bRotate)				{ m_bRotateAxis = bRotate; }

	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual bool OnPostNode( i3Node * pNode, i3Node * pParent) override;

	virtual void	Trace( i3Node * pRoot) override;

	static UINT32	Oni3AlphaFuncAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3AlphaFuncAttr * pAttr);
	static UINT32	Oni3AlphaTestEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3AlphaTestEnableAttr * pAttr);
	static UINT32	Oni3BillboardAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BillboardAttr * pAttr);
	static UINT32	Oni3BlendEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BlendEnableAttr * pAttr);
	static UINT32	Oni3BlendModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BlendModeAttr * pAttr);
	static UINT32	Oni3ClearRenderTargetAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ClearRenderTargetAttr * pAttr);
	static UINT32	Oni3ClippingEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ClippingEnableAttr * pAttr);
	static UINT32	Oni3ColorAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ColorAttr * pAttr);
	//static UINT32	Oni3EdgeAntiAliasEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3EdgeAntiAliasEnableAttr * pAttr);
	static UINT32	Oni3FaceCullModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FaceCullModeAttr * pAttr);
	static UINT32	Oni3FogColorAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogColorAttr * pAttr);
	static UINT32	Oni3FogDensityAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogDensityAttr * pAttr);
	static UINT32	Oni3FogEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogEnableAttr * pAttr);
	static UINT32	Oni3FogModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogModeAttr * pAttr);
	static UINT32	Oni3FogNearFarAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3FogNearFarAttr * pAttr);
	static UINT32	Oni3GeometryAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3GeometryAttr * pAttr);
	static UINT32	Oni3LightAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3LightAttr * pAttr);
	static UINT32	Oni3LightingEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3LightingEnableAttr * pAttr);
	static UINT32	Oni3MaterialAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3MaterialAttr * pAttr);
	static UINT32	Oni3ModelViewMatrixAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ModelViewMatrixAttr * pAttr);
	static UINT32	Oni3PointSpriteAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3PointSpriteAttr * pAttr);
	static UINT32	Oni3ProjectMatrixAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ProjectMatrixAttr * pAttr);
	static UINT32	Oni3QuadAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3QuadAttr * pAttr);
	static UINT32	Oni3RenderAttrListAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3RenderAttrListAttr * pAttr);
	static UINT32	Oni3RenderModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3RenderModeAttr * pAttr);
	static UINT32	Oni3RenderTargetAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3RenderTargetAttr * pAttr);
	static UINT32	Oni3ShadeModeAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ShadeModeAttr * pAttr);
	static UINT32	Oni3SpriteAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3SpriteAttr * pAttr);
	static UINT32	Oni3StencilFuncAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3StencilFuncAttr * pAttr);
	static UINT32	Oni3StencilOperationAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3StencilOperationAttr * pAttr);
	static UINT32	Oni3StencilTestEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3StencilTestEnableAttr * pAttr);
	static UINT32	Oni3TextAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextAttr * pAttr);
	static UINT32	Oni3TextureBindAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureBindAttr * pAttr);
	static UINT32	Oni3TextureCoordSourceAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureCoordSourceAttr * pAttr);
	static UINT32	Oni3TextureEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureEnableAttr * pAttr);
	static UINT32	Oni3TextureFilterAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureFilterAttr * pAttr);
	static UINT32	Oni3TextureFunctionAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureFunctionAttr * pAttr);
	static UINT32	Oni3TextureMatrixAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureMatrixAttr * pAttr);
	static UINT32	Oni3TextureCoordScaleAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureCoordScaleAttr * pAttr);
	static UINT32	Oni3TextureCoordOffsetAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureCoordOffsetAttr * pAttr);
	static UINT32	Oni3TextureWrapAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3TextureWrapAttr * pAttr);
	//static UINT32	Oni3VertexBlendEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3VertexBlendEnableAttr * pAttr);
	//static UINT32	Oni3VertexBlendMatrixListAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BoneMatrixListAttr * pAttr);
	//static UINT32	Oni3VertexColorEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3VertexColorEnableAttr * pAttr);
	static UINT32	Oni3ViewMatrixAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ViewMatrixAttr * pAttr);
	static UINT32	Oni3ZFuncAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ZFuncAttr * pAttr);
	static UINT32	Oni3ZTestEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ZTestEnableAttr * pAttr);
	static UINT32	Oni3ZWriteEnableAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3ZWriteEnableAttr * pAttr);
	static UINT32	Oni3BoneMatrixListAttr( i3OptBuildPSPCommandList *, i3PSPCommandBuffer * pBuffer, i3BoneMatrixListAttr * pAttr);
};

#endif