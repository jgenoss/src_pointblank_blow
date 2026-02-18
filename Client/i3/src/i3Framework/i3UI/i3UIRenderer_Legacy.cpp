#include "i3FrameworkPCH.h"
#include "i3UIRenderer.h"

//#define PROFILE_LEGACY

bool i3UIRenderer::_CreateVertexArrays_Legacy(void)
{
	i3VertexFormat fmt;

	I3_SAFE_RELEASE( m_pGeoVA[0]);
	I3_SAFE_RELEASE( m_pGeoVA[1]);

	// 16bit index를 지원하는 GPU에서는 이게 최대값
	I3ASSERT( m_MaxSprite < 16384);

	I3G_USAGE usage = I3G_USAGE_FREQUENT_UPDATE | I3G_USAGE_WRITEONLY;

	//////////////////////////
	// 사각형용 Vertex Array
	{
		fmt.SetHasPosition( true);
		fmt.SetTextureCoordSetCount( 2);
		fmt.SetTextureCoordCount( 2);
		fmt.SetHasColor();


		m_pGeoVA[0] = i3VertexArray::new_object();
		m_pGeoVA[0]->Create( &fmt, 4 * m_MaxSprite, usage);

		m_pGeoVA[1] = i3VertexArray::new_object();
		m_pGeoVA[1]->Create( &fmt, 4 * m_MaxSprite, usage);

	}

	{
		UINT32 cnt = 6 * m_MaxSprite;

		I3_SAFE_RELEASE( m_pGeoIA[0]);
		I3_SAFE_RELEASE( m_pGeoIA[1]);

		m_pGeoIA[0] = i3IndexArray::new_object();
		m_pGeoIA[1] = i3IndexArray::new_object();

		//그래픽카드의 인덱스 버퍼 지원 여부에 따라 셋팅 함
		if (i3RenderContext::GetCaps()->GetMaxVertexIndex() <= 0xFFFF )
		{
			m_pGeoIA[0]->Create( cnt, usage, I3G_IDX_16BITS);
			m_pGeoIA[1]->Create( cnt, usage, I3G_IDX_16BITS);
		}
		else
		{
			m_pGeoIA[0]->Create( cnt, usage, I3G_IDX_32BITS);
			m_pGeoIA[1]->Create( cnt, usage, I3G_IDX_32BITS);
		}
	}

	//////////////////////////
	// Instancing용 Vertex Array
	{
		I3_SAFE_RELEASE( m_pInstanceVA);
	}

	I3TRACE( "ReCreate VA for UI legacy\n");

	return true;
}

//static bool s_bSaveVRAM = false;

void i3UIRenderer::_SetSpriteMesh( INT32 buffIdx, i3UISprite * pSprite, INT32 idxVtx, INT32 idxIdx, REAL32 depth, I3VT_TEX_PARAM * pTexInfo, bool bFont)
{
	REAL32 x0, y0, x1, y1;
	VEC3D pos[4];
	VEC2D uv[4], texRange[4];

	i3VertexArray * pVA = m_pGeoVA[buffIdx];

	m_pGeoIA[buffIdx]->SetIndex( idxIdx + 0, (idxVtx + 0));
	m_pGeoIA[buffIdx]->SetIndex( idxIdx + 1, (idxVtx + 2));
	m_pGeoIA[buffIdx]->SetIndex( idxIdx + 2, (idxVtx + 1));
	m_pGeoIA[buffIdx]->SetIndex( idxIdx + 3, (idxVtx + 1));
	m_pGeoIA[buffIdx]->SetIndex( idxIdx + 4, (idxVtx + 2));
	m_pGeoIA[buffIdx]->SetIndex( idxIdx + 5, (idxVtx + 3));

	// Center
	VEC3D * pCenter = pSprite->getCenter();
	VEC3D * pSize = pSprite->getSizeAndRotate();
	REAL32 hpx = 0.5f;

	x0 = (pCenter->x) - hpx;
	x1 = (pCenter->x + pSize->x) - hpx;

	y0 = (pCenter->y + pSize->y) - hpx;
	y1 = (pCenter->y) - hpx;

	// Position
	i3Vector::Set( &pos[0], x0, y0, depth);
	i3Vector::Set( &pos[1], x1, y0, depth);
	i3Vector::Set( &pos[2], x0, y1, depth);
	i3Vector::Set( &pos[3], x1, y1, depth);
	pVA->SetPositions( idxVtx, 4, pos);

	// Color
	pVA->SetColor( idxVtx, 4, pSprite->getColor());

	// Texture Cooord
	{
		#define		TEXEL_OFFSET		0.0f
		REAL32 uSize, vSize;
		VEC4D * pTexCoord = pSprite->getTexCoord();

		if( bFont == false)
		{
			REAL32 baseX, baseY;

			baseX = (pTexCoord->z * m_pVTex->getWidth()) - pTexInfo->m_BaseX;
			baseY = (pTexCoord->w * m_pVTex->getHeight()) - pTexInfo->m_BaseY;
			uSize = pTexCoord->x * m_pVTex->getWidth();
			vSize = pTexCoord->y * m_pVTex->getHeight();

			x0 = (baseX + TEXEL_OFFSET) * pTexInfo->m_RCacheWidth;
			x1 = (baseX + uSize) * pTexInfo->m_RCacheWidth;

			y0 = (baseY + vSize) * pTexInfo->m_RCacheHeight;
			y1 = (baseY + TEXEL_OFFSET) * pTexInfo->m_RCacheHeight;
		}
		else
		{
			uSize = (REAL32)((INT32)pTexCoord->x % MAX_FONT_TEX_SIZE);
			vSize = (REAL32)((INT32)pTexCoord->y % MAX_FONT_TEX_SIZE);

			x0 = (((INT32)pTexCoord->z % MAX_FONT_TEX_SIZE) + TEXEL_OFFSET) * pTexInfo->m_RCacheWidth;
			x1 = (((INT32)pTexCoord->z % MAX_FONT_TEX_SIZE) + uSize) * pTexInfo->m_RCacheWidth;

			y0 = (((INT32)pTexCoord->w % MAX_FONT_TEX_SIZE) + vSize) * pTexInfo->m_RCacheHeight;
			y1 = (((INT32)pTexCoord->w % MAX_FONT_TEX_SIZE) + TEXEL_OFFSET) * pTexInfo->m_RCacheHeight;
		}

		i3Vector::Set( &uv[0], x0,	y0);
		i3Vector::Set( &uv[1], x1,	y0);
		i3Vector::Set( &uv[2], x0,	y1);
		i3Vector::Set( &uv[3], x1,	y1);

		pVA->SetTextureCoords( 0, idxVtx, 4, uv);

		i3Vector::Set( &texRange[0], pSprite->getTexRange()->y, pSprite->getTexRange()->z);
		i3Vector::Set( &texRange[1], pSprite->getTexRange()->y, pSprite->getTexRange()->z);
		i3Vector::Set( &texRange[2], pSprite->getTexRange()->y, pSprite->getTexRange()->z);
		i3Vector::Set( &texRange[3], pSprite->getTexRange()->y, pSprite->getTexRange()->z);

		pVA->SetTextureCoords( 1, idxVtx, 4, texRange);
	}
}

INT32 i3UIRenderer::_UpdateAllInstance_Legacy( LEGACY_DRIVE_CTX * pCtx)
{
	REAL32 depthStep;
	INT32 i, idxVtx, idxIdx;
	INT32 lockOffset;

	if( m_pGeoVA[0] == nullptr)
		return 0;

	depthStep = 1.0f / m_MaxSprite;
	
	lockOffset = pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_offset;
	idxVtx = lockOffset * 4;
	idxIdx = lockOffset * 6;

	//m_pGeoVA[m_idxShow]->Lock( I3G_LOCK_OVERWRITE, lockOffset * 4, pCtx->m_Count * 4);
	//m_pGeoIA[m_idxShow]->Lock( lockOffset * 6, pCtx->m_Count * 6, I3G_LOCK_OVERWRITE);

	INT32 cntDraw = 0;

	// Alloc-List를 돌면서 차례로 Update
	{
		const I3UIRENDER_LAYERINFO* pLayer = pCtx->m_pLayer;
		const INT32 numLayerSprite = (INT32)pLayer->m_DrawSpriteList.size();

		for( i = 0; i < numLayerSprite ; ++i ) //pCtx->m_pLayer->m_Count;i++)
		{
			i3UISprite * pSprite = pCtx->m_pLayer->m_DrawSpriteList[i]; //pCtx->m_pLayer->m_idxStart + i];

			if( pSprite->isEnabled() == false || pSprite->isState( I3UI_SPRITE_STATE_SETTED) == false)
			{
				continue;
			}

			// Shader Model 2.0 이상을 사용하는 Normal Mode에서는 Font와 Sprite들을
			// 한꺼번에 Rendering할 수 있지만, Shader Model 1을 사용하는 Legacy Mode에서는
			// 이와 같이 한꺼번에 처리할 수 없다.
			// 즉, Sprite들을 Batch로 Rendering하거나, Font를 Batch Rendering하는 중에
			// 다른 종류로 전이가 발생하면, 한번 끊고 Draw를 해야한다.
			// Sprite Order를 맞추기 위해서는 위의 방법이 필수적이다.
			LEGACY_RENDERTYPE renderType = LEGACY_RENDER_SPRITE;

			if (pSprite->getTexRange()->x < 0.0f)
			{
				renderType = LEGACY_RENDER_FONT;
			}
			
			if( pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RenderType != renderType)
			{
				if( pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RenderType == LEGACY_RENDER_NA)
				{
					pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RenderType = renderType;

					if (renderType == LEGACY_RENDER_FONT)
						pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_idxFontTex = -1;//union에 의해 0값이 되지 않아야하므로 -1설정..
					else
						pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RefTexInfo = nullptr;
				}
				else
				{	// Render Context를 바꾸고 다음 것을 설정
					pCtx->SwapRender( cntDraw);
					cntDraw=0;
					i--;
					continue;
				}
			}
			
			if( renderType == LEGACY_RENDER_SPRITE)
			{
				// Virtual Texture Validation
				// Font가 아니라면...
				VEC4D * pTexCoord = pSprite->getTexCoord();

				// 해당 Sprite이 사용하는 Ref. Texture를 찾는다.
				I3VT_TEXINFO* CurrTexInfo = m_pVTex->GetRelatedRefTexture( pTexCoord->z, pTexCoord->w);

				if(CurrTexInfo == nullptr)
				{
					// Texture가 존재하지 않는 경우이다. Texture가 정리되었거나, Template가 사라진 경우에 발생할 수 있다.
					// Crash를 시키지 않기 위해, 그냥 0번 Ref Texture를 사용하도록 처리한다.
					CurrTexInfo = m_pVTex->getRefTexture(0);
				}

				if( pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RefTexInfo != CurrTexInfo)
				{
					if( pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RefTexInfo == nullptr)
					{
						// 처음으로 Ref Texture가 사용되는 상황.
						pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RefTexInfo = CurrTexInfo;

//						if (renderType == LEGACY_RENDER_FONT)
//							pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_idxFontTex = -1;//union에 의해 0값이 되지 않아야하므로 -1설정..
//						else
//							pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RefTexInfo = nullptr;

					}
					else 
					{
						// 지금까지 사용하려하던 Texture와 다르다.
						// 다른 Render Pass로 그려야 한다.
						pCtx->SwapRender( cntDraw);

						cntDraw=0;
						i--;
						continue;
					}
				}

				I3VT_TEXINFO * pTex = CurrTexInfo; // m_pVTex->getRefTexture(idxTex);

				pTex->m_bBlockAutoTrashTex = (pSprite->isStyle(I3UI_SPRITE_STYLE_BLOCK_AUTO_TRASH_TEX)) ? true : false;

				I3VT_TEX_PARAM texInfo;

				texInfo.m_BaseX			= (REAL32) pTex->m_X;
				texInfo.m_BaseY			= (REAL32) pTex->m_Y;
				texInfo.m_RCacheWidth	= 1.0f / pTex->m_Width;
				texInfo.m_RCacheHeight	= 1.0f / pTex->m_Height;

				// 해당 Ref. Texture가 VRAM에 올라갈 수 있는가를 확인한다.
				if( !m_pVTex->Validate_Legacy( pTex, pSprite->getTexRange()))
				{
					// VRAM에 할당을 실패한 경우.
					I3PRINTLOG(I3LOG_FATAL,  "load vram fail!!!");
					goto ExitPart;
				}

				if( pTex->m_bBlankTexture == false)
				{
					_SetSpriteMesh( m_idxShow, pSprite, idxVtx, idxIdx, 0.0f, &texInfo, false);

					idxVtx += 4;
					idxIdx += 6;

					cntDraw++;
				}
			}
			else	// font
			{
				VEC4D * pTexCoord = pSprite->getTexCoord();

				INT32 idxTex = m_pTextSys->FindFontTextureIndex( pTexCoord->z, pTexCoord->w, 0, 0);
				I3ASSERT( idxTex != -1);
												
				if( pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_idxFontTex != idxTex)
				{
					if( pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_idxFontTex == -1)
					{
						// 처음으로 Ref Texture가 사용되는 상황.
						pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_idxFontTex = idxTex;

//						if (renderType == LEGACY_RENDER_FONT)
//							pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_idxFontTex = -1;//union에 의해 0값이 되지 않아야하므로 -1설정..
//						else
//							pCtx->m_RenderPrimitite[pCtx->m_CurrentPrim].m_RefTexInfo = nullptr;

					}
					else 
					{
						// 지금까지 사용하려하던 Texture와 다르다.
						// 다른 Render Pass로 그려야 한다.
						pCtx->SwapRender( cntDraw);

						cntDraw=0;
						i--;
						continue;
					}
				}

				_SetSpriteMesh( m_idxShow, pSprite, idxVtx, idxIdx, 0.0f, &m_fontTexInfo, true);

				idxVtx += 4;
				idxIdx += 6;

				cntDraw++;
			}
		}
	}

ExitPart:
	if( cntDraw > 0)
		pCtx->SwapRender( cntDraw);

	return pCtx->m_pLayer->m_DrawSpriteList.size(); // m_Count;
}

void i3UIRenderer::_Render_Legacy( i3RenderContext * pRTCtx, LEGACY_DRIVE_CTX * pCtx, INT32 idxPrim)
{
	if( pCtx->m_RenderPrimitite[ idxPrim].m_Count == 0)
		return;

	// MinFilter 적용시 내부에서 강제로 옵션을 설정하게 됩니다.
	// UI는 Point filter를 사용해야 되므로 잠시 설정을 끕니다.
	I3G_TEXTURE_FILTER tempFilter = pRTCtx->getGfxOption()->getForcedMipmapFilter();
	pRTCtx->getGfxOption()->setForcedMipmapFilter( I3G_TEXTURE_FILTER_NONE);

	// Shader Parameter 설정.
	// 
	{
		i3ShaderParam * pParam = m_pShader->FindParam( I3G_SHADER_TYPE_VERTEX, "g_mUIProj");

		// Projection Matrix
		pParam->Update( &m_mtxProj);

		///////////////////////////////
		// Pixel Shader Parameters
		if( m_pVTex != nullptr)
		{
			I3ASSERT( pCtx->m_RenderPrimitite[idxPrim].m_RefTexInfo != nullptr);

			I3VT_TEXINFO * pTex = pCtx->m_RenderPrimitite[idxPrim].m_RefTexInfo;
				// m_pVTex->getRefTexture( pCtx->m_RenderPrimitite[idxPrim].m_idxRefTex);

			I3ASSERT( pTex->m_pWeakTex != nullptr);
			I3ASSERT( pTex->m_pWeakTex->m_pRealTex != nullptr);

			m_ctxCacheTex.m_pTexture = pTex->m_pWeakTex->m_pRealTex; //(i3Texture *) pTex->m_pTex;

			pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texCache");
			if( pParam != nullptr)		
			{
				m_ctxCacheTex.m_ModifyFlag = 0xFFFFFFFF;
				pParam->Update( &m_ctxCacheTex);
			}
		}
	}

	{
		pRTCtx->SetFaceCullMode( I3G_FACE_CULL_NONE);
		pRTCtx->SetVertexArray( m_pGeoVA[m_idxShow]);
		pRTCtx->SetIndexArray( m_pGeoIA[m_idxShow]);
		pRTCtx->SetShader( m_pShader);

		INT32 offset = pCtx->m_RenderPrimitite[ idxPrim].m_offset * 6;

		pRTCtx->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, offset, pCtx->m_RenderPrimitite[ idxPrim].m_Count * 2);
	}

	{
		pRTCtx->SetShader( nullptr);
		pRTCtx->getGfxOption()->setForcedMipmapFilter( tempFilter);
	}
}

void i3UIRenderer::_Render_Font( i3RenderContext * pRTCtx, LEGACY_DRIVE_CTX * pCtx, INT32 idxPrim)
{
	if( pCtx->m_RenderPrimitite[idxPrim].m_Count == 0)
		return;

	if( m_pGeoVA[ m_idxShow] == nullptr)
		return;

	// Shader Parameter 설정.
	// 
	{
		i3ShaderParam * pParam = m_pShaderFont->FindParam( I3G_SHADER_TYPE_VERTEX, "g_mUIProj");

		// Projection Matrix
		pParam->Update( &m_mtxProj);

		///////////////////////////////
		// Pixel Shader Parameters
		//
		// Font Parameters
		{

			I3ASSERT( pCtx->m_RenderPrimitite[idxPrim].m_idxFontTex != -1);

			i3Texture * pFontTex = m_pTextSys->GetFontTexture( pCtx->m_RenderPrimitite[idxPrim].m_idxFontTex);

			m_ctxFontTex.m_pTexture = pFontTex;

			pParam = m_pShaderFont->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texFont0");
			if( pParam != nullptr)
			{
				m_ctxFontTex.m_ModifyFlag = 0xFFFFFFFF;
				pParam->Update( &m_ctxFontTex);
			}

			// Factors
			VEC4D factor;

			i3Vector::Set( &factor, (REAL32)pFontTex->GetWidth(), (REAL32) pFontTex->GetHeight(),
							1.0f / pFontTex->GetWidth(), 1.0f / pFontTex->GetHeight());

			pParam = m_pShaderFont->FindParam( I3G_SHADER_TYPE_PIXEL, "g_vFontFactor");
			if( pParam != nullptr)
				pParam->Update( &factor);
		}
	}

	{
		pRTCtx->SetFaceCullMode( I3G_FACE_CULL_NONE);
		pRTCtx->SetVertexArray( m_pGeoVA[m_idxShow]);
		pRTCtx->SetIndexArray( m_pGeoIA[m_idxShow]);
		pRTCtx->SetShader( m_pShaderFont);

		INT32 offset = pCtx->m_RenderPrimitite[ idxPrim].m_offset * 6;

		pRTCtx->DrawIndexedPrim( m_pShaderFont, I3G_PRIM_TRILIST, offset, pCtx->m_RenderPrimitite[ idxPrim].m_Count * 2);
	}

	{
		pRTCtx->SetShader( nullptr);
	}
}

static I3G_COMP_FUNC oldZFunc = I3G_COMP_LEQUAL;
static bool oldZTest = true;
static bool oldZWrite = true;

void i3UIRenderer::_RenderProc_Start( i3SceneTracer * pTracer, i3RenderContext * pContext)
{
	i3UIRenderer * pRenderer = g_pUIRenderer;
	I3ASSERT( pRenderer != nullptr);

	if ( pContext->IsRenderFlag( I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) == false )
	{
		if( pRenderer->m_bLegacyMode)
		{
			// Legacy Mode인 경우, Sprite의 Drawing Order가 순서와는 매우 다르게 되기 때문에
			// 올바른 출력을 위해, Depth Buffering을 이용한다.
			pContext->Clear( I3G_CLEAR_DEPTH, I3G_CLEAR_COLOR);
			oldZFunc	= pContext->GetZFunction();
			oldZTest	= pContext->GetZTestEnable();
			oldZWrite	= pContext->GetZWriteEnable();

			pContext->SetZFunction( I3G_COMP_LEQUAL);
			pContext->SetZTestEnable(true);
			pContext->SetZWriteEnable(true);
		}
	}
}


/*static*/ void i3UIRenderer::_RenderLayerProc_Legacy( i3RenderContext * pContext, I3UIRENDER_LAYERINFO * pLayer)
{
	INT32 i, j, cnt;
	LEGACY_DRIVE_CTX ctx;

#if !defined( I3_NO_PROFILE)
	INT64 s, e;
	INT64 ts;
	ts = i3Timer::GetPerformanceTime();
#endif

	i3UIRenderer * pRenderer = g_pUIRenderer;
	I3ASSERT( pRenderer != nullptr);

	if( pRenderer->m_cntAlloc <= 0)
		return;

	if( pRenderer->m_pVTex == nullptr)
		return;

	UILOG( "----- UI(Legacy) Render ----\n");

	const INT32 numLayerSpriteCount = (INT32) pLayer->m_DrawSpriteList.size();

	pRenderer->InitRender( numLayerSpriteCount); //pLayer->m_Count);

	ctx.Reset( pLayer );

	pRenderer->_Transfer();

	for( i = 0; i < numLayerSpriteCount; ) // i < pLayer->m_Count;)
	{
		// Instance Information 설정.
		{
		#if !defined( I3_NO_PROFILE)
			s = i3Timer::GetPerformanceTime();
		#endif

			cnt = pRenderer->_UpdateAllInstance_Legacy( &ctx);

		#if !defined( I3_NO_PROFILE)
			e = i3Timer::GetPerformanceTime();
			pRenderer->m_tickUpdate += e - s;
		#endif

		if( cnt > 0)
			i += cnt;
		else
			i++;
		}
	}

	pRenderer->ReadyRender();

	// Update가 완료되어 있는 Instance를 Rendering.
	//
	// 이전 Frame에 Update되어 있는 Instance 정보를 바탕으로 Rendering을 시작한다.
	// Drawing과 Update를 Overlapping 시키기 위한...
	for( j = 0; j < ctx.m_CurrentPrim; j++)
	{
	#if !defined( I3_NO_PROFILE)
		s = i3Timer::GetPerformanceTime();
	#endif

		if( ctx.m_RenderPrimitite[j].m_RenderType == LEGACY_RENDER_SPRITE)
		{
			pRenderer->_Render_Legacy( pContext, &ctx, j);
			UILOG( "-- Render_Legacy\n");
		}
		else
		{
			pRenderer->_Render_Font( pContext, &ctx, j);
			UILOG( "-- Render_Font\n");
		}

	#if !defined( I3_NO_PROFILE)
		e = i3Timer::GetPerformanceTime();
		pRenderer->m_tickRender += e - s;
		pRenderer->m_cntDrawPass ++;
	#endif
	}

	pRenderer->EndRender();

	UILOG( "----- UI(Legacy) Render Finished ----\n");

#if !defined( I3_NO_PROFILE)
	pRenderer->m_tickTotal = i3Timer::GetPerformanceTime() - ts;
#endif
}

/*static*/ void i3UIRenderer::_RenderProc_Finish( i3SceneTracer * pTracer, i3RenderContext * pContext)
{
	// Restore
	i3UIRenderer * pRenderer = g_pUIRenderer;
	I3ASSERT( pRenderer != nullptr);


	if ( pContext->IsRenderFlag( I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) == false )
	{
		if( pRenderer->m_bLegacyMode)
		{
			pContext->SetZFunction( oldZFunc);
			pContext->SetZTestEnable( oldZTest);
			pContext->SetZWriteEnable( oldZWrite);
		}
	}

	pRenderer->_ResetLayer();

#ifdef PROFILE_LEGACY
	RESET_PROFILE();
	REPORT_PROFILE();
#endif
}

