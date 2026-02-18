#include "i3OptPCH.h"
#include "i3OptMacroTexture.h"
#include "i3OptBuildIndexedGeometry.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/string/ext/extract_filename.h"

I3_CLASS_INSTANCE( i3OptMacroTexture);

i3OptMacroTexture::i3OptMacroTexture(void)
{
	m_Class = CLASS_TEXTURE;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

i3OptMacroTexture::~i3OptMacroTexture(void)
{
	_removeAllMergeInfo();
	_removeAllMacroTex();
}

bool i3OptMacroTexture::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3Geometry*>(pNode))
	{
		//i3Geometry * pGeo = (i3Geometry *) pNode;

		// 가장 마지막 Diffuse Texture Bind를 찾는다.
		i3TextureBindAttr * pTexBind = _findLastTexture( pNode, m_pBindMeta);

		if( pTexBind != nullptr)
		{
			i3Texture * pTex = pTexBind->GetTexture();

			if((pTex == nullptr) || ((INT32) pTex->GetWidth() > m_TargetCX) || ( (INT32) pTex->GetHeight() > m_TargetCY))
			{
				// 1. Texture가 없거나
				// 2. 폭과 높이에서 목표 크기보다 크다면 대상에서 제외한다.
				return true;
			}

			// 해당 Texture를 포함한 Merge Info.를 검색한다.
			TEX_MERGE_INFO * pInfo = _findMergeInfoByTex( pTexBind->GetTexture());

			// 현재 Node를 추가해 둔다.
			if( pInfo == nullptr)
			{
				pInfo = _addMergeInfo( pTexBind->GetTexture());
			}

			pInfo->m_GeoList.push_back(static_cast<i3Geometry*>(pNode));
			pInfo->m_BindList.push_back( pTexBind);
		}
	}

	return true;
}

bool					i3OptMacroTexture::_isTilling( TEX_MERGE_INFO * pInfo, i3GeometryAttr * pGeoAttr)
{
	// 해당 Geometry의 Vertex Data를 검색해,
	// Tilling이 사용되고 있는지를 확인.

	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	INT32 i;
	VEC2D uv;
	bool bResult = false;
	REAL32 errX, errY;

	// 만약 Diffuse Texture Coordinates를 가지고 있지 않은 Vertex라면
	// Tiling이 되어 있는 Geometry라고 임시로 반환해
	// 처리가 되지 않도록 한다.
	if( pVA->GetFormat()->GetTextureCoordSetCount() <= m_idxTexCoord)
		return true;

	pVA->Lock();

	errX = 5.0f / pInfo->m_Width;
	errY = 5.0f / pInfo->m_Height;

	for( i = 0; i < (INT32) pVA->GetCount(); i++)
	{
		pVA->GetTextureCoord( m_idxTexCoord, i, &uv);

		if( (i3Math::abs( uv.x) > (1.0f + errX)) || (i3Math::abs( uv.y) > (1.0f + errY)))
		{
			bResult = true;
			break;
		}
	}

	pVA->Unlock();
	
	return bResult;
}

bool i3OptMacroTexture::_checkTiledGeometry( TEX_MERGE_INFO * pInfo)
{
	INT32 k;
	i3Geometry * pGeo;
	i3RenderAttr * pAttr;

	for(size_t j = 0; j < pInfo->m_GeoList.size(); j++)
	{
		pGeo = pInfo->m_GeoList[j];

		for( k = 0; k < pGeo->GetGeometryAttrCount(); k++)
		{
			pAttr = pGeo->GetGeometryAttr( k);

			if( i3::kind_of<i3GeometryAttr* >(pAttr))
			{
				if( _isTilling( pInfo, (i3GeometryAttr *) pAttr))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void i3OptMacroTexture::_FilterTiledTextures(void)
{
	TEX_MERGE_INFO * pInfo;

	for(size_t i = 0; i < m_MergeInfoList.size(); i++)
	{
		pInfo = m_MergeInfoList[i];

		if( _checkTiledGeometry( pInfo))
		{
			m_MergeInfoList.erase(m_MergeInfoList.begin() + i);

			delete pInfo;

			i--;
		}
	}
}

// 해당 Node의 Parent를 따라가며, i3TextureBindAttr를 찾는다.
i3TextureBindAttr *		i3OptMacroTexture::_findLastTexture( i3Node * pNode, i3ClassMeta * pBindMeta)
{
	i3Node * pRoot = getTraceRoot()->GetParent();		// pRoot는 nullptr일 수 있다.
	INT32 i;
	i3RenderAttr * pAttr;

	while( pNode != pRoot)
	{
		if( i3::kind_of<i3AttrSet* >(pNode))
		{
			i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

			for( i = 0; i < pAttrSet->GetAttrCount(); i++)
			{
				pAttr = pAttrSet->GetAttr( i);

				if( pAttr->same_of( pBindMeta))
				{
					return (i3TextureBindAttr *) pAttr;
				}
			}
		}

		pNode = pNode->GetParent();
	}

	return nullptr;
}

TEX_MERGE_INFO *		i3OptMacroTexture::_addMergeInfo( i3Texture * pTex)
{
	TEX_MERGE_INFO * pInfo;

	pInfo = new TEX_MERGE_INFO;

	pInfo->m_bPlaced = false;
	pInfo->m_Width = pTex->GetWidth();
	pInfo->m_Height = pTex->GetHeight();
	pInfo->m_X = -1;
	pInfo->m_Y = -1;
	pInfo->m_pTex = pTex;

	m_MergeInfoList.push_back( pInfo);

	return pInfo;
}

TEX_MERGE_INFO *		i3OptMacroTexture::_findMergeInfoByTex( i3Texture * pTex)
{
	TEX_MERGE_INFO * pInfo;

	for(size_t i = 0; i < m_MergeInfoList.size(); i++)
	{
		pInfo = m_MergeInfoList[i];

		if( pInfo->m_pTex == pTex)
			return pInfo;
	}

	return nullptr;
}

void					i3OptMacroTexture::_removeAllMergeInfo(void)
{
	TEX_MERGE_INFO * pInfo;

	for(size_t i = 0; i < m_MergeInfoList.size(); i++)
	{
		pInfo = m_MergeInfoList[i];

		delete pInfo;
	}

	m_MergeInfoList.clear();
}

bool i3OptMacroTexture::_canBePlace( MACRO_TEX_INFO * pMacro, i3::pack::RECT * pRect, bool bCheckForPlaced)
{
	TEX_MERGE_INFO * pTemp;
	i3::pack::RECT rt;

	// 오른쪽으로 영역이 벗어났는지 확인
	if( pRect->right > m_TargetCX)
		return false;

	// 아래쪽으로 영역이 벗어났는지 확인
	if( pRect->bottom > m_TargetCY)
		return false;

	for(size_t i = 0; i < pMacro->m_MergeList.size(); i++)
	{
		pTemp = pMacro->m_MergeList[i];

		if( bCheckForPlaced)
		{
			if( pTemp->m_bPlaced == false)
				continue;
		}

		rt.left		= (REAL32) pTemp->m_X;
		rt.top		= (REAL32) pTemp->m_Y;
		rt.right	= (REAL32) (pTemp->m_X + pTemp->m_Width - 1);
		rt.bottom	= (REAL32) (pTemp->m_Y + pTemp->m_Height - 1);

		if( i3Math::RectInRect( &rt, pRect) != TOUCH_NONE)
			return false;
	}

	return true;
}

bool i3OptMacroTexture::_TryToPlace( MACRO_TEX_INFO * pMacro, TEX_MERGE_INFO * pMerge)
{
	TEX_MERGE_INFO * pTemp;
	i3::pack::RECT rt;

	// Merge List가 하나도 없는 경우에 대한 처리
	rt.left = 0.0f;
	rt.top = 0.0f;
	rt.right = (REAL32) pMerge->m_Width;
	rt.bottom = (REAL32) pMerge->m_Height;

	if( _canBePlace( pMacro, &rt))
	{
		pMerge->m_X = 0;
		pMerge->m_Y = 0;
		return true;
	}

	// 이미 배치된 Merge Texture들의 인접 오른 측면과 아래면으로
	// 배치가 가능한지 확인한다.
	for(size_t i = 0; i < pMacro->m_MergeList.size(); i++)
	{
		pTemp = pMacro->m_MergeList[i];

		// 오른쪽 측면
		rt.left		= (REAL32) (pTemp->m_X + pTemp->m_Width + 1);
		rt.top		= (REAL32) (pTemp->m_Y);
		rt.right	= (REAL32) (rt.left + pMerge->m_Width);
		rt.bottom	= (REAL32) (rt.top + pMerge->m_Height);

		if( _canBePlace( pMacro, &rt))
		{
			pMerge->m_X = (INT32) rt.left;
			pMerge->m_Y = (INT32) rt.top;
			return true;
		}

		// 아래쪽
		rt.left		= (REAL32) pTemp->m_X;
		rt.top		= (REAL32) (pTemp->m_Y + pTemp->m_Height + 1);
		rt.right	= (REAL32) (rt.left + pMerge->m_Width);
		rt.bottom	= (REAL32) (rt.top + pMerge->m_Height);

		if( _canBePlace( pMacro, &rt))
		{
			pMerge->m_X = (INT32) rt.left;
			pMerge->m_Y = (INT32) rt.top;
			return true;
		}
	}

	return false;
}

MACRO_TEX_INFO * i3OptMacroTexture::_FindFittedMacroTex( TEX_MERGE_INFO * pInfo)
{

	MACRO_TEX_INFO * pMacro;

	for(size_t i = 0; i < m_MacroTexList.size(); i++)
	{
		pMacro = m_MacroTexList[i];

		if( _TryToPlace( pMacro, pInfo))
		{
			return pMacro;
		}
	}

	return nullptr;
}

MACRO_TEX_INFO *	i3OptMacroTexture::_addMacroTex( void)
{
	MACRO_TEX_INFO * pNew;

	pNew = new MACRO_TEX_INFO;

	pNew->m_pMacroTex = nullptr;
	pNew->m_Width = 0;
	pNew->m_Height = 0;

	m_MacroTexList.push_back( pNew);

	return pNew;
}

void					i3OptMacroTexture::_createMacroTex( MACRO_TEX_INFO * pInfo)
{
	pInfo->m_pMacroTex = i3Texture::new_object_ref();
	pInfo->m_pMacroTex->Create( pInfo->m_Width, pInfo->m_Height, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

	char conv[MAX_PATH];
	char szName[ MAX_PATH];
	INT32 idx;

	idx = INT32(i3::vu::index_of(m_MacroTexList, pInfo));
	if (idx == INT32(m_MacroTexList.size()) ) idx = -1;
		
	szName[0] = 0;

	i3::extract_filetitle( getSg()->GetName(), szName);

	sprintf( conv, "%s_%s_MacroTex%d", szName, m_szPrefix, idx);
	pInfo->m_pMacroTex->SetName( conv);

	pInfo->m_pMacroTex->Lock( 0);

	// Clear
	i3mem::FillZero( pInfo->m_pMacroTex->GetLockedBuffer( 0), pInfo->m_pMacroTex->GetDataSize(0));

	pInfo->m_pMacroTex->Unlock( 0);
}

void					i3OptMacroTexture::_removeAllMacroTex(void)
{
	MACRO_TEX_INFO * pInfo;

	for(size_t i = 0; i < m_MacroTexList.size(); i++)
	{
		pInfo = m_MacroTexList[i];

		delete pInfo;
	}

	m_MacroTexList.clear();
}

/*
void					i3OptMacroTexture::_buildMactoTex(void)
{
	TEX_MERGE_INFO * pInfo;
	MACRO_TEX_INFO *	pMacro;
	i3Texture * pSrc, * pDest;

	for(size_t i = 0; i < m_MergeInfoList.size(); i++)
	{
		pInfo = m_MergeInfoList[i];

		// 가장 적합한 Macro Texture에 배치한다.
		// 만약 없다면 추가한다.

		pMacro = _FindFittedMacroTex( pInfo);
		if( pMacro != nullptr)
		{
			// pInfo의 X, Y 좌표는 이미 설정이 되어있다.
			pMacro->m_MergeList.push_back( pInfo);

			I3TRACE( "%s -> %s (%d, %d)\n", pInfo->m_pTex->GetName(), pMacro->m_pMacroTex->GetName(), pInfo->m_X, pInfo->m_Y);
		}
		else
		{
			pMacro = _addMacroTex();

			bool bRv =  _TryToPlace( pMacro, pInfo);
			I3ASSERT( bRv);

			pMacro->m_MergeList.push_back( pInfo);

			I3TRACE( "%s -> %s (%d, %d)\n", pInfo->m_pTex->GetName(), pMacro->m_pMacroTex->GetName(), pInfo->m_X, pInfo->m_Y);
		}
	}

	for(size_t i = 0; i < m_MacroTexList.size(); i++)
	{
		pMacro = m_MacroTexList[i];

		_copyToMacroTex( pMacro);
	}

	I3TRACE( "*** Total %d Texture(s) are merged into %d macro textures...\n", m_MergeInfoList.GetCount(), m_MacroTexList.GetCount());
}
*/

bool i3OptMacroTexture::_isAllPlaced(void)
{
	bool bPlaced = true;
	TEX_MERGE_INFO * pInfo;

	for(size_t i = 0; i < m_MergeInfoList.size(); i++)
	{
		pInfo = m_MergeInfoList[i];

		bPlaced = bPlaced && pInfo->m_bPlaced;
	}

	return bPlaced;
}

bool i3OptMacroTexture::_Rec_TryToPlace( MACRO_TEX_INFO * pMacro, RECT * pRect)
{
	TEX_MERGE_INFO * pInfo, * pFindInfo = nullptr;
	INT32 srcSpan, destSpan;
	INT32 srcCX, srcCY;
	INT32 minDiff = 0x7FFFFFFF;
	RECT rt;

	if( pRect == nullptr)
	{
		rt.left = 0;	rt.top = 0;
		rt.right = m_TargetCX;
		rt.bottom = m_TargetCY;

		pRect = &rt;
	}

	srcCX = pRect->right - pRect->left;
	srcCY = pRect->bottom - pRect->top;
	srcSpan = srcCX * srcCY;

	// 배치되지 않은 Merge Tex들 중에 해당 Rect에 가장 적합하게 들어갈 수 있는
	// 놈을 찾는다. 면적의 차이를 비교
	for(size_t i = 0; i < m_MergeInfoList.size(); i++)
	{
		pInfo = m_MergeInfoList[i];

		if( pInfo->m_bPlaced)
			continue;						// 이미 배치가 끝난 놈은 제외

		// 가로든 세로든 지정된 범위를 벗어난다면 제외
		if((pInfo->m_Width > srcCX) || (pInfo->m_Height > srcCY))
			continue;

		destSpan = pInfo->m_Width * pInfo->m_Height;

		INT32 diff = abs( srcSpan - destSpan);

		if( minDiff > diff)
		{
			pFindInfo = pInfo;
			minDiff = diff;
		}
	}

	if( pFindInfo == nullptr)
	{
		// 더 이상 배치 가능한 것이 없다면 배치를 종료한다.
		// 그러나, 이 상황이 항상 문제인 것은 아니며
		// 실제로 배치가 정상적으로 종료된 경우에도 발생할 수 있다.
		return true;
	}

	// 배치!!!!
	{
		pFindInfo->m_bPlaced = true;
		pFindInfo->m_X = pRect->left;
		pFindInfo->m_Y = pRect->top;

		pMacro->m_MergeList.push_back( pFindInfo);
	}

	// 남는 Right, Bottom 영역에 대해 배치 시도
	{
		RECT remnant;
		i3::pack::RECT temp;

		// Right영역
		remnant.left	= pFindInfo->m_X + pFindInfo->m_Width;
		remnant.top		= pFindInfo->m_Y;
		remnant.right	= pRect->right;
		remnant.bottom	= pFindInfo->m_Y + pFindInfo->m_Height;

		temp.left		= (REAL32) remnant.left;
		temp.top		= (REAL32) remnant.top;
		temp.right		= (REAL32) remnant.right;
		temp.bottom		= (REAL32) remnant.bottom;

		if( _canBePlace( pMacro, &temp))
		{
			_Rec_TryToPlace( pMacro, &remnant);
		}

		// bottom 영역
		//
		// 우선 Right영역과 중복이 될 수 있는 4-4분면도 포함시킨 공간을 시도한다.
		remnant.left	= pFindInfo->m_X;
		remnant.top		= pFindInfo->m_Y + pFindInfo->m_Height;
		remnant.right	= pRect->right;
		remnant.bottom	= pRect->bottom;

		temp.left		= (REAL32) remnant.left;
		temp.top		= (REAL32) remnant.top;
		temp.right		= (REAL32) remnant.right;
		temp.bottom		= (REAL32) remnant.bottom;

		if( _canBePlace( pMacro, &temp))
		{
			_Rec_TryToPlace( pMacro, &remnant);
		}
	}

	return true;
}

void					i3OptMacroTexture::_buildMactoTex(void)
{
	MACRO_TEX_INFO * pMacro = nullptr;
	INT32 cnt = 0;

	// 각 Texture들을 배치한다.
	ReportSubStart( (INT32) m_MacroTexList.size(), "Try to place piece of textures...");
	do
	{
		pMacro = _addMacroTex();

		cnt++;
		
		_Rec_TryToPlace( pMacro, nullptr);

	} while( _isAllPlaced() == false);

	// 배치된 Texture들에서 MacroTexture의 최대 필요 크기를 계산한다.
	for(size_t i = 0 ; i < m_MacroTexList.size(); i++)
	{
		pMacro = m_MacroTexList[i];

		for(size_t j = 0; j < pMacro->m_MergeList.size(); j++)
		{
			TEX_MERGE_INFO * pInfo = pMacro->m_MergeList[j];

			pMacro->m_Width = max( pMacro->m_Width, pInfo->m_X + pInfo->m_Width);
			pMacro->m_Height = max( pMacro->m_Height, pInfo->m_Y + pInfo->m_Height);
		}

		// 2의 승수로 조정.
		pMacro->m_Width = i3Math::GetNearestPowerOf2( (INT32) pMacro->m_Width);
		pMacro->m_Height = i3Math::GetNearestPowerOf2( (INT32) pMacro->m_Height);

		_createMacroTex( pMacro);

		ReportSubProgress( cnt, pMacro->m_pMacroTex->GetName());

		// 실제 Texture를 복사한다.
		_copyToMacroTex( pMacro);
	}

	ReportSubEnd();


	I3TRACE( "*** Total %d Texture(s) are merged into %d macro textures...\n", m_MergeInfoList.size(), m_MacroTexList.size());
}

// 구성된 Macro Texture의 Texel을 복사한다.
void i3OptMacroTexture::_copyToMacroTex( MACRO_TEX_INFO * pMacro)
{
	RECT srcRect, destRect;
	
	i3Texture * pSrc, * pDest;

	pDest = pMacro->m_pMacroTex;

	for(size_t j = 0; j < pMacro->m_MergeList.size(); j++)
	{
		TEX_MERGE_INFO * pInfo = pMacro->m_MergeList[j];

		pSrc = pInfo->m_pTex;

		srcRect.left = 0;
		srcRect.top = 0;
		srcRect.right = pSrc->GetWidth();
		srcRect.bottom = pSrc->GetHeight();

		destRect.left = pInfo->m_X;
		destRect.top  = pInfo->m_Y;
		destRect.right = pInfo->m_X + pInfo->m_Width;
		destRect.bottom = pInfo->m_Y + pInfo->m_Height;

		i3Surface * pSrcSurface = pSrc->getSurface( 0);
		i3Surface * pDestSurface = pDest->getSurface( 0);

		::D3DXLoadSurfaceFromSurface( pDestSurface->getSurface(), NULL, &destRect,
									pSrcSurface->getSurface(), NULL, &srcRect,
										D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER,
										0xFF000000);

		{
			IDirect3DTexture9 * pDXTex = ((i3TextureDX *) pDest)->GetDXImage();

			pDXTex->AddDirtyRect( &destRect);
		}
	}

	if( m_bDump)
	{
		i3ImageFile file;
		char szName[ MAX_PATH];

		sprintf( szName, "C:\\%s.i3i", pDest->GetName());

		file.Save( szName, pDest);
	}
}

static void		_normalizeUV( VEC2D * pTex, REAL32 errX, REAL32 errY)
{
	if( i3Math::abs( pTex->x) <= errX)
		pTex->x = 0.0f;
	else if( i3Math::abs(1.0f - pTex->x) <= errX)
		pTex->x = 1.0f;

	if( i3Math::abs( pTex->y) <= errY)
		pTex->y = 0.0f;
	else if( i3Math::abs(1.0f - pTex->y) <= errY)
		pTex->y = 1.0f;
}

static void		_adjustGeometryTexCoord( MACRO_TEX_INFO * pMacro, TEX_MERGE_INFO * pInfo, i3Geometry * pGeo, INT32 idxTex)
{
	INT32 i, j;
	i3GeometryAttr * pGeoAttr;
	i3VertexArray * pVA;
	REAL32 scaleX, scaleY, offX, offY, errX, errY;
	bool bNegX, bNegY;

	scaleX = (REAL32) pInfo->m_Width / pMacro->m_pMacroTex->GetWidth();
	scaleY = (REAL32) pInfo->m_Height / pMacro->m_pMacroTex->GetHeight();
	offX = (REAL32) pInfo->m_X / pMacro->m_pMacroTex->GetWidth();
	offY = (REAL32) pInfo->m_Y / pMacro->m_pMacroTex->GetHeight();
	errX = 5.0f / pInfo->m_Width;
	errY = 5.0f / pInfo->m_Height;

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		i3RenderAttr * pAttr = pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr* >(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		if( pGeoAttr->GetIndexArray() != nullptr)
		{
			i3OptBuildIndexedGeometry opt;

			// 만약 Indexed-geometry라면 Non-Indexed로 변환한다.
			opt.ConvertIndexed2Vertex( pGeoAttr);
		}

		pVA = pGeoAttr->GetVertexArray();

		if( pVA->GetFormat()->GetTextureCoordSetCount() < 1)
			continue;

		if( pVA->GetFormat()->GetTextureCoordSetCount() <= idxTex)
		{
			I3TRACE( "Texture 좌표가 모자람.");
			continue;
		}

		pVA->Lock();

		for( j = 0; j < (INT32) (pVA->GetCount() / 3); j++)
		{
			VEC2D tex0, tex1, tex2;

			pVA->GetTextureCoord( idxTex, (j * 3) + 0, &tex0);
			pVA->GetTextureCoord( idxTex, (j * 3) + 1, &tex1);
			pVA->GetTextureCoord( idxTex, (j * 3) + 2, &tex2);

			_normalizeUV( &tex0, errX, errY);
			_normalizeUV( &tex1, errX, errY);
			_normalizeUV( &tex2, errX, errY);

			// 음의 좌표가 있는지 확인한다.
			bNegX = bNegY = false;

			if( (tex0.x < 0.0f) || (tex1.x < 0.0f) || (tex2.x < 0.0f))
				bNegX = true;

			if( (tex0.y < 0.0f) || (tex1.y < 0.0f) || (tex2.y < 0.0f))
				bNegY = true;

			if( bNegX == false)
			{
				tex0.x = (tex0.x * scaleX) + offX;
				tex1.x = (tex1.x * scaleX) + offX;
				tex2.x = (tex2.x * scaleX) + offX;
			}
			else
			{
				tex0.x = ((tex0.x + 1.0f) * scaleX) + offX;
				tex1.x = ((tex1.x + 1.0f) * scaleX) + offX;
				tex2.x = ((tex2.x + 1.0f) * scaleX) + offX;
			}

			if( bNegY == false)
			{
				tex0.y = (tex0.y * scaleY) + offY;
				tex1.y = (tex1.y * scaleY) + offY;
				tex2.y = (tex2.y * scaleY) + offY;
			}
			else
			{
				tex0.y = ((tex0.y + 1.0f) * scaleY) + offY;
				tex1.y = ((tex1.y + 1.0f) * scaleY) + offY;
				tex2.y = ((tex2.y + 1.0f) * scaleY) + offY;
			}

			pVA->SetTextureCoord( idxTex, (j * 3) + 0, &tex0);
			pVA->SetTextureCoord( idxTex, (j * 3) + 1, &tex1);
			pVA->SetTextureCoord( idxTex, (j * 3) + 2, &tex2);
		}

		pVA->Unlock();
	}
}

void					i3OptMacroTexture::_adjustTexCoord(void)
{
	
	MACRO_TEX_INFO * pMacro;
	TEX_MERGE_INFO * pInfo;

	ReportSubStart( (INT32) m_MacroTexList.size(), "Adjusting texture coordinates...");

	for(size_t i = 0; i < m_MacroTexList.size(); i++)
	{
		pMacro = m_MacroTexList[i];

		ReportSubProgress( i, pMacro->m_pMacroTex->GetName());

		for(size_t j = 0; j < pMacro->m_MergeList.size(); j++)
		{
			pInfo = pMacro->m_MergeList[j];

			for(size_t k = 0; k < pInfo->m_GeoList.size(); k++)
			{
				_adjustGeometryTexCoord( pMacro, pInfo, pInfo->m_GeoList[k], m_idxTexCoord);
			}
		}
	}

	ReportSubEnd();
}

void					i3OptMacroTexture::_removeOldTexBind(void)
{
	
	MACRO_TEX_INFO * pMacro;
	TEX_MERGE_INFO * pInfo;

	for(size_t i = 0; i < m_MacroTexList.size(); i++)
	{
		pMacro = m_MacroTexList[i];

		for(size_t j = 0; j < pMacro->m_MergeList.size(); j++)
		{
			pInfo = pMacro->m_MergeList[j];

			for(size_t k = 0; k < pInfo->m_BindList.size(); k++)
			{
				i3TextureBindAttr * pAttr = pInfo->m_BindList[k];

				pAttr->SetTexture( pMacro->m_pMacroTex);
			}
		}
	}
}

void i3OptMacroTexture::Optimize( i3SceneGraphInfo * pSg)
{
	bool bTempLinked = false;

	setSg( pSg);

	ReportStart( 3, "Build Macro textures...");

	// Texture Merge Information을 만든다.
	// Common Sg.를 포함한 전체 Sg.를 임시로 만들어 생성한다.
	i3Node * pRoot;

	ReportProgress( 1, "Merging scene-graph.");
	if( pSg->getCommonSg() != nullptr)
	{
		i3Node * pTemp;

		pRoot = pSg->getCommonSg();
		
		// 가장 말단을 찾는다.
		for( pTemp = pRoot; pTemp->GetChildCount() > 0;)
		{
			pTemp = pTemp->GetChild( 0);
		}

		// 임시로 Common Sg.와 Instance Sg.를 붙인다.
		if( pTemp->IsChild( pSg->getInstanceSg()) == false)
		{
			pTemp->AddChild( pSg->getInstanceSg());
			bTempLinked = true;
		}
	}
	else
	{
		// Instance Sg.만을 대상으로 처리
		pRoot = pSg->getInstanceSg();
	}

	ReportProgress( 1, "Process for diffuse maps...");
	// Diffuse에 대한 처리.
	{
		m_pBindMeta = i3TextureBindAttr::static_meta();
		m_idxTexCoord = 0;
		i3::safe_string_copy( m_szPrefix, "Diffuse", 256);

		// Merge Info를 Build한다.
		Trace( pRoot);

		// 생성된 Merge Texture List에서 Macro Textur를 생성할 수 없는 놈들은 제거한다.
		_FilterTiledTextures();

		if( m_MergeInfoList.size() <= 0)
			return;

		// 생성할 Macro Texture의 크기를 결정한다.

		// Macro Texture에 Merge 시킨다.
		_buildMactoTex();

		// 각 Geometry의 UV 좌표를 조정한다.
		_adjustTexCoord();

		// 기존의 Texture Bind를 제거
		_removeOldTexBind();

		_removeAllMergeInfo();
		_removeAllMacroTex();
	}

	ReportProgress( 2, "Process for diffuse maps...");
	// LuxMap에 대한 처리.
	{
		m_pBindMeta = i3LuxMapBindAttr::static_meta();
		m_idxTexCoord = 1;
		i3::safe_string_copy( m_szPrefix, "Lux", 256);

		// Merge Info를 Build한다.
		Trace( pRoot);

		// 생성된 Merge Texture List에서 Macro Textur를 생성할 수 없는 놈들은 제거한다.
		_FilterTiledTextures();

		if( m_MergeInfoList.size() <= 0)
			return;

		// 생성할 Macro Texture의 크기를 결정한다.

		// Macro Texture에 Merge 시킨다.
		_buildMactoTex();

		// 각 Geometry의 UV 좌표를 조정한다.
		_adjustTexCoord();

		// 기존의 Texture Bind를 제거
		_removeOldTexBind();

		_removeAllMergeInfo();
		_removeAllMacroTex();
	}

	ReportEnd();
}

bool	i3OptMacroTexture::isSupportProgress(void)
{
	return true;
}

const char * i3OptMacroTexture::getDesc(void)
{
	return "커다란 하나의 Texture로 통합 (World)";
}
