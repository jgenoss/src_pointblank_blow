#if !defined( __I3_UI_RENDERER_H)
#define __I3_UI_RENDERER_H

#include "i3GameObj.h"
#include "i3UISprite.h"
#include "i3VirtualTexture.h"
#include "i3UITextManager.h"

//#define	USE_DRAWUP
//#define		VA_STREAM

//#define UI_RENDER_LAYER_COUNT	150
#define UI_RENDER_PRIM_COUNT	512

struct I3VT_TEX_PARAM
{
	REAL32		m_BaseX = 0.0f;
	REAL32		m_BaseY = 0.0f;
	REAL32		m_RCacheWidth = 0.0f;
	REAL32		m_RCacheHeight = 0.0f;
};

struct I3UIRENDER_LAYERINFO : i3::class_common_pool<I3UIRENDER_LAYERINFO>
{
//	INT32				m_idxStart;
//	INT32				m_Count;
	I3UI_SPRITE_STYLE	m_SpriteStyle = I3UI_SPRITE_STYLE_INSTANCE;
	i3Geometry*			m_pInstanceNode = nullptr;
	i3Node*				m_pNodeSprite = nullptr;
	INT32				m_Depth = 0;
	UINT32				m_DrawSpriteIndex = 0;
	i3::vector<i3UISprite*>	m_DrawSpriteList;

};		//	i3UserAttr * m_pUserAttr; 은 쓰이지 않으므로 일단 폐기합니다.

enum LEGACY_RENDERTYPE
{
	LEGACY_RENDER_NA,
	LEGACY_RENDER_SPRITE,					// Sprite를 Rendering해야 함을 의미.
	LEGACY_RENDER_FONT						// Font를 Rendering해야 함을 의미.
};

struct LEGACY_RENDER_CTX
{
	LEGACY_RENDERTYPE	m_RenderType = LEGACY_RENDERTYPE::LEGACY_RENDER_NA;	// Sprite Rendering인지, Font Rendering인지를 나타냄.

	union
	{
		I3VT_TEXINFO*	m_RefTexInfo = nullptr;	// Rendering되는 Sprite의 Texture Ref
		INT32			m_idxFontTex;
	};

	INT32				m_offset = 0;		// Rendering되는 SpriteIdx의 시작Index
	INT32				m_Count = 0;		// Render Primitive count
};

struct LEGACY_DRIVE_CTX
{
//	INT32				m_idxStart;			// 이번에 처리되어야할 첫번째 Sprite의 Index
//	INT32				m_Count;			// 등록된 Sprite 갯수

	LEGACY_RENDER_CTX	m_RenderPrimitite[ UI_RENDER_PRIM_COUNT];
	INT32				m_CurrentPrim = 0;		// Renderer에 등록할 Primitive Count

	I3UIRENDER_LAYERINFO *	m_pLayer = nullptr;

	void	Reset( I3UIRENDER_LAYERINFO * pLayer)
	{
		m_pLayer = pLayer;
//		m_idxStart = pLayer->m_idxStart;
//		m_Count = pLayer->m_Count;

		for( UINT32 i = 0; i < UI_RENDER_PRIM_COUNT; i++)
		{
			m_RenderPrimitite[i].m_RenderType = LEGACY_RENDER_NA;
			m_RenderPrimitite[i].m_RefTexInfo = nullptr;
			m_RenderPrimitite[i].m_offset = 0;
			m_RenderPrimitite[i].m_Count = 0;
		}
		m_CurrentPrim = 0;
	}

	void	SwapRender( INT32 count);
};

#if defined( USE_DRAWUP)
struct DRAW_VERTEX
{
	VEC3D		m_Pos;
	VEC2D		m_Tex;
	VEC2D		m_TexRange;
	UINT32		m_Color = 0;
};
#endif

class I3_EXPORT_FRAMEWORK i3UIRenderer : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3UIRenderer, i3GameObj);

	friend class i3UIControl;
	friend class i3UIImageBoxEx;
	friend class i3UI3DViewBox;
	friend class i3UIDummyAttrSet;
	friend class i3UIText;

protected:
	bool				m_bLegacyMode = true;
	bool				m_bStretch = false;		// 화면 비율에 맞춰 늘려준다. default : false

	i3VertexArray *		m_pGeoVA[2] = { nullptr, nullptr };	// double buffer로 변경. soongoo. 2012.08.24
	i3IndexArray *		m_pGeoIA[2] = { nullptr, nullptr };

#if defined( USE_DRAWUP)
	DRAW_VERTEX	*		m_pDrawVA = nullptr;
	UINT32 *			m_pDrawIA = nullptr;
	IDirect3DVertexDeclaration9 *	m_pDrawVADecl = nullptr;
#endif

#if defined( VA_STREAM)
	INT32				m_accmDraw = 0;
#endif

	i3VertexArray *		m_pInstanceVA = nullptr;

	i3Node*								m_pLayerRoot = nullptr;		// 레이어 루트가 필요하다...노드 구조에서는 떼였다붙였다와 순서문제를 신경쓴다..
	i3::vector<I3UIRENDER_LAYERINFO*>	m_LayerStorage;		
	INT32								m_LayerCount = 0;

	i3AttrSet *			m_pRoot = nullptr;
	i3ProjectMatrixAttr *	m_pProjAttr = nullptr;
	
	i3Shader *			m_pShader = nullptr;
	i3Shader *			m_pShaderFont = nullptr;

	i3UISprite *		m_pSprites = nullptr;
	INT32				m_MaxSprite = 0;

	INT32				m_idxFreeHead = -1;
	INT32				m_idxAllocHead = -1;
	INT32				m_idxAllocTail = -1;		// 할당 List는 빈번한 Add/Remove가 발생할 수 있기 때문에 Double-LinkedList를 사용한다.
	INT32				m_cntAlloc = 0;
	INT32				m_cntDraw = 0;
	INT32				m_DrawCountPerFrame = 0;

	//INT32				m_idxFreeHead;		// 해제 List는 항상 Head 지점에서만 Add/Remove가 발생하기 때문에 Single-LinkedList를 사용한다.

	INT32				m_idxModifyHead = -1;	// 수정 List는 항상 Head 지점에서만 Add/Remove가 발생하기 때문에 Single-LinkedList를 사용한다.

	INT32				m_idxShow = 0;			// Instance VA는 Double-Buffing을 한다. 이 변수는 Double Buffer 중에 어느 것이 Rendering될 것인지를 나타냄.

	MATRIX				m_mtxProj;
	REAL32				m_ScreenWidth = 0.0f;
	REAL32				m_ScreenHeight = 0.0f;

	bool				m_bTopMost = false;

	void *				m_pVBDecl = nullptr;

	i3VirtualTexture *	m_pVTex = nullptr;
	I3G_TEXTURE_BIND_CONTEXT	m_ctxCacheTex;
	I3G_TEXTURE_BIND_CONTEXT	m_ctxIndirectTex;
	I3G_TEXTURE_BIND_CONTEXT	m_ctxFontTex;

	i3UITextManager *	m_pTextSys = nullptr;

	//i3::vector<i3UISprite*>		m_DrawSpriteList;
	i3::vector<I3UIRENDER_LAYERINFO*>	m_TopDrawLayerList;
	I3UIRENDER_LAYERINFO *		m_pLastLayer = nullptr;

	i3::vector<INT32>		m_FreeSpriteList;

	i3::vector<INT32>		m_AllocSpriteList;


	//
	// Legacy Mode 전용
	// 
//	List				m_UsedTexList;			// 사용된 모든 Texture들을 들고 있으면서, 큰 순서대로 Sorting되어 있다. (쓰이지 않고 잘 몰라서 주석)
	INT32				m_PassCount = 0;
	bool				m_bChangeDepth = false;		// FrameWnd마다 Depth를 주자

	I3VT_TEX_PARAM		m_fontTexInfo;
	INT32				m_DrawDepth = 0;

	// Performance Profile
#if !defined( I3_NO_PROFILE)
	INT64				m_tickRender = 0L;
	INT64				m_tickUpdate = 0L;
	INT64				m_tickTransfer = 0L;
	INT64				m_tickUpdateIndirect = 0L;
	INT64				m_tickValidate = 0L;
	INT64				m_tickTotal = 0L;
	INT32				m_cntDrawPass = 0L;
	INT64				m_tickKick = 0L;
#endif

protected:
	bool				_CreateVertexArrays_Legacy(void);
	bool				_CreateVertexArrays_Instancing(void);
	bool				_CreateVertexArrays(void);

	void				_AddToModifyList( INT32 idx);
	void				_RemoveFromModifyList( INT32 idx);

	i3UISprite *		getSprite( INT32 idx)
	{
		I3_BOUNDCHK( idx, m_MaxSprite);
		//if(idx < 0 || idx >= m_MaxSprite)
		//{
		//	I3PRINTLOG(I3LOG_FATAL, "Sprite Count Overflow(%s:%d) idx : %d, count : %d", pszFile, line, idx, GetCurrentSpriteAllocCount());
		//}
		return & m_pSprites[ idx];
	}

	void					_unlink( INT32 idx);
	void					_link( INT32 idxPrev, INT32 idx);

	bool					isInAllocList( i3UISprite * pSprite);
	bool					_CheckList(void);

	
protected:
	void					_SetResolution( REAL32 w, REAL32 h);
	void					_SelectShader(void);

	INT32					_UpdateAllInstance( I3UIRENDER_LAYERINFO * pLayer, INT32 idxStart, INT32 cnt);
	void					_Transfer( REAL32 maxTime = 0.2f);
	void					_Render( i3RenderContext * pCtx);

	// Legacy Mode Functions
	void					_SetSpriteMesh( INT32 buffIdx, i3UISprite * pSprite, INT32 idxVtx, INT32 idxIdx, REAL32 depth, I3VT_TEX_PARAM * pTexInfo, bool bFont);
	INT32					_UpdateAllInstance_Legacy( LEGACY_DRIVE_CTX * pCtx);
	void					_Render_Legacy( i3RenderContext * pRTCtx, LEGACY_DRIVE_CTX * pCtx, INT32 idxPrim);
	void					_Render_Font( i3RenderContext * pRTCtx, LEGACY_DRIVE_CTX * pCtx, INT32 idxPrim);
	static void				_RenderProc_Start( i3SceneTracer * pTracer, i3RenderContext * pContext);
	static void				_RenderProc_Finish( i3SceneTracer * pTracer, i3RenderContext * pContext);

	void					_Reset(void);

	void					FlushVRAM(void);

	I3UIRENDER_LAYERINFO *	_AllocLayer(void);
	I3UIRENDER_LAYERINFO *	_FindLayer( INT32 depth);
	void					_FreeLayer( INT32 idx);
	INT32					_GetLayerCount( void);
	void					_ResetLayer(void);
	
	static void				_RenderLayerProc( i3RenderContext * pContext, I3UIRENDER_LAYERINFO * pLayerInfo);
	static void				_RenderLayerProc_Legacy( i3RenderContext * pContext, I3UIRENDER_LAYERINFO * pLayerInfo);

public:
	i3UIRenderer(void);
	virtual ~i3UIRenderer(void);

	bool				Create( i3Framework * pFramework, INT32 maxPrim = 5000, const bool& bLegacy = true);
	
	INT32				GetCurrentSpriteAllocCount() const { return m_cntAlloc; }

	void				Reset(void);

	virtual void		OnUpdate( REAL32 rDeltaSeconds) override;

	i3UITextManager *	getTextManager(void)					{ return m_pTextSys; }

	bool				getLegacyModeState(void)					{ return m_bLegacyMode; }

	INT32				AllocSprite( I3UI_SPRITE_STYLE style = I3UI_SPRITE_STYLE_INSTANCE, i3ElementBase * pOwner = nullptr);
	void				FreeSprite( INT32	hndl);
	INT32				getTail(void)							{ return m_idxAllocTail;}

	void				AdjustOrder( INT32 hSpriteEnd, INT32 * pSpriteArr, INT32 cnt);

	void				SetTexture( i3VirtualTexture * pTex);

	i3Node *			GetRoot(void)							{ return m_pRoot; }

	VEC3D *				getPos( INT32 idx)						{ return m_pSprites[ idx].getCenter(); }
	void				setPos( INT32 idx, VEC3D * pCenter);
	void				setPos( INT32 idx, REAL32 x, REAL32 y, REAL32 z);

	VEC3D *				getSizeAndRotate( INT32 idx)				{ return m_pSprites[ idx].getSizeAndRotate(); }
	void				setSizeAndRotate( INT32 idx, REAL32 w, REAL32 h, REAL32 rot);

	VEC4D *				getTexCoord( INT32 idx)						{ return m_pSprites[ idx].getTexCoord(); }
	void				setTexCoord( INT32 idx, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2);

	VEC4D *				getTexRange( INT32 idx)						{ return m_pSprites[ idx].getTexRange(); }
	void				setTexRange( INT32 idx, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2);

	UINT32				getColor( INT32 idx)						{ return m_pSprites[ idx].getColor(); }
	void				setColor( INT32 idx, COLOR * pColor);
	void				setColor( INT32 idx, UINT8 r, UINT8 g, UINT8 b, UINT8 a);

	bool				getEnableSprite( INT32 idx)						{ return m_pSprites[idx].isEnabled(); }
	void				setEnableSprite( INT32 idx, bool bFlag);

	void				Draw_Start( void)							{ _Reset(); }
	void				Draw_Kick( INT32 hSprite);
	void				Draw_SetTopMost( bool bTopMost);
	void				Draw_Finish(void);

	void				OnSize( INT32 w, INT32 h);
	void				OnResetVRAMCache(void);

	virtual bool		OnLostDevice( bool bLostDevice) override;
	virtual bool		OnRevive( i3RenderContext * pCtx) override;

	void				GetProfileString( char * pszBuf, INT32 len);
	void				GetProfileString( i3::string& inout );

	void				CheckList(void);

	void				setSpriteColor( INT32 idx, COLOR* pColor);
	void				getSpriteColor( INT32 idx, COLOR* pColor);

	bool				PrepareTextureForSprite( VEC4D * pTexCoord, VEC4D * pTexRange);
	bool				PrepareTextureForSprite( INT32 hSprite);

	void				InitRender( INT32 primCnt);
	void				ReadyRender( void);
	void				EndRender( void);

	void				DepthPlus( void)			{ m_DrawDepth++; m_bChangeDepth = true; }
	void				DepthMinus( void)			{ m_DrawDepth--; m_bChangeDepth = true; }

	I3UIRENDER_LAYERINFO* GetCurrentLastLayer() const { return m_pLastLayer; }
	INT32				  GetCurrentDrawDepth() const { return m_DrawDepth; }

	void				SetCurrentLastLayer(I3UIRENDER_LAYERINFO* pInfo) { m_pLastLayer = pInfo; }
	void				SetCurrentDrawDepth(INT32 depth) {	m_DrawDepth = depth; m_bChangeDepth = true;	}



#if defined( I3_DEBUG)
	void				SaveVRAMCache( const char * pszPath)		{ m_pVTex->SaveVRAMCache( pszPath); }
	void				DumpVirtualTexture(void)					{ m_pVTex->DumpVirtualTexture(); }
	void				SaveFontCache( const char * pszPath);
#endif
	void				DumpSprite( void);


	static i3UIRenderer *	getInstance(void);
};

extern i3UIRenderer *	g_pUIRenderer;

#endif

