#include "i3GfxType.h"
#include "i3GfxOccCuller.h"
#include "i3GfxGlobalVariableDX.h"

#include "i3Shader.h"
#include "i3ShaderCache.h"
#include "i3Math/structure/mat4.h"
#include "i3Math/octree/octree_item.h"
#include "i3Math/structure/aabb.h"
#include "i3Math/octree/occ_quad_set.h"

I3_CLASS_INSTANCE(i3GfxOccCuller);

namespace detail_occ
{
	struct BOX_VERTEX
	{
		D3DXVECTOR3 pos;    // Position of the vertex
	};

	struct BOX_GEOMETRY
	{
		IDirect3DVertexBuffer9*         VB = nullptr;
		IDirect3DIndexBuffer9*          IB = nullptr;
		i3::vector<i3::octree_item*>	OccludeeList;
	};

	struct QUAD_INSTANCE
	{
		static const DWORD MAX_INSTANCE = 256;				//	일단 256개 단위로 끊는다..더적을수도 있겠다.
		IDirect3DVertexBuffer9*			VB = nullptr;
		IDirect3DIndexBuffer9*			IB = nullptr;
		i3::vector<BOX_VERTEX>			vecVerts;
	};

	struct SHADER_PARAM
	{
		i3ShaderParam* mWorld = nullptr;
		i3ShaderParam* mViewProj = nullptr;
		i3ShaderParam* diffuseColor = nullptr;
	};



	void CreateQuadInstance(QUAD_INSTANCE* outInst)
	{
		g_pD3DDevice->CreateVertexBuffer(
			4 * QUAD_INSTANCE::MAX_INSTANCE*sizeof(BOX_VERTEX), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &outInst->VB, 0);

		g_pD3DDevice->CreateIndexBuffer(6 * QUAD_INSTANCE::MAX_INSTANCE* sizeof(WORD), D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16, D3DPOOL_DEFAULT, &outInst->IB, 0);

		// 버텍스 쪽은 실시간으로 채우게 됨...
		// 여기서는 인덱스 쪽만 미리 매핑해두면 될것 같다..

		WORD*		pIndices;

		outInst->IB->Lock(0, 0, (void**)&pIndices, 0);

		const INT32 num_index = 6 * QUAD_INSTANCE::MAX_INSTANCE;

		for (INT32 i = 0, idx = 0; i < num_index; i += 6, idx += 4)
		{
			pIndices[i + 0] = WORD(idx + 0);
			pIndices[i + 1] = WORD(idx + 1);
			pIndices[i + 2] = WORD(idx + 2);
			pIndices[i + 3] = WORD(idx + 0);
			pIndices[i + 4] = WORD(idx + 2);
			pIndices[i + 5] = WORD(idx + 3);
		}
		
		outInst->IB->Unlock();

		outInst->vecVerts.reserve(4 * QUAD_INSTANCE::MAX_INSTANCE);
	}

	void ClearQuadInstance(QUAD_INSTANCE*& outInst)
	{
		if (outInst == nullptr)
			return;
		I3_SAFE_RELEASE_NODBG(outInst->IB);
		I3_SAFE_RELEASE_NODBG(outInst->VB);
		delete outInst;	outInst = nullptr;
	}

	//
	// 스트리밍 처리 대신에 그냥 DISCARD로 꽉 꽉 채워가면서 그리도록 한다..
	//

	void RenderQuadInstance(QUAD_INSTANCE* inout)
	{
		g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		g_pD3DDevice->SetStreamSource(0, inout->VB, 0, sizeof(detail_occ::BOX_VERTEX));
		g_pD3DDevice->SetIndices(inout->IB);

		const i3::vector<BOX_VERTEX>& vecQuad = inout->vecVerts;

		const size_t numQuad = vecQuad.size() / 4;
		const size_t numBatchFull = numQuad / QUAD_INSTANCE::MAX_INSTANCE;
		const size_t remain = numQuad % QUAD_INSTANCE::MAX_INSTANCE;

		BOX_VERTEX* pVert;
				
		// 넘치는 경우엔 락과 언락 / Draw를 번갈아 처리한다..
		for (size_t i = 0; i < numBatchFull; ++i)
		{
			const BOX_VERTEX* input = &vecQuad[i * QUAD_INSTANCE::MAX_INSTANCE * 4];

			inout->VB->Lock(0, sizeof(BOX_VERTEX)* QUAD_INSTANCE::MAX_INSTANCE * 4, (void**)&pVert, D3DLOCK_DISCARD);

			::memcpy(pVert, input, sizeof(BOX_VERTEX)* QUAD_INSTANCE::MAX_INSTANCE * 4);

			inout->VB->Unlock();

			g_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, QUAD_INSTANCE::MAX_INSTANCE * 4, 0,
				2 * QUAD_INSTANCE::MAX_INSTANCE);
		}

		if (remain)
		{
			const BOX_VERTEX* input = &vecQuad[numBatchFull* QUAD_INSTANCE::MAX_INSTANCE * 4];

			inout->VB->Lock(0, sizeof(BOX_VERTEX)* remain * 4, (void**)&pVert, D3DLOCK_DISCARD);

			::memcpy(pVert, input, sizeof(BOX_VERTEX)* remain * 4);

			inout->VB->Unlock();

			g_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, remain * 4, 0,
				2 * remain);
		}
		
	}

	int RenderBoxGeometry(BOX_GEOMETRY* BoxGeom, SHADER_PARAM* ShaderParam, LPDIRECT3DQUERY9 query)
	{
		int numOccludeeToRender = 0;
		g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		g_pD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

		g_pD3DDevice->SetStreamSource(0, BoxGeom->VB, 0, sizeof(detail_occ::BOX_VERTEX));
		g_pD3DDevice->SetIndices(BoxGeom->IB);

		const size_t numAABB = BoxGeom->OccludeeList.size();
		i3::vec3 center;
		i3::vec3 length;
		MATRIX matWorld = I3_IDENTITYMATRIX;

		for (size_t i = 0; i < numAABB; ++i)
		{
			i3::octree_item* item = BoxGeom->OccludeeList[i];

			const i3::aabb& ab = item->get_aabb();

			ab.calc_center(center);
			ab.calc_length(length);

			matWorld.m30 = center.x;	matWorld.m31 = center.y;	matWorld.m32 = center.z;
			matWorld.m00 = length.x;	matWorld.m11 = length.y;	matWorld.m22 = length.z;
			
			ShaderParam->mWorld->Update(&matWorld);

			query->Issue(D3DISSUE_BEGIN);

			// 개별 AABB 렌더링 (루프돌면서 각각) // 쓰기 버퍼를 끄고 그리는게 좋겠다.
			g_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 6 * 2);

			query->Issue(D3DISSUE_END);
			
			DWORD pixel_count;
			while (query->GetData(&pixel_count, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE);
			// while문에서 나름의 오버헤드가 존재한다..

			if (pixel_count > 0)
			{
				item->on_visible_occluder_aabb();
				++numOccludeeToRender;
			}
			else
			{
				item->on_hidden_occluder_aabb();
			}
		}

		return numOccludeeToRender;
	}
	
	void CreateBox(BOX_GEOMETRY* outGeom)
	{
		g_pD3DDevice->CreateVertexBuffer(8 * sizeof(BOX_VERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &outGeom->VB, 0);
		g_pD3DDevice->CreateIndexBuffer(6 * 6 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &outGeom->IB, 0);
		
		BOX_VERTEX* pVert;
		WORD*		pIndices;
		outGeom->VB->Lock(0, 0, (void**)&pVert, 0);

		// z축이 DX와 반대인듯 하여, z값 부호만 일단 뒤집어보았다..
		pVert[0].pos = D3DXVECTOR3(-0.5f, -0.5f, 0.5f);
		pVert[1].pos = D3DXVECTOR3( 0.5f, -0.5f, 0.5f);
		pVert[2].pos = D3DXVECTOR3(-0.5f,  0.5f, 0.5f);
		pVert[3].pos = D3DXVECTOR3( 0.5f,  0.5f, 0.5f);
		pVert[4].pos = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		pVert[5].pos = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		pVert[6].pos = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		pVert[7].pos = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);

		outGeom->VB->Unlock();
		
		outGeom->IB->Lock(0, 0, (void**)&pIndices, 0);
		
		// 앞면..
		pIndices[0] = 0;	pIndices[1] = 2;	pIndices[2] = 1;
		pIndices[3] = 1;	pIndices[4] = 2;	pIndices[5] = 3;

		// 왼쪽 면
		pIndices[6] = 4;	pIndices[7] = 6;	pIndices[8] = 0;
		pIndices[9] = 0;	pIndices[10] = 6;	pIndices[11] = 2;

		// 뒷면
		pIndices[12] = 5;	pIndices[13] = 7;	pIndices[14] = 4;
		pIndices[15] = 4;	pIndices[16] = 7;	pIndices[17] = 6;

		// 윗면
		pIndices[18] = 2;	pIndices[19] = 6;	pIndices[20] = 3;
		pIndices[21] = 3;	pIndices[22] = 6;	pIndices[23] = 7;
		
		// 오른쪽 면
		pIndices[24] = 1;	pIndices[25] = 3;	pIndices[26] = 5;
		pIndices[27] = 5;	pIndices[28] = 3;	pIndices[29] = 7;

		// 바닥면
		pIndices[30] = 4;	pIndices[31] = 0;	pIndices[32] = 5;
		pIndices[33] = 5;   pIndices[34] = 0;	pIndices[35] = 1;
			
		outGeom->IB->Unlock();
	}

	void ClearBox(BOX_GEOMETRY*& outGeom)
	{
		if (outGeom == nullptr)
			return;

		I3_SAFE_RELEASE_NODBG(outGeom->IB);
		I3_SAFE_RELEASE_NODBG(outGeom->VB);
		delete outGeom;	outGeom = nullptr;
	}

}

i3GfxOccCuller::i3GfxOccCuller()
{
	m_ShaderParam  = new detail_occ::SHADER_PARAM;
}

i3GfxOccCuller::~i3GfxOccCuller(void)
{
	delete m_ShaderParam;
	I3_SAFE_RELEASE(m_occShader);
	I3_SAFE_RELEASE_NODBG(m_VertexDecl);
	I3_SAFE_RELEASE_NODBG(m_occDepthSurf);
	I3_SAFE_RELEASE_NODBG(m_occSurface);
	I3_SAFE_RELEASE_NODBG(m_d3dQuery);

	ClearBox(m_BoxGeom);
	ClearQuadInstance(m_QuadInstance);
}

// 화면 해상도의 1/4만 사용하도록 하자... fill rate 절약을 위해서..

void	i3GfxOccCuller::Create()
{
	if (m_d3dQuery)
		return;
	
	m_BoxGeom = new detail_occ::BOX_GEOMETRY;
	m_BoxGeom->IB = nullptr;
	m_BoxGeom->VB = nullptr;

	m_QuadInstance = new detail_occ::QUAD_INSTANCE;
	m_QuadInstance->IB = nullptr;
	m_QuadInstance->VB = nullptr;


	g_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_d3dQuery);

	INT32 width = g_pD3Dpp->BackBufferWidth;
	INT32 height = g_pD3Dpp->BackBufferHeight;
	D3DFORMAT fmt = g_pD3Dpp->BackBufferFormat;

	INT32 eff_w = width >> 3;
	INT32 eff_h = height >> 3;
	
	HRESULT hr = g_pD3DDevice->CreateRenderTarget(eff_w, eff_h, fmt, D3DMULTISAMPLE_NONE, 0, FALSE, &m_occSurface, NULL);

	if (hr == D3DERR_OUTOFVIDEOMEMORY)
	{
		I3PRINTLOG(I3LOG_NOTICE, "CreateRenderTarget for OCC - D3DERR_OUTOFVIDEOMEMORY");

		g_pD3DDevice->EvictManagedResources();

		hr = g_pD3DDevice->CreateRenderTarget(eff_w, eff_h, fmt, D3DMULTISAMPLE_NONE,
			0, FALSE, &m_occSurface, NULL);

		if (hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			I3PRINTLOG(I3LOG_FATAL, "CreateRenderTarget for OCC - D3DERR_OUTOFVIDEOMEMORY");
			return;
		}
	}

	hr = g_pD3DDevice->CreateDepthStencilSurface(eff_w, eff_h, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0,
		FALSE, &m_occDepthSurf, NULL);
	
	if (hr == D3DERR_OUTOFVIDEOMEMORY)
	{
		I3PRINTLOG(I3LOG_NOTICE, "CreateDepthStencilSurface for OCC - D3DERR_OUTOFVIDEOMEMORY");

		g_pD3DDevice->EvictManagedResources();

		hr = g_pD3DDevice->CreateDepthStencilSurface(eff_w, eff_h, D3DFMT_D16, D3DMULTISAMPLE_NONE,
			0, FALSE, &m_occDepthSurf, NULL);
		if (hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			I3PRINTLOG(I3LOG_FATAL, "CreateDepthStencilSurface for OCC - D3DERR_OUTOFVIDEOMEMORY");
			return;
		}
	}
	
	D3DVERTEXELEMENT9 VertexElemConstants[] =
	{
		{ 0, 0,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
		D3DDECL_END()
	};

	g_pD3DDevice->CreateVertexDeclaration(VertexElemConstants, &m_VertexDecl);

	// occ Shader 생성

	I3_SAFE_RELEASE(m_occShader);
	m_occShader = i3ShaderCache::LoadShader("i3GfxOccCuller.hlsl");
	if (m_occShader == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not load a i3GfxOccCuller.hlsl.");
		return;
	}
	m_occShader->AddRef();
		
	// 셰이더 파라미터를 미리 매핑한다..  
	i3ShaderParam* param;

	param = m_occShader->FindParam(I3G_SHADER_TYPE_VERTEX, "g_mWorld");
	if (param == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not find g_mWorld in i3GfxOccCuller.hlsl.");
		return;
	}
	m_ShaderParam->mWorld = param;


	param = m_occShader->FindParam(I3G_SHADER_TYPE_VERTEX, "g_mViewProj");
	if (param == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not find g_mViewProj in i3GfxOccCuller.hlsl.");
		return;
	}
	m_ShaderParam->mViewProj = param;

	param = m_occShader->FindParam(I3G_SHADER_TYPE_PIXEL, "g_Color");
	if (param == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not find g_Color in i3GfxOccCuller.hlsl.");
		return;
	}

	m_ShaderParam->diffuseColor = param;

	CreateBox(m_BoxGeom);
	CreateQuadInstance(m_QuadInstance);
	
}

int	i3GfxOccCuller::Update(const i3::mat4& matViewProj)
{
	if (i3GfxOccCuller::isDestroyed())
	{
		// 콜백이 필요할수도 있겠다.
		return 0;
	}
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	LPDIRECT3DSURFACE9 pOrigSurf;
	g_pD3DDevice->GetRenderTarget(0, &pOrigSurf);		// 레퍼런스 주의..
	g_pD3DDevice->SetRenderTarget(0, m_occSurface);

	LPDIRECT3DSURFACE9 pOrigDepth;
	g_pD3DDevice->GetDepthStencilSurface(&pOrigDepth);
	g_pD3DDevice->SetDepthStencilSurface(m_occDepthSurf);

	g_pD3DDevice->BeginScene();

	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.f, 0);

	g_pD3DDevice->SetVertexDeclaration(m_VertexDecl);

	i3ShaderContext * pCtx = m_occShader->getContext();

	IDirect3DVertexShader9* pVS = (IDirect3DVertexShader9*)pCtx->getShader(I3G_SHADER_TYPE_VERTEX);
	IDirect3DPixelShader9* pPS = (IDirect3DPixelShader9*)pCtx->getShader(I3G_SHADER_TYPE_PIXEL);

	g_pD3DDevice->SetVertexShader(pVS);
	g_pD3DDevice->SetPixelShader(pPS);

	m_ShaderParam->mViewProj->Update(const_cast<MATRIX*>(reinterpret_cast<const MATRIX*>(&matViewProj)));
	m_ShaderParam->mWorld->Update(&I3_IDENTITYMATRIX);		

	COLORREAL clr(1.f, 1.f, 1.f, 1.f);
	m_ShaderParam->diffuseColor->Update(&clr);

	detail_occ::RenderQuadInstance(m_QuadInstance);
	 
	// 여기에 오클루더를 별도의 계산없이 모두 렌더링 ( 쿼드들은 DYNAMIC 렌더링으로 처리하는게 좋을것 같다..)

	// 그다음에 오클루디의 목록을 하나씩 렌더링하면서 픽셀 카운트 체크...
	// 이 경우, 처리된 오클루디가 오클루더에 자동 포함되면 안되기 때문에 쓰기버퍼를 끄고 그리도록 한다...
	 
	// 오클루디 AABB는 그냥 단일 렌더링으로 따로 따로 처리하는게 좋을 것으로 보인다...
	int numRenderOccludeeToRender = detail_occ::RenderBoxGeometry(m_BoxGeom, m_ShaderParam, m_d3dQuery);

	g_pD3DDevice->EndScene();

	g_pD3DDevice->SetDepthStencilSurface(pOrigDepth);
	pOrigDepth->Release();

	g_pD3DDevice->SetRenderTarget(0, pOrigSurf);
	pOrigSurf->Release();

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_BoxGeom->OccludeeList.clear();
	m_QuadInstance->vecVerts.clear();

	return numRenderOccludeeToRender;
}

bool	i3GfxOccCuller::Destroy(bool bLostDevice)
{
	// VB, IB 역시 정리해두도록 하자..
	ClearQuadInstance(m_QuadInstance);
	ClearBox(m_BoxGeom);

	I3_SAFE_RELEASE_NODBG(m_occDepthSurf);
	I3_SAFE_RELEASE_NODBG(m_occSurface);
	I3_SAFE_RELEASE_NODBG(m_d3dQuery);
	
	return i3GfxResource::Destroy(bLostDevice);
}

bool	i3GfxOccCuller::isDestroyed(void)
{
	return m_d3dQuery == nullptr;
}

bool	i3GfxOccCuller::Revive(i3RenderContext * pCtx)
{
	i3GfxOccCuller::Create();

	return i3GfxResource::Revive(pCtx);
}

void	i3GfxOccCuller::AddOccluderQuad(const i3::vec3& p0, const i3::vec3& p1, const i3::vec3& p2, const i3::vec3& p3)
{
	if (i3GfxOccCuller::isDestroyed())
		return;

	detail_occ::BOX_VERTEX vtx;

	::memcpy(&vtx.pos, &p0, sizeof(D3DXVECTOR3));
	m_QuadInstance->vecVerts.push_back(vtx);
	::memcpy(&vtx.pos, &p1, sizeof(D3DXVECTOR3));
	m_QuadInstance->vecVerts.push_back(vtx);
	::memcpy(&vtx.pos, &p2, sizeof(D3DXVECTOR3));
	m_QuadInstance->vecVerts.push_back(vtx);
	::memcpy(&vtx.pos, &p3, sizeof(D3DXVECTOR3));
	m_QuadInstance->vecVerts.push_back(vtx);
}

void	i3GfxOccCuller::AddOccluderTri(const i3::vec3& p0, const i3::vec3& p1, const i3::vec3& p2)	//degen 시켜서 처리함..
{
	if (i3GfxOccCuller::isDestroyed())
		return;

	detail_occ::BOX_VERTEX vtx;
	
	::memcpy(&vtx.pos, &p0, sizeof(D3DXVECTOR3));
	m_QuadInstance->vecVerts.push_back(vtx);
	::memcpy(&vtx.pos, &p1, sizeof(D3DXVECTOR3));
	m_QuadInstance->vecVerts.push_back(vtx);
	::memcpy(&vtx.pos, &p2, sizeof(D3DXVECTOR3));
	m_QuadInstance->vecVerts.push_back(vtx);
	::memcpy(&vtx.pos, &p2, sizeof(D3DXVECTOR3));
	m_QuadInstance->vecVerts.push_back(vtx);
}

void	i3GfxOccCuller::AddOccludee(i3::octree_item* item)
{
	if (i3GfxOccCuller::isDestroyed())
		return;

	m_BoxGeom->OccludeeList.push_back(item);		// 이거면 충분...업데이트 후엔 클리어 되어야함...
}


int	i3GfxOccCuller::UpdateOcclusions(const i3::vector<i3::occ_quad*>& occluder_list,
	const i3::vector<i3::octree_item*>& occludee_list, const i3::mat4& matViewProj)
{
	const size_t num_occluder = occluder_list.size();

	for (size_t i = 0; i < num_occluder; ++i)
	{
		i3::occ_quad* quad = occluder_list[i];

		if (quad->is_quad_triangle())
		{
			i3GfxOccCuller::AddOccluderTri(quad->get_quad_vertex_pos(0),
				quad->get_quad_vertex_pos(1), quad->get_quad_vertex_pos(2));
		}
		else
		{
			i3GfxOccCuller::AddOccluderQuad(quad->get_quad_vertex_pos(0),
				quad->get_quad_vertex_pos(1), quad->get_quad_vertex_pos(2), quad->get_quad_vertex_pos(3));
		}
	}

	const size_t num_occludee = occludee_list.size();

	for (size_t i = 0; i < num_occludee; ++i)
	{
		i3GfxOccCuller::AddOccludee(occludee_list[i]);
	}

	return i3GfxOccCuller::Update(matViewProj);
}


