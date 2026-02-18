#include "i3FrameworkPCH.h"
#include "i3UIRenderer.h"
#include "../i3Framework.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE( i3UIRenderer);

i3UIRenderer *		g_pUIRenderer = nullptr;

#define		SYNC_RENDER

void LEGACY_DRIVE_CTX::SwapRender( INT32 count)
{
	//I3ASSERT( count > 0);

	m_RenderPrimitite[m_CurrentPrim].m_Count = count;

	INT32 offset = m_RenderPrimitite[m_CurrentPrim].m_offset + count;

	m_CurrentPrim++;
	I3ASSERT( m_CurrentPrim < UI_RENDER_PRIM_COUNT);

	m_RenderPrimitite[m_CurrentPrim].m_offset = offset;
}

i3UIRenderer::i3UIRenderer(void)
{
	g_pUIRenderer = this;
	
	
	i3mem::FillZero( &m_ctxCacheTex, sizeof(m_ctxCacheTex));
	i3mem::FillZero( &m_ctxFontTex, sizeof(m_ctxFontTex));
	i3mem::FillZero( &m_ctxIndirectTex, sizeof(m_ctxIndirectTex));
	
	m_LayerStorage.reserve( 128);
	m_TopDrawLayerList.reserve( 128);
}

i3UIRenderer::~i3UIRenderer(void)
{
	I3_SAFE_RELEASE( m_pTextSys);
	I3_SAFE_RELEASE( m_pVTex);
	I3_SAFE_RELEASE( m_pGeoVA[0]);
	I3_SAFE_RELEASE( m_pGeoVA[1]);
	I3_SAFE_RELEASE( m_pGeoIA[0]);
	I3_SAFE_RELEASE( m_pGeoIA[1]);
	I3_SAFE_RELEASE( m_pInstanceVA);

	const size_t storage_size = m_LayerStorage.size();
	for (size_t i = 0 ; i < storage_size ; ++i)
	{
		I3UIRENDER_LAYERINFO* info = m_LayerStorage[i];
		I3_MUST_RELEASE( info->m_pNodeSprite);
		I3_MUST_RELEASE( info->m_pInstanceNode);			// 부모연결과는 별도의 레퍼런스 한개를 갖고 있으며, 반드시 릴리즈처리한다..
		I3_SAFE_DELETE( info);
	}

	I3_SAFE_RELEASE(m_pShader);
	I3_SAFE_RELEASE(m_pShaderFont);

	I3_SAFE_NODE_RELEASE( m_pRoot);

//	INT32 i;
//	for( i = 0; i < UI_RENDER_LAYER_COUNT; i++)
//	{
//		_FreeLayer( i);
//	}

	I3_SAFE_DELETE_ARRAY( m_pSprites);

	if( m_pVBDecl != nullptr)
	{
		IDirect3DVertexDeclaration9 * pDecl = (IDirect3DVertexDeclaration9 *) m_pVBDecl;
		pDecl->Release();
		m_pVBDecl = nullptr;
	}

	#if defined( USE_DRAWUP)
	I3MEM_SAFE_FREE( m_pDrawVA);
	I3MEM_SAFE_FREE( m_pDrawIA);
	I3_SAFE_RELEASE( m_pDrawVADecl);
	#endif

	g_pUIRenderer = nullptr;

}

void i3UIRenderer::_SetResolution( REAL32 w, REAL32 h)
{


	REAL32 ratioX, ratioY;
	REAL32 ax, ay, tx, ty;
	i3UIManager * pManager = i3UI::getManager();
	REAL32 offX, offY;

	// 실제 해상도
	m_ScreenWidth = w;
	m_ScreenHeight = h;

	// UI가 제작된 목표 해상도
	tx = (REAL32) pManager->getWidth();
	ty = (REAL32) pManager->getHeight();

	// 실제 해상도에 맞추어 Viewing될 해상도 (목표 해상도의 Aspect Ratio가 유지되도록 조정된다.
	ax = (REAL32) pManager->getActualWidth();
	ay = (REAL32) pManager->getActualHeight();

	if( m_bStretch)
	{
		ratioX = 1.f;
		ratioY = 1.f;
	}
	else
	{
		ratioX = ax / w;
		ratioY = ay / h;
	}

	offX = ((w - ax)) / w;
	offY = ((h - ay)) / h;

	i3Matrix::OrthoRH( &m_mtxProj, tx, ty, 0.0f, 1.0f);
	i3Matrix::PostScale( &m_mtxProj, ratioX, -ratioY, 1.0f);
	i3Matrix::PostTranslate( &m_mtxProj, -1.0f + offX, 1.0f - offY, 0.0f);

	m_pProjAttr->SetMatrix( &m_mtxProj);

	if( m_bLegacyMode)
	{
		if( ay != ty)
		{
			m_ctxCacheTex.m_MagFilter = I3G_TEXTURE_FILTER_LINEAR;
			m_ctxCacheTex.m_MinFilter = I3G_TEXTURE_FILTER_LINEAR;
		}
		else
		{
			m_ctxCacheTex.m_MagFilter = I3G_TEXTURE_FILTER_POINT;
			m_ctxCacheTex.m_MinFilter = I3G_TEXTURE_FILTER_POINT;
		}
	}

	_SelectShader();
}

void i3UIRenderer::_SelectShader(void)
{


	I3_SAFE_RELEASE( m_pShader);
	I3_SAFE_RELEASE( m_pShaderFont);

	if( m_bLegacyMode)
	{
		m_pShader = i3ShaderCache::LoadShader( "i3UIRender_Legacy.hlsl");

		m_pShaderFont = i3ShaderCache::LoadShader( "i3UIRender_Font.hlsl");
		if( m_pShaderFont == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN,  "Could not load a font shader which is used for rendering UIs.");
			return;
		}
		I3_SAFE_ADDREF( m_pShaderFont);
	}
	else
	{
		i3UIManager * pManager = i3UI::getManager();

		if( ( pManager->getWidth() < pManager->getActualWidth()) ||
			( pManager->getHeight() < pManager->getActualHeight()))
		{
			// 화면이 확대된다.
			m_pShader = i3ShaderCache::LoadShader( "i3UIRender_Ext.hlsl");
		}
		else
		{
			m_pShader = i3ShaderCache::LoadShader( "i3UIRender.hlsl");
		}
	}

	if( m_pShader == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not load a shader which is used for rendering UIs.");
		return;
	}

	I3_SAFE_ADDREF( m_pShader);
}


bool i3UIRenderer::Create( i3Framework * pFramework, INT32 maxPrim, const bool& bLegacy)
{


	I3ASSERT( m_pSprites == nullptr);
	I3ASSERT( maxPrim > 0);

	m_MaxSprite = maxPrim;

	//m_DrawSpriteList.reserve( m_MaxSprite);

	// Legacy mode
	{
		i3GfxCaps * pCaps = g_pRenderContext->GetCaps();

		if( pCaps->getPixelShaderVersionMajor() < 2)
			m_bLegacyMode = true;
		else
			m_bLegacyMode = false;

		if( pCaps->getVertexShaderVersionMajor() < 3)
			m_bLegacyMode = true;

		if( g_pRenderContext->CheckSupportedFormat( I3G_IMAGE_FORMAT_ABGR_16F, 0) == false)
			m_bLegacyMode = true;

		// legacy mode로 작동하도록 합니다.
		// VT는 현재 버그가 있음..
		// UI툴에서 버그가 있어.. 선택 가능 하도록 변경..
		if (true == bLegacy)
			m_bLegacyMode = true;
		else
			m_bLegacyMode = false;
	}

	// Shader
	{
		_SelectShader();
	}

	m_pSprites = new i3UISprite[ maxPrim];

	NetworkDump_Malloc( m_pSprites, sizeof( i3UISprite) * maxPrim, __FILE__, __LINE__);

	m_FreeSpriteList.resize( m_MaxSprite);
	for( INT32 i = 0; i < m_MaxSprite; i++)
		m_FreeSpriteList[i] = i;

	{
		m_pRoot = i3AttrSet::new_object();

		{
			i3ViewMatrixAttr * pAttr = i3ViewMatrixAttr::new_object_ref();

			m_pRoot->AddAttr( pAttr);
		}

		{
			m_pProjAttr = i3ProjectMatrixAttr::new_object_ref();

			m_pRoot->AddAttr( m_pProjAttr);
		}

		{
			i3UserNode * pUserNode = i3UserNode::new_object_ref();
			pUserNode->SetCallBack( (I3USERNODEPROC) _RenderProc_Start, g_pRenderContext);
			m_pRoot->AddChild( pUserNode);
		}


		m_pLayerRoot = i3Node::new_object_ref();	
		m_pRoot->AddChild(m_pLayerRoot);			// 실제 세부 정보는 레이어루트에 실시간으로 넣고 뺀다...넣을때는 AddChild이고 
													// 뺄때는 RemoveAllChild	

		{
			i3UserNode * pFinishNode = i3UserNode::new_object_ref();
			pFinishNode->SetCallBack( (I3USERNODEPROC) _RenderProc_Finish, g_pRenderContext);
			m_pRoot->AddChild( pFinishNode);
		}

#if defined( I3_DEBUG)
		m_pRoot->SetName( "UIRenderRoot");
#endif	

		
		// Attach
		pFramework->Get2DRoot()->InsertChild( 0, m_pRoot);

	}

	{
		m_pTextSys = i3UITextManager::new_object();
		m_pTextSys->Create( m_bLegacyMode);

		m_fontTexInfo.m_BaseX			= 0.0f;
		m_fontTexInfo.m_BaseY			= 0.0f;
		m_fontTexInfo.m_RCacheWidth		= 1.0f / MAX_FONT_TEX_SIZE;
		m_fontTexInfo.m_RCacheHeight	= 1.0f / MAX_FONT_TEX_SIZE;
	}

	{
		// Cache Texture
		m_ctxCacheTex.m_bEnable = true;
		m_ctxCacheTex.m_BindType = I3G_TEXTURE_BIND_DIFFUSE;
		m_ctxCacheTex.m_BorderColor = 0;
		m_ctxCacheTex.m_CoordCount =2;
		m_ctxCacheTex.m_bProjective = false;
		m_ctxCacheTex.m_HWrap	= I3G_TEXTURE_WRAP_CLAMP;
		m_ctxCacheTex.m_MagFilter = I3G_TEXTURE_FILTER_POINT;
		m_ctxCacheTex.m_MinFilter = I3G_TEXTURE_FILTER_POINT;
		m_ctxCacheTex.m_VWrap	= I3G_TEXTURE_WRAP_CLAMP;

		// Redirect Texrure
		m_ctxIndirectTex.m_bEnable = true;
		m_ctxIndirectTex.m_BindType = I3G_TEXTURE_BIND_DIFFUSE;
		m_ctxIndirectTex.m_BorderColor = 0;
		m_ctxIndirectTex.m_CoordCount = 2;
		m_ctxIndirectTex.m_bProjective = false;
		m_ctxIndirectTex.m_HWrap	= I3G_TEXTURE_WRAP_CLAMP;
		m_ctxIndirectTex.m_MagFilter = I3G_TEXTURE_FILTER_POINT;
		m_ctxIndirectTex.m_MinFilter = I3G_TEXTURE_FILTER_POINT;
		m_ctxIndirectTex.m_VWrap	= I3G_TEXTURE_WRAP_CLAMP;

		// Font Texrure
		m_ctxFontTex.m_bEnable = true;
		m_ctxFontTex.m_BindType = I3G_TEXTURE_BIND_DIFFUSE;
		m_ctxFontTex.m_BorderColor = 0;
		m_ctxFontTex.m_CoordCount = 2;
		m_ctxFontTex.m_bProjective = false;
		m_ctxFontTex.m_HWrap	= I3G_TEXTURE_WRAP_CLAMP;
		m_ctxFontTex.m_MagFilter = I3G_TEXTURE_FILTER_LINEAR;
		m_ctxFontTex.m_MinFilter = I3G_TEXTURE_FILTER_LINEAR;
		m_ctxFontTex.m_VWrap	= I3G_TEXTURE_WRAP_CLAMP;
	}

	Reset();

	{
		i3Viewer * pViewer = pFramework->GetViewer();

		_SetResolution( (REAL32) pViewer->GetViewWidth(), (REAL32) pViewer->GetViewHeight());
	}

	return _CreateVertexArrays();
}

bool i3UIRenderer::_CreateVertexArrays(void)
{


	if( m_bLegacyMode)
	{
		return _CreateVertexArrays_Legacy();
	}

	return _CreateVertexArrays_Instancing();
}

bool i3UIRenderer::_CreateVertexArrays_Instancing(void)
{


	i3VertexFormat fmt;

	I3_SAFE_RELEASE( m_pGeoVA[0]);

	//////////////////////////
	// 사각형용 Vertex Array
	{
		VEC3D pos[6];
		VEC2D uv[4];

		fmt.SetHasPosition( true);
		fmt.SetTextureCoordSetCount( 1);
		fmt.SetTextureCoordCount( 2);

		m_pGeoVA[0] = i3VertexArray::new_object();
		m_pGeoVA[0]->Create( &fmt, 4, 0);
		m_pGeoVA[0]->Lock();

		i3Vector::Set( &pos[0], 0.0f, 1.0f, 1.0f);
		i3Vector::Set( &pos[1], 0.0f, 0.0f, 1.0f);
		i3Vector::Set( &pos[2], 1.0f, 1.0f, 1.0f);
		i3Vector::Set( &pos[3], 1.0f, 0.0f, 1.0f);

		i3Vector::Set( &uv[0], 0.0f, 1.0f);
		i3Vector::Set( &uv[1], 0.0f, 0.0f);
		i3Vector::Set( &uv[2], 1.0f, 1.0f);
		i3Vector::Set( &uv[3], 1.0f, 0.0f);

		m_pGeoVA[0]->SetPositions( 0, 4, pos);

		m_pGeoVA[0]->SetTextureCoords( 0, 0, 4, uv);

		m_pGeoVA[0]->Unlock();
	}

	{
		I3_SAFE_RELEASE( m_pGeoIA[0]);

		m_pGeoIA[0] = i3IndexArray::new_object();
		m_pGeoIA[0]->Create( 6, 0, m_pGeoVA[0]->GetCount());

		m_pGeoIA[0]->Lock();

		m_pGeoIA[0]->SetIndex( 0, 0);
		m_pGeoIA[0]->SetIndex( 1, 1);
		m_pGeoIA[0]->SetIndex( 2, 2);

		m_pGeoIA[0]->SetIndex( 3, 2);
		m_pGeoIA[0]->SetIndex( 4, 1);
		m_pGeoIA[0]->SetIndex( 5, 3);

		m_pGeoIA[0]->Unlock();
	}

	//////////////////////////
	// Instancing용 Vertex Array
	{
		fmt.SetHasPosition( false);
		fmt.SetTextureCoordSetCount( 4);
		fmt.SetTextureCoordCount( 4);
		fmt.SetHasColor( true);

		I3_SAFE_RELEASE( m_pInstanceVA);

		m_pInstanceVA = i3VertexArray::new_object();
		m_pInstanceVA->Create( &fmt, m_MaxSprite * 2, I3G_USAGE_FREQUENT_UPDATE | I3G_USAGE_WRITEONLY);
	}

	if(  m_pVBDecl == nullptr)
	{
		m_pVBDecl = i3VertexArrayDX::GetCombinedVBDecl( m_pGeoVA[0]->GetFormat(), m_pInstanceVA->GetFormat());
	}

	return true;
}

void i3UIRenderer::Reset(void)
{

	INT32 i, idxPrev, idxNext;

	idxPrev = -1;

	// 모든 Sprite들을 Free List로 구성한다.
	for( i = 0; i < m_MaxSprite; i++)
	{
		idxNext = i + 1;
		if( idxNext >= m_MaxSprite)
			idxNext = -1;

		m_pSprites[i].removeState( I3UI_SPRITE_STATE_ALL);
		m_pSprites[i].setPrev( idxPrev);
		m_pSprites[i].setNext( idxNext);
		m_pSprites[i].setModifyNext( -1);
		m_pSprites[i].setInstanceIndex( -1);

		idxPrev = i;
		
	}

	m_idxAllocHead = -1;
	m_idxAllocTail = -1;

	m_idxFreeHead = 0;
	m_idxModifyHead = -1;
}

void i3UIRenderer::OnUpdate( REAL32 rDeltaSeconds)
{


	//_Reset();

	i3GameObj::OnUpdate( rDeltaSeconds);
}

void i3UIRenderer::_AddToModifyList( INT32 idx)
{


	i3UISprite * pSprite;

	pSprite = getSprite( idx);

	I3ASSERT( pSprite->isModified() == false);

	pSprite->setModifyNext( m_idxModifyHead);
	pSprite->setModified();

	m_idxModifyHead = idx;

}

void i3UIRenderer::_RemoveFromModifyList( INT32 idx)
{


	I3ASSERT( m_idxModifyHead != -1);
	i3UISprite * pSprite, * pTemp;
	INT32 idxTemp, idxPrev = -1;

	pSprite = getSprite( idx);

	// 삭제하려는 Sprite의 전(previous) Sprite을 검색.
	// Single-Linked-List로 관리되는 Modifiy List이기 때문에
	// 선두에서부터 순차적으로 찾을 수 밖에...
	idxTemp = m_idxModifyHead;

	while( ( idxTemp != idx) && (idxTemp != -1))
	{
		idxPrev = idxTemp;

		pTemp = getSprite( idxTemp);
		idxTemp = pTemp->getModifyNext();
	}

	// 찾았다.
	if( idxPrev != -1)
	{
		i3UISprite * pPrev = getSprite( idxPrev);

		pPrev->setModifyNext( pSprite->getModifyNext());
	}
	else
	{
		// idxPrev == -1 은 pSprite가 Modify List의 선두임을 의미한다.
		m_idxModifyHead = pSprite->getModifyNext();
	}
}

INT32 i3UIRenderer::AllocSprite( I3UI_SPRITE_STYLE style, i3ElementBase * pOwner)
{
	i3_prof_func();

	INT32 idxRet;
	i3UISprite * pSprite, * pTail;

	if( m_idxFreeHead == -1)
	{
//#if defined( I3_DEBUG)
		DumpSprite();
//#endif
		I3PRINTLOG(I3LOG_WARN,  "[UIRenderer] Not enough sprite!!!");
		return -1;
	}

	// Free-List에서 제거
	{
		idxRet = m_idxFreeHead;
		I3_BOUNDCHK( idxRet, m_MaxSprite);

#if defined( I3_DEBUG)
		// i3::vector 최신화 후 버그 발견..(2017.03.31)...삭제루틴이 수정되면 디버그 퍼포먼스가 조금 떨어짐..
	//	m_FreeSpriteList.erase( m_FreeSpriteList.begin() + m_idxFreeHead);
		int foundIdx = i3::vu::int_index_of(m_FreeSpriteList, m_idxFreeHead);
		if (foundIdx != -1)
			m_FreeSpriteList.erase(m_FreeSpriteList.begin() + foundIdx);
		else
			I3ASSERT(foundIdx != -1);
#endif

		pSprite = & m_pSprites[ idxRet];

		I3ASSERT( !pSprite->isState( I3UI_SPRITE_STATE_USED));

		m_idxFreeHead = pSprite->getNext();
	}

	// Alloc-List에 추가
	{
		// 그리는 순서를 고려 가장 마지막에 추가해야 한다.
		if( m_idxAllocTail != -1)
		{
			I3_BOUNDCHK( m_idxAllocTail, m_MaxSprite);

			// 이전 Alloc-Tail의 Next로 설정.
			pTail = & m_pSprites[ m_idxAllocTail];

			pTail->setNext( idxRet);
		}

		pSprite->setPrev( m_idxAllocTail);
		pSprite->setNext( -1);
		
		m_idxAllocTail = idxRet;

		if( m_idxAllocHead == -1)
			m_idxAllocHead = idxRet;
	}

	{
		pSprite->clearState();
		pSprite->setEnableState( false);
		pSprite->setStyle( style);
		pSprite->setNode( nullptr);
		pSprite->addState( I3UI_SPRITE_STATE_USED);

#if defined( I3_DEBUG)
		pSprite->SetOwner( pOwner);
#endif
	}

	m_cntAlloc++;

	I3ASSERT( m_cntAlloc <= m_MaxSprite);

	//_CheckList();

	return idxRet;
}

void i3UIRenderer::FreeSprite( INT32	hndl)
{
	i3_prof_func();

	i3UISprite * pSprite, * pTemp;
	INT32 idxPrev, idxNext;

	I3_BOUNDCHK( hndl, m_MaxSprite);

	pSprite = & m_pSprites[ hndl];

	I3ASSERT( pSprite->isState( I3UI_SPRITE_STATE_USED) );

	// Alloc-List에서 제거한다.
	{
		idxPrev = pSprite->getPrev();
		idxNext = pSprite->getNext();

		// Prev
		if( idxPrev != -1)
		{
			pTemp = getSprite( idxPrev);
			pTemp->setNext( idxNext);
		}

		// Next
		if( idxNext != -1)
		{
			pTemp = getSprite( idxNext);
			pTemp->setPrev( idxPrev);
		}

		// 만약 제거하는 것이 Alloc-Head 또는 Tail 이라면...
		if( m_idxAllocHead == hndl)
			m_idxAllocHead = idxNext;

		if( m_idxAllocTail == hndl)
			m_idxAllocTail = idxPrev;
	}

	// Free-Head에 추가.
	{

		pSprite->setPrev( -1);
		pSprite->setNext( m_idxFreeHead);
		
		m_idxFreeHead = hndl;

#if defined( I3_DEBUG)
		m_FreeSpriteList.push_back(m_idxFreeHead);
#endif
	}

	// Modify List에서 제거
	if( pSprite->isModified())
	{
		//_RemoveFromModifyList( hndl);
	}

	pSprite->setEnableState( false);
	pSprite->removeState( I3UI_SPRITE_STATE_USED);

	m_cntAlloc --;

	I3ASSERT( m_cntAlloc >= 0);

	//_CheckList();
}

//#if defined( I3_DEBUG)
void i3UIRenderer::DumpSprite( void)
{
	I3PRINTLOG(I3LOG_NOTICE,  "--------------------------------------------------------");
	I3PRINTLOG(I3LOG_NOTICE,  "------------------  Dump Sprite  -----------------------");
	I3PRINTLOG(I3LOG_NOTICE,  " - Total Count : %d", m_MaxSprite);
	I3PRINTLOG(I3LOG_NOTICE,  " - Free Count : %d", m_FreeSpriteList.size());

	for( UINT32 i = 0; i < m_AllocSpriteList.size(); i++)
	{
		INT32 idx = m_AllocSpriteList[i];
		I3ASSERT( idx < m_MaxSprite);

		I3ASSERT( m_pSprites[ idx].GetOwner() != nullptr);
		i3ElementBase * pObj = reinterpret_cast<i3ElementBase*>(m_pSprites[ idx].GetOwner());

		if( i3::kind_of<i3UIControl*>(pObj))
		{
			i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(pObj);
			if( pCtrl->hasName())
			{
				I3PRINTLOG(I3LOG_NOTICE,  " (%d) %s", i, pCtrl->GetName());
			}
			else
			{
				I3PRINTLOG(I3LOG_NOTICE,  " (%d) Unknown ctrl [%s]", i, pCtrl->meta()->class_name());
			}
		}
		else if( i3::same_of<i3UIText*>(pObj))
		{
		//	char szTemp[ 1024] = {0,};
		//  단순이 트레이스목적으로만 변환되기 때문에 비교적 불필요한 부하라고 볼수 있다.
			i3::string str;
			i3UIText * pText = static_cast<i3UIText*>(pObj);
			i3::utf16_to_mb(pText->getText(), str);
		//	i3String::CopyWCHARtoASCII( szTemp, sizeof(szTemp), pText->getText(), i3::generic_string_size( pText->getText()));
			I3PRINTLOG(I3LOG_NOTICE,  " (%d) Text ctrl - %s", i, str );

		}
		else
		{
			I3PRINTLOG(I3LOG_NOTICE,  " (%d) Unknown ctrl [%s]", i, pObj->meta()->class_name());
		}
	}

	I3PRINTLOG(I3LOG_NOTICE,  "--------------------------------------------------------");
}
//#endif

void i3UIRenderer::_unlink( INT32 idx)
{

	INT32 idxPrev, idxNext;
	i3UISprite * pTemp, * pSprite;

	pSprite = getSprite( idx);

	idxPrev = pSprite->getPrev();
	idxNext = pSprite->getNext();

	if( idxPrev != -1)
	{
		pTemp = getSprite( idxPrev);

		pTemp->setNext( idxNext);
	}
	else
	{
		m_idxAllocHead = idxNext;
	}

	if( idxNext != -1)
	{
		pTemp = getSprite( idxNext);

		pTemp->setPrev( idxPrev);
	}
	else
	{
		m_idxAllocTail = idxPrev;
	}

	pSprite->setPrev( -1);
	pSprite->setNext( -1);
}

void i3UIRenderer::_link( INT32 idxInsert, INT32 idx)
{


	i3UISprite * pPrev, * pTemp, * pSprite;
	INT32 idxNext;

	pPrev = getSprite( idxInsert);
	pSprite = getSprite( idx);

	// Next
	idxNext = pPrev->getNext();
	if( idxNext != -1)
	{
		pTemp = getSprite( idxNext);

		pTemp->setPrev( idx);
	}

	// Prev
	pPrev->setNext( idx);

	pSprite->setPrev( idxInsert);
	pSprite->setNext( idxNext);

	if( m_idxAllocTail == idxInsert)
	{
		m_idxAllocTail = idx;
	}
}

void i3UIRenderer::AdjustOrder( INT32 hSpriteEnd, INT32 * pSpriteArr, INT32 cnt)
{



	INT32 i, idx;

	for( i = 0; i < cnt; i++)
	{
		idx = pSpriteArr[ i];

		_unlink( idx);

		_link( hSpriteEnd, idx);

		hSpriteEnd = idx;
	}

	//_CheckList();
}

bool i3UIRenderer::isInAllocList( i3UISprite * pSprite)
{


	i3UISprite * pTemp;
	INT32 idx = m_idxAllocHead;

	while( idx != -1)
	{
		pTemp = getSprite( idx);
	
		if( pTemp == pSprite)
			return true;

		idx = pTemp->getNext();
	}

	return false;
}

bool i3UIRenderer::_CheckList(void)
{



	i3UISprite * pSprite;
	INT32 idx = m_idxFreeHead;


	while( idx != -1)
	{
		pSprite = getSprite( idx);

		if( isInAllocList( pSprite) == true)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Invalid list!!!!");
			return false;
		}

		idx = pSprite->getNext();
	}

	return true;
}

void i3UIRenderer::setPos( INT32 idx, VEC3D * pCenter)
{
	i3UISprite * pSprite = getSprite( idx );
	if( pSprite )
		pSprite->setCenter( pCenter);
}

void i3UIRenderer::setPos( INT32 idx, REAL32 x, REAL32 y, REAL32 z)
{
	i3UISprite * pSprite = getSprite( idx);
	if( pSprite )
		pSprite->setCenter( x, y, z);

	/*
	if( pSprite->isModified() == false)
		_AddToModifyList( idx);
		*/
}

void i3UIRenderer::setSizeAndRotate( INT32 idx, REAL32 w, REAL32 h, REAL32 rot)
{
	i3UISprite * pSprite = getSprite( idx);
	if( pSprite )
		pSprite->setSizeAndRotate( w, h, rot);

	/*
	if( pSprite->isModified() == false)
		_AddToModifyList( idx);
		*/
}

void i3UIRenderer::setTexCoord( INT32 idx, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{
	if( idx < 0)
		return;

	i3UISprite * pSprite = getSprite( idx);

	pSprite->setTexCoord( u1, v1, u2, v2);
	pSprite->removeState( I3UI_SPRITE_STATE_VALIDATED);

	if( m_pVTex != nullptr)
	{
		REAL32 tx, ty;
		INT32 sx, sy;

		tx = u1 * m_pVTex->getWidth();
		ty = u2 * m_pVTex->getHeight();

		sx = (INT32)(tx / I3VT_TILE_SIZE);
		sy = (INT32)(ty / I3VT_TILE_SIZE);

		INT32 idxTex = m_pVTex->getTileIndex( sx, sy);
		pSprite->setTexIndex( idxTex);
	}

	/*
	if( pSprite->isModified() == false)
		_AddToModifyList( idx);
		*/
}

void i3UIRenderer::setTexRange( INT32 idx, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{


	if( idx < 0)
		return;

	i3UISprite * pSprite = getSprite( idx);

	pSprite->setTexRange( u1, v1, u2, v2);
	pSprite->removeState( I3UI_SPRITE_STATE_VALIDATED);

	/*
	if( pSprite->isModified() == false)
		_AddToModifyList( idx);
		*/
}

void i3UIRenderer::setColor( INT32 idx, COLOR * pColor)
{
	if( idx < 0)
		return;

	i3UISprite * pSprite = getSprite( idx);

	pSprite->setColor( pColor);

	/*
	if( pSprite->isModified() == false)
		_AddToModifyList( idx);
		*/
}

void i3UIRenderer::setColor( INT32 idx, UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	COLOR c;

	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;

	setColor( idx, &c);
}

void i3UIRenderer::setEnableSprite( INT32 idx, bool bFlag)
{


	i3UISprite * pSprite = getSprite( idx);

	pSprite->setEnableState( bFlag);
}

void i3UIRenderer::SetTexture( i3VirtualTexture * pTex)
{

	I3_REF_CHANGE( m_pVTex, pTex);

	if( m_pVTex != nullptr)
	{
		if( m_pVTex->getCache() == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "To use a virtual texture, you have to call PrepareForRendering() function.");
		}
	}
}

bool i3UIRenderer::PrepareTextureForSprite(VEC4D * pCoord, VEC4D * pTexRange)
{


	if( m_bLegacyMode)
		return true;
	
	VEC4D rect;

	rect.x = pCoord->x;
	rect.y = pCoord->y;
	rect.z = pCoord->z - pCoord->x;
	rect.w = pCoord->w - pCoord->y;

	return m_pVTex->Validate( &rect, pTexRange, false);
}

void i3UIRenderer::InitRender( INT32 primCnt)
{
	m_DrawCountPerFrame = 0;

	m_DrawDepth = 0;

	if( m_bLegacyMode )
	{
		m_pGeoVA[m_idxShow]->Lock( I3G_LOCK_OVERWRITE, 0, primCnt * 4);
		m_pGeoIA[m_idxShow]->Lock( 0, primCnt * 6, I3G_LOCK_OVERWRITE);
	}
}

void i3UIRenderer::ReadyRender( void)
{
	if( m_bLegacyMode)
	{
		m_pGeoVA[m_idxShow]->Unlock();
		m_pGeoIA[m_idxShow]->Unlock();
	}
}

void i3UIRenderer::EndRender( void)
{
	m_idxShow = (m_idxShow + 1) & 1;		// 0, 1, 0, 1, 0, ...
}

bool i3UIRenderer::PrepareTextureForSprite( INT32 hSprite)
{

	if( m_bLegacyMode)
		return true;

	VEC4D rect, * pCoord;
	i3UISprite * pSprite = getSprite( hSprite);

	if( pSprite->isState( I3UI_SPRITE_STATE_SETTED) )
	{
		pCoord = pSprite->getTexCoord();
		rect.x = pCoord->z;			// u1
		rect.y = pCoord->w;			// v1
		rect.z = pCoord->x;			// (u2 - u1)
		rect.w = pCoord->y;			// (v2 - v1)

		return m_pVTex->Validate( &rect, pSprite->getTexRange(), false);
	}

	return false;
}

void i3UIRenderer::OnSize( INT32 w, INT32 h)
{
	_SetResolution( (REAL32) w, (REAL32) h);
}

void i3UIRenderer::OnResetVRAMCache(void)
{

	INT32 idx = m_idxAllocHead;
	i3UISprite * pTemp;

	// 모든 Sprite들의 Loading 상태를 Clear한다.
	while( idx != -1)
	{
		pTemp = getSprite( idx);

		pTemp->removeState( I3UI_SPRITE_STATE_VALIDATED);

		idx = pTemp->getNext();
	}
}

//static bool		s_bDump  = false;

INT32 i3UIRenderer::_UpdateAllInstance( I3UIRENDER_LAYERINFO * pLayer, INT32 idxStart, INT32 cnt)
{
	INT32 i;
	i3UISprite * pSprite;
	
	//m_idxShow = (m_idxShow + 1) & 1;		// 0, 1, 0, 1, 0, ...

	m_cntDraw = 0;

	{
		INT32 lockOffset, lockSize;

		lockSize = m_cntAlloc;

		// 전체 Instance VA의 반크기만큼을 Lock한다.
		lockOffset = (m_idxShow * m_MaxSprite);

		m_pInstanceVA->Lock( I3G_LOCK_OVERWRITE, lockOffset, lockSize);
	}

	// Alloc-List를 돌면서 차례로 Update
	{
		VEC4D *		pInst;
		VEC3D *		pSrc;

		INT32 idxVtx = (m_idxShow * m_MaxSprite);

		for( i = 0; i < cnt; i++)
		{
			pSprite = pLayer->m_DrawSpriteList[ idxStart+i];

			if (pSprite == nullptr)
				continue;

			if( pSprite->isEnabled() && pSprite->isState( I3UI_SPRITE_STATE_SETTED))
			{
				// Virtual Texture Validation
				if( (pSprite->getTexRange()->x >= 0.0f))
				{
					INT64 tick = i3Timer::GetPerformanceTime();

					{
						VEC4D rect, * pCoord;

						pCoord = pSprite->getTexCoord();
						rect.x = pCoord->z;			// u1
						rect.y = pCoord->w;			// v1
						rect.z = pCoord->x;			// (u2 - u1)
						rect.w = pCoord->y;			// (v2 - v1)

						if( m_pVTex->Validate( &rect, pSprite->getTexRange(), true) == false)
						{
							// 이 Sprite을 처리하기엔 VRAM Cache가 부족하다.
							// 이전까지 설정된 것으로 우선 Draw를 수행하고
							// 다음에 이어서 Draw한다.
							goto ExitPart;
						}
					}

				#if !defined (I3_NO_PROFILE)
					m_tickValidate += i3Timer::GetPerformanceTime() - tick;
				#endif

					//pSprite->addState( I3UI_SPRITE_STATE_VALIDATED);
				}

				pInst = (VEC4D *) m_pInstanceVA->GetTextureCoordPointer( 0, idxVtx);

				I3ASSERT( pSprite->isState( I3UI_SPRITE_STATE_SETTED));

				// Center
				pSrc = pSprite->getCenter();
				if (pSrc)
					i3Vector::Set( pInst + 0, pSrc->x, pSrc->y, pSrc->z, 1.0f);
				else
				{
					I3PRINTLOG(I3LOG_NOTICE, "pSprite->getCenter == nullptr",pSprite->getNode()->GetName());
				}

				// Size
				pSrc = pSprite->getSizeAndRotate();
				if (pSrc)
					i3Vector::Set( pInst + 1, pSrc->x, pSrc->y, pSrc->z, 0.0f);
				else
				{
					I3PRINTLOG(I3LOG_NOTICE, "pSprite->getSizeAndRotate == nullptr",pSprite->getNode()->GetName());
				}

				// Texture Cooord
				i3Vector::Copy( pInst + 2, pSprite->getTexCoord());

				// Texture Range
				i3Vector::Copy( pInst + 3, pSprite->getTexRange());

				// Color
				m_pInstanceVA->SetColor( idxVtx, 1, pSprite->getColor());

				
				idxVtx++;

				m_cntDraw++;
			}
		}
	}

ExitPart:

	m_pInstanceVA->Unlock();

	return i;
}

void i3UIRenderer::_Transfer( REAL32 maxTime)
{


	INT64 tick;

	if( m_pVTex != nullptr)
	{
		// Disc로부터 RAM으로 읽혀진 Texture를 다시 VRAM으로 Upload하는 부분.
		tick = i3Timer::GetPerformanceTime();
		m_pVTex->Transfer_RAM_VRAM( maxTime);
	
	#if !defined( I3_NO_PROFILE)
		m_tickTransfer += i3Timer::GetPerformanceTime() - tick;
	#endif

	}
}

void i3UIRenderer::_Render( i3RenderContext * pCtx)
{

	INT64 tick;
	
	if( m_cntDraw <= 0)
		return;

	// Shader Parameter 설정.
	// 

	// MinFilter 적용시 내부에서 강제로 옵션을 설정하게 됩니다.
	// UI는 Point filter를 사용해야 되므로 잠시 설정을 끕니다.
	I3G_TEXTURE_FILTER tempFilter = pCtx->getGfxOption()->getForcedMipmapFilter();
	pCtx->getGfxOption()->setForcedMipmapFilter( I3G_TEXTURE_FILTER_POINT);

	{
		i3ShaderParam * pParam = m_pShader->FindParam( I3G_SHADER_TYPE_VERTEX, "g_mUIProj");

		// Projection Matrix
		pParam->Update( &m_mtxProj);

		///////////////////////////////
		// Pixel Shader Parameters
		if( m_pVTex != nullptr)
		{
			// Cache의 LRU 처리에서 Tile이 사용된 시점을 관리하는 용도인 FrameID를 증가시킨다.
			// 이 FrameID를 바탕으로 Cache들은 각 Tile이 언제 Update된 것인지를 판단한다.

			m_pVTex->incFrameID();

			VEC4D vec;
			//i3VirtualTextureCache * pCache = m_pVTex->getCache();		// C4189

			tick = i3Timer::GetPerformanceTime();
			m_pVTex->UpdateIndirectTexture();

		#if !defined( I3_NO_PROFILE)
			m_tickUpdateIndirect += i3Timer::GetPerformanceTime() - tick;
		#endif

			// Cache Texture
			{
				m_ctxCacheTex.m_pTexture = m_pVTex->getCacheTexture();

				pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texCache");
				if( pParam != nullptr)		
				{
					m_ctxCacheTex.m_ModifyFlag = 0xFFFFFFFF;
					pParam->Update( &m_ctxCacheTex);
				}
			}

			// Redirect Texture
			{
				m_ctxIndirectTex.m_pTexture = m_pVTex->getRedirectTexture();

				pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texRedirect");
				if( pParam != nullptr)	
				{
					m_ctxIndirectTex.m_ModifyFlag = 0xFFFFFFFF;
					pParam->Update( &m_ctxIndirectTex);
				}
			}

			// Redirect Factors
			{
				pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_vRedirFactor");
				if( pParam != nullptr)		pParam->Update( m_pVTex->getRedirectShaderParam());
			}

			// Cache Texture resolution
			{
				m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_vCache", m_pVTex->getCacheShaderParam(), false);

				vec.x = m_pVTex->getCacheShaderParam()->z * 0.5f;
				vec.y = m_pVTex->getCacheShaderParam()->w * 0.5f;
				vec.z = m_pVTex->getCacheShaderParam()->z * I3VT_TILE_SIZE;
				vec.w = m_pVTex->getCacheShaderParam()->w * I3VT_TILE_SIZE;

				m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_vCacheTexel", &vec, false);
			}
		}

		// Font Parameters
		{
			i3Texture * pFontTex = m_pTextSys->GetFontTexture(0);

			I3ASSERT( pFontTex != nullptr);

			m_ctxFontTex.m_pTexture = pFontTex;

			pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texFont0");
			if( pParam != nullptr)
			{
				m_ctxFontTex.m_ModifyFlag = 0xFFFFFFFF;
				pParam->Update( &m_ctxFontTex);
			}

			// Factors
			VEC4D factor;

			i3Vector::Set( &factor, (REAL32)pFontTex->GetWidth(), (REAL32) pFontTex->GetHeight(),
							1.0f / pFontTex->GetWidth(), 1.0f / pFontTex->GetHeight());

			pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_vFontFactor");
			if( pParam != nullptr)
				pParam->Update( &factor);
		}
	}

	{
		I3GFX_INSTANCING_INFO info;

		info.count				= m_cntDraw;
		info.m_pVStream			= m_pGeoVA[0];
		info.m_pSecondVStream	= m_pInstanceVA;
		info.m_pVBDecl			= (IDirect3DVertexDeclaration9 *) m_pVBDecl;
		info.m_Offset_Stream0	= 0;
		info.m_Offset_Stream1	= m_idxShow * m_MaxSprite;

		pCtx->SetInstancingArray( &info);
		pCtx->SetIndexArray( m_pGeoIA[0]);
		pCtx->SetShader( m_pShader);
		//pCtx->SetFaceCullMode( I3G_FACE_CULL_NONE);
		pCtx->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, 0, 2);
	}

	{
		pCtx->SetShader( nullptr);
		pCtx->SetInstancingArray( nullptr);

		pCtx->getGfxOption()->setForcedMipmapFilter( tempFilter);
	}
}

void i3UIRenderer::_Reset(void)
{


#if !defined( I3_NO_PROFILE)
	m_tickUpdate = 0L;
	m_tickRender = 0L;
	m_tickTransfer = 0L;
	m_tickUpdateIndirect = 0L;
	m_tickValidate = 0L;
	m_tickTotal = 0L;
	m_cntDrawPass = 0;
	m_tickKick = 0L;
#endif

	m_bChangeDepth = false;
	m_PassCount = 0;

	if (m_pVTex)
		m_pVTex->incFrameID();
	
}

void i3UIRenderer::_ResetLayer(void)
{
	// 일단 UISprite 아닌 노드는 떼야한다..
	for (INT32 i = 0 ; i < m_LayerCount ; ++i )
	{
		I3UIRENDER_LAYERINFO* info = m_LayerStorage[i];			
			
		if ( info->m_pNodeSprite->GetChildCount() > 0)
			info->m_pNodeSprite->RemoveAllChild();
	//	info->m_DrawSpriteList.clear();					// 현재 수정된 사항으로는, 여기서 클리어하면 안된다....
	}

	if(m_pLayerRoot)
		m_pLayerRoot->RemoveAllChild();

	m_LayerCount = 0;
	m_pLastLayer = nullptr;
	//m_DrawSpriteList.clear();
	m_TopDrawLayerList.clear();
}

void i3UIRenderer::Draw_Kick( INT32 hSprite)
{
#if !defined( I3_NO_PROFILE)
	INT64 s, e;
	s = i3Timer::GetPerformanceTime();
#endif

	i3UISprite * pSprite = getSprite( hSprite);

	if( (m_pLastLayer != nullptr) &&
		((m_pLastLayer->m_SpriteStyle & 0x01) != (pSprite->getStyle() & 0x01)) )
	{	// 반드시 레이어를 생성해야 하는 경우
		DepthPlus();
	}

	if( m_bChangeDepth)
	{
		m_pLastLayer = _FindLayer( m_DrawDepth);
		m_bChangeDepth = false;
	}

	if( m_pLastLayer == nullptr )
	{
		// 새로운 Layer가 필요한 경우
		I3UIRENDER_LAYERINFO * pNewLayer = _AllocLayer();

		pNewLayer->m_SpriteStyle = pSprite->getStyle();
		pNewLayer->m_Depth = m_DrawDepth;

		m_pLastLayer = pNewLayer;

		if( m_bTopMost)
			m_TopDrawLayerList.push_back( pNewLayer);
	}

	if( pSprite->isStyle( I3UI_SPRITE_STYLE_NODE))
	{
		m_pLastLayer->m_pNodeSprite->AddChild(pSprite->getNode());
	}

	// Depth에 따라 순서 설정
	{
		m_pLastLayer->m_DrawSpriteList.push_back( pSprite);
	}

#if !defined( I3_NO_PROFILE)
	e = i3Timer::GetPerformanceTime();
	m_tickKick += e - s;
#endif
}

void i3UIRenderer::Draw_SetTopMost( bool bTopMost)
{
	DepthPlus();
	m_bTopMost = bTopMost;
}

void i3UIRenderer::Draw_Finish(void)
{
	for( UINT32 i = 0; i < m_TopDrawLayerList.size(); i++)
	{
		I3UIRENDER_LAYERINFO * pLayerInfo = m_TopDrawLayerList.at( i);
		
		pLayerInfo->m_pInstanceNode->ChangeParent( m_pLayerRoot);		// 소유권 공유..

		if( pLayerInfo->m_pNodeSprite != nullptr)
		{
			pLayerInfo->m_pNodeSprite->ChangeParent( m_pLayerRoot);
		}
	}

	m_TopDrawLayerList.clear();

	m_DrawDepth = 0;
}

void i3UIRenderer::_RenderLayerProc( i3RenderContext * pContext, I3UIRENDER_LAYERINFO * pLayer)
{



	INT64 s, e;//, ts;
	INT32 i, idx, cnt;

	I3ASSERT( pContext != nullptr);
	I3ASSERT( pLayer != nullptr);

	i3UIRenderer * pRenderer = g_pUIRenderer;
	I3ASSERT( pRenderer != nullptr);

	if( pRenderer->m_cntAlloc <= 0)
		return;

	if( pRenderer->m_pGeoVA[0] == nullptr)
		return;

	if( ( pRenderer->m_pVTex == nullptr) || (pRenderer->m_pVTex->isReadyToWork() == false))
		return;

	if( pLayer->m_SpriteStyle == I3UI_SPRITE_STYLE_NODE)
	{
		// 이 Layer는 Virtual-Texture 및 Instancing을 사용하는 Node가 아니다!.
		// 이 Layer는 i3Sprite2D Node 등에 의해 그려지기 때문에 처리를 할 필요가 없다.
		return;
	}

#if !defined( I3_NO_PROFILE)
	INT64 ts;
	ts = i3Timer::GetPerformanceTime();
#endif

	const INT32 myLayerSpriteCount = (INT32) pLayer->m_DrawSpriteList.size();

	pRenderer->InitRender( myLayerSpriteCount); // pLayer->m_Count);

	// Layer에 속한 모든 Sprite들을 그륀다!
	pRenderer->_Transfer();

	for( i = 0; i < myLayerSpriteCount; ) // pLayer->m_Count;)
	{
		idx = i; //pLayer->m_idxStart + i;

		// Instance Information 설정.
		{
			s = i3Timer::GetPerformanceTime();

			cnt = pRenderer->_UpdateAllInstance( pLayer, idx, myLayerSpriteCount - i ); // pLayer->m_Count - i);

			e = i3Timer::GetPerformanceTime();

		#if !defined( I3_NO_PROFILE)
			pRenderer->m_tickUpdate += e - s;
		#endif
		}

		// Update가 완료되어 있는 Instance를 Rendering.
		//
		// 이전 Frame에 Update되어 있는 Instance 정보를 바탕으로 Rendering을 시작한다.
		// Drawing과 Update를 Overlapping 시키기 위한...
		if( pRenderer->m_idxShow != -1)
		{
		#if !defined( I3_NO_PROFILE)
			s = i3Timer::GetPerformanceTime();
		#endif

			pRenderer->_Render( pContext);

		#if !defined( I3_NO_PROFILE)
			e = i3Timer::GetPerformanceTime();
			pRenderer->m_tickRender += e - s;
		#endif
		}

		if( cnt > 0)
			i += cnt;
		else
			i++;
		
	#if !defined( I3_NO_PROFILE)
		pRenderer->m_cntDrawPass ++;
	#endif
	}

	#if !defined( I3_NO_PROFILE)
		pRenderer->m_tickTotal = i3Timer::GetPerformanceTime() - ts;
	#endif

	pRenderer->EndRender();
}

void i3UIRenderer::FlushVRAM(void)
{


	// Sprite들의 모든 Validate 상태를 제거한다.
	{
		INT32 idxTemp = m_idxAllocHead;

		while( idxTemp != -1)
		{
			i3UISprite * pSprite = getSprite( idxTemp);

			pSprite->removeState( I3UI_SPRITE_STATE_VALIDATED);

			idxTemp = pSprite->getNext();
		}
	}

	m_pVTex->FlushVRAM();
}

I3UIRENDER_LAYERINFO * i3UIRenderer::_AllocLayer(void)
{
	if ( m_LayerCount >= (INT32)m_LayerStorage.size() )		// 이 경우에 그냥 추가해놓고, 그냥 놔둔다..
	{
		const size_t next_layer_count = m_LayerCount + 1;
		for (size_t i = m_LayerStorage.size() ; i < next_layer_count ; ++i )
		{
			I3UIRENDER_LAYERINFO* info = new I3UIRENDER_LAYERINFO;	 // 구조체이므로 초기화는 조금 신경 써준다.

			NetworkDump_Malloc( info, sizeof( I3UIRENDER_LAYERINFO), __FILE__, __LINE__);

	//		info->m_Count = 0;	info->m_idxStart = 0;
			info->m_SpriteStyle = 0;
			info->m_Depth = -1;
			info->m_DrawSpriteList.reserve( 512);
			
			i3Geometry* pGeometry = info->m_pInstanceNode = i3Geometry::new_object();			// 레퍼런스 소유권을 가져야한다..
			i3UserAttr* pUserAttr = i3UserAttr::new_object_ref();
			i3Node*		pNodeSprite = info->m_pNodeSprite = i3Node::new_object();

			if (m_bLegacyMode)
				pUserAttr->SetCallBack( (I3USERATTRPROC) _RenderLayerProc_Legacy, info);
			else
				pUserAttr->SetCallBack( (I3USERATTRPROC) _RenderLayerProc, info);

			pGeometry->AppendGeometryAttr(pUserAttr);
			pGeometry->AddFlag( I3_NODEFLAG_VOLATILE );
			pNodeSprite->AddFlag( I3_NODEFLAG_VOLATILE );
			m_LayerStorage.push_back(info);
		}

		I3TRACE( "- Current Maximum UI Sprite Layer Capacity : %d\n", next_layer_count);
	}
	
	I3UIRENDER_LAYERINFO * pInfo =	m_LayerStorage[m_LayerCount];
//	pInfo->m_idxStart = 0;		// 생성대신에 기존에 있는걸 리셋한다.
//	pInfo->m_Count = 0;
	pInfo->m_DrawSpriteIndex = 0;
	pInfo->m_DrawSpriteList.clear();

	I3ASSERT( m_pRoot != nullptr);
	
	m_LayerCount++;

	m_pLayerRoot->AddChild( pInfo->m_pInstanceNode );			// 소유권 공유..
	m_pLayerRoot->AddChild( pInfo->m_pNodeSprite );

	return pInfo;
}

I3UIRENDER_LAYERINFO * i3UIRenderer::_FindLayer( INT32 depth)
{
	if( m_LayerCount > 0)
	{
	//	for( UINT32 i = m_LayerCount-1; i > 0; i--)
		for ( INT32 i = 0 ; i < m_LayerCount ; ++i  )
		{
			I3UIRENDER_LAYERINFO * pInfo = m_LayerStorage.at( m_LayerCount - i - 1);
			if( pInfo->m_Depth == depth)
			{
				return pInfo;
			}
		}
	}

	return nullptr;
}

void i3UIRenderer::_FreeLayer( INT32 idx)
{	
//	I3_BOUNDCHK( idx, UI_RENDER_LAYER_COUNT);
}


bool i3UIRenderer::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	I3_SAFE_RELEASE( m_pGeoVA[0]);
	I3_SAFE_RELEASE( m_pGeoVA[1]);
	I3_SAFE_RELEASE( m_pGeoIA[0]);
	I3_SAFE_RELEASE( m_pGeoIA[1]);
	I3_SAFE_RELEASE( m_pInstanceVA);

#if defined( I3G_DX)
	{
		IDirect3DVertexDeclaration9 * pDecl = (IDirect3DVertexDeclaration9 *) m_pVBDecl;
		if( pDecl != nullptr) pDecl->Release();
		m_pVBDecl = nullptr;
	}
#endif

	return true;
}

bool i3UIRenderer::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	_CreateVertexArrays();

	return true;
}


void i3UIRenderer::GetProfileString( char * pszBuf, INT32 len)
{
#if !defined( I3_NO_PROFILE)
	char szTemp[ 512];
	REAL64 timeRes = i3Timer::GetProfileResolution();

	i3::snprintf( szTemp, sizeof(szTemp), "---------------- UI Renderer ------------------\n");
	i3::generic_string_cat( pszBuf, szTemp);

	if( m_bLegacyMode)
		i3::generic_string_cat( pszBuf,             " Legacy Mode : ON\n");

	i3::snprintf( szTemp, sizeof(szTemp), " (Total)        : %.2f\n", m_tickTotal * timeRes);
	i3::generic_string_cat( pszBuf, szTemp);

	i3::snprintf( szTemp, sizeof(szTemp), " Render         : %.2f\n", m_tickRender * timeRes);
	i3::generic_string_cat( pszBuf, szTemp);

	i3::snprintf( szTemp, sizeof(szTemp), " + Transfer     : %.2f\n", m_tickTransfer * timeRes);
	i3::generic_string_cat( pszBuf, szTemp);

	i3::snprintf( szTemp, sizeof(szTemp), " Update         : %.2f\n", m_tickUpdate * timeRes);
	i3::generic_string_cat( pszBuf, szTemp);

	i3::snprintf( szTemp, sizeof(szTemp), " + Validate     : %.2f\n", m_tickValidate * timeRes);
	i3::generic_string_cat( pszBuf, szTemp);

	i3::snprintf( szTemp, sizeof(szTemp), " + Inidrect     : %.2f\n", m_tickUpdateIndirect * timeRes);
	i3::generic_string_cat( pszBuf, szTemp);

	i3::snprintf( szTemp, sizeof(szTemp), " Kick         : %.2f\n", m_tickKick * timeRes);
	i3::generic_string_cat( pszBuf, szTemp);

	i3::snprintf( szTemp, sizeof(szTemp), " Draw Pass      : %d\n", m_cntDrawPass);
	i3::generic_string_cat( pszBuf, szTemp);

	if( m_pVTex != nullptr)
	{
		i3VirtualTextureCache * pCache = m_pVTex->getCache();

		pCache->GetProfileString( pszBuf, len);
	}
#endif
}

void i3UIRenderer::GetProfileString( i3::string& inout)
{
#if !defined( I3_NO_PROFILE)

	char szTemp[ 512];
	REAL64 timeRes = i3Timer::GetProfileResolution();

	i3::snprintf( szTemp, sizeof(szTemp), "---------------- UI Renderer ------------------\n");
	inout += szTemp;

	if( m_bLegacyMode)
		inout += " Legacy Mode : ON\n";

	i3::snprintf( szTemp, sizeof(szTemp), " (Total)        : %.2f\n", m_tickTotal * timeRes);
	inout += szTemp;

	i3::snprintf( szTemp, sizeof(szTemp), " Render         : %.2f\n", m_tickRender * timeRes);
	inout += szTemp;

	i3::snprintf( szTemp, sizeof(szTemp), " + Transfer     : %.2f\n", m_tickTransfer * timeRes);
	inout += szTemp;

	i3::snprintf( szTemp, sizeof(szTemp), " Update         : %.2f\n", m_tickUpdate * timeRes);
	inout += szTemp;

	i3::snprintf( szTemp, sizeof(szTemp), " + Validate     : %.2f\n", m_tickValidate * timeRes);
	inout += szTemp;

	i3::snprintf( szTemp, sizeof(szTemp), " + Inidrect     : %.2f\n", m_tickUpdateIndirect * timeRes);
	inout += szTemp;

	i3::snprintf( szTemp, sizeof(szTemp), " Draw Pass      : %d\n", m_cntDrawPass);
	inout += szTemp;

	if( m_pVTex != nullptr)
	{
		i3VirtualTextureCache * pCache = m_pVTex->getCache();

		pCache->GetProfileString(inout);
	}
#endif
}


void i3UIRenderer::CheckList(void)
{


	i3UISprite * pTemp;
	INT32 temp, cnt = 0;

	temp = m_idxAllocHead;

	while( temp != -1)
	{
		cnt++;

		pTemp = getSprite( temp);

		temp = pTemp->getNext();

		I3ASSERT( cnt < 3000);
	}

}

void i3UIRenderer::setSpriteColor( INT32 idx, COLOR* pColor)
{
	I3_BOUNDCHK( idx, m_MaxSprite);

	i3UISprite* pSprite = &m_pSprites[idx];
	
	pSprite->setColor( pColor);
}

void i3UIRenderer::getSpriteColor( INT32 idx, COLOR* pColor)
{
	I3_BOUNDCHK( idx, m_MaxSprite);

	i3UISprite* pSprite = &m_pSprites[idx];
	
	UINT32 color = pSprite->getColor();

	i3Color::ConvertARGB32( color, pColor);
}

i3UIRenderer * i3UIRenderer::getInstance(void)
{
	return g_pUIRenderer;
}

#if defined( I3_DEBUG)
void i3UIRenderer::SaveFontCache( const char * pszPath)
{
	i3Texture * pFontTex = m_pTextSys->GetFontTexture(0);
	if( pFontTex != nullptr)
		pFontTex->Dump( pszPath);
}

#endif
